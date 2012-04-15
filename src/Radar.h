/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _RADAR_INCLUDED_
#define _RADAR_INCLUDED_


#include "objects.h"


class CPlayer;

class CRadar:public CObject
{
protected:
	float lastupdate;
	CPlayer* parent;
	LPDIRECTDRAWSURFACE7 lpSurface;
	DWORD size;

	void Update();
	void UpdateSurface();
public:
	BOOLEAN Enabled;

	CRadar(CPlayer* parent);
	virtual ~CRadar();

	void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void Execute(float elapsed);

	virtual void Invalidate();
};








#endif