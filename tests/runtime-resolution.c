#include <windows.h>
#include <stdio.h>
static unsigned intercepted;
static FARPROC WINAPI intercepted_lookup(HMODULE m,LPCSTR n){(void)m;(void)n;intercepted++;return NULL;}
#define GetProcAddress intercepted_lookup
#include "../loader/loader.c"
#undef GetProcAddress
int main(void){
 FARPROC target=resolve_export(1);
 if(!system_d3d12 || target!=system_export(system_d3d12,"D3D12CreateDevice") || intercepted){puts("FAIL: runtime lookup used overlay-interceptable GetProcAddress");return 1;}
 for(unsigned i=0;i<18;i++)if(!resolve_export(i)){puts("FAIL: missing export");return 1;}
 puts("PASS: all 18 system targets resolve even when GetProcAddress is intercepted; runtime lookup bypasses shim.");return 0;
}
