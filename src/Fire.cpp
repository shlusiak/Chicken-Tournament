/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "objects.h"
#include "sounds.h"
#include "fire.h"
#include "game.h"
#include "gibs.h"
#include "config.h"





CFire::CFire(const UID vref,const float vspawnradius)
:CBlood(D3DVECTOR(0,0,0)),spawnradius(vspawnradius),ref(vref),nextspawn(0.0f),sound(NULL)
{ }

CFire::~CFire()
{
	if (sound)
	{
		sound->Stop();
		sound->Remove();
	}
}

BOOLEAN CFire::Init()
{
	CBlood::Init();

	for (int i=0;i<numparticel;i++)
		Particel[i].size=0.0f;

	CGib* g=(CGib*)chain->Find(ref);
	game->AddSound(sound=new CSound(game->lpBuffers[34],FALSE));
	sound->Set3DParameters(g->pos);
	sound->Play(DSBPLAY_LOOPING);
	return TRUE;
}

void CFire::Spawn()
{
	CGib* g=(CGib*)chain->Find(ref);

	for (int i=0;i<numparticel;i++)if (Particel[i].size==0.0f)
	{
		Particel[i].size=0.15f;
		Particel[i].pos=g->pos+D3DVECTOR(randf()-0.5f,randf()-0.5f,randf()-0.5f)*spawnradius*2.0f;
		Particel[i].liegt=FALSE;
		Particel[i].speed=D3DVECTOR(randf()-0.5f,randf()-0.5f,randf()-0.5f)*2.5f;

		if (i==numparticel-1)Particel[0].size=0.0f;
		else Particel[i+1].size=0.0f;
		return;
	}
}

void CFire::Execute(float elapsed)
{
	CParticleSystem::Execute(elapsed);

	CGib* g=(CGib*)chain->Find(ref);
	if (g==NULL)
	{
		Remove();
		return;
	}

	sound->Set3DParameters(g->pos);

	// Neue Partikel spawnen
	nextspawn-=elapsed;
	while (nextspawn<0.0f)
	{
		Spawn();
		if ((rand()%2)==0)Spawn();
		nextspawn+=(0.015f+randf()*0.02f)/game->gorelevel;
	}

	// Partikel bewegen

	for (int i=0;i<numparticel;i++)if (Particel[i].size!=0.0f)
	{
		const float auftrieb=5.0f;
		const float lw=powf(0.16f,elapsed);

		Particel[i].pos+=(Particel[i].speed*elapsed);

		Particel[i].speed.y+=(elapsed*auftrieb);

		Particel[i].speed.x*=lw;
		Particel[i].speed.z*=lw;
		Particel[i].speed.y*=lw;

		switch(i%4)
		{
		case 0:Particel[i].rot+=elapsed*13.0f;
			break;
		case 1:Particel[i].rot-=elapsed*13.0f;
			break;
		case 2:Particel[i].rot+=elapsed*7.0f;
			break;
		case 3:Particel[i].rot-=elapsed*7.0f;
			break;
		}

		if (Particel[i].size<0.8f)Particel[i].size+=elapsed*0.70f;
		else Particel[i].size+=elapsed*0.4f;

		if (Particel[i].size>0.95f)Particel[i].size=0.0f;

		if (!world->IsInside(Particel[i].pos))Particel[i].size=0.0f;
	}
}

void CFire::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);

		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);

		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCCOLOR);

		lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,0xFF);
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	lpDevice->SetTexture(0,game->lpTexture[48]);
	CParticleSystem::Draw(lpDevice);

	lpDevice->SetTexture(0,NULL);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	}
}
