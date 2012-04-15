/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "plasmacannon.h"
#include "world.h"
#include "game.h"
#include "player.h"
#include "stall.h"
#include "tree.h"
#include "drescher.h"
#include "bauer.h"
#include "sprites.h"
#include "config.h"
#include "effects.h"
#include "lensflare.h"
#include "envmap.h"


const float animationfaktor=0.25f;
const float plasmaballradius=0.272f;



class CPlasmaBall:public CObject
{
private:
	D3DVECTOR pos;
	D3DVECTOR speed;
	D3DVECTOR ang;
	CD3DFileObject* model;

	const UID owner;

	void Explode();
public:
	CPlasmaBall(const D3DVECTOR vpos,const D3DVECTOR vspeed,const UID vowner):pos(vpos),owner(vowner),speed(vspeed),ang(0.0f),model(game->models->FindObject("Kugel")) {}

	virtual void Execute(float elapsed);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
};


void CPlasmaBall::Explode()
{
	if (deleting)return;

	CSound* s=new CSound(game->lpBuffers[30],TRUE);
	s->Set3DParameters(pos);
	s->Play();
	game->AddSound(s);

	const float w=game->plasmaradius*1.6f;
	const float h=game->plasmaradius*1.6f*(5.0f/6.0f);

	{	// Nova erstellen
		float h=world->HeightAtPoint(pos.x,pos.z)+0.1f;
		if (abs(h-pos.y)>1.0f)h=pos.y;
		game->AddGimmick(new CNova(D3DVECTOR(pos.x,h,pos.z),1.0f,w*1.3f,0.325f));
	}

	// Sprite Animation starten
	if (!game->lpSprites[0]->Loaded())
		game->lpSprites[0]->Load("PLASMA.SPR");

	game->AddGimmick(new CSpriteAnimation(
		0,				// Sprite
		pos+D3DVECTOR(0,plasmaballradius+h/4.0f,0),		// Center of Sprite in 3D space
		FALSE,							// Looping
		0.0f,							// Timeout deactivated
		16.0f,							// 17,5 fps
		w,h));							// size of Sprite

	CPlayer* player=(CPlayer*)game->chain->Find(owner);

	if ((game->IsServer)&&(player!=NULL))
	{
		// Hier Kollisionen testen
		CObject* akt=chain->GetFirst();
		while (akt)
		{
			if (GetType(akt->id)==IDChicken)
			{
				CChicken* c=(CChicken*)akt;
				if ((c->dead==0.0f)&&(c->install==NULL))
				{
					const float distance=Magnitude(((CChicken*)akt)->pos-pos);
					const float aussen=3.5f;

					if (distance<=game->plasmaradius)
					{		// Im inneren Radius -> direkt grillen
						((CChicken*)akt)->Splat(SPLAT_FRY);
						if (player)player->HasHit(c->id);
						if (player)player->SendNotify(3,c->id);
					}else if (game->PlasmaBallPressure)
					if (distance<=game->plasmaradius*aussen)
					{		// Im äußeren Radius -> Wegschleudern
						D3DVECTOR pressure=Normalize(((CChicken*)akt)->pos-pos);
						pressure.y=abs(pressure.y)/1.6f+0.3f;

						pressure*=sqrtf((game->plasmaradius*aussen-distance)/(aussen-1.0f))*6.0f;

						((CChicken*)akt)->Blast(player,pressure);
						c->SendNotify(12,player->id,0,&pressure,sizeof(pressure),TRUE);
					}
				}
			}
			
			akt=akt->next;
		}
	}

	Remove();
}

void CPlasmaBall::Execute(float elapsed)
{
	pos+=speed*elapsed;
	speed.y-=elapsed*game->gravity*0.7f;
	
	ang.x+=elapsed*2.5f;
	ang.y+=elapsed*3.14159f;
	ang.z+=elapsed*0.7f;

//	if (game->IsServer)
	{
		// Hier Kollisionen testen

		CObject* akt=chain->GetFirst();
		while (akt)
		{
			switch(GetType(akt->id))
			{
			case IDStall:
				{
					if ((((CStall*)akt)->box->IsInside(pos,plasmaballradius))||
						(((CStall*)akt)->eingang->IsInside(pos,plasmaballradius)))Explode();
					break;
				}
			case IDTree:
				{
					if (((CTree*)akt)->box->IsInside(pos,plasmaballradius))Explode();
					break;
				}
			case IDChicken:
				{
					if ((((CChicken*)akt)->dead==0.0f)&&(((CChicken*)akt)->box->IsInside(pos,plasmaballradius)))Explode();
					break;
				}
			case IDBauer:
				{
					if (((CBauer*)akt)->box->IsInside(pos,plasmaballradius))Explode();
					break;
				}
			case IDDrescher:
				{
					if (((CDrescher*)akt)->box2->IsInside(pos,plasmaballradius))Explode();
					break;
				}
			}
			
			akt=akt->next;
		}
	}

	if (!world->IsInside(pos,plasmaballradius))Explode();
	if (world->HeightAtPoint(pos.x,pos.z)>pos.y-plasmaballradius)Explode();
}

void CPlasmaBall::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	D3DMATRIX m,m2;

	D3DUtil_SetRotateXMatrix(m,ang.x);
	D3DUtil_SetRotateYMatrix(m2,ang.y);

	D3DMath_MatrixMultiply(m,m,m2);

	D3DUtil_SetRotateZMatrix(m2,ang.z);
	D3DMath_MatrixMultiply(m,m,m2);

	D3DUtil_SetTranslateMatrix(m2,pos);
	D3DMath_MatrixMultiply(m,m,m2);

	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	model->Render(lpDevice,FALSE);
}






const plasmaballimagesize=45;

BOOLEAN CPlasmaCannon::Init()
{
	pos=world->Rand(1.0f);
	lpMesh=game->models->FindObject("Plasma");

	box=new CBoundingTube(pos,1.0f,1.2f);

	return TRUE;
}

void CPlasmaCannon::Execute(float elapsed)
{
	if (ejecttime)
	{
		ejecttime-=elapsed;
		if (ejecttime<0.0f)ejecttime=0.0f;
	}

	const float maxmunni=20.0f;
	if (munition<maxmunni)
	{
		munition+=elapsed*(game->plasmaregeneration);
		if (munition>maxmunni)munition=maxmunni;
	}

	if (canshoot)
	{
		canshoot-=elapsed;
		if (canshoot<0.0f)canshoot=0.0f;
	}

	if (parent==NULL)
		ang+=elapsed*1.5f;
	else box->SetPos(pos);
}

void CPlasmaCannon::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if ((parent==NULL)&&(!box->IsVisible()))return;
	D3DMATRIX m,m2;

	if (parent==NULL)
	{
		D3DUtil_SetTranslateMatrix(m,pos);
		D3DUtil_SetRotateYMatrix(m2,ang);
		D3DMath_MatrixMultiply(m,m2,m);
		D3DUtil_SetRotateXMatrix(m2,-g_PI*0.5f);
		D3DMath_MatrixMultiply(m,m2,m);
	}else{
		CPlayer* p=(CPlayer*)parent;

		D3DUtil_SetRotateXMatrix(m,-g_PI*0.5f);

		float z=0.0f;
		if (canshoot>0.0f)
		{
			const float maxz=0.225f;
			float phase=(canshoot/game->plasmarate)/animationfaktor-(1/animationfaktor)+1.0f;
			if (phase<0.0f)phase=0.0f;

			if (phase>0.8f)z=-sqr((1-phase)*5.0f)*maxz;

			if (phase<=0.8f)z=-sinf(phase*1.25f*g_PI*0.5f)*maxz;
		}

		D3DUtil_SetTranslateMatrix(m2,0.51f,-0.9f,0.35f+z);
		D3DMath_MatrixMultiply(m,m,m2);

		D3DUtil_SetRotateXMatrix(m2,-p->ang.x);
		D3DMath_MatrixMultiply(m,m,m2);

		D3DUtil_SetRotateYMatrix(m2,-p->ang.y);
		D3DMath_MatrixMultiply(m,m,m2);

		D3DUtil_SetTranslateMatrix(m2,p->pos);
		D3DMath_MatrixMultiply(m,m,m2);

		pos.x=m.m[3][0];
		pos.y=m.m[3][1];
		pos.z=m.m[3][2];
	}

	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	LPDIRECTDRAWSURFACE7 lpTexture=NULL;

#if (FALSE)
	if (Config.reflections)
	{	// Environmentmap der Plasmakanone berechnen
		D3DVERTEX* vertices;
		WORD* indices;
		DWORD numvertices,numindices;

		lpMesh->GetMeshGeometry(&vertices,&numvertices,&indices,&numindices);
		CSphereMap::ApplySphereMap(lpDevice,vertices,numvertices);

		lpTexture=game->lpTexture[31];
	}
#endif

	lpMesh->Render(lpDevice,FALSE,lpTexture);
}

void CPlasmaCannon::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	CWeapon::DrawUI(lpDevice);
	if (parent==NULL)return;
	if (!((CPlayer*)parent)->IsLocal)return;
	if (!((CPlayer*)parent)->isplayer)return;

	if (game->lpTexture[29]==NULL)
	{
		game->lpTexture[29]=CreateTextureFromResource(lpDevice,game->lpDD,NULL,"plasmaball.bmp",0,FALSE,TRUE);
		MakeTransparent(game->lpTexture[29],FALSE);
	}

	if (Config.alpha)
	{
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,200);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	MakeBillboardVertices(&mv[0],10,game->height-plasmaballimagesize-10,plasmaballimagesize,plasmaballimagesize);

	lpDevice->SetTexture(0,game->lpTexture[29]);

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,mv,4,0);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
	}

	static CHAR c[10];
	sprintf(&c[0],"x %d",int(floorf(munition)));

	game->lpD3DFont->DrawText(10.0f+plasmaballimagesize+10.0f,game->GetFontCentered(game->height-10.0f-plasmaballimagesize,game->height-10.0f),D3DRGB(1,1,1),&c[0],0);
}

void CPlasmaCannon::Rebound(CBoundingTube *t)
{
	if (box->Collidate(t))
	{
		pos=box->MoveOut(t);
		pos.y=world->HeightAtPoint(pos.x,pos.z);
		box->SetPos(pos);
	}
}

void CPlasmaCannon::Throw()
{
	CPlayer* p=(CPlayer*)parent;
	D3DMATRIX m;
	D3DVECTOR v;
	D3DUtil_SetRotateYMatrix(m,-p->ang.y);
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,2),m);

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

	parent=NULL;
	ejecttime=3.0f;
}

const BOOLEAN CPlasmaCannon::Shoot()
{
	if (parent==NULL)
	{
#ifdef _DEBUG
		if (game->ticker)game->ticker->Add("CPlasmaCannon::Shoot(): OBJECT HAS NO PARENT!",D3DRGB(1,0,0));
#endif
		return FALSE;
	}
	if (canshoot>0.0f)return FALSE;
	if (munition<1.0f)
	{
		DebugOut("Keine Munition.");
		return FALSE;
	}

	CSound* s=new CSound;
	CPlayer* p=(CPlayer*)parent;
	if ((p->IsLocal)&&(p->isplayer))
	{
		s->Create2D(game->lpBuffers[26]);
	}else
	{
		s->Create(game->lpBuffers[27]);
		s->Set3DParameters(pos);
	}
	s->Play();
	game->AddSound(s);

	canshoot=game->plasmarate;


	D3DVECTOR position,speed=D3DVECTOR(0,0,20);
	{
		D3DMATRIX m,m2;
		CPlayer* p=(CPlayer*)parent;


		D3DUtil_SetRotateXMatrix(m,-g_PI/2.0f);

		float z=1.00f;

		D3DUtil_SetTranslateMatrix(m2,0.51f,-0.185f,0.35f+z);
		D3DMath_MatrixMultiply(m,m,m2);

		D3DUtil_SetRotateXMatrix(m2,-p->ang.x);
		D3DMath_MatrixMultiply(m,m,m2);
		D3DMath_VectorMatrixMultiply(speed,speed,m2);

		speed+=p->speed;

		D3DUtil_SetRotateYMatrix(m2,-p->ang.y);
		D3DMath_MatrixMultiply(m,m,m2);
		D3DMath_VectorMatrixMultiply(speed,speed,m2);

		D3DUtil_SetTranslateMatrix(m2,p->pos);
		D3DMath_MatrixMultiply(m,m,m2);

		position.x=m.m[3][0];
		position.y=m.m[3][1];
		position.z=m.m[3][2];
	}
	speed.y+=3.5f;

	chain->Add(new CPlasmaBall(position,speed,parent->id));

	munition-=1.0f;

	return TRUE;
}

void CPlasmaCannon::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,parent?parent->id:0,FloatToLong(munition),&pos,sizeof(pos),TRUE);
}

void CPlasmaCannon::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		pos=*(D3DVECTOR*)data->userdata;
		box->SetPos(pos);
		parent=game->chain->Find(data->p2);
		munition=LongToFloat(data->p3);
	}
	DebugOut("CPlasmaCannon::ReceiveData();");
}

void CPlasmaCannon::Notify(PNETWORKDATA data)
{
	switch (data->p1)
	{
	case 1:
		Shoot();
		break;
	case 2:
		if (parent!=NULL)
			((CBauer*)parent)->object=NULL;
		Throw();
		break;
	}
}

void CPlasmaCannon::Unbind()
{
	if (parent!=NULL)((CBauer*)parent)->object=NULL;
	parent=NULL;
	world->MakeInside(pos);
	pos.y=world->HeightAtPoint(pos.x,pos.z);
	box->SetPos(pos);
}

void CPlasmaCannon::NewWorld()
{
	if (parent==NULL)
	{
		pos.y=world->HeightAtPoint(pos.x,pos.z);
		world->MakeInside(pos);
		box->SetPos(pos);
	}
}
