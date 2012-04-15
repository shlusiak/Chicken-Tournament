/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "tree.h"
#include "game.h"
#include "config.h"


const float w=3.5f;
const float h=4.5f;


CTree::CTree(const float x,const float y,const float a)
:CObject(),angy(a)
{
	pos=D3DVECTOR(x,world->HeightAtPoint(x,y),y);

	size=randf()*1.0f+0.5f;
	box=new CBoundingTube(pos,0.3f*size,h*size);
	id=MakeUID(IDTree);
}

BOOLEAN CTree::Init()
{
	const D3DVECTOR n=D3DVECTOR(0,0,1);

	vertices[0]=D3DVERTEX(D3DVECTOR(-w*0.5f,h,0)*size,n,0,0);
	vertices[1]=D3DVERTEX(D3DVECTOR(-w*0.5f,0,0)*size,n,0,1);
	vertices[2]=D3DVERTEX(D3DVECTOR(w*0.5f,h,0)*size,n,1,0);
	vertices[3]=D3DVERTEX(D3DVECTOR(w*0.5f,0,0)*size,n,1,1);

	texturenr=rand()%4;

	return TRUE;
}

void CTree::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!box->IsVisible())return;

	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);

	DWORD old;
	lpDevice->GetRenderState(D3DRENDERSTATE_CULLMODE,&old);
	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	if (Config.alpha)
	{
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,192);
	}

	int tn;
	switch(texturenr)
	{
	case 0:tn=4;
		break;
	case 1:tn=35;
		break;
	case 2:tn=36;
		break;
	case 3:tn=37;
		break;
	}
	if (game->lpTexture[tn]==NULL)tn=4;	// Standardbaum wählen

	lpDevice->SetTexture(0,game->lpTexture[tn]);


	const BYTE num=2;
	static WORD indices[]={0,1,2,1,3,2};

	D3DMATRIX m,m2;
	D3DUtil_SetTranslateMatrix(m,pos);
	D3DUtil_SetRotateYMatrix(m2,angy);
	D3DMath_MatrixMultiply(m,m2,m);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	D3DUtil_SetRotateYMatrix(m2,g_PI/num);

	for (int i=0;i<num;i++)
	{
		lpDevice->DrawIndexedPrimitive(D3DPT_TRIANGLELIST,D3DFVF_VERTEX,vertices,4,indices,6,0);
		lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);
	}

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,old);
}

void CTree::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	DRAWNAME(h*size);
}

void CTree::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,FloatToLong(angy),FloatToLong(size),&pos,sizeof(pos),TRUE);
}

void CTree::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		pos=*(D3DVECTOR*)data->userdata;
		box->SetPos(pos);
		angy=LongToFloat(data->p2);
		size=LongToFloat(data->p3);
	}
	DebugOut("CTree::ReceiveData();");
}

void CTree::NewWorld()
{
	pos.y=world->HeightAtPoint(pos.x,pos.z);

	box->SetPos(pos);
}