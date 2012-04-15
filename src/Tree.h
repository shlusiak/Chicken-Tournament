/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _TREE_INCLUDED_
#define _TREE_INCLUDED_

#include "objects.h"
#include "collision.h"


class CTree:public CObject
{
private:
	float angy;
	BYTE texturenr;
	float size;

	D3DVERTEX vertices[4];
public:
	D3DVECTOR pos;
	CBoundingTube* box;

	CTree(const float x=0.0f,const float y=0.0f,const float a=0.0f);
	virtual ~CTree() {if (box)delete box; }

	virtual BOOLEAN Init();
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void NewWorld();
};





#endif