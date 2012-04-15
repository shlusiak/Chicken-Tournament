/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "effects.h"
#include "game.h"
#include "config.h"
#include "chicken.h"
#include "drescher.h"



const float splatterfaktor=1.35f;
const float maxtime=7.0f;
const float DrescherDuration=1.5f;	// Dauer des Bluts am Drescherausgang



CBlood::CBlood(const D3DVECTOR vpos)
:CParticleSystem(D3DVECTOR(0,0,0)),pos(vpos)
{
	maxtime=::maxtime;
	numparticel=int(90.0f*powf(game->gorelevel,0.7f));
}

BOOLEAN CBlood::Init()
{
	CParticleSystem::Init();
	for (int i=0;i<numparticel;i++)
	{
		const float s=speed();
		const float winkelh=randf()*2*g_PI;
		const float winkelv=angh();

		Particel[i].pos=pos;
		Particel[i].speed.x=cosf(winkelh)*s*cosf(winkelv);
		Particel[i].speed.z=sinf(winkelh)*s*cosf(winkelv);
		Particel[i].speed.y=sinf(winkelv)*s*1.175f;
		Particel[i].liegt=FALSE;
		Particel[i].size=size();
	}
	return TRUE;
}

void CBlood::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	lpDevice->SetTexture(0,game->lpTexture[47]);
	if (Config.alpha)
	{
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);

		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,0xDF);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	CParticleSystem::Draw(lpDevice);

	lpDevice->SetTexture(0,NULL);

	if (Config.alpha)
	{
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,   FALSE);
		lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);
	}
}

const float Luftwiderstand=0.65f;

void CBlood::Execute(float elapsed)
{
	CParticleSystem::Execute(elapsed);

	for (int i=0;i<numparticel;i++)if ((!Particel[i].liegt)&&(Particel[i].size!=0.0f))
	{
//		const float lw=powf(Luftwiderstand,elapsed);
		const float oh=Particel[i].pos.y;
		Particel[i].pos+=(Particel[i].speed*elapsed);
		if (!world->IsInside(Particel[i].pos))
		{
			if ((Particel[i].pos.x<world->minx())||(Particel[i].pos.x>world->maxx()))
			{
				Particel[i].speed.x*=(-0.3f);
			}
			if ((Particel[i].pos.z<world->minz())||(Particel[i].pos.z>world->maxz()))
			{
				Particel[i].speed.z*=(-0.3f);
			}
			Particel[i].speed.y*=(0.8f);

			world->MakeInside(Particel[i].pos);
		}
		const float h=world->HeightAtPoint(Particel[i].pos.x,Particel[i].pos.z);

		if (Particel[i].pos.y<h)
		{
			Particel[i].speed.y=-Particel[i].speed.y*0.12f;

			Particel[i].speed.x*=0.5f;
			Particel[i].speed.z*=0.5f;
			Particel[i].pos.y=h;
			if (h==oh)
			{
				Particel[i].speed.y=0.0f;
				Particel[i].liegt=TRUE;
			}
		}
			
		Particel[i].speed.y-=(elapsed*game->gravity);
//		Particel[i].speed.x*=lw;
//		Particel[i].speed.z*=lw;
	}else{
		if (time>maxtime/3.0f*2.0f)
		{	// Blut versickern lassen
			Particel[i].pos.y-=Particel[i].size*(elapsed/(maxtime/3.0f));
		}
	}
}

const float CBlood::size()const
{
	return (randf()*0.5f+0.5f)*0.11f;
}

const float CBlood::speed()const
{
	return 2.0f+randf()*4.0f*splatterfaktor*powf(game->gorelevel,0.05f);
}

const float CBlood::angh()const
{
	return randf()*g_PI/2.0f;
}

const DWORD CBlood::color()const
{
	return D3DRGB(0.7+randf()*0.3f,randf()*0.1f,randf()*0.1f);
}



CDrescherBlood::CDrescherBlood(UID vref,float vtimeoffset)
:CBlood(D3DVECTOR(0,0,0)),nextspawn(vtimeoffset)
{
	ref=vref;
	timeoffset=vtimeoffset;
}

BOOLEAN CDrescherBlood::Init()
{
	CBlood::Init();

	for (int i=0;i<numparticel;i++)
		Particel[i].size=0.0f;

	return TRUE;
}

void CDrescherBlood::Spawn()
{
	CDrescher* d=(CDrescher*)game->chain->Find(ref);
	if (d==NULL)
	{
		Remove();
		return;
	}

	for (int i=0;i<numparticel;i++)if (Particel[i].size==0.0f)
	{
		Particel[i].size=size();
		Particel[i].pos=d->GetOutput()+D3DVECTOR(randf()-0.5f,0,randf()-0.05f)*0.2f;
		Particel[i].liegt=FALSE;
		Particel[i].speed=D3DVECTOR(randf()-0.5f,-3.5f-randf()*1.0f,randf()-0.5f)*(0.8f-randf()*0.5f);

		if (i==numparticel-1)Particel[0].size=0.0f;
		else Particel[i+1].size=0.0f;
		return;
	}
}

void CDrescherBlood::Execute(float elapsed)
{
	CBlood::Execute(elapsed);

	if (time<timeoffset+DrescherDuration)
		nextspawn-=elapsed;
	while (nextspawn<0.0f)
	{
		Spawn();
		if ((rand()%2)==0)Spawn();
		nextspawn+=(0.03f+randf()*0.04f)/game->gorelevel;
	}
}





CFeder::CFeder(const D3DVECTOR vpos,const BOOLEAN spherical)
:pos(vpos),Federn(NULL),anzahl(0),startspherical(spherical),canremove(TRUE)
{
	id=MakeUID(IDEffect);
}

CFeder::~CFeder()
{
	if (Federn!=NULL)delete[] Federn;
}

BOOLEAN CFeder::Init()
{
	anzahl=int(20.0f+12.0f*game->gorelevel);

	if (Federn!=NULL)delete[] Federn;

	Federn=new TFeder[anzahl];
	if (Federn==NULL)return FALSE;

	for (int i=0;i<anzahl;i++)
	{
		Federn[i].active=TRUE;
		Federn[i].pos=pos;
		Federn[i].offy=randf()*g_PI*2;

		float s=2.5f+randf()*0.8f+(game->gorelevel*0.02f);
		const float winkelh=randf()*2*g_PI;
		const float winkelv=randf()*g_PI*(startspherical?2.0f:0.75f);

		Federn[i].speed.x=cosf(winkelh)*s*cosf(winkelv);
		Federn[i].speed.z=sinf(winkelh)*s*cosf(winkelv);
		Federn[i].speed.y=sinf(winkelv)*s*(startspherical?1:1.5f);

		float f;
		f=atan2f(Federn[i].speed.z,Federn[i].speed.x);
		Federn[i].roty=f;
		Federn[i].rotspeedy=((rand()%2==0)?1.0f:-1.0f)*(3.0f+randf()*6.5f);
	}

	D3DUtil_InitMaterial(mat,1,1,1);

	return TRUE;
}

void CFeder::Execute(float elapsed)
{
	const float Luftwiderstand=0.4f;
	const float lw=powf(Luftwiderstand,elapsed);
	const float maxyspeed=1.0f;
	BOOLEAN active=FALSE;

	for (int i=0;i<anzahl;i++)
	{
		if (Federn[i].active)
		{
			Federn[i].pos+=Federn[i].speed*elapsed;
			Federn[i].speed.y-=(game->gravity*elapsed*0.75f);

			Federn[i].speed.x*=lw;
			Federn[i].speed.z*=lw;
//			Federn[i].speed.y*=lw;
			if (Federn[i].speed.y<-maxyspeed)Federn[i].speed.y=-maxyspeed;

			Federn[i].offy+=Federn[i].rotspeedy*(elapsed);

			if (Federn[i].pos.y<
				world->HeightAtPoint(Federn[i].pos.x,Federn[i].pos.z))Federn[i].active=FALSE;

			active=TRUE;
		}
	}
	if ((!active)&&(canremove))Remove();
}

void CFeder::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
#define faktor (1.1f)
#define federwidth (0.135f*faktor)
#define federheight (0.20f*faktor)
	if (Config.alpha)
	{
//		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);

		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,0xDF);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);
	}

	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);

	static D3DVERTEX Vertices[4]=
	{
		D3DVERTEX(D3DVECTOR(federwidth/2.0f,federheight/2.0f,0),D3DVECTOR(0,0,1),1,0),
		D3DVERTEX(D3DVECTOR(-federwidth/2.0f,federheight/2.0f,0),D3DVECTOR(0,0,1),0,0),
		D3DVERTEX(D3DVECTOR(federwidth/2.0f,-federheight/2.0f,0),D3DVECTOR(0,0,1),1,1),
		D3DVERTEX(D3DVECTOR(-federwidth/2.0f,-federheight/2.0f,0),D3DVECTOR(0,0,1),0,1)
	};
	lpDevice->SetTexture(0,game->lpTexture[19]);
	lpDevice->SetMaterial(&mat);
	D3DMATRIX m2,m3;

	for (int i=0;i<anzahl;i++)if (Federn[i].active)
	{
		D3DUtil_SetTranslateMatrix(m3,Federn[i].pos);

		float f=sqrtf(sqr(Federn[i].speed.x)+sqr(Federn[i].speed.z));
		f=atan2f(Federn[i].speed.y,f);

		D3DUtil_SetRotateYMatrix(m2,-Federn[i].roty);
		D3DMath_MatrixMultiply(m3,m2,m3);

		D3DUtil_SetRotateZMatrix(m2,f+g_PI/2.0f);
		D3DMath_MatrixMultiply(m3,m2,m3);

		D3DUtil_SetRotateYMatrix(m2,Federn[i].offy);
		D3DMath_MatrixMultiply(m3,m2,m3);

		D3DUtil_SetRotateXMatrix(m2,0.4f);
		D3DMath_MatrixMultiply(m3,m2,m3);

		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m3);
		lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,Vertices,4,0);

/*		// 180 Grad drehen, dann erneut rendern
		D3DUtil_SetRotateYMatrix(m2,g_PI);
		D3DMath_MatrixMultiply(m3,m2,m3);

		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m3);
		lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,Vertices,4,0);*/
	}

	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
//	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,0);
	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);

	D3DMATRIX world;
	D3DUtil_SetIdentityMatrix(world);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&world);
	lpDevice->SetTexture(0,NULL);
}



CFederschweif::CFederschweif(UID vref,float vradius,float vspawns)
:CFeder(D3DVECTOR(0,0,0),TRUE),ref(vref),spawnradius(vradius),nextspawn(0)
{
	ref=vref;
	spawnradius=vradius;
	spawns=1.0f/(vspawns+game->gorelevel*1.2f);
	nextspawn=0;

	Federn=NULL;
	anzahl=0;
	canremove=FALSE;
}

BOOLEAN CFederschweif::Init()
{
	anzahl=150+(int)game->gorelevel*60;

	if (Federn!=NULL)delete[] Federn;

	Federn=new TFeder[anzahl];
	if (Federn==NULL)return FALSE;

	for (int i=0;i<anzahl;i++)
	{
		Federn[i].active=FALSE;
	}

	D3DUtil_InitMaterial(mat,1,1,1);

	Spawn();

	return TRUE;
}

void CFederschweif::Spawn()
{
	CChicken* c=(CChicken*)game->chain->Find(ref);
	if (c==NULL)
	{
		Remove();
		return;
	}
	if (c->Golfspieler==0)Remove();
 
	for (int i=0;i<anzahl;i++)if (Federn[i].active==FALSE)
	{
		Federn[i].active=TRUE;
		Federn[i].pos=c->pos+D3DVECTOR((randf()*2.0f-1.0f),(randf()*2.0f-1.0f),(randf()*2.0f-1.0f))*spawnradius;
		Federn[i].offy=randf()*g_PI*2;

		float s=2.5f+randf()*0.8f+(game->gorelevel*0.02f);
		s*=0.9f;
		const float winkelh=randf()*2.0f*g_PI;
		const float winkelv=randf()*g_PI*2.0f;

		Federn[i].speed.x=cosf(winkelh)*s*cosf(winkelv);
		Federn[i].speed.z=sinf(winkelh)*s*cosf(winkelv);
		Federn[i].speed.y=sinf(winkelv)*s*1.0f;
		Federn[i].speed+=c->speed*0.65f;

		const float f=atan2f(Federn[i].speed.z,Federn[i].speed.x);
		Federn[i].roty=f;
		Federn[i].rotspeedy=((rand()%2==0)?1.0f:-1.0f)*(4.5f+randf()*6.5f);

		if (i==anzahl-1)
			Federn[0].active=FALSE;
		else Federn[i+1].active=FALSE;
		return;
	}
}

void CFederschweif::Execute(float elapsed)
{
	CFeder::Execute(elapsed);

	nextspawn-=elapsed;
	while (nextspawn<0.0f)
	{
		Spawn();
		nextspawn+=spawns;
	}

}




CDrescherFedern::CDrescherFedern(UID vref,float vtimeoffset)
:CFeder(D3DVECTOR(0,0,0),TRUE),ref(vref),nextspawn(vtimeoffset),time(0),timeoffset(vtimeoffset)
{
	canremove=FALSE;
}

BOOLEAN CDrescherFedern::Init()
{
	anzahl=150+(int)game->gorelevel*60;

	if (Federn!=NULL)delete[] Federn;

	Federn=new TFeder[anzahl];
	if (Federn==NULL)return FALSE;

	for (int i=0;i<anzahl;i++)
	{
		Federn[i].active=FALSE;
	}

	D3DUtil_InitMaterial(mat,1,1,1);

	Spawn();

	return TRUE;
}

void CDrescherFedern::Spawn()
{
	CDrescher* d=(CDrescher*)game->chain->Find(ref);
	if (d==NULL)
	{
		Remove();
		return;
	}

	for (int i=0;i<anzahl;i++)if (Federn[i].active==FALSE)
	{
		Federn[i].active=TRUE;
		Federn[i].pos=d->GetOutput()+D3DVECTOR(randf()-0.5f,0,randf()-0.05f)*0.2f;
		Federn[i].offy=randf()*g_PI*2;
		
		Federn[i].speed=D3DVECTOR(randf()-0.5f,-8.0f+randf()*2.0f,randf()-0.5f)*0.6f;

		const float f=atan2f(Federn[i].speed.z,Federn[i].speed.x);
		Federn[i].roty=f;
		Federn[i].rotspeedy=((rand()%2==0)?1.0f:-1.0f)*(2.5f+randf()*3.5f);

		if (i==anzahl-1)
			Federn[0].active=FALSE;
		else Federn[i+1].active=FALSE;
		return;
	}
}

void CDrescherFedern::Execute(float elapsed)
{
	CFeder::Execute(elapsed);
	time+=elapsed;


	if (time<timeoffset+DrescherDuration)
		nextspawn-=elapsed;else canremove=TRUE;
	while (nextspawn<0.0f)
	{
		Spawn();
		if ((rand()%2)==0)Spawn();
		nextspawn+=(0.05f+randf()*0.04f)/game->gorelevel;
	}
}




CEiDotter::CEiDotter(D3DVECTOR vpos)
:CBlood(vpos)
{
	numparticel=int(20.0f*powf(game->gorelevel,0.3f));
}

const DWORD CEiDotter::color()const
{
	switch(rand()%2)
	{
	case 0:	// Gelb
		return D3DRGB(0.75f+randf()*0.1f,0.5f+randf()*0.1f,randf()*0.1f);
	case 1:	// Weiß
		return D3DRGB(0.9f+randf()*0.1f,0.9f+randf()*0.1f,0.9f+randf()*0.1f);
	}
	return D3DRGB(1,1,1);
}

const float CEiDotter::size()const
{
	return (randf()*0.5f+0.2f)*0.09f;
}

const float CEiDotter::speed()const
{
	return 1.4f+randf()*1.4f*splatterfaktor*powf(game->gorelevel,0.1f);
}

const float CEiDotter::angh()const
{
	return (randf()*1.2f-0.2f)*g_PI;
}


const float emissive=0.5f;

CNova::CNova(const D3DVECTOR p,const float sradius,const float escale,const float t)
:pos(p),endscale(escale),radius(sradius),aktscale(0),time(t),akttime(0)
{
	id=MakeUID(IDEffect);
	vertnum=int(64);
	while (vertnum%4!=0)vertnum++;

	ZeroMemory(&material1,sizeof(material1));
	material1.emissive=Color(0,emissive,0);

	VertO=new D3DVERTEX[vertnum+1];
	VertU=new D3DVERTEX[vertnum+1];
	VertR=new D3DVERTEX[vertnum+1];
	SetVertex(radius,0);
}

CNova::~CNova()
{
	delete[] VertO;
	delete[] VertU;
	delete[] VertR;
}

void CNova::SetVertex(const float r1,float r2)
{
	if (r2<0)r2=0;
#define ang (2*g_PI/(vertnum-2))
#define th ((i%2==0)?0.03f:0.0f)
	for (int i=0;i<vertnum;i++)
	{
		const float r=((i%2==0)?r1:r2);
		const float c=cosf(ang*i)*r;
		const float s=sinf(ang*i)*r;
		VertO[i].x=-s;
		VertO[i].y=th;
		VertO[i].z=c;
		VertO[i].nx=0.0f;
		VertO[i].ny=1.0f;
		VertO[i].nz=0.0f;
		VertO[i].tu=0.0f;
		VertO[i].tv=0.0f;

		VertU[i].x=s;
		VertU[i].y=-th;
		VertU[i].z=c;
		VertU[i].nx=0.0f;
		VertU[i].ny=-1.0f;
		VertU[i].nz=0.0f;
		VertU[i].tu=0.0f;
		VertU[i].tv=0.0f;

		if (i%2==0)
		{
			VertR[i].x=VertR[i+1].x=s;
			VertR[i].y=th;
			VertR[i+1].y=-th;
			VertR[i].z=VertR[i+1].z=c;
			VertR[i].nx=VertR[i+1].nx=0.0f;
			VertR[i].ny=VertR[i+1].ny=0.0f;
			VertR[i].nz=VertR[i+1].nz=1.0f;
			VertR[i].tu=VertR[i+1].tu=0.0f;
			VertR[i].tv=VertR[i+1].tv=0.0f;
		}
	}
}

void CNova::Execute(float elapsed)
{
	akttime+=elapsed;

	if (aktscale<=endscale)
	{
		aktscale+=(elapsed*(endscale/time))+((aktscale/5.0f)*elapsed);
		material1.emissive.g-=((elapsed/time)*emissive)/1.0f;
		if (material1.emissive.g<0)material1.emissive.g=0;
//		material1.ambient=material1.diffuse;
		SetVertex(radius*aktscale,radius*aktscale*0.75f);
	}
	if (akttime>time)
	{
		Remove();
		return;
	}
}

void CNova::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	// Ring
	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);

	    lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_DIFFUSE);
	    lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG2,         D3DTA_CURRENT);
	    lpDevice->SetTextureStageState(0, D3DTSS_ALPHAOP,           D3DTOP_SELECTARG1);
		lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR);
		lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
	}
	D3DMATRIX matrix;
	D3DUtil_SetTranslateMatrix(matrix,pos);
	world->Kippe(matrix,pos.x,pos.z);

	lpDevice->SetTexture(0,NULL);
	lpDevice->SetMaterial(&material1);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,VertO,vertnum,0);
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,VertU,vertnum,0);
	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,VertR,vertnum,0);

	if (Config.alpha)
	{
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	    lpDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1,         D3DTA_TEXTURE);
	}
}

