/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "transitions.h"
#include "game.h"



LPDIRECTDRAWSURFACE7 CTransition::CreateTextureFromBackBuffer()
{
	LPDIRECTDRAWSURFACE7 lpTexture;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	game->lpDDSBack->GetSurfaceDesc(&ddsd);

	DWORD w,h;
	w=ddsd.dwWidth;
	h=ddsd.dwHeight;

	lpTexture=CreateTexture(game->lpDevice,&w,&h,0,FALSE,FALSE,FALSE);

	// Copy Backbuffer onto Texture
	{
		HDC back,text;
		if (FAILED(game->lpDDSBack->GetDC(&back)))goto error;
		if (FAILED(lpTexture->GetDC(&text)))
		{
			game->lpDDSBack->ReleaseDC(back);
			goto error;
		}

		SetStretchBltMode(text,HALFTONE);
		StretchBlt(text,0,0,w-1,h-1,back,0,0,game->width,game->height,SRCCOPY);

		lpTexture->ReleaseDC(text);
		game->lpDDSBack->ReleaseDC(back);
	}

	return lpTexture;

error:
	SaveRelease(lpTexture);
	return NULL;
}




CFadeTransition::CFadeTransition(const BOOLEAN vFadeIn,const D3DCOLOR color,LPDIRECTDRAWSURFACE7 Textur,const float maxtime,const float vmaxalpha)
:CTransition(maxtime),FadeIn(vFadeIn),Color(color),lpTexture(Textur),maxalpha(vmaxalpha)
{ }

void CFadeTransition::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	const float w=float(game->width),h=float(game->height);
	const float rhw=0.5f;
	const float alpha=Faktor();
//	const float alpha=1.0f;
	Color=RGBA_SETALPHA(Color,long(alpha*255));
	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(0,0,0),rhw,Color,0,0,0),
		D3DTLVERTEX(D3DVECTOR(w,0,0),rhw,Color,0,1.0f,0),
		D3DTLVERTEX(D3DVECTOR(0,h,0),rhw,Color,0,0,1.0f),
		D3DTLVERTEX(D3DVECTOR(w,h,0),rhw,Color,0,1.0f,1.0f)
	};

	lpDevice->SetTexture(0,lpTexture);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
//	lpDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
//	lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
}

