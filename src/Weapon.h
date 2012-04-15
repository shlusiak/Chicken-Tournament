/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _WEAPON_INCLUDED_
#define _WEAPON_INCLUDED_

#include "objects.h"




class CWeapon:public CObject
{
public:
	D3DVECTOR pos;
	CObject* parent;

	CWeapon():pos(D3DVECTOR(0,0,0)),parent(NULL) {}
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
};






#endif