/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "resource.h"
#include "resources\\resource.h"
#include "multiplayer.h"
#include "config.h"
#include "text.h"
#include "resources\\resources.h"

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES (DWORD)(-1)
#endif



// {71E69B75-E68E-4f52-8FE6-F12AE9CB3834}
static const GUID MYGUID = 
{ 0x71e69b75, 0xe68e, 0x4f52, { 0x8f, 0xe6, 0xf1, 0x2a, 0xe9, 0xcb, 0x38, 0x34 } };


BOOLEAN bUseDirectPlayProtocol=TRUE;



// ***** Anfang: Connection auswählen ***** //

BOOL FAR PASCAL EnumConnectionsCallback(LPCGUID pguidSP,VOID* pConnection,DWORD dwConnectionSize,
                                                   LPCDPNAME pName,DWORD dwFlags,VOID* pvContext)
{
    HRESULT       hr;
    LPDIRECTPLAY4 pDP = NULL;
    VOID*         pConnectionBuffer = NULL;
    HWND          hWndListBox = (HWND)pvContext;
    LRESULT       iIndex;

    // Create a IDirectPlay object
    if( FAILED( hr = CoCreateInstance( CLSID_DirectPlay, NULL, CLSCTX_ALL, 
                                       IID_IDirectPlay4A, (VOID**)&pDP ) ) )
        return FALSE; // Error, stop enumerating
    // Test the if the connection is available by attempting to initialize 
    // the connection
    if( FAILED( hr = pDP->InitializeConnection( pConnection, 0 ) ) )
    {
        SAFE_RELEASE( pDP );
        return TRUE; // Unavailable connection, keep enumerating
    }
    // Don't need the IDirectPlay interface anymore, so release it
    SAFE_RELEASE( pDP ); 
    // Found a good connection, so put it in the listbox
    iIndex = SendMessage( hWndListBox, LB_ADDSTRING, 0, 
                          (LPARAM)pName->lpszShortNameA );
    if( iIndex == CB_ERR )
        return FALSE; // Error, stop enumerating

	HGLOBAL mem=GlobalAlloc(GHND,dwConnectionSize);
	pConnectionBuffer=GlobalLock(mem);
	if (pConnectionBuffer==NULL)return TRUE;
	memcpy(pConnectionBuffer,pConnection,dwConnectionSize);
	GlobalUnlock(mem);
    SendMessage( hWndListBox, LB_SETITEMDATA, iIndex, 
                 (LPARAM)mem);
    return TRUE; // Keep enumerating
}


BOOL CALLBACK ConnectionProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		SendMessage( GetDlgItem(hWnd,IDC_LIST), LB_ADDSTRING, 0,(LPARAM)"- Singleplayer -");

		if(FAILED(game->lpDirectPlay->EnumConnections( &MYGUID,EnumConnectionsCallback, 
											GetDlgItem(hWnd,IDC_LIST),0)))EndDialog(hWnd,0);
		if (SendDlgItemMessage(hWnd,IDC_LIST,LB_SETCURSEL,1,0)==LB_ERR)
			EndDialog(hWnd,1);
		SendDlgItemMessage(hWnd,IDC_DIRECTPLAYPROTOCOL,BM_SETCHECK,1,0);
		break;
	case WM_DESTROY:
		{
			for (int i=0;i<SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCOUNT,0,0);i++)
			{
				HGLOBAL g=(HGLOBAL)SendDlgItemMessage(hWnd,IDC_LIST,LB_GETITEMDATA,i,0);
				if (g) GlobalFree(g);
			}
		}
		break;
	case WM_COMMAND:
		if (HIWORD(wParam)==LBN_DBLCLK)
			PostMessage(hWnd,WM_COMMAND,IDOK,0);
		if (wParam==IDCANCEL)
			EndDialog(hWnd,0);
		if (wParam==IDOK)
		{
			int i=SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCURSEL,0,0);
			if (i==LB_ERR)break;
			HGLOBAL g=(HGLOBAL)SendDlgItemMessage(hWnd,IDC_LIST,LB_GETITEMDATA,i,0);
			if (g==0)
			{
				EndDialog(hWnd,1);
				break;
			}
			int s=GlobalSize(g);
			void* p=new BYTE[s];
			void* p2=GlobalLock(g);

			memcpy(p,p2,s);

			GlobalUnlock(g);

			bUseDirectPlayProtocol=BOOLEAN(SendDlgItemMessage(hWnd,IDC_DIRECTPLAYPROTOCOL,BM_GETCHECK,0,0));

			EndDialog(hWnd,DWORD(p));
		}
		break;
	default:return FALSE;
	}
	return TRUE;
}

// ***** ---------- ***** //


// ***** Anfang: Spieleinstellungen auswählen ***** //


BOOL CALLBACK SettingsProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
		if (lParam)
		{
			DPLCONNECTION* c=(DPLCONNECTION*)lParam;
			SendDlgItemMessage(hWnd,IDC_GAME,WM_SETTEXT,0,LPARAM(c->lpSessionDesc->lpszSessionNameA));
			EnableWindow(GetDlgItem(hWnd,IDC_GAME),FALSE);
		}else{
			CHAR c[200];
			CHAR name[200];
			GetWindowText(GetDlgItem(GetWindow(hWnd,GW_OWNER),IDC_NAME),&name[0],200);
			sprintf(&c[0],T_GAMENAME,&name[0]);
			SendDlgItemMessage(hWnd,IDC_GAME,WM_SETTEXT,0,LPARAM(&c[0]));
		}

		SendDlgItemMessage(hWnd,IDC_SPIN1,UDM_SETRANGE,0,MAKELONG(5000,0));
		SendDlgItemMessage(hWnd,IDC_SPIN4,UDM_SETRANGE,0,MAKELONG(5000,0));
		SendDlgItemMessage(hWnd,IDC_SPIN2,UDM_SETRANGE,0,MAKELONG(5000,0));
		SendDlgItemMessage(hWnd,IDC_SPIN3,UDM_SETRANGE,0,MAKELONG(500,0));
		SendDlgItemMessage(hWnd,IDC_SPIN1,UDM_SETPOS32,0,Config.chicken);
		SendDlgItemMessage(hWnd,IDC_SPIN4,UDM_SETPOS32,0,Config.farmer);
		SendDlgItemMessage(hWnd,IDC_SPIN2,UDM_SETPOS32,0,Config.trees);
		SendDlgItemMessage(hWnd,IDC_SPIN3,UDM_SETPOS32,0,Config.stall);

		CHAR c[10];
		sprintf(&c[0],"%d",Config.width);
		SendDlgItemMessage(hWnd,IDC_X,WM_SETTEXT,0,LPARAM(&c[0]));
		sprintf(&c[0],"%d",Config.height);

		SendDlgItemMessage(hWnd,IDC_Z,WM_SETTEXT,0,LPARAM(&c[0]));
		break;
	case WM_DESTROY:
		break;
	case WM_COMMAND:
		if (wParam==IDCANCEL)
			EndDialog(hWnd,0);
		if (wParam==IDOK)
		{
			int x=0,z=0;
			CHAR c[100];
			SendDlgItemMessage(hWnd,IDC_X,WM_GETTEXT,99,(LPARAM)&c[0]);
			x=atoi(&c[0]);
			SendDlgItemMessage(hWnd,IDC_Z,WM_GETTEXT,99,(LPARAM)&c[0]);
			z=atoi(&c[0]);

			if ((x<10)||(x>1000)||(z<10)||(z>1000))
			{
				MessageBox(hWnd,T_INVALIDSIZE,T_INVALIDSIZETITLE,MB_OK|MB_ICONEXCLAMATION);
				return TRUE;
			}
			Config.width=x;
			Config.height=z;

			int s=GetWindowTextLength(GetDlgItem(hWnd,IDC_GAME))+2;
			PCHAR p=(PCHAR)malloc(s);
			SendDlgItemMessage(hWnd,IDC_GAME,WM_GETTEXT,s+1,(LPARAM)p);

			Config.chicken=SendDlgItemMessage(hWnd,IDC_SPIN1,UDM_GETPOS32,0,0);
			Config.farmer=SendDlgItemMessage(hWnd,IDC_SPIN4,UDM_GETPOS32,0,0);
			Config.trees=SendDlgItemMessage(hWnd,IDC_SPIN2,UDM_GETPOS32,0,0);
			Config.stall=SendDlgItemMessage(hWnd,IDC_SPIN3,UDM_GETPOS32,0,0);

			EndDialog(hWnd,int(p));
		}
		break;
	default:return FALSE;
	}
	return TRUE;
}

// ***** ---------- ***** //


// ***** Anfang: Sitzung auswählen ***** //

void ClearList(HWND hWnd)
{
	int num=SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCOUNT,0,0);
	if (num==LB_ERR)return;
	for (int i=0;i<num;i++)
	{
		void* p=(void*)SendDlgItemMessage(hWnd,IDC_LIST,LB_GETITEMDATA,i,0);
		if (p)delete p;
	}

	SendDlgItemMessage(hWnd,IDC_LIST,LB_RESETCONTENT,0,0);
}

BOOL FAR PASCAL sessioncb(LPCDPSESSIONDESC2 lpThisSD,LPDWORD lpdwTimeOut,DWORD dwFlags,LPVOID lpContext)
{
	if( dwFlags & DPESC_TIMEDOUT )
		return FALSE; 
 
	const DPSESSIONDESC2 *desc=lpThisSD;
	if (desc==NULL)return FALSE;

	CHAR c[100];
	sprintf(&c[0],"%s (%d/%d)",desc->lpszSessionName,desc->dwCurrentPlayers,desc->dwMaxPlayers);
	DebugOut(&c[0]);

	int index;
	index=SendDlgItemMessage((HWND)lpContext,IDC_LIST,LB_FINDSTRING,0,LPARAM(&c[0]));

	if (index==LB_ERR)index=SendDlgItemMessage((HWND)lpContext,IDC_LIST,LB_ADDSTRING,0,LPARAM(&c[0]));

	GUID *session=(GUID*)new BYTE[sizeof(GUID)];
	memcpy(session,&desc->guidInstance,sizeof(GUID));
	SendDlgItemMessage((HWND)lpContext,IDC_LIST,LB_SETITEMDATA,index,(LPARAM)session);

	return TRUE;
}

static DPSESSIONDESC2 desc;

void RefreshList(HWND hWnd)
{
	// Daten löschen
	int num=SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCOUNT,0,0);
	if (num!=LB_ERR)
	{
		for (int i=0;i<num;i++)
		{
			void* p=(void*)SendDlgItemMessage(hWnd,IDC_LIST,LB_GETITEMDATA,i,0);
			if (p)delete p;
			SendDlgItemMessage(hWnd,IDC_LIST,LB_SETITEMDATA,i,0);
		}
	}


	HRESULT hr=0;
	ZeroMemory(&desc,sizeof(desc));
	desc.dwSize=sizeof(desc);
	desc.guidApplication=MYGUID;
//	desc.guidApplication=GUID_NULL;
//	ClearList(hWnd);

	if ((hr=game->lpDirectPlay->EnumSessions(&desc,0,sessioncb,hWnd,DPENUMSESSIONS_ALL|DPENUMSESSIONS_ASYNC))!=DP_OK)
	{
		ClearList(hWnd);
		CHAR c[100];
		sprintf(&c[0],T_ERRORENUMSESSIONS,hr);
		SendDlgItemMessage(hWnd,IDC_LIST,LB_ADDSTRING,0,LPARAM(&c[0]));
	}else
	{
		// Gelöschte Sessions entfernen
		int num=SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCOUNT,0,0);
		if (num!=LB_ERR)
		{
			int i=0;
			while (i<SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCOUNT,0,0))
			{
				void* p=(void*)SendDlgItemMessage(hWnd,IDC_LIST,LB_GETITEMDATA,i,0);
				if (p==NULL)
				{
					SendDlgItemMessage(hWnd,IDC_LIST,LB_DELETESTRING,i,0);
					i=0;
					continue;
				}
				i++;
			}
		}

		if (SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCOUNT,0,0)==0)
		{
			ClearList(hWnd);
			SendDlgItemMessage(hWnd,IDC_LIST,LB_ADDSTRING,0,LPARAM(T_NOSESSIONS));
			EnableWindow(GetDlgItem(hWnd,IDC_LIST),FALSE);
		}else 
		{
			EnableWindow(GetDlgItem(hWnd,IDC_LIST),TRUE);
//			SendDlgItemMessage(hWnd,IDC_LIST,LB_SETCURSEL,0,0);
		}
	}
}

void ApplyOptions(HWND hWnd)
{
	if (SendDlgItemMessage(hWnd,IDC_FULLSCREEN,BM_GETCHECK,0,0)==BST_CHECKED)
		Config.fullscreen=TRUE;else Config.fullscreen=FALSE;

	Config.Textures32Bit=(SendDlgItemMessage(hWnd,IDC_32BPPTEXTURES,BM_GETCHECK,0,0)==BST_CHECKED);
	Config.soundon=(SendDlgItemMessage(hWnd,IDC_SOUND,BM_GETCHECK,0,0)==BST_CHECKED);
	Config.Musik=(SendDlgItemMessage(hWnd,IDC_MUSIK,BM_GETCHECK,0,0)==BST_CHECKED);
	SetTextureBPP(Config.Textures32Bit?32:16);

	if (Config.fullscreen)
	{
		CHAR c[100];
		PCHAR w=&c[0];

		SendDlgItemMessage(hWnd,IDC_RESOLUTION,CB_GETLBTEXT,SendDlgItemMessage(hWnd,IDC_RESOLUTION,CB_GETCURSEL,0,0),(DWORD)&c[0]);

		PCHAR h=w;
		while (*h!='x')h++;
		*h='\0';
		h++;

		PCHAR bpp=h;
		while (*bpp!='x')bpp++;
		*bpp='\0';
		bpp++;

		Config.resx=atoi(w);
		Config.resy=atoi(h);
		Config.bpp=atoi(bpp);
	}
	game->devicenr=SendDlgItemMessage(hWnd,IDC_DEVICE,CB_GETCURSEL,0,0);

	Config.ShowIntro=(SendDlgItemMessage(hWnd,IDC_INTRO,BM_GETCHECK,0,0)==BST_CHECKED);
}

HRESULT WINAPI DisplayModesEnumCallback( LPDDSURFACEDESC2 sd,
                                          VOID* pParentInfo )
{
	int bpp=sd->ddpfPixelFormat.dwRGBBitCount;
	int width=sd->dwWidth,height=sd->dwHeight;
	HWND combo=(HWND)pParentInfo;

	if (bpp>8)
	{
		CHAR c[100];
		sprintf(&c[0],"%dx%dx%d",width,height,bpp);
		SendMessage(combo,CB_ADDSTRING,0,(DWORD)&c[0]);
	}

    return DDENUMRET_OK;
}

void GetDisplayModes(HWND hWnd)
{
	LPDIRECTDRAW7 lpDD;

	HWND combo=GetDlgItem(hWnd,IDC_RESOLUTION);
	if (FAILED(DirectDrawCreateEx(NULL,(void**)&lpDD,IID_IDirectDraw7,NULL)))return;

	lpDD->EnumDisplayModes(0,NULL,(LPVOID)combo,DisplayModesEnumCallback);

	lpDD->Release();

	int index=LB_ERR;

#define SetMode(w,h,bpp) if (index==LB_ERR) { CHAR c[100]; sprintf(&c[0],"%dx%dx%d",w,h,bpp); index=SendMessage(combo,CB_FINDSTRINGEXACT,0,DWORD(&c[0])); }

	SetMode(Config.resx,Config.resy,Config.bpp);
	SetMode(1024,768,32);
	SetMode(800,600,32);
	SetMode(640,480,32);
	SetMode(640,480,16);

	if (index!=LB_ERR)
		SendMessage(combo,CB_SETCURSEL,index,0);
}

HRESULT CALLBACK DeviceEnumCallback2(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc,LPVOID lpContext)
{
	HWND c=(HWND)lpContext;

	SendMessage(c,CB_ADDSTRING,0,(DWORD)lpDeviceName);

	return D3DENUMRET_OK;
}

void GetDevices(HWND hWnd)
{
	LPDIRECTDRAW7 lpDD;

	if (FAILED(DirectDrawCreateEx(NULL,(void**)&lpDD,IID_IDirectDraw7,NULL)))return;

	LPDIRECT3D7 lpD3D=NULL;
	if (SUCCEEDED(lpDD->QueryInterface(IID_IDirect3D7,(void**)&lpD3D)))
	{
		lpD3D->EnumDevices(DeviceEnumCallback2,(LPVOID)GetDlgItem(hWnd,IDC_DEVICE));

		lpD3D->Release();
	}

	lpDD->Release();

	SendMessage(GetDlgItem(hWnd,IDC_DEVICE),CB_SETCURSEL,SendMessage(GetDlgItem(hWnd,IDC_DEVICE),CB_GETCOUNT,0,0)-1,0);
}

DPLCONNECTION* LobbyConnectionSettings;

BOOL CALLBACK DlgProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch(uMsg)
	{
	case WM_INITDIALOG:
		{
			if (Config.spielertyp==2)
				SendDlgItemMessage(hWnd,IDC_RADIO2,BM_SETCHECK,1,0);
			else SendDlgItemMessage(hWnd,IDC_SPIELERTYP,BM_SETCHECK,1,0);


			SendDlgItemMessage(hWnd,IDC_NAME,WM_SETTEXT,0,LPARAM(&Config.Playername[0]));

			LobbyConnectionSettings=(DPLCONNECTION*)lParam;
			if (lParam)
			{
				SendDlgItemMessage(hWnd,IDC_NAME,WM_SETTEXT,0,(LPARAM)LobbyConnectionSettings->lpPlayerName->lpszLongNameA);
//				EnableWindow(GetDlgItem(hWnd,IDC_NAME),FALSE);
				EnableWindow(GetDlgItem(hWnd,IDC_REFRESH),FALSE);

				if (LobbyConnectionSettings->dwFlags==DPLCONNECTION_CREATESESSION)
					EnableWindow(GetDlgItem(hWnd,IDC_JOIN),FALSE);
				else EnableWindow(GetDlgItem(hWnd,IDC_CREATE),FALSE);
			}else
			{
				SendDlgItemMessage(hWnd,IDC_LIST,LB_ADDSTRING,0,(LPARAM)T_REFRESH);

				if (game->lpDirectPlay==NULL)
				{
					EnableWindow(GetDlgItem(hWnd,IDC_REFRESH),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_JOIN),FALSE);
					EnableWindow(GetDlgItem(hWnd,IDC_LIST),FALSE);
				}
			}

#ifndef _DEBUG
			SendDlgItemMessage(hWnd,IDC_FULLSCREEN,BM_SETCHECK,Config.fullscreen,0);
//			SendDlgItemMessage(hWnd,IDC_INTRO,BM_SETCHECK,1,0);
			SendDlgItemMessage(hWnd,IDC_INTRO,BM_SETCHECK,Config.ShowIntro,0);
#else
			SendDlgItemMessage(hWnd,IDC_FULLSCREEN,BM_SETCHECK,Config.fullscreen,0);
			SendDlgItemMessage(hWnd,IDC_INTRO,BM_SETCHECK,Config.ShowIntro,0);
#endif
			EnableWindow(GetDlgItem(hWnd,IDC_RESOLUTION),Config.fullscreen);
			SendDlgItemMessage(hWnd,IDC_32BPPTEXTURES,BM_SETCHECK,Config.Textures32Bit,0);

			SendDlgItemMessage(hWnd,IDC_SOUND,BM_SETCHECK,Config.soundon,0);
			SendDlgItemMessage(hWnd,IDC_MUSIK,BM_SETCHECK,Config.Musik,0);

			GetDisplayModes(hWnd);
			GetDevices(hWnd);

			if (GetFileAttributes(IntroFile)==INVALID_FILE_ATTRIBUTES)
				EnableWindow(GetDlgItem(hWnd,IDC_INTRO),FALSE);

			EnableWindow(GetDlgItem(hWnd,IDC_LIST),FALSE);
		}
		break;
	case WM_DESTROY:
		KillTimer(hWnd,5);
		ClearList(hWnd);
		break;
	case WM_COMMAND:
		if (HIWORD(wParam)==LBN_DBLCLK)PostMessage(hWnd,WM_COMMAND,IDC_JOIN,0);
		if (wParam==IDC_CREATE)
		{
			int r=DialogBoxParam(GetDllInstance(),MAKEINTRESOURCE(IDD_GAMESETTINGS),hWnd,SettingsProc,(UINT)LobbyConnectionSettings);
			if (r!=0)
			{	// Spiel erstellen
				if (LobbyConnectionSettings)
				{
					goto createplayer1;
				}
				if (game->lpDirectPlay==NULL)
				{
					int s=GetWindowTextLength(GetDlgItem(hWnd,IDC_NAME))+2;
					PCHAR n=(PCHAR)malloc(s);
					GetWindowText(GetDlgItem(hWnd,IDC_NAME),n,s);
					strcpy(&Config.Playername[0],n);
					
					LocalPlayerID=0;
					ApplyOptions(hWnd);
					EndDialog(hWnd,MAKEWORD(1,SendDlgItemMessage(hWnd,IDC_SPIELERTYP,BM_GETCHECK,0,0)));

					break;
				}


				DPSESSIONDESC2 desc;
				ZeroMemory(&desc,sizeof(desc));
				desc.dwSize=sizeof(desc);
				desc.lpszSessionNameA=PCHAR(r);
				desc.guidApplication=MYGUID;
				desc.dwMaxPlayers=32;
				desc.dwCurrentPlayers=0;
				desc.dwFlags=DPSESSION_KEEPALIVE|DPSESSION_MIGRATEHOST|(bUseDirectPlayProtocol?DPSESSION_DIRECTPLAYPROTOCOL:0);

				HRESULT hr;
				if ((hr=game->lpDirectPlay->Open(&desc,DPOPEN_CREATE))!=DP_OK)
				{
					MessageBox(hWnd,E_CREATEGAME,E_TITLE,MB_OK|MB_ICONHAND);
				}else{
createplayer1:
					DPID player;
					DPNAME name;
					name.dwFlags=0;
					name.dwSize=sizeof(name);
					int s=GetWindowTextLength(GetDlgItem(hWnd,IDC_NAME))+2;
					PCHAR n=(PCHAR)malloc(s);
					GetWindowText(GetDlgItem(hWnd,IDC_NAME),n,s);
						
					name.lpszLongNameA=name.lpszShortNameA=n;
		
					if (FAILED(game->lpDirectPlay->CreatePlayer(&player,&name,0,0,0,DPPLAYER_SERVERPLAYER)))
					{
						game->lpDirectPlay->Close();
						MessageBox(hWnd,E_CREATEPLAYER,E_TITLE,MB_OK|MB_ICONHAND);
					}else{	// Alles fertig! Können loslegen.
						DebugOut("Multiplayer initialized successfully! Starting Game.");
						strcpy(&Config.Playername[0],n);
					
						LocalPlayerID=player;
						ApplyOptions(hWnd);
						EndDialog(hWnd,MAKEWORD(1,SendDlgItemMessage(hWnd,IDC_SPIELERTYP,BM_GETCHECK,0,0)));
					}
					free(n);
				}
				free(PCHAR(r));
			}
		}
		if (wParam==IDC_JOIN)
		{		// Spiel beitreten
			LPVOID* p=(LPVOID*)SendDlgItemMessage(hWnd,IDC_LIST,LB_GETITEMDATA,SendDlgItemMessage(hWnd,IDC_LIST,LB_GETCURSEL,0,0),0);
			if (LobbyConnectionSettings)
			{
				goto createplayer2;
			}
			if ((p==NULL)||(DWORD(p)==LB_ERR))
			{
//				RefreshList(hWnd);
				DebugOut("Error joining game!");
			}else{	// Versuchen, Spiel zu joinen
				DPSESSIONDESC2 desc;
				ZeroMemory(&desc,sizeof(desc));
				desc.dwSize=sizeof(desc);
				desc.guidApplication=MYGUID;
				desc.guidInstance=*(GUID*)p;
				desc.dwFlags=(bUseDirectPlayProtocol?DPSESSION_DIRECTPLAYPROTOCOL:0);


				HRESULT hr;
				if ((hr=game->lpDirectPlay->Open(&desc,DPOPEN_JOIN))!=DP_OK)
				{
					MessageBox(hWnd,E_JOINGAME,E_TITLE,MB_OK|MB_ICONHAND);
				}else{
createplayer2:
					int s=GetWindowTextLength(GetDlgItem(hWnd,IDC_NAME))+2;
					PCHAR n=(PCHAR)malloc(s);
					GetWindowText(GetDlgItem(hWnd,IDC_NAME),n,s);
					strcpy(&Config.Playername[0],n);
					free(n);

					ApplyOptions(hWnd);
					EndDialog(hWnd,MAKEWORD(2,SendDlgItemMessage(hWnd,IDC_SPIELERTYP,BM_GETCHECK,0,0)));
				}
			}
		}
		if (wParam==IDC_REFRESH)
		{
			if (SendDlgItemMessage(hWnd,IDC_REFRESH,BM_GETCHECK,0,0)!=BST_CHECKED)
			{
				ZeroMemory(&desc,sizeof(desc));
				desc.dwSize=sizeof(desc);
				desc.guidApplication=MYGUID;
//				desc.guidApplication=GUID_NULL;
				KillTimer(hWnd,5);
				game->lpDirectPlay->EnumSessions(&desc,0,sessioncb,hWnd,DPENUMSESSIONS_STOPASYNC);
			}else{
				ClearList(hWnd);
				RefreshList(hWnd);
				SetTimer(hWnd,5,1000,NULL);
			}
		}

		if (wParam==IDC_FULLSCREEN)
		{
			EnableWindow(GetDlgItem(hWnd,IDC_RESOLUTION),(BOOL)SendDlgItemMessage(hWnd,IDC_FULLSCREEN,BM_GETCHECK,0,0));
		}

		if (wParam==IDCANCEL)EndDialog(hWnd,-1);
		break;

	case WM_TIMER:
		RefreshList(hWnd);
		break;

	default:return FALSE;
	}
	return TRUE;
}

// ***** ---------- ***** //




const int CGame::InitMultiplayer()
{
	DPLCONNECTION* g_pDPLConnection=NULL;

	{
		// Check for Lobby-Launch
		if( FAILED(  CoCreateInstance( CLSID_DirectPlayLobby, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IDirectPlayLobby3A,
                                       (VOID**)&lpDirectPlayLobby) ) ) 
		{
			MessageBox(wnd,"Could not create IDirectPlayLobby3A interface.",E_TITLE,MB_OK|MB_ICONHAND);
			return 0;
		}

		HRESULT hr;
		DWORD dwSize=0;
		hr=lpDirectPlayLobby->GetConnectionSettings( 0, NULL, &dwSize );
		if( FAILED(hr) && (DPERR_BUFFERTOOSMALL != hr) )
		{
	        if( DPERR_NOTLOBBIED == hr )
			{
				SaveRelease(lpDirectPlayLobby);
		
				if (FAILED(CoCreateInstance(CLSID_DirectPlay,NULL,CLSCTX_INPROC_SERVER,IID_IDirectPlay4A,(void**)&lpDirectPlay)))return 0;
				goto ui;
			}
			MessageBox(wnd,"Strange error occured while querying lobby connection settings.",E_TITLE,MB_OK|MB_ICONHAND);
			return 0; 
		}
	    // Allocate memory for the connection
		
		g_pDPLConnection = (DPLCONNECTION*)new BYTE[ dwSize ];

		// Get the connection settings
	    if( FAILED( hr = lpDirectPlayLobby->GetConnectionSettings( 0, g_pDPLConnection, 
                                                        &dwSize ) ) ) goto error;

		g_pDPLConnection->lpSessionDesc->dwFlags=(DPSESSION_KEEPALIVE|DPSESSION_MIGRATEHOST);
		lpDirectPlayLobby->SetConnectionSettings(0,0,g_pDPLConnection);


		// über Lobby gestartet

		// kleines Delay einbauen
		if (g_pDPLConnection->dwFlags==DPLCONNECTION_JOINSESSION)
			Sleep(1000);
		if (FAILED(lpDirectPlayLobby->ConnectEx(0,IID_IDirectPlay4A,(void**)&lpDirectPlay,0)))
		{
			MessageBox(wnd,"Could not establish connection through Lobby!",E_TITLE,MB_OK|MB_ICONHAND);
			goto error;
		}
	}

ui:
	int r;
	if (lpDirectPlayLobby==NULL)
	{
		if ((r=DialogBox(GetDllInstance(),MAKEINTRESOURCE(IDD_CONNECTION),0,ConnectionProc))==0)return 0;
		if (r==-1)
		{
			goto error;
		}

		if (r==1)
		{
			SaveRelease(lpDirectPlay);
		}else{
			if (FAILED(lpDirectPlay->InitializeConnection((void*)(r),0)))goto error;
			delete (void*)r;
		}
	}

	r=DialogBoxParam(GetDllInstance(),MAKEINTRESOURCE(IDD_GAMELIST),0,DlgProc,(UINT)g_pDPLConnection);

	if (r==-1)goto error;
	MultiplayerCreated=TRUE;

	switch(LOBYTE(r))
	{
	case 1:
		{	// Spiel erstellt
			IsServer=TRUE;
		}
		break;
	case 2:
		{	// Spiel beigetreten
			IsServer=FALSE;
		}
		break;
	}

	if (HIBYTE(r)==1)Config.spielertyp=1;	// Bauer
	else Config.spielertyp=2;	// Huhn

	return Config.spielertyp;


error:
	if (g_pDPLConnection)delete[] g_pDPLConnection;
	return 0;
}