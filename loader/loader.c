#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

static HMODULE self,system_d3d12;
static INIT_ONCE runtime_once=INIT_ONCE_STATIC_INIT;
static FARPROC exports[18];
static const char *names[]={"SetAppCompatStringPointer","D3D12CreateDevice","D3D12GetDebugInterface","D3D12CoreCreateLayeredDevice","D3D12CoreGetLayeredDeviceSize","D3D12CoreRegisterLayers","D3D12CreateRootSignatureDeserializer","D3D12CreateVersionedRootSignatureDeserializer","D3D12DeviceRemovedExtendedData","D3D12EnableExperimentalFeatures","D3D12GetInterface","D3D12PIXEventsReplaceBlock","D3D12PIXGetThreadInfo","D3D12PIXNotifyWakeFromFenceSignal","D3D12PIXReportCounter","D3D12SerializeRootSignature","D3D12SerializeVersionedRootSignature","GetBehaviorValue"};
// Resolve the trusted system image directly. Overlay GetProcAddress shims can
// redirect CreateDevice back through the proxy and produce infinite recursion.
static FARPROC system_export(HMODULE module,const char *name){
 unsigned char *base=(unsigned char*)module;
 IMAGE_DOS_HEADER *dos=(IMAGE_DOS_HEADER*)base;
 if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return NULL;
 IMAGE_NT_HEADERS64 *nt=(IMAGE_NT_HEADERS64*)(base+dos->e_lfanew);
 if(nt->Signature!=IMAGE_NT_SIGNATURE)return NULL;
 DWORD image_size=nt->OptionalHeader.SizeOfImage;
 IMAGE_DATA_DIRECTORY dir=nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_EXPORT];
 if(!dir.VirtualAddress || dir.VirtualAddress>=image_size || dir.Size>image_size-dir.VirtualAddress)return NULL;
 IMAGE_EXPORT_DIRECTORY *table=(IMAGE_EXPORT_DIRECTORY*)(base+dir.VirtualAddress);
 DWORD *name_rvas=(DWORD*)(base+table->AddressOfNames);
 WORD *ordinals=(WORD*)(base+table->AddressOfNameOrdinals);
 DWORD *functions=(DWORD*)(base+table->AddressOfFunctions);
 for(DWORD i=0;i<table->NumberOfNames;i++){
  if(name_rvas[i]>=image_size)return NULL;
  if(lstrcmpA((const char*)base+name_rvas[i],name))continue;
  if(ordinals[i]>=table->NumberOfFunctions)return NULL;
  DWORD rva=functions[ordinals[i]];
  if(!rva || rva>=image_size)return NULL;
  // The inspected system d3d12 has direct exports. Refuse unknown forwarders.
  if(rva>=dir.VirtualAddress && rva-dir.VirtualAddress<dir.Size)return NULL;
  return (FARPROC)(base+rva);
 }
 return NULL;
}
static BOOL CALLBACK load_runtime(PINIT_ONCE once,PVOID param,PVOID *context){
 (void)once;(void)param;(void)context;
 wchar_t path[MAX_PATH];UINT n=GetSystemDirectoryW(path,MAX_PATH);
 if(!n || n+11>=MAX_PATH)return FALSE;
 lstrcatW(path,L"\\d3d12.dll");
 system_d3d12=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
 if(!system_d3d12 || system_d3d12==self)return FALSE;
 for(unsigned i=0;i<18;i++)exports[i]=system_export(system_d3d12,names[i]);
 return TRUE;
}
// Called only by exported function stubs, never from DllMain.
FARPROC resolve_export(unsigned index){
 if(index>=18 || !InitOnceExecuteOnce(&runtime_once,load_runtime,NULL,NULL) || !exports[index]){
  // An unavailable runtime export cannot be emulated with a guessed signature.
  RaiseFailFastException(NULL,NULL,0);TerminateProcess(GetCurrentProcess(),ERROR_PROC_NOT_FOUND);
 }
 return exports[index];
}
static void load_folder(const wchar_t *root,const wchar_t *folder){
 wchar_t pattern[32768],path[32768];
 if(swprintf(pattern,32768,L"%ls%ls\\*.asi",root,folder)<0)return;
 WIN32_FIND_DATAW data;HANDLE find=FindFirstFileW(pattern,&data);
 if(find==INVALID_HANDLE_VALUE)return;
 do{
  if(data.dwFileAttributes&FILE_ATTRIBUTE_DIRECTORY)continue;
  if(swprintf(path,32768,L"%ls%ls\\%ls",root,folder,data.cFileName)<0)continue;
  HMODULE plugin=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_DLL_LOAD_DIR|LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
  if(plugin){void (*initialize)(void)=(void(*)(void))GetProcAddress(plugin,"InitializeASI");if(initialize)initialize();}
 }while(FindNextFileW(find,&data));
 FindClose(find);
}
static DWORD WINAPI load_plugins(void *unused){
 (void)unused;
 wchar_t path[32768];DWORD n=GetModuleFileNameW(self,path,32768);
 if(!n || n>=32768)return 0;
 wchar_t *slash=wcsrchr(path,L'\\');if(!slash)return 0;*slash=0;
 load_folder(path,L"\\plugins");
 load_folder(path,L"\\scripts");
 return 0;
}
BOOL WINAPI DllMain(HINSTANCE instance,DWORD reason,LPVOID reserved){
 (void)reserved;
 if(reason==DLL_PROCESS_ATTACH){
  self=instance;DisableThreadLibraryCalls(instance);
  // No waiting or library loading here. Windows starts this worker after loader initialization.
  HANDLE thread=CreateThread(NULL,0,load_plugins,NULL,0,NULL);if(thread)CloseHandle(thread);
 }
 return TRUE;
}
