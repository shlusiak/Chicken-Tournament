#include "stdafx.h"
#include "resource.h"

#ifndef INVALID_FILE_ATTRIBUTES
	#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif

HINSTANCE hInstance;



const PCHAR RegisterSuccess="Registering succeeded. Ready for playing.";
const PCHAR RegisterFailed="Registering failed.";
const PCHAR UnregisterSuccess="Unregistering succeeded.";
const PCHAR UnregisterFailed="Unregistering failed.";
// {71E69B75-E68E-4f52-8FE6-F12AE9CB3834}
static const GUID MYGUID = 
{ 0x71e69b75, 0xe68e, 0x4f52, { 0x8f, 0xe6, 0xf1, 0x2a, 0xe9, 0xcb, 0x38, 0x34 } };






BOOLEAN Register(HWND owner)
{   
	CHAR path[MAX_PATH];
	if (GetCurrentDirectory(MAX_PATH,&path[0])==0)return FALSE;
	if (path[strlen(&path[0])-1]!='\\')strcat(&path[0],"\\");

	{	// Search path for EXE
		CHAR c[MAX_PATH];
		strcpy(&c[0],&path[0]);
		strcat(&c[0],"CT.EXE");

		if (GetFileAttributes(&c[0])==INVALID_FILE_ATTRIBUTES)
		{
			CHAR c[MAX_PATH];
			c[0]='\0';
			OPENFILENAME of;
			ZeroMemory(&of,sizeof(of));
			of.lStructSize=sizeof(of);
			of.hInstance=hInstance;
			of.hwndOwner=owner;
			of.lpstrFilter="CT Executable file (ct.exe)\0ct.exe\0All Files (*.*)\0*.*\0\0";
			of.lpstrDefExt="exe";
			of.Flags=OFN_FILEMUSTEXIST|OFN_PATHMUSTEXIST;
			of.lpstrFile=&c[0];
			of.nMaxFile=MAX_PATH;
			of.lpstrTitle="Searching for CT executable.";

			if (GetOpenFileName(&of)==FALSE)return FALSE;

			CHAR p[MAX_PATH];
			PCHAR filename;
			GetFullPathName(&c[0],MAX_PATH,&p[0],&filename);

			*filename='\0';
			strcpy(&path[0],&p[0]);
		}
	}


	DPAPPLICATIONDESC myapp;
	ZeroMemory(&myapp,sizeof(myapp));
	myapp.dwSize=sizeof(myapp);
	myapp.dwFlags=0;
	myapp.lpszApplicationNameA="CT";
	myapp.guidApplication=MYGUID;
	myapp.lpszFilenameA="CT.EXE";
	myapp.lpszCommandLineA="";
	myapp.lpszPathA=&path[0];
	myapp.lpszCurrentDirectoryA=myapp.lpszPathA;
	myapp.lpszDescriptionA="Chicken Tournament";



	LPDIRECTPLAYLOBBY3A lpDirectPlayLobby;

	if( FAILED( CoCreateInstance( CLSID_DirectPlayLobby, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                                       (VOID**)&lpDirectPlayLobby) ) )return FALSE;

	BOOLEAN b=SUCCEEDED(lpDirectPlayLobby->RegisterApplication(0,&myapp));


	lpDirectPlayLobby->Release();

	return b;
}

BOOLEAN Unregister()
{
	LPDIRECTPLAYLOBBY3A lpDirectPlayLobby;
	if( FAILED( CoCreateInstance( CLSID_DirectPlayLobby, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                                       (VOID**)&lpDirectPlayLobby) ) )return FALSE;

	BOOLEAN b=SUCCEEDED(lpDirectPlayLobby->UnregisterApplication(0,MYGUID));

	lpDirectPlayLobby->Release();

	return b;
}



BOOL WINAPI DialogProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_COMMAND:
		switch(wParam)
		{
		case IDOK:
		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		case IDC_REGISTER:
			if (Register(hWnd)==FALSE)
				MessageBox(hWnd,RegisterFailed,NULL,MB_OK|MB_ICONHAND);else
				MessageBox(hWnd,RegisterSuccess,NULL,MB_OK|MB_ICONINFORMATION);
			break;
		case IDC_UNREGISTER:
			if (Unregister()==FALSE)
				MessageBox(hWnd,UnregisterFailed,NULL,MB_OK|MB_ICONHAND);else
				MessageBox(hWnd,UnregisterSuccess,NULL,MB_OK|MB_ICONINFORMATION);
			break;
		}
		break;
/*		case WM_ERASEBKGND:	
			{
				PAINTSTRUCT ps;
				HDC dc=BeginPaint(hWnd,&ps);
				
				PatBlt(dc,0,0,300,300,WHITENESS);

				EndPaint(hWnd,&ps);
			}
			return TRUE;*/

	default:return FALSE;
	}
	return TRUE;
}




int APIENTRY WinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPSTR     lpCmdLine,
                     int       nCmdShow )
{
	::hInstance=hInstance;

	CoInitialize(NULL);

	DialogBox(hInstance,MAKEINTRESOURCE(IDD_DIALOG),0,DialogProc);

	CoUninitialize();

	return 0;
}



