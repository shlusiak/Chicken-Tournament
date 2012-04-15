/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _LENSFLARE_INCLUDED_
#define _LENSFLARE_INCLUDED_



class CLensFlare
{
private:
	const D3DVECTOR lightpos;
	float fadefaktor;
	float sx,sy,sz;
	float faktor;
public:
	BOOLEAN isvisible;

	CLensFlare(const D3DVECTOR vlightpos,const BOOLEAN forcevisible,const BOOLEAN vfade);
	~CLensFlare();

	void DrawFlare(const D3DCOLOR color,const float axis,const float radius)const;
	static const BOOLEAN IsVisible(const D3DVECTOR vlightpos);
};


class CSunFlare
{
private:
	BOOLEAN visible;
	float faktor;
public:
	CSunFlare() {visible=FALSE; faktor=0.0f;}

	void Render(D3DVECTOR playerpos);
	void Execute(const float elapsed);
};

void RenderLensFlare(const D3DVECTOR playerpos,const float elapsed);
void RenderSun(const D3DVECTOR playerpos);




#endif