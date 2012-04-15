/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "drescher.h"
#include "game.h"
#include "bauer.h"
#include "tree.h"
#include "effects.h"
#include "stall.h"
#include "config.h"





CDrescher::CDrescher()
:sound(NULL),box1(NULL),box2(NULL),rolle(0.0f),speed(D3DVECTOR(0,0,0)),acc(D3DVECTOR(0,0,0)),
 rot(0.0f),schaukelphase(0.0f)
{
	statusmessage=new CMessageChain(2);
	schaukelphase=0.0f;
	Normale=IstNormale=D3DVECTOR(0,1,0);
}

BOOLEAN CDrescher::Init()
{
	pos=world->Rand(4.0f);

	ang=D3DVECTOR(0,randf()*6.0f,0);

	mesh1=game->models->FindObject("Drescher");
	mesh2=game->models->FindObject("Rolle");

	id=MakeUID(IDDrescher);

	box1=new CBoundingTube(GetCenter1(),3.6f/2.0f,1.5f);
	box2=new CBoundingTube(GetCenter2(),4.2f/2.0f,1.5f);

	sound=new CSound(game->lpBuffers[16],FALSE);
	sound->Set3DParameters(pos);
	sound->SetFrequency(16000);

	game->AddSound(sound);
	sound->Play(DSBPLAY_LOOPING);

	UpdateWinkel();
	return TRUE;
}

CDrescher::~CDrescher()
{
	statusmessage->ShutDown();
	if (box1)delete box1;
	if (box2)delete box2;
	if (sound)
	{
		sound->Stop();
		sound->Remove();
	}
}

void CDrescher::UpdateWinkel()
{
	const D3DVECTOR back=GetCenter1();
	world->HeightAtPoint(back.x,back.z,&Normale);
	Normale=Normalize(Normale);

	if (parent==NULL)
	{
		IstNormale=Normale;
		alpha=acosf(DotProduct(IstNormale,D3DVECTOR(0,1,0)));
		beta=atan2f(IstNormale.x,IstNormale.z);
	}
}

const float hoehe=0.44f; // Hˆhe der Walze ¸ber dem Boden

void CDrescher::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!box2->IsVisible())return;

	lpDevice->SetTexture(0,NULL);
	D3DMATRIX matrix,m;

	D3DUtil_SetTranslateMatrix(matrix,pos.x,pos.y+hoehe,pos.z);

	{	//  M‰hdrescher gem‰ﬂ Steigung kippen

		D3DUtil_SetRotateYMatrix(m,beta);
		D3DMath_MatrixMultiply(matrix,m,matrix);

		D3DUtil_SetRotateXMatrix(m,alpha);
		D3DMath_MatrixMultiply(matrix,m,matrix);

		D3DUtil_SetRotateYMatrix(m,-beta);
		D3DMath_MatrixMultiply(matrix,m,matrix);
	}
	
	
	D3DUtil_SetRotateYMatrix(m,ang.y);
	D3DMath_MatrixMultiply(matrix,m,matrix);


	if (schaukelphase>0.0f)
	{ // schaukeln
		const float mittelpunkt=2.0f;
		D3DUtil_SetTranslateMatrix(m,0,0,-mittelpunkt);
		D3DMath_MatrixMultiply(matrix,m,matrix);
		D3DUtil_SetRotateXMatrix(m,schaukelwinkel());
		D3DMath_MatrixMultiply(matrix,m,matrix);

		D3DUtil_SetTranslateMatrix(m,0,0,mittelpunkt);
		D3DMath_MatrixMultiply(matrix,m,matrix);
	}

	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);
	mesh1->Render(lpDevice,FALSE);

	// Rolle
	D3DUtil_SetRotateXMatrix(m,rolle);
	D3DMath_MatrixMultiply(matrix,m,matrix);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);

	mesh2->Render(lpDevice,FALSE);
}

const BOOLEAN CDrescher::GetIn(CPlayer* p)
{
	if (parent!=NULL)return FALSE;

	parent=(CObject*)p;
	speed=D3DVECTOR(0,0,0);

	return TRUE;
}

void CDrescher::GetOut()
{
	parent=NULL;
	speed=acc=D3DVECTOR(0,0,0);
	sound->SetFrequency(16000);
}

void CDrescher::HandleInput(const int mx,const int my,const int mz)
{
	acc=D3DVECTOR(0,0,0);

	// Beschl.
	if ((game->KeyDown(DIK_W))||(game->KeyDown(DIK_UP)))
	{
		if (speed.z>0.0f)acc.z=5.75f;else acc.z=7.5f;
	}
	if ((game->KeyDown(DIK_S))||(game->KeyDown(DIK_DOWN)))
	{
		if (speed.z>0.0f)acc.z=-7.0f;else acc.z=-4.0f;
	}

	acc*=game->harvesterspeed;

	rot=0.0f;
	// Drehen
	if ((game->KeyDown(DIK_A))||(game->KeyDown(DIK_LEFT)))
		if (speed.z<0.0f)rot+=1.0f;else rot-=1.0f;
	if ((game->KeyDown(DIK_D))||(game->KeyDown(DIK_RIGHT)))
		if (speed.z<0.0f)rot-=1.0f;else rot+=1.0f;

	rot*=game->harvesterrotspeed;
}

inline const D3DVECTOR korrigieren(const D3DVECTOR &mitte,const D3DVECTOR &offset)
{
	if (world->IsInside(mitte+offset))return mitte;

	D3DVECTOR v=mitte+offset;
	world->MakeInside(v);
	return v-offset;
}

const float smoothspeed=0.17f;

inline void GlattAnpassen(float &ist,const float soll,const float elapsed)
{
	if (ist<soll)
	{
		ist+=elapsed*smoothspeed;
		if (ist>soll)ist=soll;
	}
	if (ist>soll)
	{
		ist-=elapsed*smoothspeed;
		if (ist<soll)ist=soll;
	}
}

void CDrescher::Execute(float elapsed)
{
	if (schaukelphase>0.0f)
	{
		schaukelphase-=(elapsed*12.0f);
		if (schaukelphase<0.0f)schaukelphase=0.0f;
	}
	if (IstNormale!=Normale)
	{	// Den M‰hdrescher "weich" an Untergrundsteigung anpassen
		GlattAnpassen(IstNormale.x,Normale.x,elapsed);
		GlattAnpassen(IstNormale.z,Normale.z,elapsed);

		// Y-Koordinate anpassen
		IstNormale.y=sqrtf(1-sqr(IstNormale.x)-sqr(IstNormale.z));

		alpha=acosf(DotProduct(IstNormale,D3DVECTOR(0,1,0)));
		beta=atan2f(IstNormale.x,IstNormale.z);
	}
	if (parent!=NULL)
	{
		rolle+=elapsed*5.75f;
 
		D3DMATRIX m;
		D3DVECTOR v;
		D3DUtil_SetRotateYMatrix(m,ang.y);
		D3DMath_VectorMatrixMultiply(v,speed,m);
		pos+=(v*elapsed);
		pos.y=world->HeightAtPoint(pos.x,pos.z);

		float r=box1->GetRadius()*1.2f;
		D3DVECTOR c=GetCenter1();
		c=korrigieren(c,D3DVECTOR(r,0,0));
		c=korrigieren(c,D3DVECTOR(-r,0,0));
		c=korrigieren(c,D3DVECTOR(0,0,r));
		c=korrigieren(c,D3DVECTOR(0,0,-r));
		pos=pos-GetCenter1()+c;

		UpdateBox();

		speed+=(acc*elapsed);
		speed*=powf(0.45f,elapsed);

		const float s=sqrtf(sqr(speed.x)+sqr(speed.z));
		if (rot!=0.0f)
		{
			float s2=sqrtf(s/5.0f);
			ang.y+=rot*s2*elapsed;
		}

		CPlayer* driver=(CPlayer*)parent;
		driver->pos=GetCenter1();
		driver->ang=ang;
		driver->ang.y=-driver->ang.y;

		sound->SetFrequency(ULONG(20000.0f+s*3200.0f));
		sound->Set3DParameters(pos);

		if (driver->IsLocal)
		{	// An Objekten abprallen und Position senden 
			CObject* o;
			o=chain->GetFirst();
			while (o)
			{
				if (GetType(o->id)==IDTree)
				{
					CTree* t=(CTree*)o;

					if (t->box->Collidate(box2))
					{
						pos=PrivotFromCenter2(box2->MoveOut(t->box));
						UpdateBox();
						goto weiter;
					}
				}
				if (GetType(o->id)==IDStall)
				{
					CStall* s=(CStall*)o;

					if (s->box->Collidate(box2))
					{
						pos=PrivotFromCenter2(box2->MoveOut(s->box));
						UpdateBox();
						goto weiter;
					}
				}
				if ((o->IsType(IDDrescher))&&(o!=this))
				{
					CDrescher* d=(CDrescher*)o;
					if (d->box2->Collidate(box2))
					{	
						pos=PrivotFromCenter2(box2->MoveOut(d->box2));
						UpdateBox();
						goto weiter;
					}
				}


weiter:
				o=o->next;
			}

			struct _r
			{
				D3DVECTOR pos,ang,speed;
			}r;
			r.pos=pos;
			r.ang=ang;
			r.speed=speed;

			statusmessage->Send(DPID_ALLPLAYERS,NM_NOTIFY,id,1,0,0,&r,sizeof(r),FALSE);
		}
	}else rolle+=elapsed*0.7f;

	statusmessage->Execute(elapsed);

	// Kollisionen nur bei Server checken
	if (!game->IsServer)return;


	// Kollisionen mit H¸hnern testen
	CObject* o=chain->GetFirst();
	while (o)
	{
		if (GetType(o->id)==IDChicken)
		{
			CChicken* c=(CChicken*)o;
			if (c->dead!=0.0f)goto weiter2;
//			if (c->bound!=NULL)goto weiter2;

			if (parent!=NULL)if (HitRolle(c->box))
			{	// Getroffen
				schaukelphase=2*g_PI;

				c->Splat(SPLAT_NORMAL);
				((CPlayer*)parent)->HasHit(c->id);
				((CPlayer*)parent)->SendNotify(3,c->id);
				if (Config.blut)game->AddGimmick(new CDrescherBlood(id,1.5f));
				if (Config.federn)game->AddGimmick(new CDrescherFedern(id,1.5f));
				goto weiter2;
			}
			CBoundingTube* box=(parent?box1:box2);
			if (c->box->Collidate(box))	// Huhn nach ausserhalb bewegen
			{
				c->pos=c->box->MoveOut(box);
				goto weiter2;
			}
		}
weiter2:
		o=o->next;
	}
}

const D3DVECTOR CDrescher::GetCenter1()const
{
	D3DMATRIX m;

	D3DUtil_SetRotateYMatrix(m,ang.y);

	D3DVECTOR v;
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,-1.7f),m);

	return v+pos;
}

const D3DVECTOR CDrescher::GetCenter2()const 
{
	D3DMATRIX m;

	D3DUtil_SetRotateYMatrix(m,ang.y);

	D3DVECTOR v;
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,-1.186f),m);

	return v+pos;
}

const D3DVECTOR CDrescher::PrivotFromCenter2(const D3DVECTOR p)const
{
	D3DMATRIX m;

	D3DUtil_SetRotateYMatrix(m,ang.y);

	D3DVECTOR v;
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,-1.186f),m);
	return p-v;
}

const BOOLEAN CDrescher::PointInRolle(D3DVECTOR point,const float radius)const
{
	point-=pos;
	D3DMATRIX m;

	{	//  M‰hdrescher gem‰ﬂ Steigung kippen

		D3DUtil_SetRotateYMatrix(m,-beta);
		D3DMath_VectorMatrixMultiply(point,point,m);

		D3DUtil_SetRotateXMatrix(m,-alpha);
		D3DMath_VectorMatrixMultiply(point,point,m);

		D3DUtil_SetRotateYMatrix(m,beta);
		D3DMath_VectorMatrixMultiply(point,point,m);
	}

	D3DUtil_SetRotateYMatrix(m,-ang.y);

	D3DMath_VectorMatrixMultiply(point,point,m);

	if (abs(point.x)>3.85f*0.5f+radius)return FALSE;
	if (abs(point.y)>0.85f*0.5f+radius)return FALSE;
	if (abs(point.z)>0.85f*0.5f+radius)return FALSE;

	return TRUE;
}

void CDrescher::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,parent?parent->id:0,FloatToLong(ang.y),&pos,sizeof(pos),TRUE);
}

void CDrescher::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		pos=*(D3DVECTOR*)data->userdata;
		parent=game->chain->Find(data->p2);
		ang.y=LongToFloat(data->p3);
		UpdateBox();
	}
	DebugOut("CDrescher::ReceiveData();");
}

void CDrescher::Notify(PNETWORKDATA data)
{
	switch(data->p1)
	{
	case 1:
		{	// Position empfangen
			struct _r
			{
				D3DVECTOR pos,ang,speed;
			}*r=(_r*)data->userdata;
			pos=r->pos;
			ang=r->ang;
			speed=r->speed;
			UpdateBox();
		}
		break;
	}
}

void CDrescher::Unbind()
{
	if (parent!=NULL)
	{
		CBauer* b=(CBauer*)parent;
		b->pos=pos;
		b->ang=ang;
		D3DMATRIX m;
		D3DVECTOR v=D3DVECTOR(0,0,-5.5f);
		D3DUtil_SetRotateYMatrix(m,b->ang.y);
		D3DMath_VectorMatrixMultiply(v,v,m);
		b->ang.y=-b->ang.y;
		b->pos+=v;
		b->object=NULL;
	}
}

void CDrescher::NewWorld()
{
	pos.y=world->HeightAtPoint(pos.x,pos.z);
	UpdateBox();
}

const D3DVECTOR CDrescher::GetOutput()const
{
	D3DMATRIX m,matrix;
	D3DUtil_SetTranslateMatrix(matrix,pos);

	{	//  M‰hdrescher gem‰ﬂ Steigung kippen

		D3DUtil_SetRotateYMatrix(m,beta);
		D3DMath_MatrixMultiply(matrix,m,matrix);

		D3DUtil_SetRotateXMatrix(m,alpha);
		D3DMath_MatrixMultiply(matrix,m,matrix);

		D3DUtil_SetRotateYMatrix(m,-beta);
		D3DMath_MatrixMultiply(matrix,m,matrix);
	}
	
	
	D3DUtil_SetRotateYMatrix(m,ang.y);
	D3DMath_MatrixMultiply(matrix,m,matrix);

	D3DUtil_SetTranslateMatrix(m,D3DVECTOR(-1.48f,1.24f,-3.1f));
	D3DMath_MatrixMultiply(matrix,m,matrix);

	if (schaukelphase>0.0f)
	{ // schaukeln
		const float mittelpunkt=2.0f;
		D3DUtil_SetTranslateMatrix(m,0,0,-mittelpunkt);
		D3DMath_MatrixMultiply(matrix,m,matrix);
		D3DUtil_SetRotateXMatrix(m,schaukelwinkel());
		D3DMath_MatrixMultiply(matrix,m,matrix);

		D3DUtil_SetTranslateMatrix(m,0,0,mittelpunkt);
		D3DMath_MatrixMultiply(matrix,m,matrix);
	}

	D3DVECTOR v;
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,0),matrix);

	return v;
}

void CDrescher::GetViewMatrix(D3DMATRIX &view)const
{
	if (parent==NULL)return;
	D3DMATRIX m;
	const BOOLEAN lookback=game->KeyDown(DIK_B);
	switch(abs(BYTE(((CPlayer*)parent)->camera%2)))
	{
	case 0:	// Dachkamera
		D3DUtil_SetTranslateMatrix(view,-pos.x,-pos.y,-pos.z);

		if (1==2)
		{	//  M‰hdrescher gem‰ﬂ Steigung kippen
			D3DUtil_SetRotateYMatrix(m,-beta);
			D3DMath_MatrixMultiply(view,view,m);

			D3DUtil_SetRotateXMatrix(m,-alpha);
			D3DMath_MatrixMultiply(view,view,m);

			D3DUtil_SetRotateYMatrix(m,beta);
			D3DMath_MatrixMultiply(view,view,m);
		}

		D3DUtil_SetTranslateMatrix(m,0,lookback?-2.2f:-2.8f,0);
		D3DMath_MatrixMultiply(view,view,m);

		D3DUtil_SetRotateYMatrix(m,-ang.y);
		D3DMath_MatrixMultiply(view,view,m);
		D3DUtil_SetTranslateMatrix(m,0,0,lookback?0.7f:2.5f);
		D3DMath_MatrixMultiply(view,view,m);

		D3DUtil_SetRotateXMatrix(m,(lookback?0.15f:-0.35f)-schaukelwinkel()*0.5f);
		D3DMath_MatrixMultiply(view,view,m);

		if (lookback)
		{
			D3DUtil_SetRotateYMatrix(m,g_PI);
			D3DMath_MatrixMultiply(view,view,m);
		}
		break;
	case 1:	// Von oben
		D3DUtil_SetTranslateMatrix(view,-pos.x,-pos.y-16.0f,-pos.z);
		D3DUtil_SetRotateYMatrix(m,-ang.y);
		D3DMath_MatrixMultiply(view,view,m);
		D3DUtil_SetTranslateMatrix(m,0,0,lookback?-5.0f:10.0f);
		D3DMath_MatrixMultiply(view,view,m);

		D3DUtil_SetRotateXMatrix(m,lookback?0.9f:-0.9f);
		D3DMath_MatrixMultiply(view,view,m);

		if (lookback)
		{
			D3DUtil_SetRotateYMatrix(m,g_PI);
			D3DMath_MatrixMultiply(view,view,m);
		}
		break;
	}
}












