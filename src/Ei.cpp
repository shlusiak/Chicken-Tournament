/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "ei.h"
#include "effects.h"
#include "bauer.h"
#include "chickenplayer.h"
#include "drescher.h"
#include "tree.h"
#include "stall.h"
#include "config.h"
#include "text.h"



const float EGG_SIZE=0.05f;


CEi::CEi()
:pos(D3DVECTOR(0,0,0)),ang(D3DVECTOR(0,0,0)),speed(D3DVECTOR(0,0,0)),owner(0),lpMesh(game->models->FindObject("Ei"))
{
	id=MakeUID(IDEi);
}

CEi::CEi(const D3DVECTOR vpos,const D3DVECTOR vang,D3DVECTOR ospeed,const D3DVECTOR opos,const BOOLEAN relativespeed,const float vspeed,const UID vowner)
:pos(vpos),ang(vang),speed(D3DVECTOR(0,0,vspeed)),owner(vowner),lpMesh(game->models->FindObject("Ei"))
{
	D3DVECTOR v;
	D3DMATRIX m,m2;
	D3DUtil_SetRotateYMatrix(m,ang.y);
	if (!relativespeed)D3DMath_VectorMatrixMultiply(ospeed,ospeed,m);
	D3DUtil_SetRotateXMatrix(m2,-ang.x);
	D3DMath_MatrixMultiply(m,m2,m);

	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,1)+opos,m);
	pos+=v;

	D3DMath_VectorMatrixMultiply(speed,speed,m);
	speed+=ospeed;
	speed.y+=ei_offsety;
	id=MakeUID(IDEi);
}

void CEi::Execute(float elapsed)
{
	speed.y-=(game->gravity*elapsed); // Gravitation
 
	// Lage im Raum anpassen
	const float f=sqrtf(sqr(speed.x)+sqr(speed.z));
	if (f==0)ang.x=g_PI*0.5f; else ang.x=atanf(speed.y/f);

	pos+=(speed*elapsed);
	if (game->IsServer)
	{
		if ((!world->IsInside(pos))||(world->HeightAtPoint(pos.x,pos.z)>pos.y))
		{
			SendNotify(1);
			Splat();
		}else {
			// Kollisionen mit Spieler testen
			CObject* o=chain->GetFirst();
			CBauer* b;

			while (o)
			{
				switch(GetType(o->id))
				{
				case IDBauer:
					b=(CBauer*)o;
					if ((b->box->IsInside(pos,EGG_SIZE))&&(b->dead==0.0f))
					{	// Bauer getrofen
						HitPlayer(b,1.0f);
					}
					goto weiter;
				case IDDrescher:
					if (((CDrescher*)o)->parent!=NULL)
					{
						CDrescher* d=(CDrescher*)o;
						
						if (d->box2->IsInside(pos,EGG_SIZE))
							HitPlayer((CBauer*)d->parent,0.4f);
					}else if (((CDrescher*)o)->box2->IsInside(pos,EGG_SIZE))
					{
						SendNotify(1);
						Splat();
					}
					goto weiter;
				case IDTree:
					if (((CTree*)o)->box->IsInside(pos,EGG_SIZE))
					{
						SendNotify(1);
						Splat();
					}
					goto weiter;
				case IDStall:
					if ((((CStall*)o)->box->IsInside(pos,EGG_SIZE))||(((CStall*)o)->eingang->IsInside(pos,EGG_SIZE)))
					{
						SendNotify(1);
						Splat();
					}
					goto weiter;
				case IDChicken:if (o->id!=owner)
					{	// Als Huhn ein Ei auffangen
						CChicken* c=(CChicken*)o;
						if (c->box->IsInside(pos,EGG_SIZE))
						{
							if (c->IsLocal)
								c->eier++;
							else c->SendNotify(121);
							SendNotify(3);
							Remove();
						}
					}
					goto weiter;
/*				case IDChicken:
					{
						CChicken* c=(CChicken*)o;
						if ((box->Collidate(c->box))&&(c->dead==0.0f))
						{
							SendNotify(1);
							c->Splat();
							Splat();
						}
						goto weiter;
					}*/
				}
weiter:
				if (deleting)return;
				o=o->next;
			}
		}
	}
}

void CEi::HitPlayer(CBauer* b,const float damagefaktor)
{
	if (b->dead!=0.0f)return;
	{	// Bauer getroffen
		const float d=game->damage*(randf()+0.5f)*damagefaktor;
		SendNotify(2,b->id,FloatToLong(d),&b->life,sizeof(b->life));

		CChickenPlayer* c=(CChickenPlayer*)chain->Find(owner);

		if (c)c->hit+=d*0.01f;

		if ((b->Hit(d))&&(c))
		{	// Bauer getötet
			c->HasHit(b->id);
			if ((game->IsServer)&&(c->isplayer)&&(b->isplayer))
			{
				// Erst Meldung an Huhn bringen
				DPID id=UID2DPID(b->id);
				if (id!=DPID(-1))
				{
					CHAR name[100];
					GetPlayerName(id,name);

					if (c->IsLocal)
					{
						CHAR text[100];
						sprintf(&text[0],T_CHICKENHASHIT,name);
						game->ticker->Add(&text[0],D3DRGB(0,1,0));
					}else c->SendNotify(20,0,0,&name[0],strlen(&name[0])+1,TRUE);
				}

				// Dann Meldung an Bauern schicken
				id=UID2DPID(c->id);
				if (id!=DPID(-1))
				{
					CHAR name[100];
					GetPlayerName(id,name);

					if (b->IsLocal)
					{
						CHAR text[100];
						sprintf(&text[0],T_FARMERWASHIT,name);
						game->ticker->Add(&text[0],D3DRGB(0,1,0));
					}else b->SendNotify(20,0,0,&name[0],strlen(&name[0])+1,TRUE);
				}

			}
		}
		Splat();
	}
}

void CEi::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (!CBoundingTube::IsSphereVisible(pos,EGG_SIZE))return;
	D3DMATRIX m,m2;

	D3DUtil_SetTranslateMatrix(m,pos);
	D3DUtil_SetRotateYMatrix(m2,ang.y);
	D3DMath_MatrixMultiply(m,m2,m);
	D3DUtil_SetRotateXMatrix(m2,-ang.x+g_PI*0.5f);
	D3DMath_MatrixMultiply(m,m2,m);

	lpDevice->SetTexture(0,NULL);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	lpMesh->Render(lpDevice,FALSE);
}

void CEi::Splat()
{
	DebugOut("CEi::Splat();");
	CSound* s=new CSound(game->lpBuffers[17],TRUE);
	s->Set3DParameters(pos);
	s->Play();
	game->AddSound(s);

	if (Config.blut)game->AddGimmick(new CEiDotter(pos));
	Remove();
}

void CEi::Notify(PNETWORKDATA data)
{
	switch(data->p1)
	{
	case 1:	// Splat
		Splat();
		break;
	case 2: // Getroffen, Schaden abziehen
		{
			CBauer* b=(CBauer*)chain->Find(data->p2);
			if (b==NULL)return;
			b->life=*(float*)data->userdata;
			CChickenPlayer* c=(CChickenPlayer*)chain->Find(owner);
			if (c)c->hit+=LongToFloat(data->p3)*0.01f;

			// Bauer getötet
			if ((b->Hit(LongToFloat(data->p3)))&&(c))
				c->HasHit(b->id);
			
			Splat();
		}
		break;
	case 3:	// Objekt einfach entfernen
		Remove();
		break;
	}
}

void CEi::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,owner,FloatToLong(ang.x),&pos,sizeof(pos),TRUE);
	Send(to,NM_OBJECTDATA,id,2,FloatToLong(ang.y),FloatToLong(ang.z),&speed,sizeof(speed),TRUE);
}

void CEi::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{
		pos=*(D3DVECTOR*)data->userdata;
		owner=data->p2;
		ang.x=LongToFloat(data->p3);
	}
	if (data->p1==2)
	{
		speed=*(D3DVECTOR*)data->userdata;
		ang.y=LongToFloat(data->p2);
		ang.z=LongToFloat(data->p3);
	}
}