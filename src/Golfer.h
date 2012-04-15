/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _GOLFER_INCLUDED_
#define _GOLFER_INCLUDED_

#include "objects.h"
#include "weapon.h"
#include "collision.h"



class CGolfer:public CWeapon
{
private:
	CD3DFileObject* mesh;

	void Rebound(CBoundingTube* t);
public:
	CBoundingTube* box;
	float ejecttime;

	struct SCHLAG
	{
		float phase;
		BOOLEAN getroffen;
	}schlag;

	virtual BOOLEAN Init();
	virtual ~CGolfer();
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Execute(float elapsed);
	const BOOLEAN Schlag();
	CObject* PerformHit(CObject* akt=NULL)const;
	const D3DMATRIX BerechneSchlag();
	void Throw();

	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void Notify(PNETWORKDATA data);
	virtual void Unbind();
	virtual void NewWorld();
};


#endif