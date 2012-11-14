/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "objects.h"
#include "particlesystem.h"
#include "config.h"


const int vertexpacken=10;	// 10 Partikel -> 10*6 Vertices = 60 V


CParticleSystem::CParticleSystem(const D3DVECTOR vcenter)
:center(vcenter),Particel(NULL),numparticel(0),time(0),maxtime(0),lpVertices(NULL)
{
	id=MakeUID(IDEffect);
}

CParticleSystem::~CParticleSystem()
{
	if (lpVertices)delete[] lpVertices;
	if (Particel)delete[] Particel;
}

void CParticleSystem::Execute(float elapsed)
{
	time+=elapsed;
	if (time>maxtime)
		Remove();
}

BOOLEAN CParticleSystem::Init()
{
	Particel=new TParticel[numparticel];
	lpVertices=new D3DLVERTEX[vertexpacken*6];
	ZeroMemory(&Particel[0],sizeof(TParticel)*numparticel);

	for (int i=0;i<vertexpacken;i++)
	{
		const D3DCOLOR c=color();
		lpVertices[i*6+0]=D3DLVERTEX(D3DVECTOR(0,0,0),c,0,1.0f,0.0f);
		lpVertices[i*6+1]=D3DLVERTEX(D3DVECTOR(0,0,0),c,0,0.0f,0.0f);
		lpVertices[i*6+2]=D3DLVERTEX(D3DVECTOR(0,0,0),c,0,1.0f,1.0f);
		lpVertices[i*6+3]=D3DLVERTEX(D3DVECTOR(0,0,0),c,0,0.0f,0.0f);
		lpVertices[i*6+4]=D3DLVERTEX(D3DVECTOR(0,0,0),c,0,1.0f,1.0f);
		lpVertices[i*6+5]=D3DLVERTEX(D3DVECTOR(0,0,0),c,0,0.0f,1.0f);
	}

	return TRUE;
}

void CParticleSystem::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	// Vertices vorbereiten
	D3DVECTOR rightVect,upVect;
	D3DMATRIX mat,m;

	lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW,&mat);
	D3DUtil_SetTranslateMatrix(m,center);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	D3DVECTOR rv=Normalize(D3DVECTOR(mat._11,mat._21,mat._31));
	D3DVECTOR uv=Normalize(D3DVECTOR(mat._12,mat._22,mat._32));
	D3DVECTOR fv=Normalize(D3DVECTOR(mat._13,mat._23,mat._33));

	// Alles vorbereiten und rendern
	for (int i=0;i<numparticel/vertexpacken+1;i++)
	{
		const int max=i<(numparticel/vertexpacken)?(vertexpacken):(numparticel%vertexpacken);
		if (max==0)continue;

		for (int j=0;j<max;j++)
		{
			const int pnum=i*vertexpacken+j;
			const float asdf=Particel[pnum].size*0.5f;
			rightVect=rv*asdf;
			upVect=uv*asdf;

			if (Particel[pnum].rot)
			{
				D3DUtil_SetRotationMatrix(m,fv,Particel[i].rot);
				D3DMath_VectorMatrixMultiply(rightVect,rightVect,m);
				D3DMath_VectorMatrixMultiply(upVect,upVect,m);
			}

			SetVector(lpVertices[j*6+0],Particel[pnum].pos-rightVect+upVect);
			SetVector(lpVertices[j*6+1],Particel[pnum].pos+rightVect+upVect);
			SetVector(lpVertices[j*6+2],Particel[pnum].pos-rightVect-upVect);
			SetVector(lpVertices[j*6+3],Particel[pnum].pos-rightVect-upVect);
			SetVector(lpVertices[j*6+4],Particel[pnum].pos+rightVect+upVect);
			SetVector(lpVertices[j*6+5],Particel[pnum].pos+rightVect-upVect);
		}
		lpDevice->DrawPrimitive(D3DPT_TRIANGLELIST,D3DFVF_LVERTEX,lpVertices,max*6,0);
	}

	// Aufräumen

	D3DUtil_SetIdentityMatrix(m);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);
}