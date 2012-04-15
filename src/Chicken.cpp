/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "chicken.h"
#include "game.h"
#include "sounds.h"
#include "effects.h"
#include "gartenkralle.h"
#include "golfer.h"
#include "bauer.h"
#include "ei.h"
#include "config.h"
#include "gibs.h"
#include "text.h"
#include "shadows.h"
#include "envmap.h"
#include "chickenplayer.h"



CMorpher* CChicken::morpher1=NULL,*CChicken::morpher2=NULL;



CChicken::CChicken()
:bound(NULL),scaresound(NULL),Golfspieler(0),
 golfergebnis(FALSE),lastthrow(0.0f),eier(10),produktion(0.0f),produziere(FALSE),
 install(NULL),theKI(NULL),maxfallspeed(10000.0f),morphphase(randf()*5.0f),
 canthrow(randf()*1.2f)
{
	positionmessage=new CMessageChain(2);
	flagsmessage=new CMessageChain(2);

	isplayer=FALSE;
	schlagspeed=abschlagpos=speed=D3DVECTOR(0,0,0);
	box=new CBoundingTube(D3DVECTOR(0,0,0),0.37f,0.7f);

	Flug.fliege=FALSE;
	Flug.power=100.0f;
	Flug.throttle=0.0f;
	Flug.animationphase=0.0f;
	Flug.yaw=Flug.pitch=Flug.roll=0.0f;

	Zappel.von=0;
	Zappel.nach=1;
	Zappel.phase=0.0f;

	if (morpher1==NULL)
	{
		morpher1=new CMorpher(game->models,7);
		morphtarget1=morpher1->SetTarget("Huhn1_T");

		morpher1->SetFrame(0,"Huhn1_5");
		morpher1->SetFrame(1,"Huhn1_6");
		morpher1->SetFrame(2,"Huhn1_7");
		morpher1->SetFrame(3,"Huhn1_1");
		morpher1->SetFrame(4,"Huhn1_2");
		morpher1->SetFrame(5,"Huhn1_3");
		morpher1->SetFrame(6,"Huhn1_4");
	}else{
		morphtarget1=morpher1->GetTarget();
	}

	if (morpher2==NULL)
	{
		morpher2=new CMorpher(game->models,3);
		morphtarget2=morpher2->SetTarget("Huhn3_T");

		morpher2->SetFrame(0,"Huhn3_1");
		morpher2->SetFrame(1,"Huhn3_2");
		morpher2->SetFrame(2,"Huhn3_3");
	}else{
		morphtarget2=morpher2->GetTarget();
	}

	morpher1->AddRef();
	morpher2->AddRef();
}

BOOLEAN CChicken::Init()
{
	pos=world->Rand(1.0f);

	ang=D3DVECTOR(0,randf()*4.0f,0);

	mesh[0]=game->models->FindObject("Huhn1");
	mesh[1]=game->models->FindObject("Huhn2");
	mesh[2]=game->models->FindObject("Huhn3_1");

	dir=2.0f+randf()*5.0f;
	if (rand()%2==0)dir=-dir;
	time=float(rand()%7);

	id=MakeUID(IDChicken);

	theKI=new CChickenKI(this);

	D3DUtil_SetIdentityMatrix(Flug.matrix);

	return TRUE;
}

CChicken::~CChicken()
{
	positionmessage->ShutDown();
	flagsmessage->ShutDown();
	if (morpher1->Release())
	{
		SaveDelete(morpher1);
	}
	if (morpher2->Release())
	{
		SaveDelete(morpher2);
	}

//	DebugOut("CChicken::~CChicken();");
	delete box;
	if (theKI)delete theKI;
	if (scaresound)
	{
		scaresound->Stop();
		scaresound->Remove();
		scaresound=NULL;
	}
}

void CChicken::KI(const float elapsed)
{
	if (bound!=NULL)return;
	if (Golfspieler!=0)return;
	if ((theKI)&&(Config.KI)&&(game->IsServer))
	{
		theKI->Think(elapsed);
		pos.y=world->HeightAtPoint(pos.x,pos.z);

		CObject* akt=chain->GetFirst();
		while (akt)
		{
			if (GetType(akt->id)==IDItem)
			{
				CItem* item=(CItem*)akt;
				if ((box->IsInside(item->pos,0.7f))&&(PickupItem(item)))
				{
					SendNotify(21,item->id);
					item->Remove();
					Send(DPID_ALLPLAYERS,NM_REMOVE,item->id,0,0,0,NULL,0,TRUE);
				}
			}

			akt=akt->next;
		}

		return;
	}

	if (dir==0)
	{
		dir=3.0f+rand()%5;
		if (rand()%2==0)dir=-dir;
		if (Config.gackern)
		{
			CSound* s=new CSound(game->lpBuffers[0],TRUE);

			s->Set3DParameters(pos);
			s->SetFrequency(10000+rand()%2000);
			s->Play();

			game->AddSound(s);
		}
	}else {
		if (dir>0)
		{
			dir-=elapsed;
			if (dir<0)dir=0;
		}
		if (dir<0)
		{
			dir+=elapsed;
			if (dir>0)dir=0;
		}
	}
	if (bound==NULL)
	{
		if (!produziere)
		{
			float w=elapsed;
			if ((dir>0)&&(dir<g_PI*0.5f))w*=sinf(dir-g_PI*0.5f)+1.0f;
			if ((dir<0)&&(dir>-g_PI*0.5f))w*=sinf(dir-g_PI*0.5f)+1.0f;

			if (dir>0)ang.y+=w;
			if (dir<0)ang.y-=w;

			D3DVECTOR s2;
			speed=D3DVECTOR(0,0,0.7f);
			D3DMATRIX m;
			D3DUtil_SetRotateYMatrix(m,ang.y);
			D3DMath_VectorMatrixMultiply(s2,speed,m);
			pos+=(s2*float(elapsed));

			morphphase+=0.7f*elapsed*2.2f;

			pos.y=world->HeightAtPoint(pos.x,pos.z);
		}
	}
}

void CChicken::Execute(float elapsed)
{
	CPlayer::Execute(elapsed);

	positionmessage->Execute(elapsed);
	flagsmessage->Execute(elapsed);

	if (lastthrow>0.0f)
	{
		lastthrow-=elapsed;
		if (lastthrow<0.0f)lastthrow=0.0f;
	}
	const float stallfaktor=4.0f;
	if (Flug.fliege==FALSE)
	{
		Flug.power+=(elapsed*game->replenishpower)*(install?stallfaktor:1.0f);
		if (Flug.power>100.0f)Flug.power=100.0f;
	}else{
		Flug.power-=Flug.throttle*0.05f*elapsed*game->takepower;
		if (Flug.power<0.0f)Flug.power=0.0f;
	}

	if ((produziere)&&(Golfspieler==0)&&(bound==NULL))
	{
		produktion+=(elapsed*game->sittime/powf(float(eier+1),0.25f))*(install?stallfaktor:1.0f);
		while (produktion>=1.0f)
		{
			eier++;
			produktion-=1.0f;
		}
		if (eier>chicken_maxeier)eier=chicken_maxeier;
	}


	time+=elapsed;

	if (Golfspieler!=0)
	{
		ang.y+=(elapsed*9.8f);
		ang.x+=(elapsed*2.2f);


		pos+=schlagspeed*elapsed;
		schlagspeed.y-=(elapsed*game->gravity);

		if ((!world->IsInside(pos))&&(game->IsServer))
		{	// Huhn ist auf eine Wand gestoßen
			world->MakeInside(pos);
			GolfEnd();
			ang.x=0.0f;
			Splat(SPLAT_NORMAL);
			return;
		}
		CObject* akt=chain->GetFirst();
		while (akt)
		{
			if ((akt!=this)&&(GetType(akt->id)==IDChicken))
			{	// Huhn hat anderes Huhn getroffen
				CChicken* c=(CChicken*)akt;
				if ((c->box->Collidate(box))&&(c->dead==0.0f))
				{
					c->Splat(SPLAT_NORMAL);
					if (Golfspieler!=0)
					{
						CPlayer* g=(CPlayer*)chain->Find(Golfspieler);
						if (g) g->HasHit(id);
					}
				}
			}
			if ((akt->IsType(IDBauer))&&(game->IsServer))
			{	// Huhn hat Bauern getroffen. XD
				CBauer* b=(CBauer*)akt;
				if ((b->box->Collidate(box))&&(b->dead==0.0f))
				{
					GolfEnd();
					ang.x=0.0f;
					Splat(SPLAT_NORMAL);

					const float d=15+randf()*20.0f;

					hit+=d/100.0f;
					b->SendNotify(22,FloatToLong(d),NULL,0);
					if (b->Hit(d) && isplayer)
					{
						HasHit(b->id);

						// Funktioniert nicht so recht, da ist mehr Gehirnschmalz nötig
/*						if (b->isplayer && IsLocal)
						{
							// Lokale Meldung ausgeben
							CHAR name[100];
							DPID dpid=UID2DPID(b->id);
							if (dpid!=DPID(-1))
							{
								GetPlayerName(dpid,name);
								CHAR text[100];
								sprintf(&text[0],T_CHICKENHASHIT,name);
								game->ticker->Add(&text[0],D3DRGB(0,1,0));
							}

							// Dann Meldung an Bauern schicken
							dpid=UID2DPID(id);
							if (dpid!=DPID(-1))
							{
								CHAR name[100];
								GetPlayerName(dpid,name);

								b->SendNotify(20,0,0,&name[0],strlen(&name[0])+1,TRUE);
							}
						}*/
					}
				}

			}


			akt=akt->next;
		}

		if ((pos.y<world->HeightAtPoint(pos.x,pos.z))&&(IsLocal || !isplayer))
		{	// Huhn auf Boden aufgekommen
			pos.y=world->HeightAtPoint(pos.x,pos.z);

			schlagspeed.y=-schlagspeed.y*0.55f;
			if (schlagspeed.y>maxfallspeed)
				schlagspeed.y=maxfallspeed*0.55f;
			maxfallspeed=schlagspeed.y;

			if (schlagspeed.y<1.5f)
			{	// Hinstellen
				ax=0.2f;
				ang=D3DVECTOR(0,0,0);
				schlagspeed=D3DVECTOR(0,0,0);
				Golfspieler=0;
				if (IsLocal)SendFlags();
			}else{
				// Abprallen
				CSound* s=new CSound(game->lpBuffers[3],TRUE);
				s->Set3DParameters(pos);
				s->SetFrequency(19000+rand()%7000);
				s->Play();
				game->AddSound(s);
				GolfEnd();
			}
		}
		box->SetPos(pos);
		KI(elapsed);
		Zappel.Execute(elapsed);
		return;
	}

	KI(elapsed);

	if (bound==NULL)
	{
		world->MakeInside(pos);
		if ((game->IsServer)&&(isplayer==FALSE))
		{	// Position im Netzwerk updaten
			SendPosition();
		}
	}else{
		Zappel.Execute(elapsed);
		pos=((CGartenkralle*)bound)->pos;
	}

	box->SetPos(pos);
}

void CChicken::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (dead>0)return;
	if (!box->IsVisible())return;
	lpDevice->SetTexture(0,NULL);
	D3DMATRIX matrix,m;
	if (bound==NULL)
	{
		if (Flug.fliege==FALSE)
		{
			D3DUtil_SetTranslateMatrix(matrix,pos.x,pos.y,pos.z);

			world->Kippe(matrix,pos.x,pos.z);

			D3DUtil_SetRotateYMatrix(m,ang.y+g_PI*0.5f);
			D3DMath_MatrixMultiply(matrix,m,matrix);

			D3DUtil_SetTranslateMatrix(m,0,0.15f,0);
			D3DMath_MatrixMultiply(matrix,m,matrix);
		
			D3DUtil_SetRotateXMatrix(m,ang.x);
			D3DMath_MatrixMultiply(matrix,m,matrix);

			D3DUtil_SetTranslateMatrix(m,0,-0.15f,0);
			D3DMath_MatrixMultiply(matrix,m,matrix);
		}else{
			matrix=Flug.matrix;
			D3DMATRIX m;
			D3DUtil_SetRotateYMatrix(m,g_PI*0.5f);
			D3DMath_MatrixMultiply(matrix,m,matrix);
		}
	}else{
		matrix=((CGartenkralle*)bound)->BerechneStoss(); 
		D3DUtil_SetTranslateMatrix(m,0,-0.22f,0);
		D3DMath_MatrixMultiply(matrix,m,matrix);
	}
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);

	LPDIRECTDRAWSURFACE7 lpTexture=NULL;

/*	if (Config.reflections)
	{	// Environmentmap berechnen
		D3DVERTEX* vertices;
		WORD* indices;
		DWORD numvertices,numindices;

		morphtarget2->GetMeshGeometry(&vertices,&numvertices,&indices,&numindices);
		CSphereMap::ApplySphereMap(lpDevice,vertices,numvertices);


		lpTexture=game->lpTexture[31];
	}*/

	int t=GetHuhnType();
	CD3DFileObject* model=mesh[t];
	if (Config.Animations)
	{
		if ((t==0)&&(bound==NULL)&&(Golfspieler==0)&&(speed!=D3DVECTOR(0,0,0)))
		{
			morpher1->LinearMorph(morphphase,3,6);
			model=morphtarget1;
		}

		if ((t==0)&&((bound!=NULL)|(Golfspieler!=0)))
		{
			morpher1->Morph(Zappel.von,Zappel.nach,Zappel.phase);
			model=morphtarget1;
		}

		if (t==2)
		{
			morpher2->LinearMorph(Flug.animationphase,0,2);
			model=morphtarget2;
		}
	}
	model->Render(lpDevice,FALSE,lpTexture);
}

void CChicken::Splat(const enum SPLAT_TYPE art)
{
	if (dead>0.0f)return;
	if (game->IsServer)SendNotify(1,art);
	if (bound!=NULL)
		((CGartenkralle*)bound)->stoss.forked=NULL;
	if (Golfspieler!=0)
	{
		CPlayer* p=(CPlayer*)chain->Find(Golfspieler);
		if (p)p->HasHit(id);
		Golfspieler=0;
	}
	bound=NULL;
	if (scaresound)
	{
		scaresound->Stop();
		scaresound->Remove();
		scaresound=NULL;
	}

	if (Flug.fliege)EndeFlug();
	
	dead=GetDeadTime();

	ang=schlagspeed=D3DVECTOR(0,0,0);

	if (art==SPLAT_NORMAL)
	{
		if (Config.blut)game->AddGimmick(new CBlood(pos));
		if (Config.gibs)game->AddGimmick(new CGibs(pos,1));
		if (Config.federn)game->AddGimmick(new CFeder(pos,FALSE));
	}
	if (art==SPLAT_FRY)
	{
		if (Config.gibs)game->AddGimmick(new CGibs(pos,2));
		if (Config.federn)game->AddGimmick(new CFeder(pos,FALSE));
		if (Config.blut)game->AddGimmick(new CBlood(pos));
	}

	if (Config.soundon)
	{
		CSound *s=new CSound(game->lpBuffers[1],TRUE);
		s->Set3DParameters(pos);
		s->SetFrequency(20000+rand()%4000);
		s->Play();
		game->AddSound(s);
	} 
	Die();
	if (IsLocal)SendFlags();
}

void CChicken::Fork(const CObject* b)
{
	// Huhn von alter Gartenkralle lösen
	if (bound!=NULL)
		((CGartenkralle*)bound)->stoss.forked=NULL;
	bound=b;
	Golfspieler=0;

	if (b==NULL)
	{
		DebugOut("Nothing to fork!");
		return;
	}
	if (game->IsServer)SendNotify(2,b->id);
	if (Config.soundon)
	{
		if (!scaresound)game->AddSound(scaresound=new CSound(game->lpBuffers[5],FALSE));

		scaresound->Set3DParameters(pos);
		scaresound->SetFrequency(10000+rand()%2000);
		scaresound->Play();
	}
}

void CChicken::Schlag(const CObject *g)
{
	DebugOut("CChicken::Schlag();");
	if (Flug.fliege)
		EndeFlug();

	if ((Golfspieler==0)||(golfergebnis==TRUE))
	{
		abschlagpos=pos;
	}

	if (bound!=NULL)
	{	// Huhn von Gartenkralle schlagen
		((CGartenkralle*)bound)->stoss.forked=NULL;
		bound=NULL;
	}

	Golfspieler=g->id;
	maxfallspeed=10000.0f;

	D3DMATRIX m;
	schlagspeed=D3DVECTOR(0,7.0f+randf()*4.0f,11.0f+randf()*5.5f);
	golfergebnis=FALSE;
	
	D3DUtil_SetRotateYMatrix(m,-((CBauer*)g)->ang.y);
	D3DMath_VectorMatrixMultiply(schlagspeed,schlagspeed,m);

	CSound* s=new CSound(game->lpBuffers[3],TRUE);
	s->Set3DParameters(pos);
	s->SetFrequency(20000+rand()%4000);
	s->Play();
	game->AddSound(s);

	s=new CSound(game->lpBuffers[5],TRUE);
	s->Set3DParameters(pos);
	s->SetFrequency(10000+rand()%2000);
	s->Play();
	game->AddSound(s);
	if (IsLocal)SendFlags();

	if (Config.federn)
	{
		game->AddGimmick(new CFeder(pos+D3DVECTOR(0,0.4f,0),TRUE));
		game->AddGimmick(new CFederschweif(id,0.4f,35.0f));
	}
}

void CChicken::Blast(const CObject* player,const D3DVECTOR pressure)
{
	if (player==NULL)return;
	DebugOut("CChicken::Blast();");

	if (Flug.fliege)
		EndeFlug();

	abschlagpos=pos;

	Golfspieler=player->id;
	golfergebnis=TRUE;	// Kein "Golfergebnis"

	schlagspeed=pressure;
	maxfallspeed=10000.0f;

	CSound* s;
	s=new CSound(game->lpBuffers[5],TRUE);
	s->Set3DParameters(pos);
	s->SetFrequency(10000+rand()%2000);
	s->Play();
	game->AddSound(s);
	if (IsLocal)SendFlags();
}

void CChicken::Notify(PNETWORKDATA data)
{
	switch (data->p1)
	{
	case 1:		// getroffen
		world->MakeInside(pos);
		Splat((SPLAT_TYPE)data->p2);
		break;
	case 2:		// aufgespießt
		Fork(game->chain->Find(data->p2));
		break;
	case 10:
		{		// Positionsupdate
#ifdef _DEBUG
			if (IsLocal)game->ticker->Add("CChicken: Netzwerkposition bei lokalem Spieler empfangen",D3DRGB(1,0,0));
			if (bound)game->ticker->Add("CChicken: Netzwerkposition bei aufgespießten Huhn empfangen",D3DRGB(1,0,0));
#endif
			if (dead!=0.0f)return;
			struct _r
			{
				D3DVECTOR pos,schlag,ang,speed;
			}*r=(_r*)data->userdata;
			pos=r->pos;
			box->SetPos(pos);
			schlagspeed=r->schlag;
			ang=r->ang;
			speed=r->speed;
			Flug.fliege=FALSE;
		}
		break;
	case 30:
		{	// Positionsupdate / Flug
			if (dead!=0.0f)return;
			struct _r
			{
				D3DMATRIX matrix;
				float throttle;
			}*r=(_r*)data->userdata;
			Flug.matrix=r->matrix;
			Flug.throttle=r->throttle;
			Flug.fliege=TRUE;
			pos=D3DVECTOR(Flug.matrix.m[3][0],Flug.matrix.m[3][1]-0.25f,Flug.matrix.m[3][2]);
			box->SetPos(pos);
		}
		break;
	case 13:
		{		// Flagsupdate
			struct _r
			{
				UID golfer;
				float flugpower;
				BOOLEAN produziere;
				CStall* install;
			}*r=(_r*)data->userdata;

			produziere=r->produziere;
//			if (r->golfer!=0)golfer=chain->Find(r->golfer);else golfer=NULL;
			if ((r->golfer==0)&&(Golfspieler!=0))ang=D3DVECTOR(0,0,0);
			Golfspieler=r->golfer;
			Flug.power=r->flugpower;
			install=r->install;
		}
		break;
	case 11:
		{	// Ei werfen
			lastthrow=0.0f;
			eier+=(BYTE)data->p3;
			UID *id=(UID*)data->userdata;

			Throw(LongToFloat(data->p2),data->p3,id);
		}
		break;
	case 12:	// Huhn wegdrücken
		{
			CPlayer* p=(CPlayer*)chain->Find(data->p2);
			Blast(p,*(D3DVECTOR*)data->userdata);
		}
		break;
	case 21:
		{	// Gegenstand aufsammeln
			CItem* item=(CItem*)game->chain->Find(data->p2);
			if (item!=NULL)PickupItem(item);
		}
		break;
	}
}

const float CChicken::GetDeadTime()const
{
	return game->respawntime*(1.0f+randf()*0.2f);
}

void CChicken::Throw(const float ax,const int num,UID *ids)
{
	if (dead>0.0f)goto error;
	if (bound!=NULL)goto error;
	if (lastthrow>0.0f)goto error;
	if (eier==0)goto error;

	lastthrow=game->throwtime;
	if (Flug.fliege)lastthrow*=0.8f;

	{
		CSound* s=new CSound(game->lpBuffers[2],TRUE);
		s->Set3DParameters(pos);
		s->Play();
		game->AddSound(s);

		const float wurfbreite=1.0f;
		for (int i=0;i<num;i++)if (eier>0)
		{
			float x=-wurfbreite*0.5f+(i+1)*wurfbreite/(num+1);
			eier--;
			CEi* ei=new CEi(pos+D3DVECTOR(0,0.55f,0),D3DVECTOR(ax,ang.y,0),speed,D3DVECTOR(x,0,0),Flug.fliege,ei_wurfspeed,id);
			chain->Add(ei);
			if (ids[i]!=0)ei->id=ids[i];else ids[i]=ei->id;
		}else ids[i]=0;
	}
	return;

error:
	ZeroMemory(ids,num*sizeof(UID));
	return;
}

const int CChicken::GetHuhnType()const
{
	if (bound)return 0;
	if (Golfspieler!=0)return 0;

	if (produziere)return 1;
	if (Flug.fliege)return 2;

	return 0;
}

void CChicken::SendPosition()
{
	if (Flug.fliege==FALSE)
	{
		struct _r
		{
			D3DVECTOR pos,schlag,ang,speed;
		}r;

		r.pos=pos;
		r.schlag=schlagspeed;
		r.ang=ang;
		r.speed=speed;

		positionmessage->Send(DPID_ALLPLAYERS,NM_NOTIFY,id,10,0,0,&r,sizeof(r),FALSE);
	}else{
		struct _r
		{
			D3DMATRIX m;
			float throttle;
		}r;

		r.m=Flug.matrix;
		r.throttle=Flug.throttle;

		positionmessage->Send(DPID_ALLPLAYERS,NM_NOTIFY,id,30,0,0,&r,sizeof(r),FALSE);
	}
}

void CChicken::SendFlags()
{
	struct _r
	{
		UID golfer;
		float flugpower;
		BOOLEAN produziere;
		const CStall* install;
	}r;
	r.golfer=Golfspieler;
	r.produziere=produziere;
	r.flugpower=Flug.power;
	r.install=install;

	positionmessage->Clear();
	flagsmessage->Send(DPID_ALLPLAYERS,NM_NOTIFY,id,13,0,0,&r,sizeof(r),TRUE);
//	SendNotify(13,0,0,&r,sizeof(r),TRUE);
}

static float record=0.0f;

void CChicken::GolfEnd()
{
	if (golfergebnis)return;
	if (Golfspieler==0)return;

	CPlayer* player=(CPlayer*)chain->Find(Golfspieler);
	if (player==NULL)return;
	if (!player->IsLocal)return;
	if (!player->isplayer)return;
	if (abschlagpos==D3DVECTOR(0,0,0))return;

	float length=Magnitude(abschlagpos-pos);
	golfergebnis=TRUE;

	if (!Config.golfergebnisse)return;

	BOOLEAN r=FALSE;
	if (length>record)
	{
		record=length;
		CSound* s=new CSound(TRUE);
		s->Create2D(game->lpBuffers[28]);
		s->Play();
		game->AddSound(s);
		r=TRUE;
	}

	CHAR c[100];
	sprintf(&c[0],T_GOLFEND,length);
	CBillboardText* t=new CBillboardText();
	t->SetText(&c[0],r?RGB(192,192,0):RGB(255,0,0));
	game->AddGimmick(t);
}

void CChicken::Revive()
{
	pos=world->Rand(1.5f);
	Flug.power=75.0f;
	Flug.fliege=FALSE;
	eier=10;
}

const BOOLEAN CChicken::PickupItem(CItem* item)
{
	switch(item->subid)
	{
	case 1:
		if (eier>=chicken_maxeier)return FALSE;
		eier+=16;
		if (eier>chicken_maxeier)eier=chicken_maxeier;
		break;




	default:return FALSE;
	}
	return TRUE;
}

void CChicken::DrawShadow(LPDIRECT3DDEVICE7 lpDevice)
{
#if SHADOWS
	if (dead)return;
	RenderRoundShadow(lpDevice,pos.x,pos.y+0.2f,pos.z,0.3f);
#endif
}

void CChicken::StartFlug()
{
	pos.y+=0.4f;
	Flug.fliege=TRUE;
	Flug.throttle=10.0f;

	Flug.yaw=Flug.pitch=Flug.roll=0.0f;

	speed=D3DVECTOR(0,0,8.0f);
	D3DMATRIX m;
	D3DUtil_SetRotateYMatrix(Flug.matrix,ang.y);
	D3DUtil_SetRotateXMatrix(m,-ax);
	D3DMath_MatrixMultiply(Flug.matrix,m,Flug.matrix);

	D3DMath_VectorMatrixMultiply(speed,speed,Flug.matrix);

	D3DUtil_SetTranslateMatrix(m,pos);
	D3DMath_MatrixMultiply(Flug.matrix,Flug.matrix,m);

	SendFlags();
	SendPosition();
}

void CChicken::EndeFlug()
{
	Flug.fliege=FALSE;
	speed=D3DVECTOR(0,0,0);
//	pos.y=world->HeightAtPoint(pos.x,pos.z);

	Flug.matrix.m[3][0]=0.0f;
	Flug.matrix.m[3][1]=0.0f;	
	Flug.matrix.m[3][2]=0.0f;

	D3DVECTOR v=D3DVECTOR(0,0,1);
	D3DMath_VectorMatrixMultiply(v,v,Flug.matrix);

	ang.y=asinf(v.x);
	ax=asinf(v.y);

	if (IsLocal)
	{
		SendFlags();
		SendPosition();
	}
}


void CChicken::Fliege(float elapsed)
{
//	Allgemeine Geschwindigkeit bremsen
	speed*=powf(0.3f,elapsed);

	D3DMATRIX rotmat=Flug.matrix,m;
	rotmat.m[3][0]=rotmat.m[3][1]=rotmat.m[3][2]=0.0f;
	D3DMath_MatrixInvert(m,rotmat);

	D3DVECTOR localspeed=speed;
	D3DMath_VectorMatrixMultiply(localspeed,localspeed,m);

	D3DVECTOR forces,v;

	// Lokale Kräfte
	forces=D3DVECTOR(0,2.8f*sqrtaf(localspeed.z),Flug.throttle*(Flug.power/130.0f+0.1f));
	v=D3DVECTOR(0,1.5f,0.0f);

	// Lokale Kräfte auf Welt Koordinaten drehen

	D3DMath_VectorMatrixMultiply(v,v,rotmat);
	// v ist der Normalenvektor der Flügel
	// Verrechne die Auftriebskraft die Stellung der Flügel
	float moreauftrieb=DotProduct(-speed,v);
	forces.y+=moreauftrieb;//*sqrtaf(localspeed.z);

	D3DMath_VectorMatrixMultiply(forces,forces,rotmat);

	// Globale Kräfte addieren
	forces.y-=game->gravity;

	// Kräfte zu Geschwindigkeit addieren
	speed+=forces*elapsed;

	D3DVECTOR a=D3DVECTOR(0,0,1);
	D3DMath_VectorMatrixMultiply(a,a,rotmat);

	ang.y=atan2f(a.x,a.z);
	ax=asinf(a.y);

	// Geschwindigkeit zu Position addieren
	D3DUtil_SetTranslateMatrix(m,speed*elapsed);
	D3DMath_MatrixMultiply(Flug.matrix,Flug.matrix,m);

	pos=D3DVECTOR(Flug.matrix.m[3][0],Flug.matrix.m[3][1],Flug.matrix.m[3][2]);
	// "Flugzeug" drehen und schlingern lassen
//	if (1==2)
	{
		//	Lokale X-Achse drehen
		D3DMath_MatrixInvert(m,rotmat);
		D3DMath_VectorMatrixMultiply(forces,forces,m);
		D3DUtil_SetRotateXMatrix(m,-0.065f*forces.y*elapsed);
		D3DMath_MatrixMultiply(Flug.matrix,m,Flug.matrix);


		// Globale X-Achse drehen
/*		D3DMATRIX m2;
		D3DUtil_SetTranslateMatrix(m,-pos);
		D3DMath_MatrixMultiply(Flug.matrix,Flug.matrix,m);

		const float drehgeschw=0.02f;
		D3DUtil_SetRotateYMatrix(m,-ang.y);
		D3DUtil_SetRotateXMatrix(m2,drehgeschw*sqrtaf(forces.z)*elapsed);
		D3DMath_MatrixMultiply(m,m,m2);
		D3DUtil_SetRotateYMatrix(m2,ang.y);
		D3DMath_MatrixMultiply(m,m,m2);
		D3DMath_MatrixMultiply(Flug.matrix,Flug.matrix,m);

		D3DUtil_SetTranslateMatrix(m,pos);
		D3DMath_MatrixMultiply(Flug.matrix,Flug.matrix,m);*/
	}
	pos.y-=0.25f;
	box->SetPos(pos);

	{	// Huhn rotieren lassen
		D3DXQUATERNION qR;
		D3DXMATRIX m;
		D3DXQuaternionRotationYawPitchRoll(&qR,Flug.yaw*elapsed,Flug.pitch*elapsed,Flug.roll*elapsed);
		D3DXMatrixRotationQuaternion(&m,&qR);
		D3DMath_MatrixMultiply(Flug.matrix,m,Flug.matrix);
	}

	Flug.animationphase+=elapsed*(Flug.throttle*(Flug.power/150.0f+0.4f)/6.0f+0.3f);

	const float faktor=powf(0.25f,elapsed);
	Flug.yaw*=faktor;
	Flug.pitch*=faktor;
	Flug.roll*=faktor;
}

void CChicken::Unbind()
{
	if (bound!=NULL)
	{
		((CGartenkralle*)bound)->stoss.forked=NULL;

		bound=NULL;

	}
}