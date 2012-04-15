/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "chickenplayer.h"
#include "game.h"
#include "tree.h"
#include "bauer.h"
#include "drescher.h"
#include "config.h"
#include "text.h"



CChickenPlayer::CChickenPlayer()
{
	isplayer=TRUE;
	ax=0.2f;
}

CChickenPlayer::~CChickenPlayer()
{
	if (IsLocal)Send(DPID_ALLPLAYERS,NM_REMOVE,id,0,0,0,NULL,0,TRUE);
}

BOOLEAN CChickenPlayer::Init()
{
	CPlayer::Init();
	CChicken::Init();

	MakeBillboardVertices(&EierVertex[0],10,120,60,35);
	MakeBillboardVertices(&FlugVertex[0],10,160,60,35);
	return TRUE;
}

void CChickenPlayer::KI(const float elapsed)
{
	if (Golfspieler!=0)
	{
		if (!IsLocal)return;
		if (bound)return;
		// Position im Netz aktualisieren
		SendPosition();
		return;
	}
	// Aircontrol
	speed+=addspeed*elapsed;
	if (Flug.fliege) 
	{
/*		if (IsLocal)Fliege(elapsed); 
		else Flug.animationphase+=elapsed*1.4f;*/
		Fliege(elapsed);
	}else {
		speed.y=0.0f;

		morphphase+=(speed.z+speed.x*0.2f)*elapsed*1.8f;

		D3DVECTOR s2,oldpos=pos;
		D3DMATRIX m;
		D3DUtil_SetRotateYMatrix(m,ang.y);
		D3DMath_VectorMatrixMultiply(s2,speed,m);
		CChicken::pos+=(s2*elapsed);
	}

	if (pos.x<world->minx())pos.x=world->minx();
	if (pos.x>world->maxx())pos.x=world->maxx();
	if (pos.z<world->minz())pos.z=world->minz();
	if (pos.z>world->maxz())pos.z=world->maxz();

	D3DVECTOR n;
	float y=world->HeightAtPoint(pos.x,pos.z,&n);
	
	if (Flug.fliege)
	{
		if (pos.x==world->minx())CollidateFlug(D3DVECTOR(1,0,0));
		if (pos.x==world->maxx())CollidateFlug(D3DVECTOR(-1,0,0));
		if (pos.z==world->minz())CollidateFlug(D3DVECTOR(0,0,1));
		if (pos.z==world->maxz())CollidateFlug(D3DVECTOR(0,0,-1));

		if (y>=pos.y) CollidateFlug(n); // gelandet
	}else pos.y=y;

	box->SetPos(pos);

	if ((IsLocal)&&(dead==0.0f))
	{
		const CStall* oldstall=install;
		install=NULL;

		CObject* akt=chain->GetFirst();
		while (akt)
		{
			if (akt==this)goto weiter;

			if (GetType(akt->id)==IDTree)
			{
				if (box->Collidate(((CTree*)akt)->box))
				{
					pos=box->MoveOut(((CTree*)akt)->box);
					CollidateFlug(MakePlanar(box->GetPos()-(((CTree*)akt)->box)->GetPos()));
				}
			}
			if ((GetType(akt->id)==IDBauer)&&(((CBauer*)akt)->dead==0.0f))
			{
				if (box->Collidate(((CBauer*)akt)->box))
				{
					pos=box->MoveOut(((CBauer*)akt)->box);
					if (Flug.fliege)
					{
						if (CollidateFlug(MakePlanar(box->GetPos()-(((CBauer*)akt)->box)->GetPos())))
						{	// Kamikazeangriff auf Bauer. Energie abziehen
							CBauer* b=(CBauer*)akt;

							const float d=30+randf()*30.0f;

							hit+=d/100.0f;
							SendNotify(22,b->id,FloatToLong(d),&b->life,sizeof(b->life));

							if (b->Hit(d))
							{
								HasHit(b->id);

								if (b->isplayer)
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
								}
							}
						}
					}
				}
			}
			if ((GetType(akt->id)==IDChicken)&&(((CChicken*)akt)->dead==0.0f))
			{
				if (box->Collidate(((CChicken*)akt)->box))
				{
					pos=box->MoveOut(((CChicken *)akt)->box);
					if (CollidateFlug(MakePlanar(box->GetPos()-(((CChicken*)akt)->box)->GetPos())))
					{
						if (!game->IsServer)
							((CChicken*)akt)->SendNotify(1,SPLAT_NORMAL);
						((CChicken*)akt)->Splat(SPLAT_NORMAL);
					}
				}
			}

			if (GetType(akt->id)==IDDrescher)
			{
				CDrescher* d=(CDrescher*)akt;

				if (box->Collidate(d->box1))	// Huhn nach ausserhalb bewegen
				{
					pos=box->MoveOut(d->box1);
					CollidateFlug(MakePlanar(box->GetPos()-(((CDrescher*)akt)->box1)->GetPos()));
					goto weiter;
				}
			}
			if (GetType(akt->id)==IDStall)	// Stall prüfen
			{
				if (box->Collidate(((CStall*)akt)->box))
				{	// Über Eingang oder Draussen?
					CollidateFlug(MakePlanar(box->GetPos()-(((CStall*)akt)->box)->GetPos()));
					const CStall* as=(CStall*)akt;
					if (oldstall==as)install=(CStall*)akt;
						else pos=box->MoveOut(((CStall*)akt)->box);
				}else if (box->Collidate(((CStall*)akt)->eingang))
				{	// Im Eingang
					install=(CStall*)akt;
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

					CSound* s=new CSound();
					s->Create2D(game->lpBuffers[29]);
					s->Play();
					game->AddSound(s);
				}
			}

weiter:
			akt=akt->next;
		}

		if (oldstall!=install)SendFlags();
		// Position im Netz aktualisieren
		if (bound==NULL)SendPosition();
	}
	if (Flug.fliege)
	{
		Flug.matrix.m[3][0]=pos.x;
		Flug.matrix.m[3][1]=pos.y+0.25f;
		Flug.matrix.m[3][2]=pos.z;
	}
}

void CChickenPlayer::Execute(float elapsed)
{
	CChicken::Execute(elapsed);

//	CPlayer::pos=CChicken::pos;
	CPlayer::ang=CChicken::ang;
	box->SetPos(pos);
}

void CChickenPlayer::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if (IsLocal)
	{
		if (camera%2==0)return;
	}
	CChicken::Draw(lpDevice);
}

void CChickenPlayer::GetViewMatrix(D3DMATRIX &view)
{
	D3DMATRIX m;
	if (Flug.fliege)
	{
		if (camera%2==0)
		{
			D3DMath_MatrixInvert(view,Flug.matrix);
			if (game->KeyDown(DIK_B))
			{
				D3DUtil_SetRotateYMatrix(m,g_PI);
				D3DMath_MatrixMultiply(view,view,m);
			}
		}
		if (camera%2==1)
		{
			D3DMath_MatrixInvert(view,Flug.matrix);			

			if (game->KeyDown(DIK_B))
			{
				D3DUtil_SetRotateYMatrix(m,g_PI);
				D3DMath_MatrixMultiply(view,view,m);
			}

			D3DUtil_SetTranslateMatrix(m,0,-0.7f,2.0f);
			D3DMath_MatrixMultiply(view,view,m);
		}

		return;
	}
	if (camera%2==1)
	{
		D3DUtil_SetTranslateMatrix(view,-pos.x,-pos.y,-pos.z);
		D3DUtil_SetRotateYMatrix(m,-ang.y);
		D3DMath_MatrixMultiply(view,view,m);

		D3DUtil_SetRotateXMatrix(m,ax);
		D3DMath_MatrixMultiply(view,view,m);

		D3DUtil_SetTranslateMatrix(m,0,-0.9f,2.5f);
		D3DMath_MatrixMultiply(view,view,m);
		
		return;
	}

	D3DUtil_SetTranslateMatrix(view,-pos.x,-pos.y-(bound?0.0f:(produziere?0.30f:0.45f)),-pos.z);
	D3DUtil_SetRotateYMatrix(m,-ang.y);
	D3DMath_MatrixMultiply(view,view,m);

	if (Golfspieler!=0)D3DUtil_SetRotateXMatrix(m,ang.x);
		else D3DUtil_SetRotateXMatrix(m,ax);
	D3DMath_MatrixMultiply(view,view,m);
}

void CChickenPlayer::HandleInput(float elapsed,int mx,int my,int mz)
{
	if (Flug.fliege)
	{
		HandleFlug(elapsed,mx,my,mz);
		return;
	}

	ax-=float((Config.reversemouse?-my:my))/float(Config.sensitivity*10.0f);
	if (ax<-g_PI*0.5f)ax=-g_PI*0.5f;
	if (ax>g_PI*0.5f)ax=g_PI*0.5f;
	const float w=float(mx)/float(Config.sensitivity*10.0f);
	ang.y+=w;

	if (Golfspieler!=0)return;
	if (w!=0.0f)
	{
		D3DMATRIX m;
		D3DUtil_SetRotateYMatrix(m,-w);
		D3DMath_VectorMatrixMultiply(speed,speed,m);
	}

	if (dead!=0.0f)return;

	const float speedx=2.1f;
	const float speedz=4.9f;
	float sx=0.0f,sz=0.0f;

	if ((game->KeyDown(DIK_UP))||(game->KeyDown(DIK_W)))sz=speedz;
	if ((game->KeyDown(DIK_DOWN))||(game->KeyDown(DIK_S)))sz=-speedz;

	if ((game->KeyDown(DIK_LEFT))||(game->KeyDown(DIK_A)))sx=-speedx;
	if ((game->KeyDown(DIK_RIGHT))||(game->KeyDown(DIK_D)))sx=speedx;

	if (speed.y!=0.0f)
	{
		sx=speed.x;
		sz=speed.z;
	}else addspeed=D3DVECTOR(0,0,0);

	speed=D3DVECTOR(sx,speed.y,sz);
	if (game->KeyDown(DIK_SPACE))
	{	// Fliegen nur, wenn nicht im Stall und nicht aufgespießt
		if ((Flug.fliege==FALSE)&&(install==NULL)&&(Golfspieler==0)&&(bound==NULL))StartFlug();
	}

	if ((game->MouseButtonDown(0))&&(!install))
	{	// Ei werfen, wenn nicht im Stall
		UID id[2]={0,0};
		const int num=Flug.fliege?2:1;
		Throw(ax,num,&id[0]);
		if (id[0]!=0)SendNotify(11,FloatToLong(ax),num,&id[0],sizeof(id));
		if (produziere)
		{
			produziere=FALSE;
			produktion=0.0f;
			SendFlags();
		}
	}else if ((game->MouseButtonDown(1))&&(Flug.fliege==FALSE))
	{
		if (eier<chicken_maxeier)
		{
			if (!produziere)
			{
				produziere=TRUE;
				SendFlags();
			}
		}else 
		{
			if (produziere)
			{
				produziere=FALSE;
				produktion=0.0f;
				SendFlags();
			}
		}
		speed=D3DVECTOR(0,0,0);
	}else{
		if (produziere)
		{
			produziere=FALSE;
			produktion=0.0f;
			SendFlags();
		}
	}
}

const float CChickenPlayer::GetDeadTime()const
{
	return game->respawntime;
}

void CChickenPlayer::Revive()
{
	pos=world->Rand(1.5f);
	Flug.power=75.0f;
	Flug.fliege=FALSE;
	eier=10;
	ax=0.2f;
	if (IsLocal)
	{
		SendFlags();
		game->SetTransition(new CFadeTransition(FALSE,D3DRGB(1,0,0),NULL,0.6f,1));
	}
}

void CChickenPlayer::Die()
{
	CPlayer::Die();
	if (IsLocal)
	{
		CBillboardText* t=new CBillboardText();
		t->SetText(T_TOT,RGB(255,0,0));
		game->AddGimmick(t);

		game->SetTransition(new CFadeTransition(TRUE,D3DRGB(1,0,0),NULL,game->respawntime+0.2f,1));
	}
	produktion=0.0f;
	produziere=FALSE;
	if (IsLocal)SendFlags();
}

void CChickenPlayer::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	static CHAR c[20];

	CPlayer::DrawUI(lpDevice);
	if (!IsLocal)return;

	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,200);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	const float col=(install?0.25f:1.0f);
	const DWORD color=D3DRGB(col,col,col);

	for (int i=0;i<4;i++)
		FlugVertex[i].color=EierVertex[i].color=color;

	lpDevice->SetTexture(0,game->lpTexture[16]);
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,EierVertex,4,0);


	lpDevice->SetTexture(0,game->lpTexture[18]);
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,FlugVertex,4,0);

	if ((IsLocal)&&(Config.Crosshair)&&(!game->KeyDown(DIK_B)))
	{
		if (game->lpTexture[30]==NULL)
		{	// Lade dynamisch Crosshair nach
			game->lpTexture[30]=CreateTextureFromResource(lpDevice,game->lpDD,NULL,"crosshair1.bmp",0,FALSE,TRUE);
			MakeTransparent(game->lpTexture[30],FALSE);
		}

		// Crosshair malen
		lpDevice->SetTexture(0,game->lpTexture[30]);

		const float width=game->width*0.08f;
		const float height=(game->height*0.08f)*(4.0f/3.0f);
		D3DTLVERTEX ch[4];
		MakeBillboardVertices(&ch[0],int(game->width-width)/2,int((game->height-height)/2.0f),int(width),int(height),D3DRGB(Flug.fliege?1.0f:0,1.0f,0));

//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,ch,4,0);
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	}

	if (Flug.fliege)
	{
		// Texturen nachladen
		if (game->lpTexture[41]==NULL)
		{
			game->lpTexture[41]=CreateTextureFromResource(lpDevice,game->lpDD,NULL,"zeiger.bmp",2,FALSE,TRUE);
			MakeTransparent(game->lpTexture[41],FALSE);	// Zeiger
		}
		if (game->lpTexture[42]==NULL)
		{
			game->lpTexture[42]=CreateTextureFromResource(lpDevice,game->lpDD,NULL,"tacho1.bmp",0,FALSE,TRUE);
			MakeTransparent(game->lpTexture[42],FALSE);	// Tacho1
		}
		if (game->lpTexture[43]==NULL)
		{
			game->lpTexture[43]=CreateTextureFromResource(lpDevice,game->lpDD,NULL,"tacho2.bmp",0,FALSE,TRUE);
			MakeTransparent(game->lpTexture[43],FALSE);	// Tacho2
		}
	
/*		sprintf(&c[0],"Schub = %f",Flug.throttle);
		game->lpD3DFont->DrawText(10,350,D3DRGB(1,1,1),&c[0],0);

		sprintf(&c[0],"Speed = %f",Magnitude(speed));
		game->lpD3DFont->DrawText(10,370,D3DRGB(1,1,1),&c[0],0);

		sprintf(&c[0],"Höhe = %f",pos.y);
		game->lpD3DFont->DrawText(10,390,D3DRGB(1,1,1),&c[0],0);*/


		const float faktor=game->width/1024.0f;
		// Geschwindigkeitsmesser
		D3DTLVERTEX v[4];
		const float s1=230.0f*faktor;
		const float s2=180.0f*faktor;
		float alpha;

		const x1=int(120.0f*faktor);

		MakeBillboardVertices(v,x1,int(game->height-10-s1),int(s1),int(s1),D3DRGB(1,1,1));

		lpDevice->SetTexture(0,game->lpTexture[42]);
		lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

		alpha=Magnitude(speed)*0.05f*1.5f*g_PI+g_PI*0.5f;
		if (alpha>g_PI*2.0f)alpha=g_PI*2.0f;
		alpha-=20.0f/180.0f*g_PI;
		DrawZeiger(lpDevice,x1+s1*0.5f,game->height-10-s1*0.5f,s1*0.33f,alpha);

		// Schubmesser
		const x2=int(10.0f*faktor);
		const y2=int(game->height-s1-s2+35.0f*faktor);
		MakeBillboardVertices(v,x2,y2,int(s2),int(s2),D3DRGB(1,1,1));
		lpDevice->SetTexture(0,game->lpTexture[43]);
		lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

//		Flug.animationphase+=elapsed*(Flug.throttle*(Flug.power/150.0f+0.4f)/6.0f+0.3f)
		alpha=(Flug.throttle*(Flug.power/150.0f+0.4f)/6.0f+0.3f);
		alpha=alpha/4.5f*g_PI*1.5f+g_PI*0.5f;
		// min ist 0.3, max ist 4.7444 (4.5)
		if (alpha>g_PI*2.0f)alpha=g_PI*2.0f;
		DrawZeiger(lpDevice,x2+s2*0.5f,y2+s2*0.5f,s2*0.33f,alpha);

//		DrawFluglage(lpDevice,x2+s2+40.0f*faktor,(float)y2-30.0f*faktor,220.0f*faktor);
	}

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);

	if (produziere)
		sprintf(&c[0],"= %.1f",eier+produktion);
	else sprintf(&c[0],"= %d",eier);
//	RECT r={70,120,90,120+35};
	game->lpD3DFont->DrawText(70,game->GetFontCentered(120,120+35),produziere?D3DRGB(1,1.0f-0.4f*produktion,1.0f-produktion):D3DRGB(1,1,1),&c[0],0);

	sprintf(&c[0],"= %d",(int)floorf(hit));
	RECT r2={70,75,90-bh/2,90+bh/2};
	game->lpD3DFont->DrawText(70,game->GetFontCentered(75,90+bh/2),D3DRGB(1,1,1),&c[0],0);

/*	if (produziere)
	{
		const x=110;
		const y=120+7;
		const w=100;
		const h=20;
		SelectObject(dc,GetStockObject(NULL_BRUSH));
		SelectObject(dc,GetStockObject(WHITE_PEN));

		Rectangle(dc,x,y,x+w,y+h);

		SelectObject(dc,GetStockObject(NULL_PEN));
		SelectObject(dc,CreateSolidBrush(RGB(255,128,0)));

		Rectangle(dc,x+1,y+1,x+int(float(w+1)*produktion)-1,y+h);


		DeleteObject(SelectObject(dc,GetStockObject(NULL_BRUSH)));
	}*/

	RECT r3={70,160,90,160+35};
	sprintf(&c[0],"= %d %%",int(Flug.power));
	game->lpD3DFont->DrawText(70,game->GetFontCentered(160,160+35),D3DRGB(1,1,1),&c[0],0);
}

void CChickenPlayer::Notify(PNETWORKDATA data)
{
	switch(data->p1)
	{
	case 20:if (IsLocal)
		{
			CHAR c[100];
			sprintf(&c[0],T_CHICKENHASHIT,data->userdata);
			game->ticker->Add(&c[0],D3DRGB(0,1,0));
		}
		break;
	case 121:if (IsLocal)
		eier++;
		break;
	case 22:
		{
			CBauer* b=(CBauer*)chain->Find(data->p2);
			if (b==NULL)return;
			b->life=*(float*)data->userdata;
			hit+=LongToFloat(data->p3)*0.01f;
			if (b->Hit(LongToFloat(data->p3)))
			{	// Bauer getötet
				HasHit(b->id);
			}
		}
		break;

	default:CChicken::Notify(data);
	}
}

void CChickenPlayer::HandleFlug(const float elapsed,const int mx,const int my,const int mz)
{
	{	// Huhn bewegen / drehen
		// Drehgeschwindigkeit um die einzelnen Achsen
		const float key_y=0.9f;
		const float key_z=1.2f;
		const float key_x=1.0f;

		const float mf=60.0f;
		// X-Achse
		float pitch=float(Config.reverseflight?-my:my)/float(Config.sensitivity*10.0f)*mf;
		// Z-Achse
		float roll=-float(mx)/float(Config.sensitivity*10.0f)*mf;
		// Y-Achse
		float yaw=0.0f;

		if (game->KeyDown(DIK_A))
			yaw-=key_y;
		if (game->KeyDown(DIK_D))
			yaw+=key_y;

		if (game->KeyDown(DIK_LEFT))
			roll+=key_z;
		if (game->KeyDown(DIK_RIGHT))
			roll-=key_z;

		if (game->KeyDown(DIK_UP))
			pitch+=key_x;
		if (game->KeyDown(DIK_DOWN))
			pitch-=key_x;

		const float gf=1.9f;
		Flug.yaw+=yaw*elapsed*gf;
		Flug.pitch+=pitch*elapsed*gf;
		Flug.roll+=roll*elapsed*gf;

		// Geschwindigkeit auch drehen!?
/*		const float faktor=1.0f;
		D3DXQuaternionRotationYawPitchRoll(&qR,yaw*faktor,pitch*faktor,0);
		D3DXMatrixRotationQuaternion(&m,&qR);
		D3DMath_VectorMatrixMultiply(speed,speed,m);*/
	}

	if (dead!=0.0f)return;

	if ((game->MouseButtonDown(0))||(game->KeyDown(DIK_LCONTROL)))
	{
		UID id[2]={0,0};
		const int num=Flug.fliege?2:1;
		Throw(ax,num,&id[0]);
		if (id[0]!=0)SendNotify(11,FloatToLong(ax),num,&id[0],sizeof(id));
		if (produziere)
		{
			produziere=FALSE;
			produktion=0.0f;
			SendFlags();
		}
	}
	const float maxspeed=25.0f;
	const float minspeed=0.2f;
	if (game->KeyDown(DIK_W))
	{
		if (Flug.throttle<maxspeed)
		{
			Flug.throttle+=elapsed*2.0f;
			if (Flug.throttle>maxspeed)Flug.throttle=maxspeed;
		}
	}
	if (game->KeyDown(DIK_S))
	{
		if (Flug.throttle>minspeed)
		{
			Flug.throttle-=elapsed*4.0f;
			if (Flug.throttle<minspeed)Flug.throttle=minspeed;
		}
	}

	// Mausrad zur Schubregelung benutzen
	if ((mz>0)&&(Flug.throttle<maxspeed))
	{
		Flug.throttle+=1.0f;
		if (Flug.throttle>maxspeed)Flug.throttle=maxspeed;
	}
	if ((mz<0)&&(Flug.throttle>minspeed))
	{
		Flug.throttle-=1.0f;
		if (Flug.throttle<minspeed)Flug.throttle=minspeed;
	}
}

const BOOLEAN CChickenPlayer::CollidateFlug(const D3DVECTOR Normale)
{
	if (Flug.fliege==FALSE)return FALSE;
	EndeFlug();

	D3DMATRIX m=Flug.matrix;
	m.m[3][0]=m.m[3][1]=m.m[3][2]=0.0f;
	D3DVECTOR v;
	D3DMath_VectorMatrixMultiply(v,D3DVECTOR(0,0,Flug.throttle),m);

	const float a=abs(DotProduct(v,-Normalize(Normale)));

#ifdef _DEBUG
	CHAR c[100];
	sprintf(&c[0],"Flughuhn: Normaliöse Geschwindigkeit: %f",a);
	DebugOut(&c[0]);
#endif

	if (a>4.5f)
	{	// Zerschellen
		Flug.matrix.m[3][0]=pos.x;
		Flug.matrix.m[3][1]=pos.y+0.25f;
		Flug.matrix.m[3][2]=pos.z;

		if (!game->IsServer)SendNotify(1,SPLAT_NORMAL);
		Splat(SPLAT_NORMAL);	// Bei zu steilem Winkel sterben.
		return TRUE;
	}

	if (a>0.7f)
	{	// Abprallen
		abschlagpos=pos;

		Golfspieler=id;
		golfergebnis=TRUE;	// Kein "Golfergebnis"

		const D3DVECTOR vn=(DotProduct(Normalize(Normale),v))*Normalize(Normale);
		const D3DVECTOR vt=v-vn;

		const float faktor=0.9f;

		schlagspeed=vt-faktor*vn+D3DVECTOR(0,1,0);
		maxfallspeed=10000.0f;

		CSound* s=new CSound(game->lpBuffers[3],TRUE);
		s->Set3DParameters(pos);
		s->SetFrequency(20000+rand()%4000);
		s->Play();
		game->AddSound(s);
		if (IsLocal)SendFlags();
		return FALSE;
	}

	// Sicher gelandet -> Einfach hinstellen
	ax=0.2f;
	return FALSE;
}


void CChickenPlayer::DrawZeiger(LPDIRECT3DDEVICE7 lpDevice,const float mx,const float my,const float length,const float alpha)const
{
#define c D3DRGB(1,1,1)
	const float breitefaktor=1.0f/14.0f;
	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(+length*breitefaktor,length,0),0.9f,c,0,0,0),
		D3DTLVERTEX(D3DVECTOR(-length*breitefaktor,length,0),0.9f,c,0,1,0),
		D3DTLVERTEX(D3DVECTOR(+length*breitefaktor,-length*0.25f,0),0.9f,c,0,0,1),
		D3DTLVERTEX(D3DVECTOR(-length*breitefaktor,-length*0.25f,0),0.9f,c,0,1,1)
	};

#define vect(x) (*(D3DVECTOR*)(x.x))


	D3DMATRIX m,m2;
	D3DUtil_SetRotateZMatrix(m,alpha);
	D3DUtil_SetTranslateMatrix(m2,mx,my,0);
	D3DMath_MatrixMultiply(m,m,m2);
	for (int i=0;i<4;i++)
	{
		D3DVECTOR* vect=(D3DVECTOR*)(&v[i].sx);
		D3DMath_VectorMatrixMultiply(*vect,*vect,m);
	}

	lpDevice->SetTexture(0,game->lpTexture[41]);
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);
}

void CChickenPlayer::DrawFluglage(LPDIRECT3DDEVICE7 lpDevice,const float x,const float y,const float size)const
{
/*	lpDevice->SetTexture(0,game->lpTexture[44]);

	const float vabst=0.5f;

	D3DMATRIX rotmat=Flug.matrix,m;
	rotmat.m[3][0]=rotmat.m[3][1]=rotmat.m[3][2]=0.0f;
	D3DMath_MatrixInvert(m,rotmat);

	D3DVECTOR top=D3DVECTOR(0,1,0),front=D3DVECTOR(0,0,1);
	D3DMath_VectorMatrixMultiply(front,front,rotmat);
	D3DMath_VectorMatrixMultiply(top,top,rotmat);



	const float winkel1=atan2f(sqrtf(sqr(front.x)+sqr(front.z)),front.y)-g_PI/2.0f;

	float mu=0.5f;
	float mv=winkel1/(g_PI*2.0f)+0.5f;

//	const float alpha=asinf(top.y)+g_PI/2.0f;
	const float alpha=Flug.animationphase;

	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(x,y,0),0.9f,D3DRGB(1,1,1),0,-0.5f,-vabst),
		D3DTLVERTEX(D3DVECTOR(x+size,y,0),0.9f,D3DRGB(1,1,1),0,0.5f,-vabst),
		D3DTLVERTEX(D3DVECTOR(x,y+size,0),0.9f,D3DRGB(1,1,1),0,-0.5f,vabst),
		D3DTLVERTEX(D3DVECTOR(x+size,y+size,0),0.9f,D3DRGB(1,1,1),0,0.5f,vabst)
	};

	const float cos=cosf(alpha);
	const float sin=sinf(alpha);

	for (int i=0;i<4;i++)
	{
		float nu=v[i].tu*cos-(v[i].tv)*sin;
		float nv=v[i].tu*sin+(v[i].tv)*cos;

		v[i].tu=nu;
		v[i].tv=nv;
	}


	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	lpDevice->SetTexture(0,NULL);*/
}

void CChickenPlayer::Restore()
{
}