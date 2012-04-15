/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _PLASMACANNON_INCLUDED_
#define _PLASMACANNON_INCLUDED_


#include "objects.h"
#include "weapon.h"
#include "collision.h"


class CPlasmaCannon:public CWeapon
{
private:
	CD3DFileObject* lpMesh;
	float ang;
	float canshoot;
	D3DTLVERTEX mv[4];

	void Rebound(CBoundingTube *t);
public:
	CBoundingTube* box;
	float ejecttime;
	float munition;

	CPlasmaCannon():ang(0.0f),lpMesh(NULL),box(NULL),ejecttime(0.0f),munition(4.0f),canshoot(0.0f) { id=MakeUID(IDPlasmaCannon); }

	~CPlasmaCannon() {	if (box)delete box; }
	virtual BOOLEAN Init();

	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void Notify(PNETWORKDATA data);

	virtual void NewWorld();
	virtual void Unbind();

	const BOOLEAN Shoot();
	const BOOLEAN CanShoot()const { return canshoot<=0.0f; }
	void Throw();
};



#endif