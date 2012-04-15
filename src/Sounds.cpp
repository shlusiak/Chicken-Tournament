/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "sounds.h"
#include "resources/resources.h"
#include "game.h"
#include "config.h"





CSound::~CSound()
{
	if (IsPlaying())Stop();

	if (buf3d)buf3d->Release();
	if (buf)buf->Release();
}

void CSound::Execute(float elapsed)
{
	if ((canfree)&&((buf==NULL)||(!IsPlaying())))Remove();
}

void CSound::Create(const LPDIRECTSOUNDBUFFER source)
{
	SaveRelease(buf3d);
	SaveRelease(buf);

	if ((!Config.soundon)&&(canfree))
	{	// Wenn Sounds aus ist und das nur ein temporärer Sound ist, direkt löschen
		Remove();
		return;
	}

	if ((source==NULL)||(game->lpDS==NULL))
	{
		if (canfree)Remove();
		return;
	}
	
	game->lpDS->DuplicateSoundBuffer(source,&buf);
	buf3d=CreateSound3DBuffer(game->lpDS,buf);
}

const BOOLEAN CSound::Create2D(const LPDIRECTSOUNDBUFFER source)
{
	SaveRelease(buf3d);
	SaveRelease(buf);

	if ((source==NULL)||(game->lpDS==NULL))
	{
		if (canfree)Remove();
		return FALSE;
	}

	game->lpDS->DuplicateSoundBuffer(source,&buf);
	return TRUE;
}

void CSound::Play(const DWORD flags)
{
	if (buf==NULL)return;
	oldflags=flags;
	oldon=TRUE;
	if (Config.soundon)
	{
		buf->Play(0,0,flags);
	}else{
		if ((flags&DSBPLAY_LOOPING)==0)oldon=FALSE;
		if (canfree)Remove();
	}
}

void CSound::Set3DParameters(const D3DVECTOR pos,const D3DVECTOR speed)
{
	if (buf3d==NULL)return;
	DS3DBUFFER dsBP;
	ZeroMemory(&dsBP,sizeof(dsBP));
	dsBP.dwSize=sizeof(dsBP);

	buf3d->GetAllParameters(&dsBP);

	dsBP.vPosition=pos;

	dsBP.vVelocity=speed;

	dsBP.dwMode=DS3DMODE_NORMAL;
	dsBP.flMinDistance=DS3D_DEFAULTMINDISTANCE;
	dsBP.flMaxDistance=DS3D_DEFAULTMAXDISTANCE;

	buf3d->SetAllParameters(&dsBP,DS3D_DEFERRED);
}

const BOOLEAN CSound::IsPlaying()const
{
	if (buf==NULL)return FALSE;
	DWORD s;
	buf->GetStatus(&s);
	return ((s&DSBSTATUS_PLAYING)!=0);
}

void CSound::Toggle(const BOOLEAN on)
{
	if (!on)
	{
		oldon=IsPlaying();
		Stop();
		if ((oldflags&DSBPLAY_LOOPING)==0)oldon=FALSE;
		if (canfree)Remove();
	}else{
		if (oldon)Play(oldflags);
	}
}

