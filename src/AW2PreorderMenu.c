#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

static HMODULE self;
static char root[MAX_PATH], logpath[MAX_PATH], inipath[MAX_PATH];
static void logline(const char *s) {
 HANDLE h=CreateFileA(logpath,FILE_APPEND_DATA,FILE_SHARE_READ,NULL,OPEN_ALWAYS,FILE_ATTRIBUTE_NORMAL,NULL);
 if(h!=INVALID_HANDLE_VALUE){DWORD n;WriteFile(h,s,(DWORD)strlen(s),&n,NULL);WriteFile(h,"\r\n",2,&n,NULL);CloseHandle(h);}
}
static int readable(const void *p,SIZE_T n,int write) {
 MEMORY_BASIC_INFORMATION q;
 if(!p || !VirtualQuery(p,&q,sizeof(q)) || q.State!=MEM_COMMIT || (q.Protect&(PAGE_GUARD|PAGE_NOACCESS)))return 0;
 DWORD pr=q.Protect&255;
 if(write && pr!=PAGE_READWRITE && pr!=PAGE_WRITECOPY && pr!=PAGE_EXECUTE_READWRITE && pr!=PAGE_EXECUTE_WRITECOPY)return 0;
 return (uintptr_t)p+n>=(uintptr_t)p && (uintptr_t)p+n<=(uintptr_t)q.BaseAddress+q.RegionSize;
}
// Two-element vectors of 0x38-byte menu descriptors, verified against this executable.
// Category at +0x20: 0=base, 1=preorder, 2=digital Deluxe.
static int find_entry(unsigned char *base,uintptr_t rva,const char *key,volatile LONG **result) {
 unsigned char *v=base+rva;
 void *items=*(void**)v;
 unsigned count=*(unsigned*)(v+8),capacity=*(unsigned*)(v+12);
 if(!items || count==0)return 0;
 if(count!=2 || capacity<2 || capacity>16 || !readable(items,0x70,1))return -1;
 for(unsigned i=0;i<count;i++){
  unsigned char *e=(unsigned char*)items+i*0x38;
  const char *k=*(const char**)(e+8);
  SIZE_T len=strlen(key)+1;
  if(!readable(k,len,0) || memcmp(k,key,len))continue;
  volatile LONG *category=(volatile LONG*)(e+0x20);
  if(*category!=1 && *category!=2)return -1;
  *result=category;return 1;
 }
 return -1;
}
static DWORD WINAPI worker(void *unused){
 (void)unused;
 GetModuleFileNameA(self,root,MAX_PATH);
 char *slash=strrchr(root,'\\');if(!slash)return 0;*slash=0;
 // ASI and INI live together under plugins; log is in the same directory.
 snprintf(logpath,MAX_PATH,"%s\\AW2PreorderMenu.log",root);
 snprintf(inipath,MAX_PATH,"%s\\AW2PreorderMenu.ini",root);
 logline("AW2PreorderMenu E01b - E01 skins validated by user; console hiding pending validation.");
 if(!GetPrivateProfileIntA("Mod","Enabled",1,inipath)){logline("Disabled by INI.");return 0;}
 unsigned char *b=(unsigned char*)GetModuleHandleA(NULL);
 IMAGE_DOS_HEADER *dos=(IMAGE_DOS_HEADER*)b;
 if(dos->e_magic!=IMAGE_DOS_SIGNATURE)return 0;
 IMAGE_NT_HEADERS64 *nt=(IMAGE_NT_HEADERS64*)(b+dos->e_lfanew);
 if(nt->Signature!=IMAGE_NT_SIGNATURE || nt->FileHeader.Machine!=IMAGE_FILE_MACHINE_AMD64 || nt->FileHeader.TimeDateStamp!=0x6acc184a || nt->OptionalHeader.SizeOfImage!=0x3e6c000){logline("REFUSED: unsupported executable.");return 0;}
 // Check both constructor anchors and their original category immediates before any writes.
 static const unsigned char sweater[]={0xc7,0x45,0x3f,1,0,0,0};
 static const unsigned char revolver[]={0xc7,0x45,0xff,1,0,0,0};
 if(memcmp(b+0x74d6b,sweater,7)||memcmp(b+0x75086,revolver,7)||strcmp((char*)b+0x301fab0,"SWEATER_SAGA_PREORDER")||strcmp((char*)b+0x3021940,"REVOLVER_ALAN_PREORDER")){
  logline("REFUSED: menu signatures differ.");return 0;
 }
 int s=0,r=0;volatile LONG *sc=NULL,*rc=NULL;
 for(unsigned i=0;i<1200;i++){
  s=find_entry(b,0x376ea48,"SWEATER_SAGA_PREORDER",&sc);
  r=find_entry(b,0x376e8e8,"REVOLVER_ALAN_PREORDER",&rc);
  if(s<0||r<0){logline("REFUSED: unexpected live descriptor layout; inspect before another build.");return 0;}
  if(s&&r){
   LONG so=InterlockedCompareExchange(sc,2,1);
   if(so!=1 && so!=2){logline("REFUSED: sweater changed during validation.");return 0;}
   LONG ro=InterlockedCompareExchange(rc,2,1);
   if(ro!=1 && ro!=2){if(so==1)InterlockedCompareExchange(sc,1,2);logline("REFUSED: revolver changed; sweater rolled back.");return 0;}
   logline("SWEATER_SAGA_PREORDER and REVOLVER_ALAN_PREORDER: category 2 (Deluxe).");
   logline("APPLIED: two cosmetic menu categories. Test selection and appearance; not validated.");return 0;}
  Sleep(50);
 }
 logline("TIMEOUT: menu descriptors unavailable. No changes applied.");
 return 0;
}
// Hide only the console attached to this game process; keep its plugin running.
static DWORD WINAPI hide_console(void *unused){
 (void)unused;
 char path[MAX_PATH];GetModuleFileNameA(self,path,MAX_PATH);
 char *end=strrchr(path,'\\');if(!end)return 0;*end=0;
 char ini[MAX_PATH];snprintf(ini,MAX_PATH,"%s\\AW2PreorderMenu.ini",path);
 if(!GetPrivateProfileIntA("Mod","HideDebugConsole",1,ini))return 0;
 for(unsigned i=0;i<1200;i++){
  HWND console=GetConsoleWindow();
  if(console){ShowWindow(console,SW_HIDE);return 0;}
  Sleep(50);
 }
 return 0;
}
__declspec(dllexport) void InitializeASI(void){
 static volatile LONG once=0;
 if(InterlockedCompareExchange(&once,1,0))return;
 HANDLE t=CreateThread(NULL,0,worker,NULL,0,NULL);if(t)CloseHandle(t);
 HANDLE c=CreateThread(NULL,0,hide_console,NULL,0,NULL);if(c)CloseHandle(c);
}
BOOL WINAPI DllMain(HINSTANCE h,DWORD reason,LPVOID reserved){
 (void)reserved;
 if(reason==DLL_PROCESS_ATTACH){self=h;DisableThreadLibraryCalls(h);}
 return TRUE;
}
