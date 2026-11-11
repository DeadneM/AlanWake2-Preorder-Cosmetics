#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>

static HMODULE self,system_d3d12;
static INIT_ONCE runtime_once=INIT_ONCE_STATIC_INIT;
static FARPROC exports[18];
static const char *names[]={"SetAppCompatStringPointer","D3D12CreateDevice","D3D12GetDebugInterface","D3D12CoreCreateLayeredDevice","D3D12CoreGetLayeredDeviceSize","D3D12CoreRegisterLayers","D3D12CreateRootSignatureDeserializer","D3D12CreateVersionedRootSignatureDeserializer","D3D12DeviceRemovedExtendedData","D3D12EnableExperimentalFeatures","D3D12GetInterface","D3D12PIXEventsReplaceBlock","D3D12PIXGetThreadInfo","D3D12PIXNotifyWakeFromFenceSignal","D3D12PIXReportCounter","D3D12SerializeRootSignature","D3D12SerializeVersionedRootSignature","GetBehaviorValue"};
static BOOL CALLBACK load_runtime(PINIT_ONCE once,PVOID param,PVOID *context){
 (void)once;(void)param;(void)context;
 wchar_t path[MAX_PATH];UINT n=GetSystemDirectoryW(path,MAX_PATH);
 if(!n || n+11>=MAX_PATH)return FALSE;
 lstrcatW(path,L"\\d3d12.dll");
 system_d3d12=LoadLibraryExW(path,NULL,LOAD_LIBRARY_SEARCH_SYSTEM32);
 if(!system_d3d12 || system_d3d12==self)return FALSE;
 for(unsigned i=0;i<18;i++)exports[i]=GetProcAddress(system_d3d12,names[i]);
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
