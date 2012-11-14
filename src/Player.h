/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _PLAYER_INCLUDED_
#define _PLAYER_INCLUDED_

#include "objects.h"
#include "multiplayer.h"
#include "ui.h"
#include "radar.h"


const int bh=30; // Höhe des Burgersymbols



class CPlayer:public CObject
{
protected:
	WORD killsarow,killswithoutdeath;
	float lasthit;
	D3DTLVERTEX BurgerVertices[4];
	CFadeText* message;
	CFadeText *multikill,*godlike;
	CRadar* lpRadar;
	float namevalidity;

	virtual void Die();
	virtual void Revive() {}
	virtual LPDIRECTDRAWSURFACE7 lpBurger()const {return NULL;}
	virtual void UpdateName();
public:
	float dead;
	BYTE camera;
	D3DVECTOR speed,addspeed;
	D3DVECTOR ang;
	D3DVECTOR pos;
	BOOLEAN IsLocal,isplayer;
	float hit;
	PCHAR name;

	CPlayer():isplayer(TRUE),namevalidity(0.0f),name(NULL),dead(0.0f),hit(0.0f),IsLocal(TRUE),message(NULL),multikill(NULL),godlike(NULL),lpRadar(NULL) {}
	virtual ~CPlayer();
	virtual BOOLEAN Init();
	virtual void GetViewMatrix(D3DMATRIX &view);
	virtual void HandleInput(const float elapsed,const int mx,const int my,const int mz) {}
	virtual void SetSoundListener();
	virtual void HasHit(const UID whom);
	virtual void Execute(float elapsed);

	void Draw(LPDIRECT3DDEVICE7 lpDevice) { CObject::Draw(lpDevice); }
	virtual void SendData(DPID to) {}
	virtual void ReceiveData(PNETWORKDATA) {}
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void SwitchRadar() { lpRadar->Enabled=(lpRadar->Enabled==FALSE); }

	void NextCam();

	virtual void Invalidate();
	virtual void Restore();
};

extern CPlayer* player;



#endif