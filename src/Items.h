/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _ITEMS_INCLUDED_
#define _ITEMS_INCLUDED_


#include "objects.h"
#include "game.h"



class CItem:public CObject
{
protected:
	float angy,time;
	CD3DFileObject* model;
public:
	D3DVECTOR pos;
	DWORD subid;

	CItem();
	CItem(const DWORD vsubid,const PCHAR modelname);

	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);

	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void NewWorld();
};



const float spawntime=20.0f;


class CItemSpawner:public CObject
{
private:
	float lastspawn;
public:
	static void Spawn();
	CItemSpawner() {lastspawn=0.0f;}
	virtual void Execute(float elapsed)
	{
		if (!game->IsServer)return;
		lastspawn+=elapsed;
		if (lastspawn>spawntime)
		{
			lastspawn=0.0f;
			if (rand()%2==0)Spawn();
		}
	}
};







#endif