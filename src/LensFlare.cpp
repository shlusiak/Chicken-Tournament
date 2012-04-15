/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "lensflare.h"
#include "game.h"
#include "player.h"
#include "config.h"


#define c(x) D3DRGB(x*r,x*g,x*b)

const float gb=0.45f;
const float r=1.0f*gb;
const float g=1.0f*gb;
const float b=1.0f*gb;



CLensFlare::CLensFlare(const D3DVECTOR vlightpos,const BOOLEAN forcevisible,const BOOLEAN vfade)
:lightpos(vlightpos),fadefaktor(1.0f),isvisible(forcevisible?TRUE:IsVisible(vlightpos))
{
	sx=sy=sz=0.0f;

	if (!Config.alpha)return;

	if (!isvisible)
		return;

	faktor=float(game->width)/500.0f;

	D3DTLVERTEX sun=D3DTLVERTEX(vlightpos,0.0f,0,0,0,0);
	TransformVertex(game->lpDevice,&sun);
	sx=sun.sx;
	sy=sun.sy;
	sz=sun.sz;

	if (vfade)
	{
		D3DMATRIX m;
		D3DVECTOR p(0,0,0);
		game->lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW,&m);
		D3DMath_MatrixInvert(m,m);

		D3DMath_VectorMatrixMultiply(p,p,m);

		fadefaktor=1.0f-(Magnitude(p-lightpos)/50.0f);
		if (fadefaktor<0.0f)
		{
			isvisible=FALSE;
			return;
		}
	}

	game->lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	game->lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
	game->lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	game->lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_SRCCOLOR);
	game->lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_ONE);
}

CLensFlare::~CLensFlare()
{
	if (isvisible)
	{
		game->lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
		game->lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING, TRUE);
		game->lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE, TRUE);
		game->lpDevice->SetTexture(0,NULL);
	}
}

const BOOLEAN CLensFlare::IsVisible(const D3DVECTOR vlightpos)
{
	LPDIRECTDRAWSURFACE7 pSurface,pZBuffer;
	D3DTLVERTEX sun=D3DTLVERTEX(vlightpos,0.0f,0,0,0,0);
	TransformVertex(game->lpDevice,&sun);

	if ((sun.sx<0)||(sun.sx>game->width)||(sun.sy<0)||(sun.sy>game->height))return FALSE;

	BOOLEAN isvisible=TRUE;
	HRESULT hr;
    if(SUCCEEDED(hr = game->lpDevice->GetRenderTarget(&pSurface)))
    {
        DDSCAPS2 ddsc;
        ZeroMemory(&ddsc, sizeof(ddsc));
        ddsc.dwCaps = DDSCAPS_ZBUFFER;

        if(SUCCEEDED(hr = pSurface->GetAttachedSurface(&ddsc, &pZBuffer)))
        {
                DDSURFACEDESC2 ddsd;
                ddsd.dwSize = sizeof(ddsd);
                pSurface->GetSurfaceDesc(&ddsd);
                UINT cb = ddsd.ddpfPixelFormat.dwZBufferBitDepth >> 3;

                if(SUCCEEDED(hr = pZBuffer->Lock(NULL, &ddsd, DDLOCK_READONLY | DDLOCK_WAIT, NULL)))
                {
                    BYTE *pb = (BYTE *) ddsd.lpSurface + (UINT) sun.sy * ddsd.lPitch + (UINT) sun.sx * cb;
                    BYTE *pbLim = pb + cb;

                    for(; pb < pbLim; pb++)
                    {
                        if((*pb != 0xff)&&(*pb!=0x00))
                        {
                            isvisible = FALSE;
                            break;
                        }
                    }

                    pZBuffer->Unlock(NULL);
                }
            pZBuffer->Release();
        }

        pSurface->Release();
	}
	return isvisible;
}

void CLensFlare::DrawFlare(D3DCOLOR color,const float axis,float radius)const
{
	if (!isvisible)return;
	if (sz>=1)return;

	radius*=faktor*0.5f;
	const float h=0.1f;

	color=RGB_MAKE(BYTE(RGB_GETRED(color)*fadefaktor),BYTE(RGB_GETGREEN(color)*fadefaktor),BYTE(RGB_GETBLUE(color)*fadefaktor));

	const float ax=axis*(game->width*0.5f-sx);
	const float ay=axis*(game->height*0.5f-sy);

	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(sx-radius+ax,sy-radius+ay,sz),h,color,0,0,0),
		D3DTLVERTEX(D3DVECTOR(sx+radius+ax,sy-radius+ay,sz),h,color,0,1,0),
		D3DTLVERTEX(D3DVECTOR(sx-radius+ax,sy+radius+ay,sz),h,color,0,0,1),
		D3DTLVERTEX(D3DVECTOR(sx+radius+ax,sy+radius+ay,sz),h,color,0,1,1)
	};

	game->lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);
}





void CSunFlare::Render(const D3DVECTOR playerpos)
{
	if (!Config.alpha)return;

	D3DLIGHT7 light;
	ZeroMemory(&light,sizeof(light));
	game->lpDevice->GetLight(0,&light);

	D3DVECTOR lp=-light.dvDirection+playerpos;
	visible=CLensFlare::IsVisible(lp);
	CLensFlare f(lp,TRUE,FALSE);

	{
		// Blenden
		game->lpDevice->SetTexture(0,game->lpTexture[6]);

		const float brightness=0.45f;
		f.DrawFlare(D3DRGB(brightness*faktor,brightness*faktor,brightness*faktor),0.0f,2300.0f);

		// Flares
		f.DrawFlare(c(2.0f*faktor),-0.35f,90.0f);
		f.DrawFlare(c(1.9f*faktor),1.55f,70.0f);
		f.DrawFlare(c(1.9f*faktor),3.5f,80.0f);

		game->lpDevice->SetTexture(0,game->lpTexture[5]);

		f.DrawFlare(c(1.0f*faktor),-1.0f,70.0f);
		f.DrawFlare(c(1.0f*faktor),0.6f,80.0f);
		f.DrawFlare(c(1.0f*faktor),2.25f,110.0f);

		game->lpDevice->SetTexture(0,game->lpTexture[7]);

		f.DrawFlare(c(0.5f*faktor),1.1f,95.0f);
		f.DrawFlare(c(0.6f*faktor),2.9f,90.0f);
	}
}

void CSunFlare::Execute(const float elapsed)
{
	const float speed=8.5f;
	if (visible)
	{
		if (faktor<1.0f)
		{
			faktor+=elapsed*speed;
			if (faktor>1.0f)faktor=1.0f;
		}
	}else{
		if (faktor>0.0f)
		{
			faktor-=elapsed*speed;
			if (faktor<0.0f)faktor=0.0f;
		}
	}
}