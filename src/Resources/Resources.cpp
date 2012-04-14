#include "stdafx.h"
#include "stdio.h"


HINSTANCE hInstance=0;




BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
	if (ul_reason_for_call==DLL_PROCESS_ATTACH)
		hInstance=(HINSTANCE)hModule;
    return TRUE;
}



const HINSTANCE GetDllInstance()
{	return hInstance;  }

const BOOLEAN HasUTSounds()
{
#ifdef UTSOUNDS
	return TRUE;
#else
	return FALSE;
#endif
}
