/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "world.h"
#include "game.h"
#include "player.h"
#include "config.h"
#include "bauer.h"


const float GrasFaktor=0.55f;



CWorld::CWorld()
:vertices(NULL),ground(NULL),wasservertices(NULL),rows(0),cols(0),time(0),lpGras(NULL),
 lpVertexBuffer(NULL),lpSkyBuffer(NULL),lpWallBuffer(NULL)
{ }

CWorld::~CWorld()
{
	SaveRelease(lpVertexBuffer);
	SaveRelease(lpSkyBuffer);
	SaveRelease(lpWallBuffer);

	if (vertices!=NULL)delete[] vertices;
	if (ground!=NULL)
		for (int i=0;i<rows-1;i++)delete[] ground[i].vertices;
	if (ground!=NULL)delete[] ground;

	if (wasservertices)delete[] wasservertices;

	if (lpGras)delete lpGras;
}

BOOLEAN CWorld::Init()
{
	id=MakeUID(IDWorld);
	const float w=float(Config.width),h=float(Config.height);
	CreateScene(int(w/7),int(h/7),w,h);
	CreateWalls();
	CreateWater();


	ZeroMemory(&mat,sizeof(mat));
	const float d=1.0f;
	const float s=0.0f;
	mat.diffuse=Color(d,d,d);
	mat.specular=Color(s,s,s);
	mat.ambient=Color(1,1,1);
	mat.power=0.0f;
	
	ZeroMemory(&wasser,sizeof(wasser));
	const float s2=1.0f;
	wasser.diffuse=Color(0.35f,0.4f,0.4f,1.0f);
	wasser.specular=Color(1,1,1,s2);
	wasser.ambient=wasser.diffuse;
	wasser.power=40.0f;

	lpGras=new CGras();
	lpGras->Init();
//	lpGras->InitGras(int(width*height*GrasFaktor));

	return TRUE;
}

void CWorld::CreateScene(int vrows,int vcols,float sizex,float sizey)
{
	width=sizex;
	height=sizey;
	const float tumax=sizex/3.0f;
	const float tvmax=sizey/3.0f;
	int i,j;
	if (vertices!=NULL)delete[] vertices;
	if (ground!=NULL)
		for (i=0;i<rows-1;i++)delete[] ground[i].vertices;
	if (ground!=NULL)delete[] ground;

	rows=vrows+1;
	cols=vcols+1;
	numvertices=(rows)*(cols);

	vertices=new D3DVERTEX[numvertices];
	for (i=0;i<numvertices;i++)vertices[i]=D3DVERTEX(D3DVECTOR(0,0,0),D3DVECTOR(0,0,0),0,0);

	ground=new TRow[rows-1];
	for (i=0;i<rows-1;i++)
	{
		ground[i].vertices=new D3DVERTEX[(cols-1)*6];
		ground[i].numvertices=(cols-1)*6;
	}

	const float textabw=0.12f;

	
	// Markante Punkte der Landschaft erstellen
	const int punktabstand=5;
	{	
		const float hoehe=8.5f;
		const float wasser=1.7f;

		for (i=0;i<rows;i+=punktabstand)for (j=0;j<cols;j+=punktabstand)
		{
			D3DVERTEX *v=&vertices[i*cols+j];
			
			v->y=-wasser+randf()*(hoehe+wasser);

			switch (rand()%8)
			{
			case 0:		// Ein See
				v->y=-wasser;
				break;
			case 1:		// Ein Berg
				v->y=randf()*(hoehe/2.0f)+(hoehe/2.0f);
				break;
			default:	// Ebenes Terrain erstellen
				v->y=randf()*(hoehe/3.0f)+0.01f;
				break;
			}
		}
	}


#define index(x,y) ((x*cols)+(y))

	for (i=0;i<rows;i++)for (j=0;j<cols;j++)
	{
		D3DVERTEX *v=&vertices[i*cols+j];
		v->x=(float(sizex)/float(rows-1))*float(i)-(float(sizex)/2.0f);
		if (v->y==0.0f)
		{
			// Berechne Indices der angrenzenden 4 Vertexes mit markanten Punkten
			const float faktx=float(i)/float(punktabstand),fakty=float(j)/float(punktabstand);
			const int x1=(int)floorf(faktx)*punktabstand;
			const int y1=(int)floorf(fakty)*punktabstand;
			int x2=(int)ceilf(faktx)*punktabstand;
			int y2=(int)ceilf(fakty)*punktabstand;

			if (x2>=rows)x2=rows-1;
			if (y2>=cols)y2=cols-1;

			float fr;
			
#define f1(x) (cosf(x*g_PI/2.0f)*cosf(x*g_PI/2.0f))
#define f2(x) (sinf(x*g_PI/2.0f)*sinf(x*g_PI/2.0f))

			// Berechne Wirkungsfaktor der angrenzenden markanten Punkte
			const float fx1=f1(modff(faktx,&fr));
			const float fx2=f2(modff(faktx,&fr));
			const float fy1=f1(modff(fakty,&fr));
			const float fy2=f2(modff(fakty,&fr));

#undef f1
#undef f2
			// Berechne die Ortogonalen Höhenwerte
			const float h1=vertices[index(x1,y1)].y;	// Links oben
			const float h2=vertices[index(x2,y1)].y;	// Rechts oben
			const float h3=vertices[index(x1,y2)].y;	// Links unten
			const float h4=vertices[index(x2,y2)].y;	// Rechts unten

			const float oh1=(fy1*h1+fy2*h3);	// links
			const float oh2=(fy1*h2+fy2*h4);	// rechts
			const float oh3=(fx1*h1+fx2*h2);	// oben
			const float oh4=(fx1*h3+fx2*h4);	// unten

			// Höhe des aktuellen Punktes
			v->y=(fx1*oh1+fx2*oh2+fy1*oh3+fy2*oh4)*0.5f;
		}

		v->z=(sizey/float(cols-1))*float(j)-(sizey*0.5f);
		v->nx=v->nz=v->ny=0;
		v->tu=(float(i)/rows)*tumax-textabw+randf()*textabw;
		v->tv=(float(j)/cols)*tvmax-textabw+randf()*textabw;
	}


	for (i=0;i<rows-1;i++)
	{
		int akt=i*cols;
		int z;
		D3DVECTOR n;

		for (j=0;j<ground[i].numvertices-1;j+=6)
		{
			// Triangle 1
			ground[i].vertices[j+0]=vertices[akt+cols];
			ground[i].vertices[j+1]=vertices[akt];
			ground[i].vertices[j+2]=vertices[akt+1];

			D3DVERTEX *v1=&ground[i].vertices[j+0],*v2=&ground[i].vertices[j+1],*v3=&ground[i].vertices[j+2];

			n=Normalize(CrossProduct(D3DVECTOR(v1->x-v2->x,v1->y-v2->y,v1->z-v2->z),
				D3DVECTOR(v2->x-v3->x,v2->y-v3->y,v2->z-v3->z)));
 
			for (z=0;z<=2;z++)
			{
				ground[i].vertices[j+z].nx=n.x;
				ground[i].vertices[j+z].ny=n.y;
				ground[i].vertices[j+z].nz=n.z;
			}

			// Triangle 2
			ground[i].vertices[j+3]=vertices[akt+cols+1];
			ground[i].vertices[j+4]=vertices[akt+cols];
			ground[i].vertices[j+5]=vertices[akt+1];

			v1=&ground[i].vertices[j+3];
			v2=&ground[i].vertices[j+4];
			v3=&ground[i].vertices[j+5];

			n=Normalize(CrossProduct(D3DVECTOR(v1->x-v2->x,v1->y-v2->y,v1->z-v2->z),
				D3DVECTOR(v2->x-v3->x,v2->y-v3->y,v2->z-v3->z)));
		
			for (z=3;z<=5;z++)
			{
				ground[i].vertices[j+z].nx=n.x;
				ground[i].vertices[j+z].ny=n.y;
				ground[i].vertices[j+z].nz=n.z;
			}

			akt++;
		}
	}
	MakeVertexBuffer();
}

#define WALL_FVF (D3DFVF_NORMAL|D3DFVF_XYZ|D3DFVF_TEX1|D3DFVF_TEXCOORDSIZE2(0))


void CWorld::MakeWallVertices(WALLVF* v,const float miny,const float maxy,const float width,const float height,const float maxth,const float ratio,const BOOLEAN flipu)const
{
	const float maxtv1=(height/(maxy-miny))*maxth;
	const float maxtu1=(width/(maxy-miny)/ratio)*maxth*(flipu?-1.0f:1.0f);

	const D3DVECTOR n1=D3DVECTOR(1,0,-1);
	const D3DVECTOR n2=D3DVECTOR(-1,0,-1);
	const D3DVECTOR n3=D3DVECTOR(-1,0,-1);
	const D3DVECTOR n4=D3DVECTOR(1,0,1);


	// Vorne Links
	v[0]=WALLVERTEX(D3DVECTOR(-width,maxy,height),n1,maxtu1*0,0);
	v[1]=WALLVERTEX(D3DVECTOR(-width,miny,height),n1,maxtu1*0,maxth);

	// Hinten Links
	v[2]=WALLVERTEX(D3DVECTOR(-width,maxy,-height),n4,maxtv1*1,0);
	v[3]=WALLVERTEX(D3DVECTOR(-width,miny,-height),n4,maxtv1*1,maxth);

	// Hinten Rechts
	v[4]=WALLVERTEX(D3DVECTOR(width,maxy,-height),n3,maxtv1+maxtu1,0);
	v[5]=WALLVERTEX(D3DVECTOR(width,miny,-height),n3,maxtv1+maxtu1,maxth);

	// Vorne Rechts
	v[6]=WALLVERTEX(D3DVECTOR(width,maxy,height),n2,maxtu1,0);
	v[7]=WALLVERTEX(D3DVECTOR(width,miny,height),n2,maxtu1,maxth);

	// Vorne Links
	v[8]=WALLVERTEX(D3DVECTOR(-width,maxy,height),n1,maxtu1*0,0);
	v[9]=WALLVERTEX(D3DVECTOR(-width,miny,height),n1,maxtu1*0,maxth);
}


void CWorld::CreateWalls()
{
	SaveRelease(lpWallBuffer);

	const float h1=12.5f;
	const float h2=15.0f;
	const float offset=1.5f;
	const float min=-0.0f;

	lpWallBuffer=CreateVertexBuffer(game->lpD3D,&game->d3ddesc,WALL_FVF,30,TRUE);

	WALLVF* v=NULL;
	lpWallBuffer->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,(LPVOID*)&v,NULL);

	// Innere Wand
	MakeWallVertices(&v[0],min,h1,(width-0.3f)/2.0f,(height-0.3f)/2.0f,1.0f,1.5f);

	// Äussere Wand
	MakeWallVertices(&v[10],min,h2,(width+offset)/2.0f,(height+offset)/2.0f,1.0f,1.9f,TRUE);

	// Unten
	MakeWallVertices(&v[20],-10,0,(width-0.3f)/2.0f,(height-0.3f)/2.0f,3.0f,1.0f);

	lpWallBuffer->Unlock();
	lpWallBuffer->Optimize(game->lpDevice,0);
}

void CWorld::CreateWater()
{
	if (wasservertices)delete[] wasservertices;
	const float x1=-(width-0.3f)/2.0f;
	const float x2=(width-0.3f)/2.0f;
	const float y1=-(height-0.3f)/2.0f;
	const float y2=(height-0.3f)/2.0f;
	const float tu=width/3.0f;
	const float tv=height/3.0f;
	const D3DVECTOR n=D3DVECTOR(0,-1,0);

	wassernumvertices=4;
	wasservertices=new D3DVERTEX[wassernumvertices];

	wasservertices[0]=D3DVERTEX(D3DVECTOR(x1,0,y1),n,0,0);
	wasservertices[1]=D3DVERTEX(D3DVECTOR(x1,0,y2),n,0,tv);
	wasservertices[2]=D3DVERTEX(D3DVECTOR(x2,0,y1),n,tu,0);
	wasservertices[3]=D3DVERTEX(D3DVECTOR(x2,0,y2),n,tu,tv);
}

void CWorld::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	D3DMATRIX m;
	D3DUtil_SetIdentityMatrix(m);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);
	lpDevice->SetMaterial(&mat);

	{	// Ground
		lpDevice->SetTexture(0,game->lpTexture[0]);

		if (lpVertexBuffer!=NULL)lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLELIST,lpVertexBuffer,0,ground[0].numvertices*(rows-1),0);
		else 
		{
			// Welt zu gross, dann ohne Vertexbuffer rendern
			for (int i=0;i<rows-1;i++)
				lpDevice->DrawPrimitive(D3DPT_TRIANGLELIST,D3DFVF_VERTEX,ground[i].vertices,ground[i].numvertices,0);
		}
	}
	{	// Walls
		if (Config.alpha)
		{
			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,TRUE);
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAREF,192);
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHAFUNC,D3DCMP_GREATEREQUAL);

			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_MODULATE);
		}
		lpDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);

		lpDevice->SetTexture(0,game->lpTexture[1]);

		lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP,lpWallBuffer,0,10,0);
		lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP,lpWallBuffer,10,10,0);

		lpDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);
		lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,0);

		lpDevice->SetTexture(0,game->lpTexture[38]);
		lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP,lpWallBuffer,20,10,0);

		if (Config.alpha)lpDevice->SetRenderState(D3DRENDERSTATE_ALPHATESTENABLE,FALSE);

	}

	lpDevice->SetTexture(0,NULL);
}

void CWorld::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	{	// Wasser
		D3DMATRIX m;
		D3DUtil_SetIdentityMatrix(m);
		lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);
		if (Config.alpha)
		{
			lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);

			lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCCOLOR);
			lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_ONE);
		}

		{	// Dann Wasser addieren
			lpDevice->SetMaterial(&wasser);
			
			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
			lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

			lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_ADD);
			lpDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);

			lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_NONE);

			lpDevice->SetTexture(0,game->lpTexture[2]);
			lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,wasservertices,wassernumvertices,0);

			lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
		}
		
		if (Config.alpha)lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
		lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);
		lpDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
		lpDevice->SetTextureStageState(0,D3DTSS_COLORARG2,D3DTA_CURRENT);
		lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_TEXTURE);

		lpDevice->SetTextureStageState(1,D3DTSS_ALPHAOP,D3DTOP_DISABLE);

		lpDevice->SetTextureStageState(1,D3DTSS_COLOROP,D3DTOP_DISABLE);

		lpDevice->SetTexture(0,NULL);
	}
	if (Config.Gras)
	{
		if (!lpGras->IsCreated())	// Dynamisch Gras erstellen, falls gebraucht
			lpGras->InitGras(int(width*height*GrasFaktor));

		lpGras->DrawUI(lpDevice);
	}
}

void CWorld::DrawSky(const LPDIRECT3DDEVICE7 lpDevice,const D3DVECTOR PlayerPos)
{
	D3DMATRIX m,m2;
	D3DUtil_SetTranslateMatrix(m,PlayerPos);
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	lpDevice->SetMaterial(&mat);

	lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
	lpDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_CLAMP);
	lpDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_CLAMP);


//	Die 4 Seiten
	D3DUtil_SetRotateYMatrix(m2,g_PI_DIV_2);
	for (int i=0;i<4;i++)
	{
		lpDevice->SetTexture(0,game->lpTexture[9+i]);
		lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP,lpSkyBuffer,0,4,0);

		lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);
	}

//	Deckel
	lpDevice->SetTexture(0,game->lpTexture[13]);
	D3DUtil_SetRotateYMatrix(m2,-g_PI_DIV_2);
	lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);
	D3DUtil_SetRotateXMatrix(m2,-g_PI_DIV_2);
	lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);

	lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP,lpSkyBuffer,0,4,0);

//	Boden
	lpDevice->SetTexture(0,game->lpTexture[14]);
	D3DUtil_SetRotateXMatrix(m2,g_PI);
	D3DUtil_SetRotateYMatrix(m2,g_PI_DIV_2);
	lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);
	lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);
	lpDevice->DrawPrimitiveVB(D3DPT_TRIANGLESTRIP,lpSkyBuffer,0,4,0);

		
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZWRITEENABLE,TRUE);
	lpDevice->SetTextureStageState(0,D3DTSS_ADDRESSU,D3DTADDRESS_WRAP);
	lpDevice->SetTextureStageState(0,D3DTSS_ADDRESSV,D3DTADDRESS_WRAP);

	lpDevice->SetTexture(0,NULL);
}

void CWorld::DrawUnderwater(const LPDIRECT3DDEVICE7 lpDevice)
{
	if (!Config.alpha)return;

	lpDevice->SetTexture(0,NULL);
	const D3DCOLOR c=D3DRGBA(0,0.3,0.9f,0.6f);
	const float rhw=0.3f;
	const float w=float(game->width);
	const float h=float(game->height);
	D3DTLVERTEX v[4]=
	{
		D3DTLVERTEX(D3DVECTOR(0,0,0),rhw,c,0,0,0),
		D3DTLVERTEX(D3DVECTOR(w,0,0),rhw,c,0,1.0f,0),
		D3DTLVERTEX(D3DVECTOR(0,h,0),rhw,c,0,0,1.0f),
		D3DTLVERTEX(D3DVECTOR(w,h,0),rhw,c,0,1.0f,1.0f)
	};


	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_SRCBLEND,D3DBLEND_SRCALPHA);
	lpDevice->SetRenderState(D3DRENDERSTATE_DESTBLEND,D3DBLEND_INVSRCALPHA);
	lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);
	lpDevice->SetTextureStageState(0,D3DTSS_ALPHAOP,D3DTOP_SELECTARG1);
	lpDevice->SetTextureStageState(0,D3DTSS_COLORARG1,D3DTA_TEXTURE);
	lpDevice->SetTextureStageState(0,D3DTSS_COLOROP,D3DTOP_MODULATE);

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_TLVERTEX,v,4,0);

	lpDevice->SetRenderState(D3DRENDERSTATE_ALPHABLENDENABLE,FALSE);
	lpDevice->SetRenderState(D3DRENDERSTATE_ZENABLE,TRUE);
	lpDevice->SetRenderState(D3DRENDERSTATE_LIGHTING,TRUE);


}

inline void CWorld::AnimateWater(const float elapsed)
{
	const float speedx=cosf(time/12.0f)*0.130f;
	const float speedy=sinf(time/17.0f)*0.125f;
	for (int i=0;i<wassernumvertices;i++)
	{
		wasservertices[i].tu+=elapsed*speedx;
		wasservertices[i].tv+=elapsed*speedy;
	}
}

void CWorld::Execute(float elapsed)
{
	time+=elapsed;
	AnimateWater(elapsed);
}

const float CWorld::HeightAtPoint(const float x,const float y,D3DVECTOR* normale)const
{
	float a,b,c,d;

	if ((abs(x)>=width/2.0f-0.05f)||(abs(y)>=height/2.0f-0.05f))
	{
		return 0;
	}

	int n=int(floorf((y+height*0.5f)/height*(cols-1)))*6;
	int m=int(floorf((x+width*0.5f)/width*(rows-1)));

	D3DVERTEX *v1,*v2,*v3;

	v1=&ground[m].vertices[n];
	v2=&ground[m].vertices[n+1];
	v3=&ground[m].vertices[n+2];

	const D3DVECTOR l1 = Normalize(D3DVECTOR(v1->x - x, 0, v1->z - y));
	const D3DVECTOR l2 = Normalize(D3DVECTOR(v2->x - x, 0, v2->z - y));
	const D3DVECTOR l3 = Normalize(D3DVECTOR(v3->x - x, 0, v3->z - y));
	a = acosf(DotProduct(l1, l2));
	b = acosf(DotProduct(l2, l3));
	c = acosf(DotProduct(l1, l3));
	if (abs(a + b + c - g_2_PI) >= 0.01f) 
	{
		n+=3;
	}
	
	v1=&ground[m].vertices[n];
	v2=&ground[m].vertices[n+1];
	v3=&ground[m].vertices[n+2];

	const D3DVECTOR norm=CrossProduct(
		D3DVECTOR(v2->x-v1->x,v2->y-v1->y,v2->z-v1->z),
		D3DVECTOR(v3->x-v1->x,v3->y-v1->y,v3->z-v1->z));
	a=norm.x;
	b=norm.y;
	c=norm.z;
	d=-(a*v1->x+
		b*v1->y+
		c*v1->z);
	if (normale!=NULL)*normale=norm;
	return (a*x+c*y+d) / (-b) ;
}

void CWorld::SendData(DPID to)
{
	// Generelle Daten
	Send(to,NM_WORLDMSG,0,1,FloatToLong(width),FloatToLong(height),&id,sizeof(id),TRUE);

	// Welt-Vertices
	Send(to,NM_WORLDMSG,0,2,cols,rows,vertices,sizeof(D3DVERTEX)*numvertices,TRUE);
}

void CWorld::ReceiveData(PNETWORKDATA data)
{
	if (data->p1==1)
	{	// Generelle Daten empfangen (width, height)
		width=LongToFloat(data->p2);
		height=LongToFloat(data->p3);
		id=*(UID*)(data->userdata);

		CreateWalls();
		CreateWater();
	}
	if (data->p1==2)
	{	// Welt-Daten empfangen (Reihen, Spalten, Vertices)
		int i,j;
		if (vertices!=NULL)delete[] vertices;
		if (ground!=NULL)
			for (i=0;i<rows-1;i++)delete[] ground[i].vertices;
		if (ground!=NULL)delete[] ground;

		cols=data->p2;
		rows=data->p3;
		numvertices=(rows)*(cols);

		vertices=new D3DVERTEX[numvertices];
		for (i=0;i<numvertices;i++)vertices[i]=((D3DVERTEX*)data->userdata)[i];

		ground=new TRow[rows-1];
		for (i=0;i<rows-1;i++)
		{
			ground[i].vertices=new D3DVERTEX[(cols-1)*6];
			ground[i].numvertices=(cols-1)*6;
		}	

		for (i=0;i<rows-1;i++)
		{
			int akt=i*cols;
			int z;
			D3DVECTOR n;

			for (j=0;j<ground[i].numvertices-1;j+=6)
			{
				// Triangle 1
				ground[i].vertices[j+0]=vertices[akt+cols];
				ground[i].vertices[j+1]=vertices[akt];
				ground[i].vertices[j+2]=vertices[akt+1];
	
				D3DVERTEX *v1=&ground[i].vertices[j+0],*v2=&ground[i].vertices[j+1],*v3=&ground[i].vertices[j+2];

				n=Normalize(CrossProduct(D3DVECTOR(v1->x-v2->x,v1->y-v2->y,v1->z-v2->z),
					D3DVECTOR(v2->x-v3->x,v2->y-v3->y,v2->z-v3->z)));
 
				for (z=0;z<=2;z++)
				{
					ground[i].vertices[j+z].nx=n.x;
					ground[i].vertices[j+z].ny=n.y;
					ground[i].vertices[j+z].nz=n.z;
				}

				// Triangle 2
				ground[i].vertices[j+3]=vertices[akt+cols+1];
				ground[i].vertices[j+4]=vertices[akt+cols];
				ground[i].vertices[j+5]=vertices[akt+1];
	
				v1=&ground[i].vertices[j+3];
				v2=&ground[i].vertices[j+4];
				v3=&ground[i].vertices[j+5];

				n=Normalize(CrossProduct(D3DVECTOR(v1->x-v2->x,v1->y-v2->y,v1->z-v2->z),
					D3DVECTOR(v2->x-v3->x,v2->y-v3->y,v2->z-v3->z)));
		
				for (z=3;z<=5;z++)
				{
					ground[i].vertices[j+z].nx=n.x;
					ground[i].vertices[j+z].ny=n.y;
					ground[i].vertices[j+z].nz=n.z;
				}
				akt++;
			}
		}
		MakeVertexBuffer();
		if (lpGras)lpGras->Invalidate();
			//InitGras(int(width*height*GrasFaktor));
	}
}

void CWorld::MakeVertexBuffer()
{
	// Welt erstellen
	if (lpVertexBuffer!=NULL)lpVertexBuffer->Release();
	lpVertexBuffer=CreateVertexBuffer(game->lpD3D,&game->d3ddesc,D3DFVF_VERTEX,ground[0].numvertices*(rows-1),TRUE);

	if (lpVertexBuffer!=NULL)
	{
		LPBYTE data;
		DWORD size=0;

		lpVertexBuffer->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,(LPVOID*)&data,&size);

		for (int i=0;i<rows-1;i++)
		{
			size=sizeof(ground[i].vertices[0])*ground[i].numvertices;
			CopyMemory(data,ground[i].vertices,size);
			data+=(size);
		}

		lpVertexBuffer->Unlock();
		lpVertexBuffer->Optimize(game->lpDevice,0);
	}


//	Himmel erstellen

	SaveRelease(lpSkyBuffer);
	lpSkyBuffer=CreateVertexBuffer(game->lpD3D,&game->d3ddesc,D3DFVF_LVERTEX,4,TRUE);
	if (lpSkyBuffer!=NULL)
	{
		const float size=50.0f;	// Dummy: Größe der SkyBox in Metern

		const D3DLVERTEX quad[4]=
		{
			D3DLVERTEX(D3DVECTOR(-size,-size,+size),D3DRGB(1,1,1),0,0,1),
			D3DLVERTEX(D3DVECTOR(-size,+size,+size),D3DRGB(1,1,1),0,0,0),
			D3DLVERTEX(D3DVECTOR(+size,-size,+size),D3DRGB(1,1,1),0,1,1),
			D3DLVERTEX(D3DVECTOR(+size,+size,+size),D3DRGB(1,1,1),0,1,0)
		};

		LPVOID data;
		lpSkyBuffer->Lock(DDLOCK_WAIT|DDLOCK_WRITEONLY|DDLOCK_DISCARDCONTENTS,(LPVOID*)&data,NULL);

		CopyMemory(data,quad,sizeof(quad));

		lpSkyBuffer->Unlock();
		lpSkyBuffer->Optimize(game->lpDevice,0);
	}

	CreateWalls();
}

void CWorld::Restore()
{
	MakeVertexBuffer();
	if (lpGras)lpGras->Invalidate();
		//lpGras->InitGras(int(width*height*GrasFaktor));
}

void CWorld::Invalidate()
{
	SaveRelease(lpVertexBuffer);
	SaveRelease(lpSkyBuffer);
	SaveRelease(lpWallBuffer);
	if (lpGras)lpGras->Invalidate();
}

void CWorld::NewWorld()
{
	if (lpGras)lpGras->Invalidate();
		//lpGras->InitGras(int(width*height*GrasFaktor));
}

#ifdef _DEBUG
int CWorld::NumFaces()
{
	int num=0;
	if ((Config.Gras)&&(lpGras))num+=lpGras->NumFaces();

	//i+=numvertices/2;
	for (int i=0;i<rows-1;i++)
		num+=ground[i].numvertices/3;

	num+=12;	// sky

	num+=24;	// wand

	num+=2;		// wasser

	return num;
}
#endif