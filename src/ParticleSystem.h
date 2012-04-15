/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _PARTICLESYSTEM_INCLUDED_
#define _PARTICLESYSTEM_INCLUDED_


class CParticleSystem:public CObject
{
protected:
	struct TParticel
	{
		float size;
		D3DVECTOR pos;
		D3DVECTOR speed;
		float rot;
		BOOLEAN liegt;
	}*Particel;
	float time,maxtime;
	const D3DVECTOR center;
	WORD numparticel;
	D3DLVERTEX *lpVertices;

	virtual const DWORD color()const {return D3DRGB(1,1,1);}
public:
	CParticleSystem(const D3DVECTOR vcenter);
	virtual ~CParticleSystem();

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);

	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
};







#endif