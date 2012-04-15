/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _FIRE_INCLUDED_
#define _FIRE_INCLUDED_

#include "effects.h"


class CFire:public CBlood
{
protected:
	const UID ref;
	const float spawnradius;
	float nextspawn;
	CSound* sound;

	void Spawn();
	virtual const DWORD color()const { return D3DRGB(1,1,1); }
public:
	CFire(const UID vref,const float vspawnradius);
	virtual ~CFire();

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice) {}
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
};	





#endif