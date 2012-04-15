/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "golfer.h"
#include "game.h"
#include "player.h"
#include "chicken.h"
#include "bauer.h"
#include "tree.h"
#include "drescher.h"


const float schlagtimescale=1.5f;



BOOLEAN CGolfer::Init()
{
	pos=world->Rand(1.0f);
	box=new CBoundingTube(pos,0.07f,1.18f);

	mesh=game->models->FindObject("Golfer");

	parent=NULL;
	id=MakeUID(IDGolfer);
	schlag.phase=0.0f;
	schlag.getroffen=FALSE;
	ejecttime=0.0f;

	return TRUE;
}

CGolfer::~CGolfer()
{
	delete box;
}

void CGolfer::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if ((parent==NULL)&&(!box->IsVisible()))return;

	D3DMATRIX m=BerechneSchlag();

	lpDevice->SetTexture(0,NULL);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);
	mesh->Render(lpDevice,FALSE);
}

void CGolfer::Execute(float elapsed)
{
	if (ejecttime>0.0f)
	{
		ejecttime-=elapsed;
		if (ejecttime<0.0f)ejecttime=0.0f;
	}
	if (schlag.phase!=0.0f)
	{
		schlag.phase-=(elapsed*schlagtimescale);
		BerechneSchlag();
		if ((game->IsServer)&&(schlag.getroffen==FALSE)&&(schlag.phase>0.3f)&&(schlag.phase<0.5f))
		{
			CChicken* o=NULL;
			do
			{	// Hühner wegschleudern, wenn sie mit dem Golfschläger getroffen wurden
				o=(CChicken*)PerformHit(o);

				if (o==NULL)break;

				o->Schlag(parent);
				schlag.getroffen=TRUE;
				SendNotify(1,o->id);

				break;
			}while(o);
		}
		if (schlag.phase<0.0f)schlag.phase=0.0f;
	}
}

const D3DMATRIX CGolfer::BerechneSchlag()
{
	D3DMATRIX m,m2;
	if (parent==NULL)
	{
		D3DUtil_SetTranslateMatrix(m,pos.x,pos.y+0.035f,pos.z);

		return m;
	}

	CPlayer* p=(CPlayer*)parent;

	D3DUtil_SetTranslateMatrix(m,p->pos);
	D3DUtil_SetRotateYMatrix(m2,-p->ang.y);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetRotateXMatrix(m2,-p->ang.x);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetTranslateMatrix(m2,0.0f,-1.25f,0.2f);
	D3DMath_MatrixMultiply(m,m2,m);
  
	// Schläger um y-Achse drehen
	D3DUtil_SetRotateYMatrix(m2,-g_PI*0.5f-0.2f);
	D3DMath_MatrixMultiply(m,m2,m);

	// Privotpunkt auf Halter verschieben
	D3DUtil_SetTranslateMatrix(m2,0,1.2f,0);
	D3DMath_MatrixMultiply(m,m2,m);

	// Schlag schwenken
	float winkel=1.2f;
	if (schlag.phase!=0)
	{
		const float back=0.5f;
		const float front=1.1f;

		if (schlag.phase>0.8f)winkel=1.2f+(schlag.phase-1.0f)*5.0f*back;else
		if (schlag.phase>0.5f)winkel=1.2f-back;else
		if (schlag.phase>0.3f)winkel=1.2f-back-(schlag.phase-0.5f)*5.0f*(front+back);else
			winkel=1.2f+front-(0.3f-schlag.phase)/0.3f*(front);
	}

	D3DUtil_SetRotateZMatrix(m2,winkel);
	D3DMath_MatrixMultiply(m,m2,m);

	// Nach hinten kippen
	D3DUtil_SetRotateXMatrix(m2,0.45f);
	D3DMath_MatrixMultiply(m,m2,m);

	// Privotpunkt zurücksetzen
	D3DUtil_SetTranslateMatrix(m2,0,-1.2f,0);
	D3DMath_MatrixMultiply(m,m2,m);

	pos.x=m.m[3][0];
	pos.y=m.m[3][1];
	pos.z=m.m[3][2];

	box->SetPos(pos);

	return m;
}

CObject* CGolfer::PerformHit(CObject* akt)const
{
	// Check for collision
	if (akt==NULL)akt=chain->GetFirst();else akt=akt->next;
	while (akt)
	{
		if (GetType(akt->id)==IDChicken)
		{
			CChicken* c=(CChicken*)akt;
			if ((c->dead==0)&&(c->box->IsInside(pos)))
			{
				return akt;
			}
		}
		akt=akt->next;
	}
	return NULL;
}

const BOOLEAN CGolfer::Schlag()
{
	if (schlag.phase!=0)return FALSE;
	if (parent==NULL)return FALSE;

	schlag.phase=1.0f;
	schlag.getroffen=FALSE;

	CSound* s=new CSound(game->lpBuffers[2],TRUE);
	s->Set3DParameters(pos);
	s->Play();
	game->AddSound(s);

	return TRUE;
}

void CGolfer::Rebound(CBoundingTube *t)
{
	if (box->Collidate(t))
	{
		pos=box->MoveOut(t);
		pos.y=world->HeightAtPoint(pos.x,pos.z);
		box->SetPos(pos);
	}
}

void CGolfer::Throw()
{
	CPlayer* p=(CPlayer*)parent;
	D3DMATRIX m;
	D3DVECTOR v=D3DVECTOR(0,0,2);
	D3DUtil_SetRotateYMatrix(m,-p->ang.y);
	D3DMath_VectorMatrixMultiply(v,v,m);

	pos=p->pos+v;

	if (!world->IsInside(pos))world->MakeInside(pos);
	pos.y=world->HeightAtPoint(pos.x,pos.z);
	box->SetPos(pos);

	CObject* akt=chain->GetFirst();
	while (akt)
	{
		if (GetType(akt->id)==IDStall)
		{
			Rebound(((CStall*)akt)->box);
			Rebound(((CStall*)akt)->eingang);
		}
		if (GetType(akt->id)==IDTree)
			Rebound(((CTree*)akt)->box);
		if (GetType(akt->id)==IDDrescher)
			Rebound(((CDrescher*)akt)->box2);

		akt=akt->next;
	}

	schlag.phase=0.0f;
	parent=NULL;
	ejecttime=3.0f;
}

void CGolfer::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,parent?parent->id:0,0,&pos,sizeof(pos),TRUE);
}

void CGolfer::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		pos=*(D3DVECTOR*)data->userdata;
		box->SetPos(pos);
		parent=game->chain->Find(data->p2);
	}
	DebugOut("CGolfer::ReceiveData();");
}

void CGolfer::Notify(PNETWORKDATA data)
{
	switch (data->p1)
	{
	case 1:
		((CChicken*)game->chain->Find(data->p2))->Schlag(parent);
		schlag.getroffen=TRUE;
		break;
	case 3:
		Schlag();
		break;
	case 4:
		if (parent!=NULL)
			((CBauer*)parent)->object=NULL;
		Throw();
		break;
	}
}

void CGolfer::Unbind()
{
	if (parent!=NULL)((CBauer*)parent)->object=NULL;
	schlag.phase=0.0f;
	parent=NULL;
}

void CGolfer::NewWorld()
{
	if (parent==NULL)
	{
		pos.y=world->HeightAtPoint(pos.x,pos.z);
		world->MakeInside(pos);
		box->SetPos(pos);
	}
}