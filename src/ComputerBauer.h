/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _COMPUTERBAUER_INCLUDED_
#define _COMPUTERBAUER_INCLUDED_


#include "bauer.h"
#include "chicken.h"





class CComputerBauer:public CBauer
{
protected:
	const float GetWinkel(const float aktw,const D3DVECTOR lookat)const;
	const float RotateTo(const D3DVECTOR lookat,const float elapsed);
	const float RotateTo(const float winkel,const float elapsed);
	const float GetSchussY(const float x,const float alpha)const;

	void HandleDrescher(const float elapsed);
	void HandlePlasmaCannon(const float elapsed);
	void HandleGartenkralle(const float elapsed);
	void HandleGolfer(const float elapsed);

	const BOOLEAN SearchWeapon(const float elapsed);
	void HandleWeapon(const float elapsed);

	CChicken* SearchNextChicken()const;

	virtual void Revive();
public:
	CComputerBauer();

	virtual void Execute(float elapsed);
};








#endif