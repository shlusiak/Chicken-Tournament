/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "gras.h"
#include "config.h"
#include "world.h"



const int MAX_GRAS_PRO_VB=4000;
const int MAXIMUM_GRAS_OBJECTS=50000;



CGras::CGras()
:lpVertices(NULL),vertexbuffers(0),Indices(NULL),vertexnum(NULL)
{ }

CGras::~CGras()
{
	Invalidate();
}

void CGras::InitGras(int menge)
{
	if (menge>MAXIMUM_GRAS_OBJECTS)menge=MAXIMUM_GRAS_OBJECTS;
	Invalidate();

	Indices=new WORD[MAX_GRAS_PRO_VB*6];

	for (int i=0;i<MAX_GRAS_PRO_VB*6;i+=6)
	{
		const int v1=i/6*4;
		Indices[i+0]=v1;
		Indices[i+1]=v1+1;
		Indices[i+2]=v1+2;
		Indices[i+3]=v1;
		Indices[i+4]=v1+2;
		Indices[i+5]=v1+3;
	}

	vertexbuffers=menge/MAX_GRAS_PRO_VB+1;

	lpVertices=new LPDIRECT3DVERTEXBUFFER7[vertexbuffers];
	ZeroMemory(lpVertices,sizeof(LPDIRECT3DVERTEXBUFFER7)*vertexbuffers);
	vertexnum=new WORD[vertexbuffers];

#ifdef _DEBUG
	DebugOut("Initialisiere Gras:");

	CHAR c[100];
	sprintf(&c[0],"  Graspartikel: %d",menge);
	DebugOut(&c[0]);
	sprintf(&c[0],"  Grasvertices: %d",menge*4);
	DebugOut(&c[0]);
	sprintf(&c[0],"  Vertexbuffer: %d",vertexbuffers);
	DebugOut(&c[0]);
#endif

	for (int i=0;i<vertexbuffers;i++)
	{
		CreateGras(i,menge>MAX_GRAS_PRO_VB?MAX_GRAS_PRO_VB:menge);

		menge-=MAX_GRAS_PRO_VB;
		if (menge<=0)
		{
			vertexbuffers=i+1;
			break;
		}
	}
	return;
}

void CGras::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (lpVertices==NULL)return;
	if (game->lpTexture[40]==NULL)
	{	// Gras Textur nachladen
		game->lpTexture[40]=CreateTextureFromResource(lpDevice,game->lpDD,NULL,"grass.bmp",4,FALSE,TRUE); 
		MakeTransparent(game->lpTexture[40],FALSE);	// Gras
	}

	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);

	DWORD old;
	lpDevice->GetRenderState(D3DRENDERSTATE_CULLMODE,&old);
	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);
	if (Config.alpha)
	{
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,192);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	lpDevice->SetTexture(0,game->lpTexture[40]);

	D3DMATRIX m;
	D3DUtil_SetIdentityMatrix(m);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	for (int i=0;i<vertexbuffers;i++)
		lpDevice->DrawIndexedPrimitiveVB(D3DPT_TRIANGLELIST,lpVertices[i],0,vertexnum[i],Indices,vertexnum[i]/4*6,0);

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,old);
}

void CGras::CreateGras(int index,int anzahl)
{
//#define GRAS_KI

	const float sizex=1.5f/2.0f;
	const D3DVECTOR height=D3DVECTOR(0,0.475f,0);

	vertexnum[index]=anzahl*4;

	lpVertices[index]=CreateVertexBuffer(game->lpD3D,&game->d3ddesc,D3DFVF_VERTEX,anzahl*4,TRUE);
	
    D3DVERTEX* v;
    lpVertices[index]->Lock( DDLOCK_WAIT, (VOID**)&v,NULL);
    {
		D3DVECTOR pos,width,n;
		int y;
		float faktor;
		const int maxloops=25;
		WORD i;

#ifdef GRAS_KI
		D3DVECTOR *points;
		const int NumPoints=int(world->GetWidth()*world->GetHeight()/10.0f);
		if (NumPoints>0)
		{
			points=new D3DVECTOR[NumPoints];
			for (i=0;i<NumPoints;i++)
				points[i]=world->Rand(0.2f);
		}
#endif

		for (i=0;i<anzahl;i++)
		{
			float alpha=randf()*g_PI*2.0f;
			y=0;
			do
			{
				y++;
				pos=world->Rand(0.2f);

				faktor=1.0f;
#ifdef GRAS_KI
				if (NumPoints>0)
				{
					faktor=0.0f;
					for (int warscht=0;warscht<NumPoints;warscht++)
						faktor+=powf(sqr(pos.x-points[warscht].x)+sqr(pos.z-points[warscht].z)+1,0.5f);
					faktor=1.0f/(faktor/NumPoints);
				}
#endif

				if (pos.y<0.15f)
					faktor*=0.15f;
				if (y>maxloops)faktor+=0.2f;
			}while (randf()>faktor);

			width=D3DVECTOR(cosf(alpha),0,sinf(alpha));
			n=D3DVECTOR(width.z,0,width.x);
			width*=sizex*(0.5f+randf()*1.0f);
			const float hf=0.5f+randf()*1.35f;

			v[i*4+0]=D3DVERTEX(pos+width+height*hf,n,1,0);
			v[i*4+1]=D3DVERTEX(pos-width+height*hf,n,0,0);
			v[i*4+2]=D3DVERTEX(pos-width,n,0,1);
			v[i*4+3]=D3DVERTEX(pos+width,n,1,1);
		}
#ifdef GRAS_KI
		if (NumPoints>0)delete[] points;
#endif
    }
    lpVertices[index]->Unlock();
	lpVertices[index]->Optimize(game->lpDevice,0);
}

void CGras::Invalidate()
{
	if (lpVertices)
	{
		for (int i=0;i<vertexbuffers;i++)
			SaveRelease(lpVertices[i]);
		delete[] lpVertices;
		lpVertices=NULL;
	}
	if (Indices)delete[] Indices;
	if (vertexnum)delete[] vertexnum;
	Indices=NULL;
	vertexnum=NULL;
	vertexbuffers=0;
}

#ifdef _DEBUG
int CGras::NumFaces()
{
	int x=0;
	for (int i=0;i<vertexbuffers;i++)
		x+=vertexnum[i]/2;
	return x;
}
#endif

void CGras::Restore()
{
	// Grastextur entfernen, wird bei Bedarf neu erstellt
	SaveRelease(game->lpTexture[40]);
}