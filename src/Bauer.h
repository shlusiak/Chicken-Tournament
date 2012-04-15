/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _BAUER_INCLUDED_
#define _BAUER_INCLUDED_

#include "player.h"
#include "sounds.h"
#include "collision.h"
#include "ui.h"
#include "items.h"
#include "multiplayer.h"


class CBauer:public CPlayer
{
protected:
	CSound* wade;
	CSound* denied;
	float step;
	CD3DFileObject* lpMesh,*lpHut;
	BYTE hut;
	D3DTLVERTEX Herz[4];
	CMessageChain *statusmessage;

	const float GetStepPhase(const BOOLEAN correct);
	virtual LPDIRECTDRAWSURFACE7 lpBurger()const {return (game->lpTexture[3]);}
	const BOOLEAN PickupItem(CItem* item);
	virtual void Revive();
public:
	float life;
	CObject* object;
	CBoundingTube* box;

	CBauer();
	virtual ~CBauer();

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
	virtual void HandleInput(const float elapsed,const int mx,const int my,const int mz);
	virtual void GetViewMatrix(D3DMATRIX &view);
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA);
	virtual void Notify(PNETWORKDATA data);
	virtual void Unbind();

	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	const BOOLEAN Hit(const float damage);
	void NextHut();
	virtual void DrawShadow(LPDIRECT3DDEVICE7 lpDevice);
	virtual void HasHit(const UID whom);

	void GetOut();
	const BOOLEAN HasObject(const UID type)const { return ((object!=NULL)&&(object->IsType(type)));}
};




#endif