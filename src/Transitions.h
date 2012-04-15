/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _TRANSITIONS_INCLUDED_
#define _TRANSITIONS_INCLUDED_


class CTransition
{
protected:
	float time;
	const float maxtime;
	static LPDIRECTDRAWSURFACE7 CreateTextureFromBackBuffer();
	inline const float Faktor()const { return (time/maxtime); }
public:
	CTransition(const float vmaxtime):time(0.0f),maxtime(vmaxtime) {}

	virtual void Execute(float elapsed) { time+=elapsed; }
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice)=0;

	inline const BOOLEAN IsOver()const { return (time>maxtime); }
};


class CFadeTransition:public CTransition
{
private:
	LPDIRECTDRAWSURFACE7 lpTexture;
	const BOOLEAN FadeIn;
	const float maxalpha;
	D3DCOLOR Color;

	inline const float Faktor()const { if (FadeIn)return (time/maxtime)*maxalpha; else return maxalpha*(1.0f-(time/maxtime)); }
public:
	CFadeTransition(const BOOLEAN vFadeIn,const D3DCOLOR color,LPDIRECTDRAWSURFACE7 Textur,const float maxtime,const float vmaxalpha);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);

	virtual void Invalidate() {lpTexture=NULL;}
};




#endif