/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _SOUNDS_INCLUDED_
#define _SOUNDS_INCLUDED_

#include "objects.h"


class CSound:public CObject
{
private:
	const BOOLEAN canfree;
	BOOLEAN oldon;
	DWORD oldflags;
	BYTE tag;
public:
	LPDIRECTSOUNDBUFFER buf;
	LPDIRECTSOUND3DBUFFER buf3d;

	CSound():canfree(TRUE),buf(NULL),buf3d(NULL),oldon(FALSE),oldflags(0),tag(0) { id=MakeUID(IDSound); }
	CSound(const BOOLEAN vcanfree):canfree(vcanfree),buf(NULL),buf3d(NULL),oldon(FALSE),oldflags(0),tag(0) { id=MakeUID(IDSound); }
	CSound(const LPDIRECTSOUNDBUFFER source,const BOOLEAN vcanfree):canfree(vcanfree),buf(NULL),buf3d(NULL),oldon(FALSE),oldflags(0),tag(0) { id=MakeUID(IDSound); Create(source); }
	
	virtual ~CSound();

	virtual void Execute(float elapsed);

	void Create(const LPDIRECTSOUNDBUFFER source);
	const BOOLEAN Create2D(const LPDIRECTSOUNDBUFFER source);
	void Play(const DWORD flags=0);
	void Stop() {	if (buf)buf->Stop(); }

	void Set3DParameters(const D3DVECTOR pos,const D3DVECTOR speed=D3DVECTOR(0,0,0));
	void SetFrequency(const DWORD freq) {	if (buf)buf->SetFrequency(freq); }

	const BOOLEAN IsPlaying()const;
	void Toggle(const BOOLEAN on);
	void SetTag(const BYTE vtag) {tag=vtag;}
	const BYTE GetTag()const {return tag;}
};



#endif