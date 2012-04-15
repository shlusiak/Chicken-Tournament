/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _EI_INCLUDED_
#define _EI_INCLUDED_

#include "game.h"
#include "chicken.h"
#include "collision.h"
#include "bauer.h"



const float ei_offsety=1.5f;
const float ei_wurfspeed=16.0f;



class CEi:public CObject
{
private:
	CD3DFileObject* lpMesh;
	D3DVECTOR pos,speed,ang;

	UID owner;

	void Splat();
	void HitPlayer(CBauer* b,const float damagefaktor);
public:
	CEi();
	CEi(const D3DVECTOR vpos,const D3DVECTOR vang,D3DVECTOR ospeed,const D3DVECTOR opos,const BOOLEAN relativespeed,const float vspeed,const UID ownerid);

	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Execute(float elapsed);

	virtual void Notify(PNETWORKDATA data);
	
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
};






#endif