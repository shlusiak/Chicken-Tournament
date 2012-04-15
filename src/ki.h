/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _KI_INCLUDED_
#define _KI_INCLUDED_

#include "bauer.h"


class CChicken;

class CChickenKI
{
protected:
	CChicken* owner;

	BYTE walkingdir;
	float walkingtime;

	// Konstanten für motorische Fähigkeiten
	const float omega;
	const float escapespeed;
	const float movespeed;
	const float walkspeed;
	const float bauerentfernung;
	const float stallentfernung;
	const float maxattackrange;
	const float minescaperange;


	CStall* FindNextStall(const float maxdistance)const;
	CBauer* FindNextBauer(const float maxdistance)const;
	CItem* FindNextItem(const DWORD subid,const float maxdistance)const;

	void Move(const float speed,const float elapsed);
	const float RotateTo(const D3DVECTOR viewpoint,const float elapsed);

	void MoveTo(const D3DVECTOR dest,const float elapsed);
	void EscapeFrom(const D3DVECTOR from,const float elapsed);

	void AttackFarmer(CBauer* bauer,const float elapsed);
	void Sit(CBauer* bauer,const float elapsed);
	void Walk(const float elapsed);
public:
	CChickenKI(CChicken* vowner);

	void Think(const float elapsed);
};





#endif