/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "items.h"
#include "game.h"
#include "collision.h"
#include "config.h"



const PCHAR modelnames[]=
{"Medic","Schachtel"};


CItem::CItem()
:angy(randf()*g_PI),pos(D3DVECTOR(0,0,0)),subid(0),time(0),model(NULL)
{
	id=MakeUID(IDItem);
}

CItem::CItem(const DWORD vsubid,const PCHAR modelname)
:angy(randf()*g_PI),pos(world->Rand(4.0f)),subid(vsubid),time(0),model(game->models->FindObject(modelname))
{
	id=MakeUID(IDItem);
}

void CItem::Execute(float elapsed)
{
	time+=elapsed;

	angy+=elapsed*2.1f;

	if (time>120.0f)Remove();
}

void CItem::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (model==NULL)return;
	if (!CBoundingTube::IsSphereVisible(pos,1.0f))return;

	D3DMATRIX m,m2;

	D3DUtil_SetTranslateMatrix(m,pos);
	D3DUtil_SetRotateYMatrix(m2,angy);
	D3DMath_MatrixMultiply(m,m2,m);

	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	model->Render(lpDevice,FALSE);
}

void CItem::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	DRAWNAME(0.5f);
}

void CItem::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,subid,0,&pos,sizeof(pos),TRUE);
}

void CItem::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		id=data->id;
		subid=data->p2;

		pos=*(D3DVECTOR*)data->userdata;
		model=game->models->FindObject(modelnames[subid]);
	}
}

void CItem::NewWorld()
{
	pos.y=world->HeightAtPoint(pos.x,pos.z);
}






void CItemSpawner::Spawn()
{
	const DWORD subid=rand()%2;
	const PCHAR model=modelnames[subid];


#ifdef _DEBUG
	CHAR c[100];
	sprintf(&c[0],"Item Spawned: %s!",model);
	DebugOut(&c[0]);
#endif

	CItem* item;
	game->chain->Add(item=new CItem(subid,model));

	Send(DPID_ALLPLAYERS,NM_CREATEOBJECT,item->id,0,0,0,NULL,0,TRUE);
	item->SendData(DPID_ALLPLAYERS);
}