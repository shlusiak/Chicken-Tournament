/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "resources/resource.h"
#include "resource.h"
#include "resources/resources.h"
#include "text.h"
#include "sprites.h"
#include "config.h"
#include "shadows.h"




LRESULT CALLBACK WindowProc(HWND hWnd,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	return (game?game->WindowProc(hWnd,uMsg,wParam,lParam):0);
}



const DWORD fullscreenflags=WS_POPUP;
const DWORD windowflags=WS_OVERLAPPED|WS_BORDER|WS_CAPTION|WS_SYSMENU|WS_MINIMIZEBOX|WS_THICKFRAME;



const BOOLEAN CGame::InitWindow()
{
	errormessage=E_INITWINDOW;

	WNDCLASS wc;
	ZeroMemory(&wc,sizeof(wc));
	wc.style=CS_HREDRAW|CS_VREDRAW;
	wc.lpfnWndProc=(WNDPROC)::WindowProc;
	wc.cbWndExtra=0;
	wc.hInstance=hInstance;
	wc.hCursor=LoadCursor(GetDllInstance(),MAKEINTRESOURCE(IDC_NONE));
	wc.hbrBackground=(HBRUSH)GetStockObject(NULL_BRUSH);
	wc.lpszClassName=T_CLASSNAME;
	wc.hIcon=LoadIcon(hInstance,MAKEINTRESOURCE(IDR_ICON));

	RegisterClass(&wc);

	wnd=CreateWindow(T_CLASSNAME,IsServer?T_TITLESERVER:T_TITLECLIENT,
		Config.fullscreen?fullscreenflags:windowflags,
		CW_USEDEFAULT,CW_USEDEFAULT,640,480,0,0,hInstance,NULL);
	if (!wnd)return FALSE;

	ShowWindow(wnd,SW_SHOW);
	UpdateWindow(wnd);

	UpdateWindowRect();
	return TRUE;
}

const BOOLEAN CGame::InitDirectDraw()
{
	errormessage=E_DDRAW;
	HRESULT ddrval;

	ddrval=DirectDrawCreateEx(NULL,(void**)&lpDD,IID_IDirectDraw7,NULL);
	if (ddrval!=DD_OK)
	{
		return FALSE;
	}

	ddrval=lpDD->SetCooperativeLevel(wnd,(Config.fullscreen?(DDSCL_FULLSCREEN|DDSCL_EXCLUSIVE):(DDSCL_NORMAL)));
	if (ddrval!=DD_OK)
	{
		errormessage=E_SETCOOPERATIVELEVEL;
		return FALSE;
	}

	if (Config.fullscreen)
	{
		ddrval=lpDD->SetDisplayMode(Config.resx,Config.resy,Config.bpp,0,0);
		if (ddrval!=DD_OK)
		{
			errormessage=E_DISPLAYMODE;
			return FALSE;
		}
	}else{
		DDSURFACEDESC2 dd;
		dd.dwSize=sizeof(dd);
		dd.ddpfPixelFormat.dwSize=sizeof(dd.ddpfPixelFormat);
		lpDD->GetDisplayMode(&dd);
		Config.bpp=dd.ddpfPixelFormat.dwRGBBitCount;
	}

	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(ddsd));

	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS|(Config.fullscreen?DDSD_BACKBUFFERCOUNT:0);
	ddsd.ddsCaps.dwCaps=DDSCAPS_PRIMARYSURFACE|DDSCAPS_3DDEVICE|(Config.fullscreen?DDSCAPS_FLIP|DDSCAPS_COMPLEX:0);
	ddsd.dwBackBufferCount=1;
	ddrval=lpDD->CreateSurface(&ddsd,&lpDDSPrimary,NULL);
	if (ddrval!=DD_OK)
	{
		errormessage=E_PRIMARYSURFACE;
		return FALSE;
	}

	// Create Clipper
	if (!Config.fullscreen)
	{
		ddrval=lpDD->CreateClipper(NULL,&lpClip,NULL);
		if (ddrval!=DD_OK)
		{
			errormessage=E_CLIPPER;
			return FALSE;
		}
		lpClip->SetHWnd(0,wnd);
		lpDDSPrimary->SetClipper(lpClip);
	}

	if (Config.fullscreen)
	{
		DDSCAPS2 ddscaps;
		ZeroMemory(&ddscaps,sizeof(ddscaps));
		ddscaps.dwCaps=DDSCAPS_BACKBUFFER;
		ddrval=lpDDSPrimary->GetAttachedSurface(&ddscaps,&lpDDSBack);
		if (ddrval!=DD_OK)
		{
			errormessage=E_GETBACKBUFFER;
			return FALSE;
		}
	}else
	{
		ddsd.dwFlags        = DDSD_WIDTH | DDSD_HEIGHT | DDSD_CAPS;
		ddsd.dwWidth        = width;
		ddsd.dwHeight       = height;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_3DDEVICE;

		ddrval = lpDD->CreateSurface( &ddsd, &lpDDSBack, NULL );
		if (ddrval!=DD_OK)
		{
			errormessage=E_CREATEBACKBUFFER;
			return FALSE;
		}
	}
	return TRUE;
}

static DWORD zbuf=0;

static HRESULT WINAPI EnumZBufferFormatsCallback(DDPIXELFORMAT* pddpf,void* pddpfDesired)
{
	if ((pddpf==NULL)||(pddpfDesired==NULL))return D3DENUMRET_CANCEL;

	DDPIXELFORMAT* des=(DDPIXELFORMAT*)pddpfDesired;

#ifdef _DEBUG
	CHAR c[50];
	sprintf(&c[0],"Z-Buffer offered: %d bit (%s).",pddpf->dwZBufferBitDepth,(pddpf->dwFlags&DDPF_STENCILBUFFER)?"stencil":"no stencil");
	DebugOut(&c[0]);
#endif

	if ((pddpf->dwFlags==des->dwFlags)&&
		(pddpf->dwZBufferBitDepth==des->dwZBufferBitDepth))
	{
		memcpy(pddpfDesired,pddpf,sizeof(DDPIXELFORMAT));
		zbuf=pddpf->dwZBufferBitDepth;
		if (zbuf==DWORD(Config.bpp))return D3DENUMRET_CANCEL;
		return D3DENUMRET_OK;
	}
	return D3DENUMRET_OK;
}

const BOOLEAN CGame::EnumZBuffer(const LPDIRECT3D7 lpD3D,const GUID DeviceGUID,LPDIRECTDRAWSURFACE7* lplpZBuffer,DDPIXELFORMAT* ddsd,const BOOLEAN stencil,const int bpp)const
{
#ifdef _DEBUG
	CHAR c[100];
	sprintf(&c[0],"Enumeration Z-Buffers with %d bpp and %s stencil.",bpp,stencil?"with":"no");
	DebugOut(&c[0]);
#endif
	zbuf=0;
	
	ZeroMemory(ddsd,sizeof(*ddsd));
	ddsd->dwSize=sizeof(*ddsd);
	ddsd->dwFlags=DDPF_ZBUFFER|(stencil?DDPF_STENCILBUFFER:0);
	ddsd->dwZBufferBitDepth=bpp;

	lpD3D->EnumZBufferFormats(DeviceGUID,EnumZBufferFormatsCallback,(void*)ddsd);

	return (zbuf!=0);
}

const BOOLEAN CGame::InitZBuffer(const GUID DeviceGUID)
{
	zbuf=0;

	DDPIXELFORMAT m_ddpfZBuffer;
	DDSURFACEDESC2 back;
	ZeroMemory(&back,sizeof(back));
	back.dwSize=sizeof(back);

//	Stencil erstmal ausklammern, weil nicht benutzt
	ZeroMemory(&m_ddpfZBuffer,sizeof(m_ddpfZBuffer));
	m_ddpfZBuffer.dwSize=sizeof(m_ddpfZBuffer);
	m_ddpfZBuffer.dwFlags=DDPF_ZBUFFER|DDPF_STENCILBUFFER;
	m_ddpfZBuffer.dwZBufferBitDepth=32;

	lpDDSBack->GetSurfaceDesc(&back);


	BOOLEAN found=FALSE;

	const int STENCIL=SHADOWS;	// Set this to true to enable Shadows!

	if ((!found)&&(STENCIL))found=EnumZBuffer(lpD3D,DeviceGUID,&lpDDSZBuf,&m_ddpfZBuffer,TRUE,back.ddpfPixelFormat.dwRGBBitCount);
	if (!found)found=EnumZBuffer(lpD3D,DeviceGUID,&lpDDSZBuf,&m_ddpfZBuffer,FALSE,back.ddpfPixelFormat.dwRGBBitCount);

	if (!found)found=EnumZBuffer(lpD3D,DeviceGUID,&lpDDSZBuf,&m_ddpfZBuffer,FALSE,24);

	if ((!found)&&(STENCIL))found=EnumZBuffer(lpD3D,DeviceGUID,&lpDDSZBuf,&m_ddpfZBuffer,TRUE,16);
	if (!found)found=EnumZBuffer(lpD3D,DeviceGUID,&lpDDSZBuf,&m_ddpfZBuffer,FALSE,16);




	if (!found)
	{
		errormessage=E_NOZBUFFER;
		return FALSE;
	}

	canshadows=BOOLEAN((m_ddpfZBuffer.dwFlags&DDPF_STENCILBUFFER)!=0);


#ifdef _DEBUG
	CHAR c[80];
	sprintf(&c[0],"Z-Buffer: %d bit. (%s)",zbuf,(m_ddpfZBuffer.dwFlags&DDPF_STENCILBUFFER)?"StencilBuffer":"Ohne Stencil");
	DebugOut(&c[0]);
#endif

	DDSURFACEDESC2 ddsd;
	HRESULT ddrval;
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	ddsd.dwFlags=DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;

	if ((DeviceGUID==IID_IDirect3DTnLHalDevice)||
		(DeviceGUID==IID_IDirect3DHALDevice))
		ddsd.ddsCaps.dwCaps=DDSCAPS_ZBUFFER|DDSCAPS_VIDEOMEMORY;
	else
		ddsd.ddsCaps.dwCaps=DDSCAPS_ZBUFFER|DDSCAPS_SYSTEMMEMORY;
	ddsd.dwWidth=width;
	ddsd.dwHeight=height;
	ddsd.ddpfPixelFormat.dwSize=sizeof(DDPIXELFORMAT);
	ddsd.ddpfPixelFormat.dwFlags=DDPF_ZBUFFER;
	memcpy(&ddsd.ddpfPixelFormat,&m_ddpfZBuffer,sizeof(DDPIXELFORMAT));
	ddrval=lpDD->CreateSurface(&ddsd,&lpDDSZBuf,NULL);
	if (ddrval!=DD_OK)
	{
		errormessage=E_CREATEZBUFFER;
		return FALSE;
	}else lpDDSBack->AddAttachedSurface(lpDDSZBuf);

	if (lpDDSZBuf==NULL)return FALSE;

	if (FAILED(lpDevice->SetRenderTarget(lpDDSBack,0)))
	{
		errormessage=E_NOUPDATEAFTERZBUFFER;
		return FALSE;
	}
	return TRUE;
}

HRESULT CALLBACK DeviceEnumCallback(LPSTR lpDeviceDescription, LPSTR lpDeviceName, LPD3DDEVICEDESC7 lpD3DDeviceDesc,LPVOID lpContext)
{
	LPD3DDEVICEDESC7 d=(LPD3DDEVICEDESC7)lpContext;
	if (d->dwReserved1==d->dwReserved2)
	{
#ifdef _DEBUG
		CHAR c[100];
		sprintf(&c[0],"Device accepted: %s.",lpDeviceName);
		DebugOut(&c[0]);
#endif
		memcpy(lpContext,lpD3DDeviceDesc,sizeof(D3DDEVICEDESC7));
		return D3DENUMRET_CANCEL;
	}
#ifdef _DEBUG
		CHAR c[100];
		sprintf(&c[0],"Device offered: %s.",lpDeviceName);
		DebugOut(&c[0]);
#endif
	d->dwReserved1++;
	return D3DENUMRET_OK;
}

const BOOLEAN CGame::InitDirect3D()
{
	errormessage=E_D3D;

	if (FAILED(lpDD->QueryInterface(IID_IDirect3D7,(void**)&lpD3D)))
	{
		errormessage=E_NOD3D7;
		return FALSE;
	}

	if (devicenr==-1)
	{
		if ((lpD3D->CreateDevice(IID_IDirect3DTnLHalDevice,lpDDSBack,&lpDevice)!=D3D_OK))
		{
			if ((lpD3D->CreateDevice(IID_IDirect3DHALDevice,lpDDSBack,&lpDevice)!=D3D_OK))
			{
				if ((lpD3D->CreateDevice(IID_IDirect3DMMXDevice,lpDDSBack,&lpDevice)!=D3D_OK))
				{
					if ((lpD3D->CreateDevice(IID_IDirect3DRGBDevice,lpDDSBack,&lpDevice)!=D3D_OK))
					{
						errormessage=E_CREATEDEVICE;
						return FALSE;
					}else DebugOut("RGBDevice created.");
				}else DebugOut("MMXDevice created.");
			}else DebugOut("HALDevice created.");
		}else DebugOut("TnLHalDevice created.");
	}else
	{
		D3DDEVICEDESC7 d;
		ZeroMemory(&d,sizeof(d));
		d.dwReserved2=devicenr;
		lpD3D->EnumDevices(DeviceEnumCallback,&d);

		if (lpD3D->CreateDevice(d.deviceGUID,lpDDSBack,&lpDevice)!=D3D_OK)return FALSE;
	}
	lpDevice->GetCaps(&d3ddesc);

	if (!InitZBuffer(d3ddesc.deviceGUID))return FALSE;

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_TRUE);

	D3DVIEWPORT7 view;
	view.dwX=0;
	view.dwY=0;
	view.dwWidth=width;
	view.dwHeight=height;
	view.dvMinZ=0.0f;
	view.dvMaxZ=1.0f;
	if (lpDevice->SetViewport(&view)!=D3D_OK)
	{
		errormessage=E_VIEWPORT;
		return FALSE;
	}

	D3DMATRIX proj_m;
	const float FOV=0.8f;
	D3DUtil_SetProjectionMatrix(proj_m,FOV,float(height)/float(width),0.1f,200.0f);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION,&proj_m);

	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
	lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_ONE );
	lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE );

	DrawLoading(LOADED_3DENGINE);
	return TRUE;
}


const BOOLEAN CGame::InitDirectSound()
{
	errormessage=E_DIRECTSOUND;

	if (DirectSoundCreate(NULL,&lpDS,NULL)!=DS_OK)return FALSE;
	if (lpDS->SetCooperativeLevel(wnd,DSSCL_NORMAL)!=DS_OK)
	{
		SaveRelease(lpDS);
		errormessage=E_DSSETCOOPERATIVELEVEL;
		return FALSE;
	}

	if (!LoadSounds())
	{
		SaveRelease(lpDS);
		return FALSE;
	}

	DSBUFFERDESC dsbdDesc;

	ZeroMemory(&dsbdDesc,sizeof(DSBUFFERDESC));
	dsbdDesc.dwSize=sizeof(DSBUFFERDESC);
	dsbdDesc.dwFlags=DSBCAPS_PRIMARYBUFFER|DSBCAPS_CTRL3D;
	dsbdDesc.guid3DAlgorithm=DS3DALG_DEFAULT;

	if (lpDS->CreateSoundBuffer(&dsbdDesc,&lpPrimaryBuffer,NULL)!=DS_OK)
	{
		SaveRelease(lpDS);
		errormessage=E_PRIMARYBUFFER;
		return FALSE;
	}
	lpPrimaryBuffer->QueryInterface(IID_IDirectSound3DListener,(void**)&lpDirectSoundListener);

	DS3DLISTENER LP;
	ZeroMemory(&LP,sizeof(LP));
	LP.dwSize=sizeof(LP);

	lpDirectSoundListener->GetAllParameters(&LP);

	LP.flDopplerFactor=1.0f;
	LP.flRolloffFactor=0.60f;
	// So weit wegsetzen, dass niemand die Sounds beim Laden hört
	LP.vPosition=D3DVECTOR(0,-100000,0);	

	lpDirectSoundListener->SetAllParameters(&LP,DS3D_IMMEDIATE);

	DrawLoading(LOADED_SOUNDS);

	return TRUE;
}

const BOOLEAN CGame::InitDirectInput()
{
	CZeitMesser z("DirectInput initialisiert in : %d ms");
	errormessage=E_DIRECTINPUT;
	if (DirectInputCreate(hInstance,DIRECTINPUT_VERSION,&lpDI,NULL)!=DI_OK)return FALSE;

//	Keyboard
	HRESULT hr;
	hr=lpDI->CreateDevice(GUID_SysKeyboard,(LPDIRECTINPUTDEVICE*)&lpKeyboard,NULL);
	if (hr!=DI_OK)return FALSE;
	lpKeyboard->SetDataFormat(&c_dfDIKeyboard);
	lpKeyboard->SetCooperativeLevel(wnd,DISCL_FOREGROUND|DISCL_NONEXCLUSIVE);
    lpKeyboard->Acquire();

	DrawLoading(LOADED_INPUT-1,50);

	//	Mouse
	hr=lpDI->CreateDevice(GUID_SysMouse,(LPDIRECTINPUTDEVICE*)&lpMouse,NULL);
	if (hr!=DI_OK)return FALSE;
	hr=lpMouse->SetDataFormat(&c_dfDIMouse);
	lpMouse->SetCooperativeLevel(wnd,DISCL_FOREGROUND|DISCL_EXCLUSIVE);

	DIPROPDWORD dipdw=
	{
		{
			sizeof(DIPROPDWORD),
			sizeof(DIPROPHEADER),
			0,
			DIPH_DEVICE,
		},
		128
	};
	lpMouse->SetProperty(DIPROP_BUFFERSIZE,&dipdw.diph);
	lpMouse->Acquire();

	DrawLoading(LOADED_INPUT);


	return TRUE;
}

const BOOLEAN CGame::InitTimer()
{
	LONGLONG perf_cnt;
	last_time=0;

	if (QueryPerformanceFrequency((LARGE_INTEGER*)&perf_cnt))
	{
		perf_flag=TRUE;
		QueryPerformanceCounter((LARGE_INTEGER*)&last_time);
		time_scale=1.0f/perf_cnt;
	}else{
		perf_flag=FALSE;
		last_time=timeGetTime();
		time_scale=0.001f;
	}

	DrawLoading(LOADED_TIMER);
	return TRUE;
}

const BOOLEAN CGame::Init3DSettings()
{
	errormessage=E_INIT;
	const DWORD BackgroundColor=D3DRGB(0.25f,0.5f,1.0f);
	{	// Fog
		const float start=15.0f;
		const float end=70.0f;
		const DWORD color=BackgroundColor;
//		D3DRGB(1,1,1);
		const float dens=1.0f;

//		lpDevice->SetRenderState(D3DRENDERSTATE_FOGENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_FOGSTART,*((LPDWORD) (&start)));
		lpDevice->SetRenderState(D3DRENDERSTATE_FOGEND,*((LPDWORD) (&end)));
		lpDevice->SetRenderState(D3DRENDERSTATE_FOGTABLEMODE,D3DFOG_LINEAR);
		lpDevice->SetRenderState(D3DRENDERSTATE_FOGCOLOR,color);
		lpDevice->SetRenderState(D3DRENDERSTATE_FOGDENSITY,*((LPDWORD) (&dens)));
	}
	{	// Light
		D3DLIGHT7 light;
		ZeroMemory(&light,sizeof(light));
		light.dvDirection.x=-0.4f;
		light.dvDirection.y=-0.7f;
		light.dvDirection.z=0.3f;

		light.dvRange=D3DLIGHT_RANGE_MAX;
		light.dltType=D3DLIGHT_DIRECTIONAL;
		light.dcvDiffuse=Color(1.0f,1.0f,0.95f);
		light.dcvSpecular=light.dcvDiffuse;

		lpDevice->SetLight(0,&light);
		lpDevice->LightEnable(0,TRUE);

		const float bright=0.55f;
		D3DCOLOR d3dclrAmbientLightColor = D3DRGBA(bright, bright, bright, bright);

		lpDevice->SetRenderState(D3DRENDERSTATE_AMBIENT, d3dclrAmbientLightColor);
	}
	{	// Textures
#if (FALSE)
		lpDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_ANISOTROPIC);
		lpDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_ANISOTROPIC);
#else
		lpDevice->SetTextureStageState(0,D3DTSS_MINFILTER,D3DTFN_LINEAR);
		lpDevice->SetTextureStageState(0,D3DTSS_MAGFILTER,D3DTFG_LINEAR);
#endif

        lpDevice->SetTextureStageState(0,D3DTSS_MIPFILTER, D3DTFP_LINEAR);
	}
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_SPECULARENABLE ,TRUE);

//		lpDevice->SetRenderState(D3DRENDERSTATE_FILLMODE, D3DFILL_WIREFRAME);
//		lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,D3DZB_FALSE);
	}
	return TRUE;
}

const BOOLEAN CGame::LoadUTSounds()
{
	if (UTSoundsLoaded)return FALSE;
	UTSoundsLoaded=TRUE;

	// UT Sounds
#if (TRUE)
	lpBuffers[6]=CreateSoundBuffer(lpDS,"doublekill.wav",FALSE);
	lpBuffers[7]=CreateSoundBuffer(lpDS,"multikill.wav",FALSE);
	lpBuffers[8]=CreateSoundBuffer(lpDS,"megakill.wav",FALSE);
	lpBuffers[9]=CreateSoundBuffer(lpDS,"ultrakill.wav",FALSE);
	lpBuffers[31]=CreateSoundBuffer(lpDS,"monsterkill.wav",FALSE);
	lpBuffers[32]=CreateSoundBuffer(lpDS,"ludicrouskill.wav",FALSE);
	lpBuffers[33]=CreateSoundBuffer(lpDS,"holyshit.wav",FALSE);

	lpBuffers[20]=CreateSoundBuffer(lpDS,"killingspree.wav",FALSE);
	lpBuffers[21]=CreateSoundBuffer(lpDS,"rampage.wav",FALSE);
	lpBuffers[22]=CreateSoundBuffer(lpDS,"dominating.wav",FALSE);
	lpBuffers[23]=CreateSoundBuffer(lpDS,"unstoppable.wav",FALSE);
	lpBuffers[24]=CreateSoundBuffer(lpDS,"godlike.wav",FALSE);
	lpBuffers[25]=CreateSoundBuffer(lpDS,"whickedsick.wav",FALSE);
#endif
	// UT Sounds -- Ende

	int i;
	for (i=6;i<=9;i++)if (lpBuffers[i])lpBuffers[i]->SetVolume(-550);
	for (i=20;i<=25;i++)if (lpBuffers[i])lpBuffers[i]->SetVolume(-550);
	for (i=31;i<=33;i++)if (lpBuffers[i])lpBuffers[i]->SetVolume(-550);

	DebugOut("UT-Sounds geladen");

	return TRUE;
}

const BOOLEAN CGame::LoadSounds()
{
	CZeitMesser z("Sounds geladen in : %d ms");

	errormessage=E_LOADSOUNDS;
#define LoadSound(w,n,d) lpBuffers[w]=CreateSoundBuffer(lpDS,n,d); if (lpBuffers[w]==NULL)return FALSE;

	LoadSound(0,"glucke.wav",TRUE);
	LoadSound(1,"splat.wav",TRUE);
	LoadSound(2,"throw.wav",TRUE);
	LoadSound(3,"fork.wav",TRUE);
	LoadSound(4,"metal.wav",TRUE);
	LoadSound(5,"glucke2.wav",TRUE);
	
	DrawLoading(LOADED_SOUNDS-1,25);

	LoadSound(10,"step1.wav",TRUE);
	LoadSound(11,"step2.wav",TRUE);
	LoadSound(12,"step3.wav",TRUE);
	LoadSound(13,"step4.wav",TRUE);
	LoadSound(14,"jump.wav",TRUE);
	LoadSound(15,"wade.wav",TRUE);

	DrawLoading(LOADED_SOUNDS-1,50);

	LoadSound(16,"engine.wav",TRUE);

	LoadSound(17,"eisplat.wav",TRUE);

	lpBuffers[18]=CreateSoundBuffer(lpDS,"denied.wav",FALSE);
	LoadSound(19,"bodysplat.wav",TRUE);

	DrawLoading(LOADED_SOUNDS-1,75);

	LoadSound(26,"plasma.wav",FALSE);	// 2D für lokalen User
	LoadSound(27,"plasma.wav",TRUE);	// 3D für Netzwerk User
	LoadSound(30,"plasma2.wav",TRUE);
	LoadSound(28,"cool.wav",FALSE);
	LoadSound(29,"item.wav",FALSE);

	LoadSound(34,"fire.wav",TRUE);

	if (lpBuffers[18])lpBuffers[18]->SetVolume(-400);

	if (UTSoundsLoaded)
	{
		UTSoundsLoaded=FALSE;
		LoadUTSounds();
	}

	DrawLoading(LOADED_SOUNDS-1,100);

	return TRUE;
}

const BOOLEAN CGame::LoadTextures(const BOOLEAN restoring)
{
	CZeitMesser z("Texturen geladen in : %d ms");
	errormessage=E_LOADTEXTURES;

	for (int i=0;i<sizeof(lpTexture)/sizeof(lpTexture[0]);i++)
		SaveRelease(lpTexture[i]);

	SaveDelete(lpD3DFont);


	const int mipmaps=5;
#define Load(nr,name,mips) lpTexture[nr]=CreateTextureFromResource(lpDevice,lpDD,lpTexture[nr],name,mips,TRUE,FALSE,FALSE); if (lpTexture[nr]==NULL){ DebugOut(name); return FALSE;} 
#define LoadCompressed(nr,name,mips) pTexture[nr]=CreateTextureFromResource(lpDevice,lpDD,lpTexture[nr],name,mips,TRUE,FALSE,TRUE); if (lpTexture[nr]==NULL) { DebugOut(name); return FALSE;}
#define LoadAlpha(nr,name,mips) lpTexture[nr]=CreateTextureFromResource(lpDevice,lpDD,lpTexture[nr],name,mips,FALSE,TRUE,FALSE); if (lpTexture[nr]==NULL) { DebugOut(name); return FALSE;}

#undef LoadCompressed
#define LoadCompressed Load


	LoadAlpha(3,"burger.bmp",0);
	LoadAlpha(17,"herz.bmp",0);
	LoadAlpha(15,"bauer.bmp",0);
	LoadAlpha(16,"eier.bmp",0);
	LoadAlpha(18,"flughuhn.bmp",0);
	LoadAlpha(19,"feder.bmp",3);

	LoadCompressed(0,"ground.jpg",mipmaps);
	LoadAlpha(1,"wald.bmp",mipmaps);
	LoadCompressed(2,"water.bmp",mipmaps);


	Load(5,"flare1.jpg",0);
	Load(6,"flare2.jpg",0);
	Load(7,"flare3.jpg",0);
	Load(8,"flare0.jpg",0);


	if (!restoring)DrawLoading(LOADED_TEXTURES-1,50);

	LoadCompressed(9,"sky0.jpg",0);
	LoadCompressed(10,"sky1.jpg",0);
	LoadCompressed(11,"sky2.jpg",0);
	LoadCompressed(12,"sky3.jpg",0);
	LoadCompressed(13,"sky4.jpg",0);
	LoadCompressed(14,"sky5.jpg",0);

	LoadCompressed(20,"splash.jpg",mipmaps);
	LoadCompressed(31,"spheremap.jpg",4);


	LoadAlpha(4,"tree1.bmp",4);
	LoadAlpha(35,"tree2.bmp",4);
	LoadAlpha(36,"tree3.bmp",4);
	LoadAlpha(37,"tree4.bmp",4);

	LoadCompressed(38,"bark.jpg",3);
	LoadAlpha(47,"blood.bmp",2);
	LoadCompressed(48,"fire.jpg",2);


	MakeTransparent(lpTexture[1]);
	MakeTransparent(lpTexture[3],FALSE);
	MakeTransparent(lpTexture[15],FALSE);
	MakeTransparent(lpTexture[16],FALSE);
	MakeTransparent(lpTexture[17],FALSE);
	MakeTransparent(lpTexture[18],FALSE);
	MakeTransparent(lpTexture[19],FALSE);

	MakeTransparent(lpTexture[4]); // Baumtextur Transparent machen
	MakeTransparent(lpTexture[35]);			// tree2
	MakeTransparent(lpTexture[36]);			// tree3
	MakeTransparent(lpTexture[37]);			// tree4

	MakeTransparent(lpTexture[47],FALSE);	// blood

	if (!restoring)DrawLoading(LOADED_TEXTURES-1,100);

#undef Load

	if (!restoring)DrawLoading(LOADED_TEXTURES);

	errormessage=E_LOADSPRITES;

	if (!restoring)
	{
		lpSprites[0]=new CSprite();
	}else
		for (int i=0;i<sizeof(lpSprites)/sizeof(lpSprites[0]);i++)
			lpSprites[i]->Clear();


	lpD3DFont=new CD3DFont("MS SANS SERIF",10,0);
	lpD3DFont->InitDeviceObjects(lpDevice);
	lpD3DFont->RestoreDeviceObjects();

	return TRUE;
}



void CGame::ChangeResolution(const int w,const int h,const int bpp,const BOOLEAN fs)
{
	int i;
	{	// Alles zerstören
		if (Config.fullscreen)
			if (lpDD)lpDD->RestoreDisplayMode();

		if (transition)
		{
			delete transition;
			transition=NULL;
		}

		chain->Invalidate();
		sounds->Invalidate();
		gimmick->Invalidate();

		if (menu)
			delete menu;
		menu=NULL;

		for (i=0;i<(sizeof(lpSprites)/sizeof(lpSprites[0]));i++)
			SaveDelete(lpSprites[i]);
		for (i=0;i<(sizeof(lpTexture)/sizeof(lpTexture[0]));i++)
			SaveRelease(lpTexture[i]);
		D3DTextr_InvalidateAllTextures();

		SaveDelete(lpD3DFont);

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

		SaveRelease(lpDevice);
		SaveRelease(lpD3D);

		if (lpDDSZBuf&&lpDDSBack)
		{
			lpDDSBack->DeleteAttachedSurface(0,lpDDSZBuf);
			SaveRelease(lpDDSZBuf);
		}
		SaveRelease(lpDDSBack);
		SaveRelease(lpDDSPrimary);
		SaveRelease(lpClip);
		
		lpDD->SetCooperativeLevel(wnd,DDSCL_NORMAL);
		SaveRelease(lpDD);
	}

	if (w!=0)
	{
		Config.resx=w;		
		Config.resy=h;
		Config.bpp=bpp;
	}
	Config.fullscreen=fs;

	if (!Config.fullscreen)
	{	
		SetWindowLong(wnd,GWL_STYLE,windowflags);
	}else{
		SetWindowLong(wnd,GWL_STYLE,fullscreenflags);
	}

	MoveWindow(wnd,0,0,640,480,TRUE);
	InvalidateRect(NULL,NULL,TRUE);
	ShowWindow(wnd,SW_SHOW);

	UpdateWindowRect();

	if (!InitDirectDraw())
		DebugOut("InitDirectDraw() fehlgeschlagen!");
	UpdateWindowRect();
	if (!InitDirect3D())
		DebugOut("InitDirect3D() fehlgeschlagen!");
	if (!InitDirectInput())
		DebugOut("InitDirectInput() fehlgeschlagen!");
	if (!LoadTextures(FALSE))
		DebugOut("LoadTextures() fehlgeschlagen!");
	if (!Init3DSettings())
		DebugOut("Init3DSettings() fehlgeschlagen!");

	D3DTextr_RestoreAllTextures(lpDevice);

	chain->Restore();
	sounds->Restore();
	gimmick->Restore();
}

const BOOLEAN CGame::CheckDirectX()const
{
	LPDIRECT3D7 lpD3D;
	LPDIRECTDRAW7 lpDD;

	if (FAILED(DirectDrawCreateEx(NULL,(void**)&lpDD,IID_IDirectDraw7,NULL)))
		return FALSE;
	if (FAILED(lpDD->QueryInterface(IID_IDirect3D7,(void**)&lpD3D)))
	{
		lpDD->Release();
		return FALSE;
	}
	lpD3D->Release();

	LPDIRECTINPUT lpDI;
	if (DirectInputCreate(hInstance,DIRECTINPUT_VERSION,&lpDI,NULL)!=DI_OK)return FALSE;
	lpDI->Release();

	// Direct Sound nicht checken, weils noch Rechner ohne Soundkarte gibt

//	LPDIRECTSOUND lpDS;
//	if (DirectSoundCreate(NULL,&lpDS,NULL)!=DS_OK)return FALSE;
//	lpDS->Release();

	return TRUE;
}