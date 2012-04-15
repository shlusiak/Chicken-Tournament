/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "sprites.h"
#include "resources\\resources.h"
#include "game.h"
#include "config.h"





void CSprite::Clear()
{
	if (lpImages)
	{
		for (int i=0;i<anzahl;i++)SaveRelease(lpImages[i]);
		delete lpImages;
		lpImages=NULL;
	}
	anzahl=0;
}

const BOOLEAN CSprite::Load(const PCHAR name)
{
	Clear();

	const HPACKAGE package=LoadPackage(name);
	if (package.p==NULL)return FALSE;

	DWORD size;
	// Erst Anzahl in Package testen
	LPVOID data=LoadNextPackageData(&package,NULL,&size);
	anzahl=0;
	while (data)
	{
		anzahl++;
		data=LoadNextPackageData(&package,data,&size);
	}
	if (anzahl==0)
	{
		FreePackage(&package);
		return FALSE;
	}
	// Speicher reservieren

	lpImages=new LPDIRECTDRAWSURFACE7[anzahl];

	// Dann Bilder laden
	data=LoadNextPackageData(&package,NULL,&size);
	anzahl=0;
	while (data)
	{
		HBITMAP bitmap=LoadPictureFromMemory(data,size);
		if (bitmap==0)
		{
			FreePackage(&package);
			return FALSE;
		}

		lpImages[anzahl]=CreateTextureFromBitmap(game->lpDevice,game->lpDD,NULL,bitmap,3,TRUE,TRUE,FALSE);
		if (lpImages[anzahl]!=NULL)
			MakeTransparent(lpImages[anzahl],FALSE);

		DeleteObject(bitmap);
 
		anzahl++;
		data=LoadNextPackageData(&package,data,&size);
	}

	FreePackage(&package);

#ifdef _DEBUG
	CHAR c[200];
	sprintf(&c[0],"Sprite geladen: %s",name);
	DebugOut(&c[0]);
#endif

	return TRUE;
}

void CSprite::DrawSmooth(LPDIRECT3DDEVICE7 lpDevice,const D3DVECTOR pos,const float w,const float h,const DWORD color,const int spritenr)
{
	D3DVECTOR rightVect,upVect;
	D3DMATRIX mat;
	{
		lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW,&mat);
		rightVect=Normalize(D3DVECTOR(mat._11,mat._21,mat._31))*w*0.5f;
		upVect=Normalize(D3DVECTOR(mat._12,mat._22,mat._32))*h*0.5f;
	}

	const D3DVECTOR n=D3DVECTOR(0,0,0);
	D3DLVERTEX verts[4]=
	{
		D3DLVERTEX(pos-rightVect+upVect, color,0, 1.0f, 0.0f),
		D3DLVERTEX(pos+rightVect+upVect, color,0, 0.0f, 0.0f),
		D3DLVERTEX(pos-rightVect-upVect, color,0, 1.0f, 1.0f),
		D3DLVERTEX(pos+rightVect-upVect, color,0, 0.0f, 1.0f)
	};

	D3DUtil_SetIdentityMatrix(mat);
	
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&mat);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);

		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
		lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_TEXTURE);
		lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,           D3DTOP_SELECTARG1);
		lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,0x08);
	}

	lpDevice->SetTexture(0,GetSurface(spritenr));
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_LVERTEX,verts,4,0);
	lpDevice->SetTexture(0,NULL);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	}
}

void CSprite::DrawAlpha(LPDIRECT3DDEVICE7 lpDevice,const D3DVECTOR pos,const float w,const float h,const DWORD color,const int spritenr)
{
	D3DVECTOR rightVect,upVect;
	D3DMATRIX mat;
	{
		lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW,&mat);
		rightVect=Normalize(D3DVECTOR(mat._11,mat._21,mat._31))*w*0.5f;
		upVect=Normalize(D3DVECTOR(mat._12,mat._22,mat._32))*h*0.5f;
	}

	D3DLVERTEX verts[4]=
	{
		D3DLVERTEX(pos-rightVect+upVect, color,0, 1.0f, 0.0f),
		D3DLVERTEX(pos+rightVect+upVect, color,0, 0.0f, 0.0f),
		D3DLVERTEX(pos-rightVect-upVect, color,0, 1.0f, 1.0f),
		D3DLVERTEX(pos+rightVect-upVect, color,0, 0.0f, 1.0f)
	};

	D3DUtil_SetIdentityMatrix(mat);
	
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&mat);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
		lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_TEXTURE);
		lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,           D3DTOP_SELECTARG1);
		lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,8);
	}


	lpDevice->SetTexture(0,GetSurface(spritenr));
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_LVERTEX,verts,4,0);
	lpDevice->SetTexture(0,NULL);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
	}
}



/*const BOOLEAN CSingleSprite::Load(const PCHAR name)
{
	Clear();

	anzahl=1;
	// Speicher reservieren

	lpImages=new LPDIRECTDRAWSURFACE7[anzahl];

	// Dann Bild laden
	HBITMAP bitmap=LoadPicture(name);
	if (bitmap==0)
		return FALSE;

	lpImages[0]=CreateTextureFromBitmap(game->lpDevice,game->lpDD,NULL,bitmap,3,TRUE,TRUE,FALSE);
	if (lpImages[0]!=NULL)
		MakeTransparent(lpImages[0],FALSE);

	DeleteObject(bitmap);

	return TRUE;
}
*/



void CSpriteAnimation::Execute(float elapsed)
{
	lastframe+=elapsed;
	time+=elapsed;
	if ((timeout>0.0f)&&(time>=timeout))Remove();
	if (lastframe>invfps)
	{
		lastframe-=invfps;
		current++;

		if (current>=game->lpSprites[spriteindex]->GetMax())
		{
			current=0;
			if (!loop)Remove();
		}
	}
}

void CSpriteAnimation::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
 	game->lpSprites[spriteindex]->DrawSmooth(lpDevice,pos,w,h,D3DRGB(1,1,1),current); 
}




