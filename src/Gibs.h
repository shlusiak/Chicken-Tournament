/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _GIBS_INCLUDED_
#define _GIBS_INCLUDED_

#include "objchain.h"



class CGib:public CObject
{
private:
	CD3DFileObject* model;
	D3DVECTOR speed,ang,angspeed;
	BOOLEAN liegt;
	const float radius;
public:
	D3DVECTOR pos;

	CGib(const PCHAR modelname,const D3DVECTOR vpos,const float vradius);

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
};





class CGibs:public CObjChain
{
private:
	const D3DVECTOR pos;
	float time;
	const BYTE type;
public:
	CGibs(const D3DVECTOR vpos,const BYTE vtype);

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
};





#endif