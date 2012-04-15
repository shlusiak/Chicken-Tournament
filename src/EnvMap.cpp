/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "envmap.h"
#include "game.h"
#include "player.h"



/*
void ChangeRenderTarget(LPDIRECT3DDEVICE7 lpDevice,LPDIRECTDRAWSURFACE7 pddsNewRenderTarget )
{
    LPDIRECTDRAWSURFACE7 pddsOldRenderTarget = NULL;
    lpDevice->GetRenderTarget( &pddsOldRenderTarget );

    if( pddsOldRenderTarget )
    {
        LPDIRECTDRAWSURFACE7 pddsZBuffer = NULL;
        DDSCAPS2 ddscaps = { DDSCAPS_ZBUFFER, 0, 0, 0 };
        pddsOldRenderTarget->GetAttachedSurface( &ddscaps, &pddsZBuffer );

        if( pddsZBuffer )
        {
            pddsOldRenderTarget->DeleteAttachedSurface( 0, pddsZBuffer );
            pddsNewRenderTarget->AddAttachedSurface( pddsZBuffer );
            pddsZBuffer->Release();
        }
        pddsOldRenderTarget->Release();
    }

    lpDevice->SetRenderTarget( pddsNewRenderTarget, 0 );
}






CCubeMap::CCubeMap(int vtexturesize,D3DVECTOR vpos):
size(vtexturesize),pos(vpos) 
{
	Supported=(game->d3ddesc.dpcTriCaps.dwTextureCaps&D3DPTEXTURECAPS_CUBEMAP)!=0;
	lpCubeMap=NULL;
	UpdateSurface();
}

CCubeMap::~CCubeMap()
{
	SaveRelease(lpCubeMap);
}

void CCubeMap::UpdateSurface()
{
	if (!Supported)
	{
		lpCubeMap=NULL;
		return;
	}
	if (!Config.reflections)return;


	SaveRelease(lpCubeMap);
	lpCubeMap=CreateCubeMap(game->lpDD,game->lpDevice,size,size);
}

void CCubeMap::Update(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!Supported)return;
	if (!Config.reflections)return;
	if (lpCubeMap==NULL)UpdateSurface();
	if (lpCubeMap==NULL)return;
	if (lpCubeMap->IsLost())return;

	D3DMATRIX viewdir,oldprojection,projection,m;
	player->GetViewMatrix(viewdir);

	lpDevice->GetTransform(D3DTRANSFORMSTATE_PROJECTION,&oldprojection);
	D3DUtil_SetProjectionMatrix(projection,g_PI/2.0f,1.0f,0.01f,150.0f);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION,&projection);
	D3DVIEWPORT7 viewport,nv;
	lpDevice->GetViewport(&viewport);
	ZeroMemory(&nv,sizeof(nv));
	nv=viewport;
	nv.dwX=nv.dwY=0;
	nv.dwWidth=nv.dwHeight=size;
	lpDevice->SetViewport(&nv);

	viewdir._41=viewdir._42=viewdir._43=0.0f;
	D3DXPLANE plane;
	plane.a=plane.b=plane.c=plane.d=0.0f;

	D3DXMatrixReflect((D3DXMATRIX*)&m,&plane);
	D3DMath_MatrixMultiply(viewdir,m,viewdir);
	D3DMATRIX i;
	D3DUtil_SetTranslateMatrix(i,pos);
	D3DMath_MatrixInvert(i,i);
	D3DMath_MatrixMultiply(viewdir,i,viewdir);


	LPDIRECTDRAWSURFACE7 lpSides[6];
	{
		DDSURFACEDESC2 ddsd;
		ZeroMemory(&ddsd,sizeof(ddsd));
		ddsd.dwSize=sizeof(ddsd);

		for (int wNum=0;wNum<6;wNum++)
		{
			ddsd.ddsCaps.dwCaps  = DDSCAPS_COMPLEX|DDSCAPS_3DDEVICE|DDSCAPS_TEXTURE;
			if( 0 == wNum )
	            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP_POSITIVEX|DDSCAPS2_CUBEMAP;
			if( 1 == wNum )
	            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP_NEGATIVEX|DDSCAPS2_CUBEMAP;
			if( 2 == wNum )
	            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP_POSITIVEY|DDSCAPS2_CUBEMAP;
			if( 3 == wNum )
	            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP_NEGATIVEY|DDSCAPS2_CUBEMAP;
			if( 4 == wNum )
	            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP_POSITIVEZ|DDSCAPS2_CUBEMAP;
			if( 5 == wNum )
	            ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP_NEGATIVEZ|DDSCAPS2_CUBEMAP;

			if (wNum==0)
			{
				lpSides[wNum]=lpCubeMap;
				lpSides[wNum]->AddRef();
			}else lpCubeMap->GetAttachedSurface( &ddsd.ddsCaps,&lpSides[wNum]);
		}
	}

    for( WORD wNum=0; wNum < 6; wNum++ )
    {
		D3DMATRIX matview=D3DUtil_GetCubeMapViewMatrix(wNum);
		D3DMath_MatrixMultiply(matview,viewdir,matview);
		lpDevice->SetTransform(D3DTRANSFORMSTATE_VIEW,&matview);
		D3DMATRIX i;
		D3DUtil_SetIdentityMatrix(i);
		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&i);

		{	// Rendern
			ChangeRenderTarget(lpDevice,lpSides[wNum]);
			lpDevice->Clear(0,NULL,D3DCLEAR_ZBUFFER,0,1.0f,0);
			lpDevice->BeginScene();

			world->DrawSky(lpDevice,pos);
			world->Draw(lpDevice);
			world->DrawUI(lpDevice);

 			lpDevice->EndScene();
		}
	}

	ChangeRenderTarget(lpDevice,game->lpDDSBack);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_PROJECTION,&oldprojection);
	lpDevice->SetViewport(&viewport);

	for (wNum=0;wNum<6;wNum++)
	{
		lpSides[wNum]->Release();
	}
}

void CCubeMap::SetRenderStates(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!Supported)return;
	if (!Config.reflections)return;
	lpDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEREFLECTIONVECTOR);
	lpDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3 );
	lpDevice->SetTextureStageState( 0, D3DTSS_ADDRESSU,  D3DTADDRESS_MIRROR );
	lpDevice->SetTextureStageState( 0, D3DTSS_ADDRESSV,  D3DTADDRESS_MIRROR );
	lpDevice->SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS,TRUE);
}

void CCubeMap::RestoreRenderStates(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!Supported)return;
	if (!Config.reflections)return;
	lpDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_PASSTHRU );
	lpDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
	lpDevice->SetRenderState(D3DRENDERSTATE_NORMALIZENORMALS,FALSE);
}
*/






void CSphereMap::ApplySphereMap(const LPDIRECT3DDEVICE7 lpDevice,D3DVERTEX* vertices,const int numvertices)
{
	D3DMATRIX matWV;
	lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD,&matWV);

	const float m11=matWV._11,m21=matWV._21,m31=matWV._31;
	const float m12=matWV._12,m22=matWV._22,m32=matWV._32;
	const float m13=matWV._13,m23=matWV._23,m33=matWV._33;

	for (WORD i=0;i<numvertices;i++)
	{
		const float nx=vertices[i].nx;
		const float ny=vertices[i].ny;
		const float nz=vertices[i].nz;

		vertices[i].tu=0.5f*(1.0f+(nx*m11+ny*m21+nz*m31));
		vertices[i].tv=0.5f*(1.0f-(nx*m12+ny*m22+nz*m32));
	}
}