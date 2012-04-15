/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _SPRITES_INCLUDED_
#define _SPRITES_INCLUDED_


#include "objects.h"



class CSprite
{
protected:
	BYTE anzahl;
	LPDIRECTDRAWSURFACE7 *lpImages;
public:
	CSprite():lpImages(NULL),anzahl(0) {}
	virtual ~CSprite() {Clear();}

	void Clear();
	virtual const BOOLEAN Load(const PCHAR name);
	const BOOLEAN Loaded()const { return (anzahl>0); }

	const int GetMax()const { return anzahl; }
	LPDIRECTDRAWSURFACE7 GetSurface(const int nr)const { if (nr>=anzahl)return NULL; if (lpImages==NULL)return NULL; return lpImages[nr]; }
	void DrawSmooth(LPDIRECT3DDEVICE7 lpDevice,const D3DVECTOR pos,const float w,const float h,const DWORD color,const int spritenr=0);
	void DrawAlpha(LPDIRECT3DDEVICE7 lpDevice,const D3DVECTOR pos,const float w,const float h,const DWORD color,const int spritenr=0);
};
/*
class CSingleSprite:public CSprite
{
public:
	virtual const BOOLEAN Load(const PCHAR name);	// Loads an single image into this sprite holder
};*/

class CSpriteAnimation:public CObject
{
private:
	const int spriteindex;
	const BOOLEAN loop;
	const float invfps,timeout;
	const D3DVECTOR pos;
	const float w,h;

	float lastframe,time;
	int current;
public:
	CSpriteAnimation(const int vspriteindex,const D3DVECTOR vpos,const BOOLEAN vloop,const float vtimeout,const float vfps,const float vw,const float vh)
		:spriteindex(vspriteindex),loop(vloop),invfps(1.0f/vfps),current(0),lastframe(0),time(0),timeout(vtimeout),w(vw),h(vh),pos(vpos)
	{	id=MakeUID(IDEffect);	}
	
	virtual void Execute(float elapsed);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
};




#endif