/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _CHICKEN_INCLUDED_
#define _CHICKEN_INCLUDED_

#include "objects.h"
#include "sounds.h"
#include "collision.h"
#include "player.h"
#include "stall.h"
#include "ki.h"
#include "multiplayer.h"
#include "animation.h"


const chicken_maxeier=100;


enum SPLAT_TYPE
{
	SPLAT_NORMAL,SPLAT_FRY
};


class CPlasmaBall;

class CChicken:public CPlayer
{
protected:
	friend CChickenKI;
	friend CPlasmaBall;

	CChickenKI* theKI;

	D3DVECTOR ang;
	float ax;

	CD3DFileObject* mesh[3];
	CD3DFileObject* morphtarget1,*morphtarget2;
	float dir,time,morphphase;
	CSound* scaresound;
	float lastthrow,produktion;
	BOOLEAN produziere;

	struct FLUG
	{
		BOOLEAN fliege;
		D3DMATRIX matrix;
		float power,throttle,animationphase;
		float yaw,pitch,roll;
	}Flug;
	float canthrow,maxfallspeed;
	struct ZAPPEL
	{
		float phase;
		int von,nach;

		void Execute(const float elapsed) { phase-=elapsed*8.0f; if (phase<0.0f) { phase=1.0f; int i; do { i=rand()%4; }while (i==nach); von=nach; nach=i; } }
	}Zappel;

	D3DVECTOR schlagspeed;
	D3DVECTOR abschlagpos;
	BOOLEAN golfergebnis;
	CMessageChain *positionmessage,*flagsmessage;
	static CMorpher* morpher1,*morpher2;

	virtual void KI(const float elapsed);
	virtual void Revive();
	virtual const float GetDeadTime()const;
	void Throw(const float ax,const int num,UID *ids);
	const int GetHuhnType()const;
	void SendPosition();
	void SendFlags();
	void GolfEnd();
	const BOOLEAN PickupItem(CItem* item);

	void Fliege(const float elapsed);
	void StartFlug();
	void EndeFlug();
public:
	const CObject* bound;
	UID Golfspieler;
	CBoundingTube* box;
	BYTE eier;
	const CStall *install;

	CChicken();
	virtual ~CChicken();

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawShadow(LPDIRECT3DDEVICE7 lpDevice);
	void Splat(const SPLAT_TYPE art);	// Huhn ist tot
	void Fork(const CObject* b);	// Spießt das Huhn auf eine Gartenkralle auf
	void Schlag(const CObject* g);	// Schlägt das Huhn weg (vom Golfschläger)
	void Blast(const CObject* p,const D3DVECTOR pressure); // Wirbelt Huhn durch Luftdruck herum
	virtual void Notify(PNETWORKDATA data);
	virtual void Unbind();
};



#endif