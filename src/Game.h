/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _GAME_INCLUDED_
#define _GAME_INCLUDED_


#include "objchain.h"
#include "world.h"
#include "sounds.h"
#include "ui.h"
#include "wnd.h"
#include "music.h"
#include "sounds.h"
#include "sprites.h"
#include "console.h"
#include "transitions.h"
#include "chatwindow.h"
#include "lensflare.h"
#include "camera.h"


class CGame
{
private:
	LONGLONG last_time;
	BOOLEAN perf_flag;
	RECT clientrect;
	CHAR buf[256];
	BOOLEAN MouseButton[2];
	BOOL MultiplayerCreated;
	HBITMAP splash;
	BOOLEAN created;
	BOOLEAN UTSoundsLoaded;

	CPage* menu;
	CSound* announcer;
	CTransition* transition;
	CChatWindow* lpChat;

	BOOLEAN wireframe;
	CSunFlare LensFlare;


	const BOOLEAN CheckDirectX()const;
	const int InitMultiplayer();
	const BOOLEAN InitWindow();
	const BOOLEAN InitDirectDraw();
	const BOOLEAN InitDirect3D();
	const BOOLEAN InitZBuffer(GUID DeviceGUID);
	const BOOLEAN InitDirectSound();
	const BOOLEAN InitDirectInput();
	const BOOLEAN InitTimer();
	const BOOLEAN LoadSounds();
	const BOOLEAN LoadTextures(const BOOLEAN restoring);
	const BOOLEAN InitMusic(const BOOLEAN playing);
	const BOOLEAN InitGame(const int playertype);
	const BOOLEAN Init3DSettings();
	const BOOLEAN CGame::EnumZBuffer(const LPDIRECT3D7 lpD3D,const GUID DeviceGUID,LPDIRECTDRAWSURFACE7* lplpZBuffer,DDPIXELFORMAT* ddsd,const BOOLEAN stencil,const int bpp)const;

	const BOOLEAN LoadMusic();

	void CleanUp();
	void Execute(const float elapsed);
	void UpdateWindowRect();
	void HandleInput(const float elapsed);
	void RenderSun(const D3DVECTOR playerpos);
public:
	PCHAR errormessage;
	double time_scale;
//	BOOLEAN fullscreen;
	HWND wnd;
	int width,height;
	int devicenr;

	CD3DFile* models;
	BOOLEAN IsServer;
	CObjChain *chain,*sounds,*gimmick;
	CPlayerInfo* pi;
	CFPS *fps;
	CConsole *console;
	CTicker* ticker;

	CMusic* lpMusic;

	BOOLEAN canshadows;
	CCamera Camera;


	// + Optionen +
	float gorelevel,aircontrol,jumpspeed,sittime,throwtime,replenishpower,takepower,damage;
	float plasmaradius,plasmaregeneration,plasmarate,respawntime,harvesterspeed,harvesterrotspeed;
	float gravity;
	BOOLEAN PlasmaBallPressure;


	// - Optionen -

	LPDIRECTDRAW7 lpDD;
	LPDIRECTDRAWSURFACE7 lpDDSPrimary,lpDDSBack;
	LPDIRECTDRAWCLIPPER lpClip;
	LPDIRECT3D7 lpD3D;
	LPDIRECT3DDEVICE7 lpDevice;
	LPDIRECTDRAWSURFACE7 lpDDSZBuf;
	LPDIRECTINPUT lpDI;
	LPDIRECTINPUTDEVICE2 lpKeyboard,lpMouse;
	LPDIRECTSOUND lpDS;
	LPDIRECTSOUNDBUFFER lpPrimaryBuffer;
	LPDIRECTSOUND3DLISTENER lpDirectSoundListener;
	LPDIRECTPLAY4A lpDirectPlay;
	LPDIRECTPLAYLOBBY3A lpDirectPlayLobby;
	D3DDEVICEDESC7 d3ddesc;


	LPDIRECTDRAWSURFACE7 lpTexture[49];		// Texturen
	LPDIRECTSOUNDBUFFER lpBuffers[35];		// Sounds
	CSprite* lpSprites[1];					// Sprites
	CD3DFont* lpD3DFont;					// Eine Schriftart, die gerendert wird
public:
	CGame();
	~CGame();

	const BOOLEAN Run();
	const PCHAR GetErrorMessage()const {return errormessage;}
	const LRESULT WindowProc(const HWND hWnd,const UINT uMsg,const WPARAM wParam,const LPARAM lParam);
	const BOOLEAN KeyDown(const BYTE key)const {	return ((buf[key]&0x80)!=0x00); }
	const BOOLEAN MouseButtonDown(const BYTE number)const {return MouseButton[number]; }
	inline void AddSound(CSound* sound);
	inline void AddGimmick(CObject* o);
	void PlayAnnouncer(const LPDIRECTSOUNDBUFFER buffer);
	const BOOLEAN IsMusic();
	const BOOLEAN MuteMusic();
	void ToggleSound();
	void Flip();
	const BOOLEAN Draw();
	void SaveScreenShot();
	void ShowLastError();
	void SendVariables();
	inline void SetTransition(CTransition* t);
	void DrawLoading(const int abschnitt,const int prozent=-1);
	void ChangeResolution(const int w,const int h,const int bpp,const BOOLEAN fs);

	const float GetFontCentered(const float min,const float max)const { SIZE s;lpD3DFont->GetTextExtent("X",&s); return min+(max-min-s.cy)*0.5f;}
	inline void DrawName(D3DVECTOR pos,const char* name,const DWORD color);
	const BOOLEAN LoadUTSounds();
};



// inline Funktionen

inline void CGame::AddSound(CSound* sound)
{
	sounds->Add(sound);
}

inline void CGame::AddGimmick(CObject* o)
{
	gimmick->Add(o);
}

inline void CGame::SetTransition(CTransition* t)
{
	if (transition)delete transition;
	transition=t;
}

inline void CGame::DrawName(D3DVECTOR pos,const char* name,const DWORD color)
{
	if (Magnitude(pos-Camera.GetCameraPos())>18.0f)return;

	const float w=TransformVector(lpDevice,pos);
	SIZE s;

	lpD3DFont->GetTextExtent(name,&s);

	if (w>0.0f)lpD3DFont->DrawText(pos.x-s.cx*0.5f,pos.y-s.cy,color,name);
}

// ----



extern CGame* game;
extern CWorld* world;
extern CFPS* fps;



#endif 