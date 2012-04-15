/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _EFFECTS_INCLUDED_
#define _EFFECTS_INCLUDED_

#include "objects.h"
#include "particlesystem.h"


class CBlood:public CParticleSystem
{
protected:
	const D3DVECTOR pos;
	virtual const float size()const;
	virtual const float angh()const;
	virtual const float speed()const;
	virtual const DWORD color()const;
public:
	CBlood(const D3DVECTOR vpos);

	virtual BOOLEAN Init();

	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
};

class CDrescherBlood:public CBlood
{
protected:
	UID ref;
	float nextspawn,timeoffset;

	void Spawn();
public:
	CDrescherBlood(UID vref,float vtimeoffset);

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
};

class CFeder:public CObject
{
protected:
	D3DMATERIAL7 mat;

	struct TFeder
	{
		D3DVECTOR pos,speed;
		float roty,offy;
		float rotspeedy;
		BOOLEAN active;
	}*Federn;
	WORD anzahl;
	const D3DVECTOR pos;
	const BOOLEAN startspherical;
	BOOLEAN canremove;
public:
	CFeder(const D3DVECTOR pos,const BOOLEAN spherical);
	virtual ~CFeder();

	virtual BOOLEAN Init();

	virtual void Execute(float elapsed);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
};

class CFederschweif:public CFeder
{
protected:
	UID ref;
	float spawnradius,spawns;
	float nextspawn;
	void Spawn();
public:
	CFederschweif(UID vref,float vradius,float vspawns);

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
};

class CDrescherFedern:public CFeder
{
protected:
	UID ref;
	float nextspawn;
	float time,timeoffset;

	void Spawn();
public:
	CDrescherFedern(UID vref,float vtimeoffset);

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
};

class CEiDotter:public CBlood
{
protected:
	virtual const float size()const;
	virtual const float speed()const;
	virtual const float angh()const;
	virtual const DWORD color()const;
public:
	CEiDotter(D3DVECTOR vpos);
};

class CNova:public CObject
{
private:
	const float time,endscale;
	float akttime,radius;
	D3DMATERIAL7 material1;
	D3DVERTEX *VertO,*VertU,*VertR;
	WORD vertnum;
	const D3DVECTOR pos;
public:
	float aktscale;

	CNova(const D3DVECTOR p,const float sradius,const float escale=10.0f,const float t=0.9f);
	virtual ~CNova();
	virtual void Execute(float elapsed);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void SetVertex(const float r1,float r2);
};


#endif