/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "gartenkralle.h"
#include "game.h"
#include "player.h"
#include "chicken.h"
#include "sounds.h"
#include "bauer.h"
#include "stall.h"
#include "drescher.h"
#include "tree.h"
#include "shadows.h"



const float stosstimescale=1.1f;



BOOLEAN CGartenkralle::Init()
{
	pos=world->Rand(1.0f);
	box=new CBoundingTube(pos,0.31f,1.4f);

	mesh=game->models->FindObject("GK");

	parent=NULL;
	werfer=NULL;
	id=MakeUID(IDGartenkralle);
	speed=D3DVECTOR(0,0,0);
	angx=angy=0;
	stoss.phase=0.0f;
	stoss.forked=NULL;

	UpdateBox();
	maxfallspeed=0.0f;

	return TRUE;
}

CGartenkralle::~CGartenkralle()
{
	if (box)delete box;
}

void CGartenkralle::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	D3DMATRIX m=BerechneStoss();
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	if (Config.reflections)
	{	// Environmentmap berechnen
		D3DVERTEX* vertices;
		WORD* indices;
		DWORD numvertices,numindices;

		mesh->GetMeshGeometry(&vertices,&numvertices,&indices,&numindices);

		CSphereMap::ApplySphereMap(lpDevice,vertices,numvertices);
	}

	mesh->Render(lpDevice,FALSE,(Config.reflections)?game->lpTexture[31]:NULL);
}

void CGartenkralle::Execute(float elapsed)
{
#define WallHit(x) x=-x*bremse; CSound* s=new CSound(game->lpBuffers[4],TRUE);s->Set3DParameters(pos);s->Play();game->AddSound(s);SplatForked();

	if (speed.y!=0)
	{
		pos+=speed*elapsed;
		const float bremse=0.4f;
		if (pos.x<world->minx())
		{
			pos.x=world->minx();
			WallHit(speed.x);
		}
		if (pos.x>world->maxx())
		{
			pos.x=world->maxx();
			WallHit(speed.x);
		}
		if (pos.z<world->minz())
		{
			pos.z=world->minz();
			WallHit(speed.z);
		}
		if (pos.z>world->maxz())
		{
			pos.z=world->maxz();
			WallHit(speed.z);
		}
#undef WallHit

		CObject* o=NULL;
		do
		{
			o=PerformHit(o);
			if (o==NULL)break;
			if (o==stoss.forked)continue;

			// Huhn im Wurf aufspießen
			CChicken* c=(CChicken*)o;

			if (stoss.forked!=NULL)
			{
				c->Splat(SPLAT_NORMAL);
				if (werfer!=NULL)
				{
					((CPlayer*)werfer)->HasHit(c->id);
					((CPlayer*)werfer)->SendNotify(3,c->id);
				}
			}else{
				stoss.forked=c;
				c->Fork(this);
			}
		}while (o);

		{ // Gartenkralle an Objekten abprallen lassen
			o=chain->GetFirst();
			while (o)
			{
				if (GetType(o->id)==IDStall)
				{
					CheckRebound(((CStall*)o)->box);
					CheckRebound(((CStall*)o)->eingang);
				}

				if (GetType(o->id)==IDDrescher)
					CheckRebound(((CDrescher*)o)->box2);

				if (GetType(o->id)==IDTree)
					CheckRebound(((CTree*)o)->box);

				o=o->next;
			}
		}

		speed.y-=(game->gravity*elapsed);
		D3DVECTOR p2;
		GetBackPoint(&p2);
		float h1=world->HeightAtPoint(pos.x,pos.z);
		float h2=world->HeightAtPoint(p2.x,p2.z);

		if ((pos.y<h1)||(p2.y<h2))
		{
			CSound* s=new CSound(game->lpBuffers[4],TRUE);
			s->Set3DParameters(pos);
			s->Play();
			game->AddSound(s);

			SplatForked();
/*			{	// Geschwindigkeit korrigieren
				const float d=(pos.y<h1)?absf(pos.y-h1):absf(p2.y-h2);
				speed.y+=(game->gravity*elapsed);
				speed.y-=(game->gravity*(elapsed-d/absf(speed.y)));
			}*/

			if (pos.y<h1)pos.y=h1;
			else
			{
				pos.y=h2+(pos.y-p2.y);
			}

			speed.y=-speed.y*0.45f;
			if (speed.y>maxfallspeed)
				speed.y=maxfallspeed*sqr(0.45f);
			maxfallspeed=speed.y;

			if (abs(speed.y)<1.0f)
			{
				// Gartenkralle hinstellen
				speed.y=0.0f;
				pos.y=h1;

				werfer=NULL;
				SendNotify(12,0,0,&pos,sizeof(pos));
			}
			speed.x*=0.4f;
			speed.z*=0.4f;
			UpdateBox();
		}
	}

	if (stoss.phase!=0.0f)
	{
		float os=stoss.phase;
		stoss.phase-=(elapsed*stosstimescale);
		if ((os>=0.6f)&&(stoss.phase<0.6f)&&(stoss.end==FALSE))stoss.phase=0.6f;
		BerechneStoss();
		CObject* o=PerformHit();
		if (o!=NULL)
		{	// Hühner während eines Stosses aufspießen
			if ((stoss.phase>0.7f)&&(stoss.forked==NULL))
			{
				stoss.forked=o;
				((CChicken*)o)->Fork(this);
				CSound* s=new CSound(game->lpBuffers[3],TRUE);
				s->Set3DParameters(pos);
				s->Play();
				game->AddSound(s);
			}
		}
		if (stoss.phase<=0.35f)
		{
			SplatForked();
		}
		if (stoss.phase<0.0f)
		{
			SendNotify(6);
			stoss.phase=0.0f;
		}
	}
}

void CGartenkralle::GetBackPoint(D3DVECTOR *v)const
{
	if (v==NULL)return;
	D3DMATRIX m,m2;

	D3DUtil_SetTranslateMatrix(m,pos);

	if (speed.y!=0)
	{
		D3DUtil_SetRotateYMatrix(m2,-angy);
		D3DMath_MatrixMultiply(m,m2,m);
		D3DUtil_SetRotateXMatrix(m2,-angx-g_PI*0.5f);
		D3DMath_MatrixMultiply(m,m2,m);
	}	
	D3DUtil_SetTranslateMatrix(m2,0,1.35f,0);
	D3DMath_MatrixMultiply(m,m2,m);

	*v=D3DVECTOR(m.m[3][0],m.m[3][1],m.m[3][2]);
}

const D3DMATRIX CGartenkralle::BerechneStoss() 
{
	D3DMATRIX m,m2;
	if (parent==NULL)
	{
		D3DUtil_SetTranslateMatrix(m,pos);

		if (speed.y!=0)
		{
			D3DUtil_SetRotateYMatrix(m2,-angy);
			D3DMath_MatrixMultiply(m,m2,m);
			D3DUtil_SetRotateXMatrix(m2,-angx-g_PI*0.5f);
			D3DMath_MatrixMultiply(m,m2,m);
		}	
		return m;
	}

#define sinvb(v,b,i) (sinf(((i-v)/(b-v))*g_PI))

	const float maxz=0.5f;

	const float x=0.15f;
	const float y=0;
	float z=0.8f+maxz;
	float rot=0.0f;
	
	if (stoss.phase>0.85f)
		z=0.8f+maxz*(1-sinvb(0.85f,1.15f,stoss.phase));

	if ((stoss.phase>=0.20f)&&(stoss.phase<=0.5f))
	{
		rot=(stoss.phase-0.20f)/0.30f;
		rot*=-g_PI;
		rot/=2.0f;
		rot+=g_PI/2.0f;
		rot*=2.0f;
	}
	rot+=g_PI*0.2f;

	if (stoss.phase<0.25f)
		z=0.8f+maxz*(sinvb(0.0f,0.5f,stoss.phase));

#undef sinvb
	CPlayer* p=(CPlayer*)parent;
	D3DUtil_SetTranslateMatrix(m,p->pos);
	D3DUtil_SetRotateYMatrix(m2,-p->ang.y);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetRotateYMatrix(m2,-0.1f);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetRotateXMatrix(m2,-p->ang.x);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetTranslateMatrix(m2,x,y,z);

	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetRotateXMatrix(m2,-1.4f);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DUtil_SetRotateYMatrix(m2,rot);
	D3DMath_MatrixMultiply(m,m2,m);

	pos.x=m.m[3][0];
	pos.y=m.m[3][1];
	pos.z=m.m[3][2];
	UpdateBox();

	return m;
}

CObject* CGartenkralle::PerformHit(CObject* akt)
{
	if (!game->IsServer)return NULL;
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

const BOOLEAN CGartenkralle::Throw(CObject* vwerfer)
{
	if ((stoss.phase!=0)&&((stoss.phase<0.5f)||(stoss.phase>0.85f)))return FALSE;

	if (stoss.forked!=NULL)
	{
		CSound *s=new CSound(game->lpBuffers[5],TRUE);
		s->Set3DParameters(pos);
		s->Play();
		game->AddSound(s);
	}

	stoss.phase=0.0f;

	werfer=vwerfer; 

	D3DMATRIX m;
	CPlayer* p=(CPlayer*)parent;
 	angy=p->ang.y;
	angx=p->ang.x;
	speed=D3DVECTOR(0,0,2.0f);

	D3DVECTOR np=D3DVECTOR(0.15f,0,0.8f);

	D3DUtil_SetRotateXMatrix(m,-angx);
	D3DMath_VectorMatrixMultiply(speed,speed,m);
	D3DMath_VectorMatrixMultiply(np,np,m);

	speed+=(p->speed*0.15f);

	D3DUtil_SetRotateYMatrix(m,-angy);
	D3DMath_VectorMatrixMultiply(speed,speed,m);
	D3DMath_VectorMatrixMultiply(np,np,m);

	speed.y+=0.2f;
	speed*=6.5f;

	pos=((CPlayer*)(parent))->pos+np;
	((CBauer*)parent)->object=NULL;
	parent=NULL;
	maxfallspeed=10000.0f;

	UpdateBox();

	CSound *s=new CSound(game->lpBuffers[2],TRUE);
	s->Set3DParameters(pos,speed);
	s->Play();
	game->AddSound(s);
	return TRUE;
}

const BOOLEAN CGartenkralle::Stoss()
{
	if (stoss.phase!=0)return FALSE;
	if (parent==NULL)return FALSE;

	stoss.end=FALSE;
	stoss.phase=1.0f;

	CSound* s=new CSound(game->lpBuffers[2],TRUE);
	s->Set3DParameters(pos);
	s->Play();
	game->AddSound(s);

	return TRUE;
}

void CGartenkralle::SplatForked()
{
	if (stoss.forked)
	{
		const CObject* o=NULL;
		if (werfer!=NULL)o=werfer;else o=parent;
		SendNotify(3,o?o->id:0,stoss.forked->id);
		if (o!=NULL)
		{
			if (GetType(o->id)==IDBauer)
				((CPlayer*)o)->HasHit(stoss.forked->id);
		}	
		((CChicken*)stoss.forked)->Splat(SPLAT_NORMAL);
		stoss.forked=NULL;
	}
}

void CGartenkralle::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,parent?parent->id:0,werfer?werfer->id:0,&pos,sizeof(pos),TRUE);
}

void CGartenkralle::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		pos=*(D3DVECTOR*)data->userdata;
		UpdateBox();
		parent=game->chain->Find(data->p2);
		werfer=game->chain->Find(data->p3);
	}
	DebugOut("CGartenkralle::ReceiveData();");
}

void CGartenkralle::Notify(PNETWORKDATA data)
{
	switch (data->p1)
	{
	case 1:
		Throw(game->chain->Find(data->p2));
		break;
	case 2:
		Stoss();
		break;
	case 3:
		{	// SplatForked
			CObject* o=chain->Find(data->p2);
			if (o!=NULL)
			{
				if (GetType(o->id)==IDBauer)
					((CPlayer*)o)->HasHit(data->p3);
			}	
			stoss.forked=NULL;
		}
		break;
	case 4: // Stoss.end=TRUE;
		stoss.end=TRUE;
		break;
	case 6:
		stoss.end=TRUE;
		stoss.phase=0.0f;
		break;
	case 12:
		speed.y=0.0f;
		pos=*((D3DVECTOR*)data->userdata);
		UpdateBox();
		werfer=NULL;
		break;

	}
}

void CGartenkralle::EndStoss()
{
	stoss.end=TRUE;	
	SendNotify(4);
}

void CGartenkralle::Unbind()
{
	if (stoss.forked)SplatForked();
	if (parent!=NULL)((CBauer*)parent)->object=NULL;
}

void CGartenkralle::NewWorld()
{
	if (parent==NULL)
	{
		pos.y=world->HeightAtPoint(pos.x,pos.z);
		world->MakeInside(pos);
		UpdateBox();
	}
}

void CGartenkralle::CheckRebound(CBoundingTube* b)
{
	D3DVECTOR back=pos;
	GetBackPoint(&back);
	if (b->IsInside(pos))
	{
		Rebound(b,pos);
		pos=b->MoveOut(pos);
	}else if (b->IsInside(back))
	{
		Rebound(b,back);
		pos=b->MoveOut(back)+(pos-back);
	}
}

void CGartenkralle::Rebound(CBoundingTube* b,const D3DVECTOR point)
{
	{
		SplatForked();
		CSound* s=new CSound(game->lpBuffers[4],TRUE);
		s->Set3DParameters(pos);
		s->Play();
		game->AddSound(s);
	}

	D3DVECTOR normale=(b->GetPos()-point);
	D3DVECTOR s=D3DVECTOR(speed.x,0,speed.z);
	normale.y=0.0f;
	normale=Normalize(normale);

	D3DVECTOR vn=(DotProduct(normale,s))*normale;
	D3DVECTOR vt=s-vn;

	const float faktor=0.8f;
	s=vt-faktor*vn;

	speed.x=s.x;
	speed.z=s.z;
}

void CGartenkralle::DrawShadow(LPDIRECT3DDEVICE7 lpDevice)
{
}

void CGartenkralle::UpdateBox()
{
	box->SetPos(pos);
}

