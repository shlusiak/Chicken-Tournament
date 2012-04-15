/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _ENVMAP_INCLUDED_
#define _ENVMAP_INCLUDED_


#include "config.h"

/*
class CCubeMap
{
private:
	BOOLEAN Supported;
	int size;
	LPDIRECTDRAWSURFACE7 lpCubeMap;
	D3DVECTOR pos;
public:
	CCubeMap(int texturesize,D3DVECTOR vpos);
	virtual ~CCubeMap();

	LPDIRECTDRAWSURFACE7 GetTexture() { if (Config.reflections)return lpCubeMap; return NULL; }
	void Update(LPDIRECT3DDEVICE7 lpDevice);
	void SetPos(D3DVECTOR vpos) { pos=vpos; }
	void SetSize(int vsize);
	void UpdateSurface();
	void SetRenderStates(LPDIRECT3DDEVICE7 lpDevice);
	void RestoreRenderStates(LPDIRECT3DDEVICE7 lpDevice);
};*/


class CSphereMap
{
private:
	
public:
//	CSphereMap() {}
//	virtual ~CSphereMap() {}

	static void ApplySphereMap(const LPDIRECT3DDEVICE7 lpDevice,D3DVERTEX* vertices,const int numvertices);
};



////////
/*inline void CCubeMap::SetSize(int vsize)
{
	size=vsize;
	UpdateSurface();
}*/





#endif