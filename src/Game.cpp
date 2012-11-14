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
#include "chicken.h"
#include "ui.h"
#include "bauer.h"
#include "gartenkralle.h"
#include "drescher.h"
#include "golfer.h"
#include "console.h"
#include "multiplayer.h"
#include "tree.h"
#include "lensflare.h"
#include "music.h"
#include "resources\\resources.h"
#include "config.h"
#include "chickenplayer.h"
#include "helperlib\\videos.h"
#include "stall.h"
#include "shlobj.h"
#include "text.h"
#include "plasmacannon.h"
#include "items.h"
#include "shadows.h"
#include "camera.h"
#include "computerbauer.h"



CGame* game=NULL;
CWorld* world=NULL;
CPlayer* player=NULL;
HINSTANCE hInstance;




CGame::CGame()
:errormessage(""),wnd(0),width(0),height(0),devicenr(-1),lpDD(NULL),lpDDSBack(NULL),
 lpDDSPrimary(NULL),lpClip(NULL),lpD3D(NULL),lpDevice(NULL),lpDDSZBuf(NULL),chain(NULL),
 sounds(NULL),lpDI(NULL),lpKeyboard(NULL),lpMouse(NULL),lpDirectPlay(NULL),
 lpDirectPlayLobby(NULL),MultiplayerCreated(FALSE),IsServer(FALSE),ticker(NULL),menu(NULL),
 wireframe(FALSE),UTSoundsLoaded(FALSE),canshadows(TRUE),fps(NULL),console(NULL),lpChat(NULL),
 lpD3DFont(NULL),gimmick(NULL),splash(0),pi(NULL),created(FALSE),announcer(NULL),transition(NULL),
 lpMusic(NULL),lpDS(NULL),lpDirectSoundListener(NULL),lpPrimaryBuffer(NULL),models(NULL),

 gorelevel(1.0f),aircontrol(1.4f),jumpspeed(1.5f),sittime(10.0f),throwtime(0.3f),
 replenishpower(2.0f),takepower(2.25f),damage(10.0f),plasmaregeneration(0.13f),plasmaradius(2.6f),
 plasmarate(1.9f),gravity(10.0f),PlasmaBallPressure(TRUE),respawntime(2.0f),harvesterspeed(1.0f),harvesterrotspeed(1.0f)
{
	int i;
	game=this;
	player=NULL;
	LocalPlayerID=0;

	MouseButton[0]=MouseButton[1]=FALSE;

	for (i=0;i<(sizeof(lpBuffers)/sizeof(lpBuffers[0]));i++)
		lpBuffers[i]=NULL;
	for (i=0;i<(sizeof(lpTexture)/sizeof(lpTexture[0]));i++)
		lpTexture[i]=NULL;
	for (i=0;i<(sizeof(lpSprites)/sizeof(lpSprites[0]));i++)
		lpSprites[i]=NULL;
}

CGame::~CGame()
{
	SaveConfig();
	CleanUp();
	game=NULL;
	player=NULL;
}

const BOOLEAN CGame::InitGame(const int playertype)
{
	errormessage=E_INIT;

	{	// Models
		errormessage=E_MODELS;
		models=new CD3DFile();
		if (FAILED(models->Load("Models.x",GetDllInstance())))
		{
			delete models;
			models=NULL;
			return FALSE;
		}
		DrawLoading(LOADED_MODELS-1,80);
		D3DTextr_RestoreAllTextures(lpDevice);

		DrawLoading(LOADED_MODELS);
	}

	chain=new CObjChain;
	sounds=new CObjChain;
	gimmick=new CObjChain;

	chain->Add(world=new CWorld());
	AddGimmick(console=new CConsole);
	AddGimmick(fps=new CFPS());
	if (IsServer)
	{
		int i;
		const int max=Config.chicken+Config.stall+Config.trees+5+4;
		player=NULL;
		if (playertype==1)chain->Add(player=new CBauer());
		if (playertype==2)chain->Add(player=new CChickenPlayer());

		if (lpDirectPlay)lpDirectPlay->SetPlayerData(LocalPlayerID,&player->id,sizeof(UID),DPSET_REMOTE);
		for (i=0;i<Config.chicken;i++)	// Hühner erstellen
		{
			chain->Add(new CChicken());
			if (i%50==0)
				DrawLoading(LOADED_OBJECTS-1,i*100/max);
		}

		for (i=0;i<Config.farmer;i++)	// Computerbauer erstellen
			chain->Add(new CComputerBauer);


		DrawLoading(LOADED_OBJECTS-1,Config.chicken*100/max);

		for (i=0;i<Config.stall;i++)	// Ställe erstellen
			chain->Add(new CStall);

		for (i=0;i<Config.trees;i++)	// Ein paar Bäume pflanzen
			chain->Add(new CTree(world->RandX(1.0f),world->RandY(1.0f),randf()*g_PI*2));

		DrawLoading(LOADED_OBJECTS-1,(Config.chicken+Config.stall+Config.trees)*100/max);

		// Eins von jeder Waffenart
		chain->Add(new CGartenkralle);
		chain->Add(new CDrescher);
		chain->Add(new CGolfer);
		chain->Add(new CPlasmaCannon);

	}
	AddGimmick(new CItemSpawner);
	AddGimmick(ticker=new CTicker);
	AddGimmick(pi=new CPlayerInfo);
	AddGimmick(lpChat=new CChatWindow);

	DrawLoading(LOADED_OBJECTS-1,100);

	CBillboardText* t=new CBillboardText;
	t->SetText(T_LOSGEHTS,RGB(255,0,0));
	AddGimmick(t);

	DrawLoading(LOADED_OBJECTS);
	return TRUE;
}

void CGame::CleanUp()
{
	int i;
	if (lpMusic)delete lpMusic;

	if (menu)delete menu;
	if (chain)delete chain;
	if (gimmick)delete gimmick;
	if (sounds)delete sounds;
	if (transition)delete transition;

	for (i=0;i<(sizeof(lpSprites)/sizeof(lpSprites[0]));i++)
		SaveDelete(lpSprites[i]);

	D3DTextr_InvalidateAllTextures();
	if (models)delete models;

	created=FALSE;

	if (Config.fullscreen)
		if (lpDD)lpDD->RestoreDisplayMode();

	for (i=0;i<(sizeof(lpTexture)/sizeof(lpTexture[0]));i++)
		SaveRelease(lpTexture[i]);

	SaveDelete(lpD3DFont);

	for (i=0;i<(sizeof(lpBuffers)/sizeof(lpBuffers[0]));i++)
		SaveRelease(lpBuffers[i]);


	if (LocalPlayerID!=0)lpDirectPlay->DestroyPlayer(LocalPlayerID);

	if (MultiplayerCreated)if (lpDirectPlay)lpDirectPlay->Close();

	SaveRelease(lpDirectPlayLobby);
	SaveRelease(lpDirectPlay);
	DoneMultiplayer();
	SaveRelease(lpDS);


	if (lpKeyboard)
	{
		lpKeyboard->Unacquire();
		lpKeyboard->Release();
		lpKeyboard=NULL;
	}
	if (lpMouse)
	{
		lpMouse->Unacquire();
		lpMouse->Release();
		lpMouse=NULL;
	}
	SaveRelease(lpDI);


	if (lpDDSZBuf&&lpDDSBack)
	{
		lpDDSBack->DeleteAttachedSurface(0,lpDDSZBuf);
		SaveRelease(lpDDSZBuf);
	}
	SaveRelease(lpDevice);
	D3DXUninitialize();
	SaveRelease(lpD3D);

	SaveRelease(lpDDSBack);
	SaveRelease(lpDDSPrimary);
	SaveRelease(lpClip);
	SaveRelease(lpDD);

	ShowCursor(TRUE);
	if (wnd)
	{
//		ShowWindow(wnd,SW_HIDE);
		DestroyWindow(wnd);
	}
}

const BOOLEAN CGame::Run()
{
	D3DXInitialize();

	BOOLEAN soundon;
	if (LoadConfig()==FALSE)
	{	// Config nicht geladen

	}
#ifdef _DEBUG
//	Config.fullscreen=FALSE;
#endif

	if (!MapStrings())
	{
		if (MessageBox(0,E_LANGUAGE,E_TITLE,MB_YESNO|MB_ICONQUESTION)==IDNO)
			return TRUE;
	}

	if (!CheckDirectX())
	{
		errormessage="You need at least DirectX 7 to run this game.";
		return FALSE;
	}


	const int i=InitMultiplayer();
	if (i==0)return TRUE;

	{	// Initialize Game
		if (!InitWindow())return FALSE;
		ShowCursor(FALSE);

		errormessage=E_INTRO;

		// Bereit, jetzt Intro zeigen
		if (Config.ShowIntro)
		{
			if (!ShowVideo(hInstance,wnd,Config.fullscreen,IntroFile))return TRUE;
		}
		// Intro gezeigt
		if (!InitDirectDraw())return FALSE;
		UpdateWindowRect();

		splash=LoadPicture("splash.jpg");

		DrawLoading(0);


		soundon=Config.soundon;
		Config.soundon=TRUE;

		// Weiter laden
		if (!InitDirect3D())return FALSE;
		if (!InitDirectSound())
		{

		}

		if (!InitDirectInput())return FALSE;
		if (!InitTimer())return FALSE;
		if (!LoadTextures(FALSE))return FALSE;
		if (!Init3DSettings())return FALSE;
		if (!InitGame(i))return FALSE;

		if (InitMusic(Config.Musik)==FALSE)
		{
			DebugOut(E_MUSIC);
		}

		DrawLoading(LOADED_MUSIC);
		if (!IsServer)
		{
			if (!SynchronizeWorld(i))return FALSE;
		}
		DrawLoading(LOADED_NETWORK);

		// Jetzt sounds starten
		player->SetSoundListener();
		Config.soundon=soundon;
		CObject* o=sounds->GetFirst();
		while (o)
		{
			((CSound*)o)->Toggle(Config.soundon);
			o=o->next;
		}
		// Das wars, Spiels starten
		SetTransition(new CFadeTransition(FALSE,D3DRGB(1,1,1),lpTexture[20],1.5f,1));

		if (splash)DeleteObject(splash);
		splash=0;
		created=TRUE;
		UpdateWindowRect();
	}

	const HACCEL acc=LoadAccelerators(hInstance,MAKEINTRESOURCE(IDR_MAINFRAME));

	MSG msg;
	LONGLONG cur_time;
	double time_elapsed;
	if (perf_flag)
		QueryPerformanceCounter((LARGE_INTEGER*)&cur_time);
	else
		cur_time=timeGetTime();
	last_time=cur_time;

	DebugOut("----- LOS GEHTS!!! -----");

	PeekMessage(&msg,NULL,0,0,PM_NOREMOVE);

	fps->SetStartTime(2);
	while (msg.message!=WM_QUIT)
	{
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		{
			if (!TranslateAccelerator(msg.hwnd,acc,&msg))
			{
				if ((msg.message==WM_KEYDOWN)||(msg.message==WM_KEYUP))
				{
					// Dead Char Nachrichten dem Übersetzer vorenthalten
					if ((msg.wParam!=VK_OEM_5)&&(msg.wParam!=VK_OEM_6))
						TranslateMessage(&msg);
				}
				DispatchMessage(&msg);
			}
		}else{
			if (perf_flag)
				QueryPerformanceCounter((LARGE_INTEGER*)&cur_time);
			else
				cur_time=timeGetTime();

 			{	// Objekte bewegen, und Frame rendern
				time_elapsed=(cur_time-last_time)*time_scale;
				last_time=cur_time;

				fps->SetEndTime(2);
				fps->SetStartTime(2);


				fps->SetStartTime(0);
				BOOLEAN b=Draw();
				if (b)Flip();
				fps->SetEndTime(0);

				HandleInput(float(time_elapsed));
				Execute(float(time_elapsed));
			}
			if (IsIconic(wnd))Sleep(10);
#ifdef _DEBUG
			Sleep(0);
#endif
		}
	}

	ShowCursor(TRUE);
	return (msg.wParam==0);
}

const BOOLEAN CGame::Draw()
{
	if (IsIconic(wnd))return FALSE;
	if (lpDDSPrimary->IsLost()==DDERR_SURFACELOST)
	{
		DebugOut("IsLost!");
		if (FAILED(lpDDSPrimary->Restore()))return FALSE;
		if (FAILED(lpDD->RestoreAllSurfaces()))
		{
			DebugOut("Nicht alle Oberflächen konnten wiederhergestellt werden!!");
		}
		D3DTextr_RestoreAllTextures(lpDevice);
		if (LoadTextures(TRUE)==FALSE)
		{
			ticker->Add(E_RESTORETEXTURES,D3DRGB(1,0,0));
			DebugOut("Texturen NICHT KORREKT wiederhergestellt!!!");
		}else DebugOut("Texturen wiederhergestellt.");

		chain->Restore();
		sounds->Restore();
		gimmick->Restore();
		if (menu)
		{
			menu->InvalidateAll();
			menu->SetCursor();
		}
	}

	if (menu)
	{	// Nur Menü Rendern und FPS
		menu->Blt(lpDDSBack);
		if (fps)
		{
			if (SUCCEEDED(lpDevice->BeginScene()))
			{
				fps->DrawUI(lpDevice);
				lpDevice->EndScene();
			}
		}
		return TRUE;
	}

	lpDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER|(wireframe?D3DCLEAR_TARGET:0)|(canshadows?D3DCLEAR_STENCIL:0),0,1.0f,0);

	if (SUCCEEDED(lpDevice->BeginScene()))
	{
		D3DMATRIX world,i;

		player->GetViewMatrix(world);
		Camera.SetViewMatrix(world);
		Camera.OptimizeCamera();
		//CCamera::OptimizeCamera(&world);

		const D3DVECTOR playerpos=Camera.GetCameraPos();

		lpDevice->SetTransform(D3DTRANSFORMSTATE_VIEW,Camera.GetViewMatrix());
		D3DUtil_SetIdentityMatrix(i);
		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&i);

//		Erst Himmel und Sonne rendern
		::world->DrawSky(lpDevice,playerpos);
		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&i);
		RenderSun(playerpos);

//		Feste Objekte rendern
		chain->Draw(lpDevice);
		gimmick->Draw(lpDevice);
//		Alphablending oder UI Objekte rendern
		chain->DrawUI(lpDevice);
		gimmick->DrawUI(lpDevice);

#if (SHADOWS)
		if ((canshadows))
		{
			// Schatten in Stencil rendern
			chain->DrawShadow(lpDevice);
			gimmick->DrawShadow(lpDevice);

			// Gerenderte Schatten sichtbar machen
			DrawShadow(lpDevice);
		}
#endif

//		Lensflare rendern
		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&i);
		LensFlare.Render(playerpos);

		if (playerpos.y<0.0f)
			::world->DrawUnderwater(lpDevice);

		// Überblendung rendern, wenn nötig
		if (transition)
			transition->Draw(lpDevice);

		lpDevice->EndScene();
	}
	return TRUE;
}

void CGame::Execute(const float elapsed)
{
	fps->SetStartTime(1);

	if (menu)
	{
		menu->Execute(elapsed);
		if (menu->closed)
		{
			delete menu;
			menu=NULL;
		}
	}

	chain->Execute(elapsed);
	sounds->Execute(elapsed);
	gimmick->Execute(elapsed);

	LensFlare.Execute(elapsed);

	if (transition)
	{
		transition->Execute(elapsed);
		if (transition->IsOver())
		{
			delete transition;
			transition=NULL;
		}
	}


	if ((Config.soundon)&&(!menu))player->SetSoundListener();

	// Netzwekdaten verarbeiten
	PNETWORKDATA p=PollNetwork();
	while (p)
	{
		DispatchNetworkMessage(p);
		p=PollNetwork();
	}

	fps->SetEndTime(1);
}

const LRESULT CGame::WindowProc(const HWND hWnd,const UINT uMsg,const WPARAM wParam,const LPARAM lParam)
{
	// Superwichtige Nachrichten verarbetien
	switch (uMsg)
	{
	case WM_SIZE:
	case WM_MOVE:
		if (created)UpdateWindowRect();
		break;
	case WM_KEYUP:
		if (wParam==VK_SNAPSHOT)
		{
			SaveScreenShot();
			return 0;
		}
		break;
	case WM_PAINT:if (created)
		{
			Draw();
		}
		break;
	case WM_SYSKEYDOWN:
		if ((wParam==VK_RETURN)&&((lParam&(1<<29))>0))
			ChangeResolution(0,0,0,!Config.fullscreen);
		break;
	}

	// Wenn Menü, dann Nachrichten nur von Menü verarbeiten
	if (created) if ((menu)&&(menu->PerformMessage(hWnd,uMsg,wParam,lParam)))return 0;

	switch (uMsg)
	{
	case WM_DESTROY:
		if (created)PostQuitMessage(0);
		break;
	case WM_CHAR:
		if (!created)return 0;
		if ((console!=NULL)&&(console->IsVisible()))console->WMCHAR((WORD)wParam);
		if ((lpChat!=NULL)&&(lpChat->IsVisible()))lpChat->WMCHAR((WORD)wParam);
		break;
	case WM_KEYDOWN:
		if (!created)return 0;
		if (menu)return 0;
		if ((console!=NULL)&&(console->IsVisible()))
		{
			console->WMKEYDOWN(wParam);
			return 0;
		}
		if ((lpChat!=NULL)&&(lpChat->IsVisible()))
		{
			lpChat->WMKEYDOWN(wParam);
			return 0;
		}
			
		{
			if ((wParam==VK_TAB)&&(pi!=NULL))
			{
				if (pi->visible==FALSE)
				{
					pi->visible=TRUE;
					pi->Refresh();
				}
			}
			if (wParam==VK_ESCAPE)if (menu==NULL)
			{
				menu=new CMainPage();
				menu->Create();
			}
			if (wParam==VK_F5)ToggleSound();
			if (wParam==VK_F6)
			{
				lpMusic->ToggleMusic();
			}
			if (wParam==VK_F4)
			{
				DWORD s;
				lpDevice->GetRenderState(D3DRENDERSTATE_FILLMODE,&s);
				if (s==D3DFILL_WIREFRAME)
				{
					lpDevice->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_POINT);
					lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_FALSE);
					lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
					ticker->Add(T_WIREFRAME1);
					wireframe=TRUE;
				}else if (s==D3DFILL_POINT)
				{
					lpDevice->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_SOLID);
					lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_TRUE);
					lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
					ticker->Add(T_WIREFRAME2);
					wireframe=FALSE;
				}else{
					lpDevice->SetRenderState(D3DRENDERSTATE_FILLMODE,D3DFILL_WIREFRAME);
					lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_FALSE);
					lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
					ticker->Add(T_WIREFRAME3);
					wireframe=TRUE;
				}
			}
			if (wParam==VK_F8)fps->full=!fps->full;

			if (wParam=='R')
			{
//				Config.reversemouse=!Config.reversemouse;
//				ticker->Add(Config.reversemouse?T_MOUSEREVERSED:T_MOUSENORMAL,D3DRGB(1,1,0));
				player->SwitchRadar();
			}
			if (wParam=='C')player->NextCam();
			if (wParam=='H')
			{
				if (GetType(player->id)==IDBauer)
					((CBauer*)player)->NextHut();

			}
			if (wParam==VK_OEM_5)
			{
				console->Toggle();
			}
			if (wParam==VK_BACK)
				if (lpChat)lpChat->Toggle();
			if (wParam==VK_RETURN)
			{
				if (player->IsType(IDBauer))
					((CBauer*)player)->GetOut();
			}
		}
		break;
	case WM_KEYUP:
		if (!created)return 0;
		if ((console!=NULL)&&(console->IsVisible()))	;else
		{
			if (wParam==VK_TAB)pi->visible=FALSE;
		}
		break;
	case WM_ENTERSIZEMOVE:
	case WM_ENTERMENULOOP:
		if (!menu)
		{
			ShowCursor(TRUE);
			if (lpMouse)lpMouse->Unacquire();
			if (lpKeyboard)lpKeyboard->Unacquire();
		}
		break;
	case WM_EXITSIZEMOVE:
	case WM_EXITMENULOOP:
		if (!menu)
		{
			ShowCursor(FALSE);
			if (lpMouse)lpMouse->Acquire();
			if (lpKeyboard)lpKeyboard->Acquire();
		}
		break;
	default:
		return DefWindowProc(hWnd,uMsg,wParam,lParam);
	}
	return 0;
}

void CGame::UpdateWindowRect()
{
	GetClientRect(wnd,&clientrect);
	width=clientrect.right-clientrect.left;
	height=clientrect.bottom-clientrect.top;
 	ClientToScreen(wnd,&clientrect);

	if ((lpDDSBack)&&(!Config.fullscreen))
	{
		LPDIRECTDRAWSURFACE7 n=NULL;
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd,sizeof(ddsd));
		ddsd.dwSize=sizeof(ddsd);
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth        = width;
		ddsd.dwHeight       = height;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		lpDD->CreateSurface( &ddsd, &n, NULL );

		if (n!=NULL)
		{
			if (lpDDSZBuf&&lpDDSBack)
			{
				lpDDSBack->DeleteAttachedSurface(0,lpDDSZBuf);
				lpDDSZBuf->Release();
				lpDDSZBuf=NULL;
			}
			lpDDSBack->Release();
			lpDDSBack=n;
			if (lpDevice)if (InitZBuffer(d3ddesc.deviceGUID)==FALSE)
			{
				DebugOut(errormessage);
			}
			if (lpDevice)lpDevice->SetRenderTarget(lpDDSBack,0);
		}
	}
	if (lpDevice)
	{
		if (width==0)return;
		if (height==0)return;
		D3DVIEWPORT7 view;
		view.dwX=view.dwY=0;
		view.dwWidth=width;
		view.dwHeight=height;
		view.dvMinZ=0.0f;
		view.dvMaxZ=1.0f;
		lpDevice->SetViewport(&view);
	}
}

void CGame::HandleInput(const float elapsed)
{
	if (IsIconic(wnd))return;
	if (menu)return;
	HRESULT hr;
               
	if ((hr=lpKeyboard->GetDeviceState(sizeof(buf),(LPVOID)&buf))!= DI_OK)
    {
		if (lpKeyboard->Acquire() == DI_OK)
			lpKeyboard->GetDeviceState(sizeof(buf),(LPVOID)&buf);
		else ZeroMemory(buf,256);
	}

	DIDEVICEOBJECTDATA data;
	memset(&data,0,sizeof(DIDEVICEOBJECTDATA));
	DWORD dwNumElements=1;
	int iDX=0,iDY=0,iDZ=0;
	static int oldDX=0,oldDY=0;

//	if (pMouse->Acquire()==DI_OK)

	do
	{
		if (lpMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&data,&dwNumElements,0)!=DI_OK)
		{
			if (lpMouse->Acquire()==DI_OK)
				hr=lpMouse->GetDeviceData(sizeof(DIDEVICEOBJECTDATA),&data,&dwNumElements,0);
			else break;
		}

		switch(data.dwOfs)
		{
		case DIMOFS_X:
			iDX+=data.dwData;
			break;
		case DIMOFS_Y:
			iDY+=data.dwData;
			break;
		case DIMOFS_Z:
			iDZ+=data.dwData;
			break;
		case DIMOFS_BUTTON0:
			if (data.dwData&0x80)
				MouseButton[0]=TRUE; else MouseButton[0]=FALSE;
			break;
		case DIMOFS_BUTTON1:
			if (data.dwData&0x80)
				MouseButton[1]=TRUE; else MouseButton[1]=FALSE;
			break;
		}
	}while (dwNumElements!=0);

	if (Config.smoothmouse)
	{
		int tx=iDX,ty=iDY;
		iDX+=oldDX/2;
		iDY+=oldDY/2;
		oldDX=tx;
		oldDY=ty;
	}

	if (menu)return;

	if ((console->IsVisible())||(lpChat->IsVisible()))
	{
		ZeroMemory(buf,sizeof(buf));
		MouseButton[0]=MouseButton[1]=FALSE;

		if (player)player->HandleInput(elapsed,0,0,0);
	}else if (player)player->HandleInput(elapsed,iDX,iDY,iDZ);
}

void CGame::Flip()
{
	if (Config.fullscreen)
	{
		lpDDSPrimary->Flip(0,(Config.vsync?0:DDFLIP_NOVSYNC));
	}else{
		RECT r2={0,0,width,height};
        lpDDSPrimary->Blt(&clientrect,lpDDSBack,&r2,0,NULL);
	}
}

void CGame::DrawLoading(const int abschnitt,const int prozent)
{
	HDC dc;
	lpDDSBack->GetDC(&dc);
	if (splash!=0)
	{
		const HDC memdc=CreateCompatibleDC(dc);
		const HBITMAP old=(HBITMAP)SelectObject(memdc,splash);
		BITMAP bitmap;
		GetObject(splash,sizeof(bitmap),&bitmap);
		
		SetStretchBltMode(dc,HALFTONE);
		StretchBlt(dc,0,0,width,height,memdc,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);

		SelectObject(memdc,old);
		DeleteDC(memdc);
	}else{
		SelectObject(dc,GetStockObject(BLACK_PEN));
		SelectObject(dc,GetStockObject(BLACK_BRUSH));
		Rectangle(dc,0,0,width,height);
	}

	SetBkMode(dc,TRANSPARENT);
	SetTextColor(dc,RGB(48,128,192));
	const HFONT font=CreateFont(height/8,0,0,0,FW_BOLD,0,0,0,0,0,0,NONANTIALIASED_QUALITY,0,"ARIAL"),oldfont=(HFONT)SelectObject(dc,font);

	RECT r={0,0,width,height/4};
	DrawText(dc,T_LOADING,strlen(T_LOADING),&r,DT_CENTER|DT_VCENTER|DT_SINGLELINE);

	SelectObject(dc,oldfont);
	DeleteObject(font);


	const HBITMAP ok=LoadBitmap(GetDllInstance(),MAKEINTRESOURCE(IDB_OK));
	const HBITMAP loading=LoadBitmap(GetDllInstance(),MAKEINTRESOURCE(IDB_LOADING));
	const HBITMAP okmask=LoadBitmap(GetDllInstance(),MAKEINTRESOURCE(IDB_OKMASK));
	const HBITMAP loadingmask=LoadBitmap(GetDllInstance(),MAKEINTRESOURCE(IDB_LOADINGMASK));

	const HDC memdc=CreateCompatibleDC(dc);
	const HBITMAP oldbmp=(HBITMAP)SelectObject(memdc,ok);


	SIZE w;
	GetTextExtentPoint(dc,"XYZ",3,&w);
	for (int i=0;i<sizeof(T_LOADED)/sizeof(T_LOADED[0]);i++)
	{
		const int y=int(height*0.3f)+i*(w.cy+3);

		SetTextColor(dc,RGB(0,0,0));
		CHAR c[200];
		strcpy(&c[0],T_LOADED[i]);
		TextOut(dc,int(width*0.5f)+2,y+1,&c[0],strlen(&c[0]));

		SetTextColor(dc,RGB(96,214,255));
		TextOut(dc,int(width*0.5f),y,&c[0],strlen(&c[0]));

		GetTextExtentPoint(dc,&c[0],strlen(&c[0]),&w);
		if (abschnitt>=i)
		{	// Prozentwert anzeigen
			SetTextColor(dc,(abschnitt==i)?RGB(255,255,0):RGB(255,255,255));
			CHAR c2[100]="\0";

			if (abschnitt==i)
			{
				sprintf(&c2[0]," (%d %%)",(prozent==-1)?0:prozent);
			}
			if (abschnitt>i)sprintf(&c2[0]," (%d %%)",100);
			TextOut(dc,int(width*0.5f)+2+w.cx,y+1,&c2[0],strlen(&c2[0]));
		}
		if (abschnitt>i)
		{
			SelectObject(memdc,okmask);
			BitBlt(dc,width/2-30,y+w.cy/2-10,20,20,memdc,0,0,SRCAND);
			SelectObject(memdc,ok);
			BitBlt(dc,width/2-30,y+w.cy/2-10,20,20,memdc,0,0,SRCPAINT);
		}else if (abschnitt==i)
		{
			SelectObject(memdc,loadingmask);
			BitBlt(dc,width/2-30,y+w.cy/2-10,20,20,memdc,0,0,SRCAND);
			SelectObject(memdc,loading);
			BitBlt(dc,width/2-30,y+w.cy/2-10,20,20,memdc,0,0,SRCPAINT);
		}
	}

	SelectObject(memdc,oldbmp);
	DeleteDC(memdc);
	DeleteObject(ok);
	DeleteObject(loading);
	DeleteObject(okmask);
	DeleteObject(loadingmask);


#ifdef _DEBUG
	SetTextColor(dc,RGB(255,255,255));
	SetBkMode(dc,TRANSPARENT);
	TextOut(dc,10,10,"DEBUG VERSION",13);
#endif

	lpDDSBack->ReleaseDC(dc);

	Flip();
}

void CGame::ToggleSound()
{
	Config.soundon=!Config.soundon;
	CObject* o=sounds->GetFirst();
	while (o)
	{
		((CSound*)o)->Toggle(Config.soundon);
		o=o->next;
	}
	if (Config.soundon)ticker->Add(T_SOUNDON); else ticker->Add(T_SOUNDOFF);
}

DWORD WINAPI SaveThread(LPVOID param)
{
	LPDIRECTDRAWSURFACE7 screenshot=(LPDIRECTDRAWSURFACE7)param;
	HDC dc=0,memdc=0;
	HBITMAP bitmap=0;
	const int width=game->width;
	const int height=game->height;

	if (SUCCEEDED(screenshot->GetDC(&dc)))
	{
		memdc=CreateCompatibleDC(dc);
		bitmap=CreateCompatibleBitmap(dc,width,height);

		SelectObject(memdc,bitmap);

		BitBlt(memdc,0,0,width,height,dc,0,0,SRCCOPY);

		screenshot->ReleaseDC(dc);
		screenshot->Release();
	}
	CHAR filename[MAX_PATH];
	CHAR date[10],time[10];

	GetDateFormat(LOCALE_USER_DEFAULT,0,NULL,"yyMMdd",&date[0],10);
	GetTimeFormat(LOCALE_USER_DEFAULT,0,NULL,"HHmmss",&time[0],10);

	{	// Search for file name
		CHAR desktop[MAX_PATH];
		if (SHGetSpecialFolderPath(game->wnd,&desktop[0],CSIDL_DESKTOPDIRECTORY,FALSE)==FALSE)
			goto error1;
		if (desktop[strlen(&desktop[0])]!='\\')strcat(&desktop[0],"\\");

		int i=1;
		do
		{
			CHAR c[100];
			sprintf(&c[0],T_SCREENSHOTFILE,&date[0],&time[0]);
			strcpy(&filename[0],&desktop[0]);
			if (i>1)
			{
				CHAR c2[10];
				sprintf(&c2[0],"_%d",i);
				strcat(&c[0],&c2[0]);
			}
			strcat(&c[0],".bmp");

			strcat(&filename[0],&c[0]);
			if (i>100)goto error1;
			i++;
		}while (GetFileAttributes(&filename[0])!=DWORD(-1));
	}

	SaveBitmap(bitmap,memdc,&filename[0]);

#ifdef _DEBUG
	{
		CHAR c[1024];
		sprintf(&c[0],"Screenshot saved to: %s.",&filename[0]);
		DebugOut(&c[0]);
	}
#endif

error1:
	DeleteDC(memdc);
	DeleteObject(bitmap);
	return 123;
}

void CGame::SaveScreenShot()
{
	LPDIRECTDRAWSURFACE7 screenshot=NULL;
	screenshot=CreateSurface(lpDD,width,height);

	// Warten, bis Rendervorgänge abgeschlossen sind.
	while (lpDDSBack->GetBltStatus(DDGBS_CANBLT)!=DD_OK) Sleep(1);

	screenshot->BltFast(0,0,lpDDSBack,NULL,DDBLTFAST_WAIT);

	ticker->Add(T_SCREENSHOTSAVED,D3DRGB(0,1,0),2.0f);

	DWORD id;
	CloseHandle(CreateThread(NULL,0,SaveThread,LPVOID(screenshot),0,&id));
}

void CGame::PlayAnnouncer(const LPDIRECTSOUNDBUFFER buffer)
{
	if (!Config.soundon)return;
	if (buffer==NULL)return;
	if (announcer)announcer->Remove();
	announcer=new CSound(FALSE);
	announcer->Create2D(buffer);
	announcer->Play();
	AddSound(announcer);
}

void CGame::ShowLastError()
{
	CHAR c[1024];
	ConvertLastErrorToString(&c[0],1023);
	MessageBox(wnd,&c[0],E_TITLE,MB_OK|MB_ICONINFORMATION);
}

void CGame::SendVariables()
{
	if (!IsServer)return;
	
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,1,0,0,&gravity,sizeof(gravity),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,2,0,0,&aircontrol,sizeof(aircontrol),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,3,0,0,&jumpspeed,sizeof(jumpspeed),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,4,0,0,&sittime,sizeof(sittime),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,5,0,0,&throwtime,sizeof(throwtime),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,6,0,0,&replenishpower,sizeof(replenishpower),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,7,0,0,&takepower,sizeof(takepower),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,8,0,0,&damage,sizeof(damage),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,11,0,0,&plasmaregeneration,sizeof(plasmaregeneration),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,12,0,0,&plasmaradius,sizeof(plasmaradius),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,13,0,0,&plasmarate,sizeof(plasmarate),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,14,0,0,&respawntime,sizeof(respawntime),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,15,0,0,&harvesterspeed,sizeof(harvesterspeed),TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,16,0,0,&harvesterrotspeed,sizeof(harvesterrotspeed),TRUE);

	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,30,Config.KI,0,NULL,0,TRUE);
	Send(DPID_ALLPLAYERS,NM_VARCHANGED,0,31,PlasmaBallPressure,0,NULL,0,TRUE);
}


void CGame::RenderSun(const D3DVECTOR playerpos)
{
	if (!Config.alpha)return;
	const float faktor=float(game->width)/400.0f;
	// Sonne

	D3DLIGHT7 light;
	ZeroMemory(&light,sizeof(light));
	game->lpDevice->GetLight(0,&light);

	CLensFlare f(-light.dvDirection+playerpos,TRUE,FALSE);

	game->lpDevice->SetTexture(0,game->lpTexture[8]);
	f.DrawFlare(D3DRGB(1.0f,1.0f,1.0f),0.0f,250.0f*faktor);
}
