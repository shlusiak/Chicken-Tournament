/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "gibs.h"
#include "game.h"
#include "collision.h"
#include "fire.h"
#include "config.h"





CGib::CGib(const PCHAR modelname,const D3DVECTOR vpos,const float vradius)
:pos(vpos),model(game->models->FindObject(modelname)),ang(D3DVECTOR(randf()-0.5f,randf()-0.5f,randf()-0.5f)*2.0f),angspeed(D3DVECTOR(0,0,0)),liegt(FALSE),radius(vradius) 
{}

BOOLEAN CGib::Init()
{
	if (model==NULL)Remove();
//	box=new CBoundingTube(pos,radius,2.0f*radius);

	const float rotspeed=8.0f;
	angspeed=D3DVECTOR(randf()*rotspeed+0.5f,randf()*rotspeed+0.5f,randf()*rotspeed/2.0f+-0.5f);
	if (rand()%2==0)angspeed.x=-angspeed.x;
	if (rand()%2==0)angspeed.y=-angspeed.y;

	const float s=3.6f+game->gorelevel/15.0f;
	const float winkelh=randf()*2*g_PI;
	const float winkelv=randf()*g_PI/2.0f;

	speed.x=cosf(winkelh)*s*cosf(winkelv);
	speed.z=sinf(winkelh)*s*cosf(winkelv);
	speed.y=sinf(winkelv)*s*1.4f;

	return TRUE;
}

void CGib::Execute(float elapsed)
{
	if (liegt)return;
	ang+=angspeed*elapsed;
	pos+=speed*elapsed;

	speed.y-=game->gravity*elapsed;

	const float h=world->HeightAtPoint(pos.x,pos.z);
	if (pos.y<h+0.2f)
	{
		pos.y=h+0.2f;
		speed.y=-speed.y*0.5f;
		speed.x*=0.8f;
		speed.z*=0.8f;
		angspeed*=0.8f;

		if (abs(speed.y)<0.7f)liegt=TRUE;
	}
	world->MakeInside(pos);
//	box->SetPos(pos);
}

void CGib::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!CBoundingTube::IsSphereVisible(pos,radius*2.0f))return;
	D3DMATRIX matrix,m;

	D3DUtil_SetTranslateMatrix(matrix,pos.x,pos.y,pos.z);

	D3DUtil_SetRotateYMatrix(m,ang.y);
	D3DMath_MatrixMultiply(matrix,m,matrix);
		
	D3DUtil_SetRotateXMatrix(m,ang.x);
	D3DMath_MatrixMultiply(matrix,m,matrix);


	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);
	model->Render(lpDevice,FALSE);
}





CGibs::CGibs(const D3DVECTOR vpos,const BYTE vtype)
:pos(vpos+D3DVECTOR(0,0.25f,0)),time(0),type(vtype)
{ }

BOOLEAN CGibs::Init()
{
	if (type==1)
	{
		Add(new CGib("Gib1",pos,0.26f));
		Add(new CGib("Gib2",pos,0.13f));
		Add(new CGib("Gib2",pos,0.13f));
		Add(new CGib("Gib3",pos,0.60f));
		Add(new CGib("Gib4",pos,0.38f));
		Add(new CGib("Gib5",pos,0.26f));
	}
	if (type==2)
	{
		CGib* g;
		Add(g=new CGib("Gib6",pos,0.53f));
		if (Config.fire)Add(new CFire(g->id,0.2f));
	}

	CSound *s=new CSound(game->lpBuffers[19],TRUE);
	s->Set3DParameters(pos);
	s->SetFrequency(9000+rand()%4000);
	s->Play();
	game->AddSound(s);

	return TRUE;
}

void CGibs::Execute(float elapsed)
{
	if ((time+=elapsed)>8.0f)
	{
		Remove();
		return;
	}
	CObjChain::Execute(elapsed);
}