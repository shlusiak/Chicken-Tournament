/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "bauer.h"
#include "game.h"
#include "gartenkralle.h"
#include "drescher.h"
#include "golfer.h"
#include "sounds.h"
#include "tree.h"
#include "chicken.h"
#include "ui.h"
#include "effects.h"
#include "stall.h"
#include "config.h"
#include "text.h"
#include "plasmacannon.h"
#include "shadows.h"




const float head=0.9f;



CBauer::CBauer()
:denied(NULL),wade(NULL),object(NULL),life(100.0f),lpHut(NULL),step(0.0f)
{
	statusmessage=new CMessageChain(2);
	box=new CBoundingTube(D3DVECTOR(0,0,0),0.6f,1.5f);
	MakeBillboardVertices(&Herz[0],11,120,55,35);
}

CBauer::~CBauer()
{
	statusmessage->ShutDown();
//	DebugOut("CBauer::~CBauer();");
	if ((IsLocal)&&(isplayer))Send(DPID_ALLPLAYERS,NM_REMOVE,id,0,0,0,NULL,0,TRUE);
	delete box;

	if (wade)
		wade->Remove();

	if (denied)
		denied->Remove();
}

BOOLEAN CBauer::Init()
{
	if (CPlayer::Init()==FALSE)return FALSE;

	id=MakeUID(IDBauer);
	wade=new CSound(game->lpBuffers[15],FALSE);
	game->AddSound(wade);

	if ((isplayer)&&(game->lpBuffers[18]))
	{
		denied=new CSound(FALSE);
		if (denied->Create2D(game->lpBuffers[18])==FALSE)
		{
			delete denied;
			denied=NULL;
		}else game->AddSound(denied);
	}

	hut=0;

	lpMesh=game->models->FindObject("Bauer");

	// Maximale Anzahl Hüten suchen und einen zufälligen auswählen
	CHAR c[30];
	do
	{
		hut++;
		sprintf(&c[0],"Hut%d",hut);
		lpHut=game->models->FindObject(&c[0]);
	}while(lpHut);
	hut=rand()%(hut);
	sprintf(&c[0],"Hut%d",hut);
	lpHut=game->models->FindObject(&c[0]);

	return TRUE;
}

const float CBauer::GetStepPhase(const BOOLEAN correct)
{
	const float steppoint=0.4f*(speed.x!=0?0.9f:1.0f)*(speed.z!=0?0.9f:1.0f);
	if (correct)while (step>=steppoint)step-=steppoint;

	return (step/steppoint);
}

void CBauer::Revive()
{
	life=100.0f;
	if (IsLocal)
	{
		pos=world->Rand(2.0f);
		if (isplayer)game->SetTransition(new CFadeTransition(FALSE,D3DRGB(1,0,0),NULL,0.6f,1));
	}
}

void CBauer::Execute(float elapsed)
{
	CPlayer::Execute(elapsed);

	statusmessage->Execute(elapsed);

	if (HasObject(IDDrescher))return;

	if (life<100.0f)
	{	// Life regenerieren
		life+=(elapsed*0.4f);
		if (life>100.0f)life=100.0f;
	}

	// Aircontrol
	speed+=addspeed*elapsed;

	if (((speed.x!=0)||(speed.z!=0))&&(speed.y==0.0f))
	{	// Schrittsound (nur wenn auf dem Boden)
		if (pos.y>head)
		{
//			float steppoint=0.4f*(speed.x!=0?0.9f:1.0f)*(speed.z!=0?0.9f:1.0f);
			step+=elapsed;

			if (GetStepPhase(FALSE)>1.0f)
			{
				GetStepPhase(TRUE);
	
				CSound* s=new CSound(game->lpBuffers[10+rand()%4],TRUE);
				s->Set3DParameters(pos);
				s->Play();
				game->AddSound(s);
			}
			if (wade->IsPlaying())wade->Stop();
		}else{
			// Wade-Sound spielen
			wade->Set3DParameters(pos);
			if (!wade->IsPlaying())wade->Play(DSBPLAY_LOOPING);
		}
	}

	D3DVECTOR s2;
	D3DMATRIX m;
	D3DUtil_SetRotateYMatrix(m,-ang.y);
	D3DMath_VectorMatrixMultiply(s2,speed,m);
	pos+=s2*elapsed;

	world->MakeInside(pos);
	const float y=world->HeightAtPoint(pos.x,pos.z)+head;

	if (speed.y==0.0f)
	{
		pos.y=y;
	}else{
		speed.y-=(game->gravity*elapsed);
		if (pos.y<y)
		{	// Gelandet
			pos.y=y;
			speed.y=0.0f;
		}
	}
	box->SetPos(pos-D3DVECTOR(0,head,0));

	BOOLEAN stallhit=FALSE;
	if (((IsLocal)||(game->IsServer))&&(dead==0.0f))
	{
		CObject* akt=chain->GetFirst();
		while (akt)
		{
			if (IsLocal)
			{
				if (akt->IsType(IDDrescher))
				{
					CDrescher* d=(CDrescher*)akt;
					if (((object!=NULL)||(d->parent!=NULL))&&(d->box2->Collidate(box)))
					{
						pos=box->MoveOut(d->box2);
						pos.y+=head;
					}
				}
				if (akt->IsType(IDTree))
				{
					if (box->Collidate(((CTree*)akt)->box))
					{
						pos=box->MoveOut(((CTree*)akt)->box);
						pos.y+=head;
					}
				}
				if ((GetType(akt->id)==IDStall))
				{
					CStall* s=(CStall*)akt;
					if (box->Collidate(s->box))
					{
						stallhit=TRUE;
						pos=box->MoveOut(s->box);
						
						if ((IsLocal)&&(isplayer)&&(denied))
						{
							if ((denied->GetTag()==0)&&(!denied->IsPlaying()))
								denied->Play();
							denied->SetTag(1);
						}
						pos.y+=head;
					}
				}
				if ((GetType(akt->id)==IDBauer)&&(akt!=this))
				{
					CBauer* b=(CBauer*)akt;
					if (b->dead==0.0f)
						if (box->Collidate(b->box))
					{
						pos=box->MoveOut(b->box);
						pos.y+=head;
					}
				}
			}
	
			if (game->IsServer)
			{
				if ((akt->IsType(IDGartenkralle))&&(object==NULL))
				{
					CGartenkralle* g=(CGartenkralle*)akt;
	
					if ((g->parent==NULL)&&(g->speed.y==0.0f)&&(g->box->Collidate(box)))
					{
						DebugOut("Gartenkralle eingesammelt.");
						g->parent=this;
						object=g;
						SendNotify(1,g->id);
						if ((IsLocal)&&(isplayer))
							if (message)message->SetText(T_GOTGARTENKRALLE,RGB(255,255,255));
					}
				}
				if (akt->IsType(IDDrescher))
				{
					CDrescher* d=(CDrescher*)akt;
					if (d->box2->Collidate(box))
					{
						if ((object==NULL)&&(d->GetIn(this)))
						{
							speed=addspeed=D3DVECTOR(0,0,0);
							object=d;
							if ((wade)&&(wade->IsPlaying()))wade->Stop();
							DebugOut("In Drescher eingestiegen.");
							SendNotify(6,d->id);
							if ((IsLocal)&&(isplayer))
								if (message)message->SetText(T_GOTDRESCHER,RGB(255,255,255));
						}else
						{
							pos=box->MoveOut(d->box2);
							pos.y+=head;
						}
					}
				}
				if ((GetType(akt->id)==IDGolfer)&&(object==NULL))
				{
					CGolfer* g=(CGolfer*)akt;
	
					if ((g->parent==NULL)&&(g->ejecttime==0.0f)&&(g->box->Collidate(box)))
					{
						DebugOut("Golfschläger eingesammelt.");
						g->parent=this;
						object=g;
						SendNotify(5,g->id);
						if ((IsLocal)&&(isplayer))
							if (message)message->SetText(T_GOTGOLFER,RGB(255,255,255));
					}
				}
				if ((akt->IsType(IDPlasmaCannon))&&(object==NULL))
				{
					CPlasmaCannon* o=(CPlasmaCannon*)akt;
	
					if ((o->parent==NULL)&&(o->ejecttime==0.0f)&&(o->box->Collidate(box)))
					{
						DebugOut("PlasmaCannon eingesammelt.");
						o->parent=this;
						object=o;
						SendNotify(12,o->id);
						if ((IsLocal)&&(isplayer))
							if (message)message->SetText(T_GOTPLASMA,RGB(255,255,255));
					}
				}
				if (GetType(akt->id)==IDItem)
				{
					CItem* item=(CItem*)akt;
					if ((box->IsInside(item->pos,0.7f))&&(PickupItem(item)))
					{
						SendNotify(21,item->id);

						item->Remove();
						Send(DPID_ALLPLAYERS,NM_REMOVE,item->id,0,0,0,NULL,0,TRUE);

						if ((IsLocal)&&(isplayer))
						{
							CSound* s=new CSound();
							s->Create2D(game->lpBuffers[29]);
							s->Play();
							game->AddSound(s);
						}
					}
				}
			}
			if ((GetType(akt->id)==IDChicken))
			{
				CChicken* c=(CChicken*)akt;

				if ((c->dead==0.0f)&&(box->Collidate(c->box)))
					c->pos=c->box->MoveOut(box);
			}

			akt=akt->next;
		}
		if ((isplayer)&&(!stallhit)&&(denied))denied->SetTag(0);

		if (IsLocal)
		{
			// Position im Netz aktualisieren
			struct _r
			{
				D3DVECTOR pos,ang,speed;
			}r;

			r.pos=pos;
			r.ang=ang;
			r.speed=speed;

			statusmessage->Send(DPID_ALLPLAYERS,NM_NOTIFY,id,2,0,0,&r,sizeof(r),FALSE);
		}
	}
}

void CBauer::GetOut()
{
	if (!HasObject(IDDrescher))return;
		
	CDrescher* d=(CDrescher*)object;

	d->GetOut();
	pos=d->pos;
	ang=d->ang;
	D3DMATRIX m;
	D3DVECTOR v;

	D3DUtil_SetRotateYMatrix(m,ang.y);
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,-5.5f),m);
	ang.y=-ang.y;
	pos+=v;
	object=NULL;
	DebugOut("Aus Mähdrescher ausgestiegen.");
	box->SetPos(D3DVECTOR(pos.x,pos.y-head,pos.z));
	SendNotify(7);
}

void CBauer::HandleInput(const float elapsed,const int mx,const int my,const int mz)
{
	if (HasObject(IDDrescher))
	{
		CDrescher* d=(CDrescher*)object;
		d->HandleInput(mx,my,mz);

		if (game->MouseButtonDown(1))
			GetOut();
		
		return;
	}


	ang.x-=float((Config.reversemouse?-my:my))/float(Config.sensitivity*10.0f);
	if (ang.x<-g_PI/2.0f)ang.x=-g_PI/2.0f;
	if (ang.x>g_PI/2.0f)ang.x=g_PI/2.0f;
	float w=float(mx)/float(Config.sensitivity*10.0f);
	ang.y-=w;

	if (w!=0.0f)
	{
		D3DMATRIX m;
		D3DUtil_SetRotateYMatrix(m,-w);
		D3DMath_VectorMatrixMultiply(speed,speed,m);
	}

	if (dead>0.0f)return;

	const float speedx=2.8f;
	const float speedz=5.5f;
	float sx=0.0f,sy=speed.y,sz=0.0f;


	if ((game->KeyDown(DIK_UP))||(game->KeyDown(DIK_W)))sz=speedz;
	if ((game->KeyDown(DIK_DOWN))||(game->KeyDown(DIK_S)))sz=-speedz;

	if ((game->KeyDown(DIK_LEFT))||(game->KeyDown(DIK_A)))sx=-speedx;
	if ((game->KeyDown(DIK_RIGHT))||(game->KeyDown(DIK_D)))sx=speedx;

	if (sy!=0.0f)
	{
		addspeed=D3DVECTOR(sx,0,sz)*game->aircontrol;
		sx=speed.x;
		sz=speed.z;
	}else addspeed=D3DVECTOR(0,0,0);

	if (game->KeyDown(DIK_SPACE))
	{	// Jump
		if (sy==0)
		{
			sy=7.0f;
			sx*=game->jumpspeed;
			sz*=game->jumpspeed;

			CSound* s=new CSound(game->lpBuffers[14],TRUE);
			s->Set3DParameters(pos);
			s->Play();
			game->AddSound(s);
		}
	}

	speed=D3DVECTOR(sx,sy,sz);

	// Objekte checken
	if (object!=NULL)
	{
		if (GetType(object->id)==IDGartenkralle)
		{
			CGartenkralle* g=(CGartenkralle*)object;
			if (game->MouseButtonDown(1))
			{
				if (g->Throw(this))
				{
					g->SendNotify(1,id);
					return;
				}
			}

			if (game->MouseButtonDown(0))
			{
				if (g->Stoss())g->SendNotify(2);
			}else if (g->stoss.phase!=0.0f)g->EndStoss();
		}
		if (GetType(object->id)==IDPlasmaCannon)
		{
			CPlasmaCannon* o=(CPlasmaCannon*)object;
			if (game->MouseButtonDown(0))
			{
				if (o->Shoot())	o->SendNotify(1);
			}
			if (game->MouseButtonDown(1))
			{
				o->Throw();
				o->SendNotify(2);
				object=NULL;
				DebugOut("PlasmaCannon weggeworfen.");
				return;
			}
		}
		if (GetType(object->id)==IDGolfer)
		{
			CGolfer* g=(CGolfer*)object;
			if (game->MouseButtonDown(1))
			{
				// Wegwerfen
				g->Throw();
				g->SendNotify(4);
				object=NULL;
				DebugOut("Golfschläger weggeworfen.");
				return;
			}
			if (game->MouseButtonDown(0))
			{
				if (g->Schlag())g->SendNotify(3);
			}
		}
	}
}

void CBauer::GetViewMatrix(D3DMATRIX &view)
{
	if (HasObject(IDDrescher))
	{
		CDrescher* d=(CDrescher*)object;
		d->GetViewMatrix(view);
	}else
	{
		if (camera%2==0)
		{
			CPlayer::GetViewMatrix(view);
			D3DMATRIX m;
			const float f=GetStepPhase(FALSE);
			const float max=0.005f;
			D3DUtil_SetTranslateMatrix(m,sinf(f*g_PI)*max,cosf(f*g_PI*2.0f)*max*0.6f,0);
			D3DMath_MatrixMultiply(view,view,m);
		}
		if (camera%2==1)
		{
			D3DMATRIX m;
			D3DUtil_SetTranslateMatrix(view,-pos.x,-pos.y,-pos.z);
			D3DUtil_SetRotateYMatrix(m,ang.y);
			D3DMath_MatrixMultiply(view,view,m);

			D3DUtil_SetRotateXMatrix(m,ang.x);
			D3DMath_MatrixMultiply(view,view,m);

			D3DUtil_SetTranslateMatrix(m,0,0.3f,3.5f);
			D3DMath_MatrixMultiply(view,view,m);
		}
	}
}

void CBauer::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,FloatToLong(hit),hut,NULL,0,TRUE);
	Send(to,NM_OBJECTDATA,id,2,object?object->id:0,0,NULL,0,TRUE);
}

void CBauer::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1) 
	{
		hit=LongToFloat(data->p2);
		hut=(BYTE)data->p3;
		CHAR c[20];
		sprintf(&c[0],"Hut%d",hut);
		lpHut=game->models->FindObject(&c[0]);
	}
	if (data->p1==2)
	{
		object=chain->Find(data->p2); 
	}
}

void CBauer::Notify(PNETWORKDATA data)
{
	switch (data->p1)
	{
	case 1:
		{	// Gartenkralle eingesammelt
#ifdef _DEBUG
			game->ticker->Add("Gartenkralle eingesammelt (Netz)");
#endif
			Unbind();
			CGartenkralle* g=(CGartenkralle*)game->chain->Find(data->p2);
			if (g->parent!=NULL)
			{
				((CBauer*)(g->parent))->object=NULL;
			}
			object=g;
			g->parent=this;
			DebugOut("Gartenkralle eingesammelt (Netz).");
			if (IsLocal)
				if (message)message->SetText(T_GOTGARTENKRALLE,RGB(255,255,255));
		}
		break;
	case 2:
		{	// Position empfangen, aktualisieren
#ifdef _DEBUG
			if (IsLocal)game->ticker->Add("Position bei lokalem Spieler empfangen!!!",D3DRGB(1,0,0));
#endif
			struct _r
			{
				D3DVECTOR pos,ang,speed;
			}*r=(_r*)data->userdata;
			pos=r->pos;
			speed=r->speed;
			ang=r->ang;
		}
		break;
	case 3:HasHit(data->p2);
		break;
	case 5:
		{	// Golfschläger eingesammelt
#ifdef _DEBUG
			game->ticker->Add("Golfschläger eingesammelt (Netz)");
#endif
			Unbind();
			CGolfer* g=(CGolfer*)game->chain->Find(data->p2);
			if (g->parent!=NULL)
			{
				((CBauer*)(g->parent))->object=NULL;
			}
			object=g;
			g->parent=this;
			DebugOut("Golfschläger eingesammelt (Netz).");
			if (IsLocal)
				if (message)message->SetText(T_GOTGOLFER,RGB(255,255,255));
		}
		break;
	case 6:
		{	// In Mähdrescher eingestiegen
#ifdef _DEBUG
			game->ticker->Add("In Mähdrescher eingestiegen (Netz)");
#endif
			Unbind();
			CDrescher* d=(CDrescher*)game->chain->Find(data->p2);
			if (d->GetIn(this)==FALSE)
			{
				game->ticker->Add("AAAH!!! Fehler aufgetreten (Bauernotify: Schon jemand im Mähdrescher). Wird ignoriert. Hoffen wir das Beste...",D3DRGB(1,0,0));
				return;
			}
			speed=addspeed=D3DVECTOR(0,0,0);
			object=d;
			if (wade->IsPlaying())wade->Stop();
			DebugOut("In Drescher eingestiegen (Netz).");
			if (IsLocal)
				if (message)message->SetText(T_GOTDRESCHER,RGB(255,255,255));
		}
		break;
	case 7:
		{
			CDrescher* d=(CDrescher*)object;
			d->GetOut();
			pos=d->pos;
			ang=d->ang;
			D3DMATRIX m;
			D3DVECTOR v=D3DVECTOR(0,0,-5.5f);
			D3DUtil_SetRotateYMatrix(m,ang.y);
			D3DMath_VectorMatrixMultiply(v,v,m);
			ang.y=-ang.y;
			pos+=v;
			object=NULL;
			DebugOut("Aus Mähdrescher ausgestiegen (Netz).");
		}
		break;
	case 8:	// Nächster Hut
		NextHut();
		break;
	case 12:
		{	// PlasmaCannon eingesammelt
#ifdef _DEBUG
			game->ticker->Add("Plasmakanone eingesammelt (Client)");
#endif
			Unbind();
			CPlasmaCannon* g=(CPlasmaCannon*)game->chain->Find(data->p2);
			if (g->parent!=NULL)
			{
				((CBauer*)(g->parent))->object=NULL;
			}
			object=g;
			g->parent=this;
			DebugOut("PlasmaCannon eingesammelt (Netz).");
			if (IsLocal)
				if (message)message->SetText(T_GOTPLASMA,RGB(255,255,255));
		}
		break;
	case 20:if ((IsLocal)&&(isplayer))	// Getötet von...
		{
			CHAR c[150];
			sprintf(&c[0],T_FARMERWASHIT,data->userdata);
			game->ticker->Add(&c[0],D3DRGB(0,1,0));
		}
		break;
	case 21:	// Gegenstand eingesammelt
		{
#ifdef _DEBUG
			game->ticker->Add("Gegenstand eingesammelt (Client)");
#endif
			CItem* item=(CItem*)game->chain->Find(data->p2);
			if (item!=NULL)
			{
				PickupItem(item);
				if (IsLocal)
				{
					CSound* s=new CSound();
					s->Create2D(game->lpBuffers[29]);
					s->Play();
					game->AddSound(s);
				}
			}
		}
		break;
	case 22:	// Getroffen
		{
			Hit(LongToFloat(data->p2));
		}
		break;
	}
}

void CBauer::Unbind()
{
	if (object!=NULL)
	{
		switch(GetType(object->id))
		{
		case IDGartenkralle:
			{
				CGartenkralle* k=(CGartenkralle*)object;

				if (k->stoss.forked)k->SplatForked();
				k->parent=k->werfer=NULL;
				k->pos.y=world->HeightAtPoint(((CGartenkralle*)object)->pos.x,((CGartenkralle*)object)->pos.z);
				k->UpdateBox();
			}
			break;
		case IDGolfer:
			{
				((CGolfer*)object)->parent=NULL;
				((CGolfer*)object)->pos.y=world->HeightAtPoint(((CGolfer*)object)->pos.x,((CGolfer*)object)->pos.z);
				((CGolfer*)object)->schlag.phase=0.0f;
			}
			break;
		case IDDrescher:
			((CDrescher*)object)->GetOut();
			break;
		case IDPlasmaCannon:
			((CPlasmaCannon*)object)->parent=NULL;
			world->MakeInside(((CPlasmaCannon*)object)->pos,1.0f);
			((CPlasmaCannon*)object)->pos.y=world->HeightAtPoint(((CPlasmaCannon*)object)->pos.x,((CPlasmaCannon*)object)->pos.z);
			world->MakeInside(((CPlasmaCannon*)object)->pos,1.0f);
			((CPlasmaCannon*)object)->box->SetPos(((CPlasmaCannon*)object)->pos);
			break;
		}
	}
	object=NULL;
}

void CBauer::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if ((camera%2==0)&&(IsLocal)&&(isplayer))return;
	if (HasObject(IDDrescher))return;
	if (dead!=0.0f)return;

	lpDevice->SetTexture(0,NULL);
	D3DMATRIX matrix,m;

	D3DUtil_SetTranslateMatrix(matrix,pos.x,pos.y-head,pos.z);
	D3DUtil_SetRotateYMatrix(m,-ang.y);
	D3DMath_MatrixMultiply(matrix,m,matrix);

	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);
	lpMesh->Render(lpDevice,FALSE);
	if (lpHut)lpHut->Render(lpDevice,FALSE);
}

const BOOLEAN CBauer::Hit(const float damage)
{
	life-=damage;

	if (life<=0.0f)
	{	// gestorben
		Die();
		life=0.0f;
		dead=game->respawntime;
		Unbind();
		speed=addspeed=D3DVECTOR(0,0,0);

		if ((IsLocal)&&(isplayer))
		{
			CBillboardText* b=new CBillboardText;
			b->SetText(T_TOT,RGB(255,0,0));
			game->AddGimmick(b);

			game->SetTransition(new CFadeTransition(TRUE,D3DRGB(1,0,0),NULL,game->respawntime+0.3f,1));
		}

		if (Config.blut)game->AddGimmick(new CBlood(pos));
		pos.y+=3.0f;
		if (!IsLocal)pos=D3DVECTOR(0,-100,0);
		return TRUE;
	}
	if ((IsLocal)&&(isplayer))
	{
		game->SetTransition(new CFadeTransition(FALSE,D3DRGB(1,0,0),NULL,0.2f,0.5f));
	}

	return FALSE;
}

void CBauer::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	CPlayer::DrawUI(lpDevice);
	if ((!IsLocal)||(!isplayer))return;

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,200);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	lpDevice->SetTexture(0,game->lpTexture[17]);

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,Herz,4,0);

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	
	if (IsLocal)
	{
		static CHAR c[20];
		sprintf(&c[0],"= %d",(int)floorf(hit));
		game->lpD3DFont->DrawText(70,game->GetFontCentered(75,90+bh/2),D3DRGB(1,1,1),&c[0]);

		sprintf(&c[0],"= %d %%",int(life));
		game->lpD3DFont->DrawText(70,game->GetFontCentered(120,120+35),D3DRGB(1,1,1),&c[0]);
	}
}

void CBauer::NextHut()
{
	if (IsLocal)SendNotify(8);
	hut++;
	CHAR c[120];
	sprintf(&c[0],"Hut%d",hut);
	lpHut=game->models->FindObject(&c[0]);
	if (lpHut==NULL)hut=0;
	if ((IsLocal)&&(isplayer)&&(hut>=0)&&(hut<sizeof(T_HATS)/sizeof(T_HATS[0])))
	{
		sprintf(&c[0],T_HAT,T_HATS[hut]);
		message->SetText(&c[0],RGB(255,255,255));
	}
}

const BOOLEAN CBauer::PickupItem(CItem* item)
{
	const float lifeoffset=50.0f;
	const float maxlife=125.0f;

	switch(item->subid)
	{
	case 0:if (life==maxlife)return FALSE;
		life+=lifeoffset;
		if (life>maxlife)life=maxlife;
		break;

	default:return FALSE;
	}
	return TRUE;
}

void CBauer::DrawShadow(LPDIRECT3DDEVICE7 lpDevice)
{
#if (SHADOWS)
	if (dead)return;
	RenderRoundShadow(lpDevice,pos.x,pos.y-0.55f,pos.z,0.85f);
#endif
}

void CBauer::HasHit(const UID whom)
{
	hit++;
	CPlayer::HasHit(whom);

	if (!isplayer)return;

	CChicken* c=(CChicken*)chain->Find(whom);
	if (c==NULL)return;

	if (IsLocal)
	{	// Meldung, wenn der Bauer lokal ist
		DPID id=UID2DPID(c->id);
		if (id!=DPID(-1))
		{	// Wenn Huhn ein Mensch ist
			CHAR name[100];
			GetPlayerName(id,name);

			CHAR text[200];
			sprintf(&text[0],T_FARMERHASHIT,name);
			game->ticker->Add(&text[0],D3DRGB(0,1,0));
		}
	}
	if ((c->IsLocal)&&(c->isplayer))
	{	// Meldung, wenn Huhn lokal ist, und ein Mensch
		DPID id=UID2DPID(this->id);
		if (id!=DPID(-1))
		{
			CHAR name[100];
			GetPlayerName(id,name);

			CHAR text[200];
			sprintf(&text[0],T_CHICKENWASHIT,name);
			game->ticker->Add(&text[0],D3DRGB(0,1,0));
		}
	}
}