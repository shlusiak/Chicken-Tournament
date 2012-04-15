/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _GARTENKRALLE_INCLUDED_
#define _GARTENKRALLE_INCLUDED_

#include "objects.h"
#include "weapon.h"
#include "collision.h"
#include "envmap.h"



class CGartenkralle:public CWeapon
{
private:
	CD3DFileObject* mesh;
	float maxfallspeed;

	void CheckRebound(CBoundingTube* b);
	void Rebound(CBoundingTube* b,const D3DVECTOR point);
public:
	CObject* werfer;
	D3DVECTOR speed;
	CBoundingTube* box;
	float angy,angx;
	struct STOSS
	{
		float phase;
		BOOLEAN end;
		CObject *forked;
	}stoss;

	virtual BOOLEAN Init();
	virtual ~CGartenkralle();
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Execute(float elapsed);
	const BOOLEAN Throw(CObject *vwerfer);
	const BOOLEAN Stoss();
	CObject* PerformHit(CObject* akt=NULL);
	const D3DMATRIX BerechneStoss();
	void SplatForked();
	void GetBackPoint(D3DVECTOR* v)const;
	void EndStoss();
	virtual void DrawShadow(LPDIRECT3DDEVICE7 lpDevice);

	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void Notify(PNETWORKDATA data);
	virtual void Unbind();
	virtual void NewWorld();
	void UpdateBox();
};


#endif