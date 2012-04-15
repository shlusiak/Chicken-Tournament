/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "camera.h"
#include "game.h"




void CCamera::OptimizeCamera(D3DMATRIX *view)
{
	D3DVECTOR pp=GetCameraPos(view);

	const float epsilon=0.05f;
	if ((!world->IsInside(pp))||(pp.y<world->HeightAtPoint(pp.x,pp.z)+epsilon))
	{
		world->MakeInside(pp);
		float h=world->HeightAtPoint(pp.x,pp.z);
		if (pp.y<h+epsilon)pp.y=h+epsilon;

		D3DMATRIX m;
		D3DVECTOR *p=(D3DVECTOR*)&m.m[3];

		D3DMath_MatrixInvert(m,*view);
		*p=pp;
		D3DMath_MatrixInvert(*view,m);
	}
}

const D3DVECTOR CCamera::GetCameraPos(D3DMATRIX *view)
{
	D3DMATRIX m;
	D3DMath_MatrixInvert(m,*view);

	D3DVECTOR playerpos;
	D3DMath_VectorMatrixMultiply(playerpos,D3DVECTOR(0,0,0),m);
	return playerpos;
}