/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _CHICKENPLAYER_INCLUDED_
#define _CHICKENPLAYER_INCLUDED_

#include "player.h"
#include "chicken.h"



class CChickenPlayer:public CChicken
{
protected:
	D3DTLVERTEX EierVertex[4],FlugVertex[4];

	virtual void KI(const float elapsed);
	virtual const float GetDeadTime()const;
	virtual void Revive();
	virtual void Die();
	virtual LPDIRECTDRAWSURFACE7 lpBurger()const { return game->lpTexture[15];}

	void HandleFlug(const float elapsed,const int mx,const int my,const int mz);

	const BOOLEAN CollidateFlug(const D3DVECTOR Normale);

	void DrawZeiger(LPDIRECT3DDEVICE7 lpDevice,const float mx,const float my,const float length,const float alpha)const;
	void DrawFluglage(LPDIRECT3DDEVICE7 lpDevice,const float x,const float y,const float size)const;
public:
	CChickenPlayer();
	virtual ~CChickenPlayer();

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);

	virtual void GetViewMatrix(D3DMATRIX &view);
	virtual void HandleInput(const float elapsed,const int mx,const int my,const int mz);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Notify(PNETWORKDATA data);
	virtual void Restore();
};



#endif