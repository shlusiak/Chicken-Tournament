/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "stall.h"
#include "game.h"
#include "config.h"






const float breite=4.5f;
const float hoehe=3.825f;
const float einganghoehe=0.8f;
const float winkel=randf()*g_PI*2.0f;



CStall::CStall()
:pos(D3DVECTOR(0,0,0)),ang(0),lpMesh(NULL),lpFlag1(NULL),lpFlag2(NULL),time(0)
{
	box=new CBoundingTube(D3DVECTOR(0,0,0),(breite/2.0f)*sqrtf(2),hoehe-einganghoehe);
	eingang=new CBoundingTube(D3DVECTOR(0,0,0),(breite/2.0f)*sqrtf(2),einganghoehe);
	id=MakeUID(IDStall);
}

CStall::~CStall()
{
	SaveRelease(lpFlag1);
	SaveRelease(lpFlag2);
	if (box)delete box;
	if (eingang)delete eingang;
}

BOOLEAN CStall::Init()
{
	lpMesh=game->models->FindObject("Stall");
	if (lpMesh==NULL)return FALSE;

	pos=world->Rand(breite*sqrtf(2.0f)+1.0f);
	ang=randf()*g_PI*2.0f;

	time=randf()*5.0f;

	FitWorld();
	CreateFlag();

	D3DUtil_InitMaterial(material,1,1,1);
	material.ambient=Color(0.25f,0.25f,0.25f);
	material.specular=Color(0.5f,0.5f,0.5f);
	material.power=20.0f;

	D3DUtil_InitMaterial(bild,1,1,1);
	bild.ambient=bild.diffuse;//Color(0.7f,0.7f,0.7f);
	bild.specular=Color(0.4f,0.4f,0.4f);
	bild.power=30.0f;

	WORD i;
	for (i=0;i<4;i++)
	{
nochmal:
		bilder[i].ratio=0.75f;
		switch(rand()%17)
		{
		case 0:
			bilder[i].was=21;	// Me 2003
			break;
		case 1:
			bilder[i].was=22;	// Koala
			break;
		case 2:
			bilder[i].was=23;	// Farmer and Plasmacannon
			bilder[i].ratio=1.0f;
			break;
		case 3:
			bilder[i].was=24;	// Yellow Tree
			break;
		case 4:
			bilder[i].was=25;	// Baum Alina
			bilder[i].ratio=0.9f;
			break;
		case 5:
			bilder[i].was=26;	// Felsen
			bilder[i].ratio=0.8f;
			break;
		case 6:
			bilder[i].was=27;	// Owl Alina
			bilder[i].ratio=1.0f;
			break;
		case 7:
			bilder[i].was=28;	// Melbourne Towers
			bilder[i].ratio=0.8f;
			break;
		case 8:
			bilder[i].was=32;	// Empire State Building
			break;
		case 9:
			bilder[i].was=33;	// BR2
			bilder[i].ratio=1.3f;
			break;
		case 10:				// Me 2008
		case 15:
			bilder[i].was=34;
			bilder[i].ratio=0.8f;
			break;
		case 11:				// Harvester 1
			bilder[i].was=39;
			bilder[i].ratio=1.0f;
			break;
		case 12:
			bilder[i].was=44;	// Harvester 2
			bilder[i].ratio=0.7f;
			break;
		case 13:
			bilder[i].was=45;	// Flower
			bilder[i].ratio=0.59f;
			break;
		case 14:
			bilder[i].was=46;	// Totoro
			bilder[i].ratio=0.9f;
			break;

		default:
			if (rand()%2==0)goto nochmal;	// Nur im Notfall kein Bild nehmen.
			bilder[i].was=0;	// Keins
			break;
		}
		for (int j=0;j<i;j++)if (bilder[j].was==bilder[i].was)goto nochmal;
		if (bilder[i].was!=0)if (game->lpTexture[bilder[i].was]==NULL)	// wenn Bilder nicht existieren nochmal oder keins
		{
			if (LoadPicture(bilder[i].was)==FALSE)
			{
				if (rand()%4!=0)goto nochmal;
				bilder[i].was=0;	// keins
			}
		}
	}

#ifdef _DEBUG
	CHAR c[100];
	sprintf(&c[0],"Bilder in Stall: %d, %d, %d, %d",bilder[0].was,bilder[1].was,bilder[2].was,bilder[3].was);
	DebugOut(&c[0]);
#endif

	WORD ix;
	for( i=0, ix=0; ix<FLAG_SIZE; ix++ )
    {
        for( WORD iy=0; iy<FLAG_SIZE; iy++ )
        {
            m_pFlagIndices[i++] = (ix+0) + (iy+1)*(FLAG_SIZE+1);
            m_pFlagIndices[i++] = (ix+1) + (iy+0)*(FLAG_SIZE+1);
            m_pFlagIndices[i++] = (ix+0) + (iy+0)*(FLAG_SIZE+1);
            m_pFlagIndices[i++] = (ix+0) + (iy+1)*(FLAG_SIZE+1);
            m_pFlagIndices[i++] = (ix+1) + (iy+1)*(FLAG_SIZE+1);
            m_pFlagIndices[i++] = (ix+1) + (iy+0)*(FLAG_SIZE+1);
        }
    }

    for( int r=0; r<8; r++ )
    {
        FLOAT theta = (r/8.0f)*2*3.1415926283f;
        FLOAT x     = (FLOAT)cos(theta)*0.1f;
        FLOAT z     = -(FLOAT)sin(theta)*0.1f;

        D3DVECTOR vNorm = Normalize( D3DVECTOR( x, 0.0f, z ) );

        m_pPoleVertices[2*r+0] = D3DVERTEX( D3DVECTOR( x, 10.0f, z ),
                                            vNorm, r/8.0f, 0.0f );
        m_pPoleVertices[2*r+1] = D3DVERTEX( D3DVECTOR( x, 0.0f, z ),
                                            vNorm, r/8.0f, 1.0f );
    }


	return TRUE;
}

void CStall::Execute(float elapsed)
{
	if (!Config.Animations)return;
	time+=elapsed;

	DWORD size;
    D3DVERTEX* pFlagVertices1,*pFlagVertices2;
    if ((lpFlag1->Lock( DDLOCK_WAIT, (VOID**)&pFlagVertices1,&size))!=D3D_OK)return;
    if ((lpFlag2->Lock( DDLOCK_WAIT, (VOID**)&pFlagVertices2,&size))!=D3D_OK)
	{
		lpFlag1->Unlock();
		return;
	}
	const float invfs=1.0f/(FLAG_SIZE+1);
	{
        for( int ix=0; ix<FLAG_SIZE+1; ix++ )
        {
            for( int iy=0; iy<FLAG_SIZE+1; iy++ )
            {
				D3DVERTEX *v=&pFlagVertices1[ix+iy*(FLAG_SIZE+1)];
                const float z = ix*0.07f*sinf(ix-time*5.5f )*invfs;
                v->z = z;
				D3DVECTOR n=D3DVECTOR(-cosf(ix-time*5.5f)*ix*0.7f*invfs,0,-1);
				n/=sqrtf(sqr(n.x)+1);
				v->nx=-n.x;
				v->nz=n.z;

				pFlagVertices2[ix+iy*(FLAG_SIZE+1)].z=z;
				pFlagVertices2[ix+iy*(FLAG_SIZE+1)].nx=n.x;
				pFlagVertices2[ix+iy*(FLAG_SIZE+1)].nz=-n.z;
            }
        }
	}
    lpFlag2->Unlock();
    lpFlag1->Unlock();
}

void CStall::DrawPicture(LPDIRECT3DDEVICE7 lpDevice,const int wo,D3DMATRIX matrix,const float w)const
{
	const float h=1.0f;
	static const D3DVECTOR n(0,0,1);
	D3DVERTEX picturevertex[4]=
	{
		D3DVERTEX(D3DVECTOR(w*0.5f,h*0.5f,-2.20f),n,0,0),
		D3DVERTEX(D3DVECTOR(-w*0.5f,h*0.5f,-2.20f),n,1,0),
		D3DVERTEX(D3DVECTOR(w*0.5f,-h*0.5f,-2.20f),n,0,1),
		D3DVERTEX(D3DVECTOR(-w*0.5f,-h*0.5f,-2.20f),n,1,1)
	};

	D3DMATRIX m;
	D3DUtil_SetTranslateMatrix(m,0,1.75f,0);
	D3DMath_MatrixMultiply(matrix,matrix,m);
	D3DUtil_SetRotateYMatrix(m,float(wo)*g_PI*0.5f);
	D3DMath_MatrixMultiply(matrix,m,matrix);

	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&matrix);

	lpDevice->DrawPrimitive(D3DPT_TRIANGLESTRIP,D3DFVF_VERTEX,picturevertex,4,0);
}

void CStall::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	if ((!box->IsVisible())&&(!eingang->IsVisible()))return;

	D3DMATRIX m,m2;
	int i;

	D3DUtil_SetTranslateMatrix(m,pos);
	D3DUtil_SetRotateYMatrix(m2,ang);
	D3DMath_MatrixMultiply(m,m2,m);

	const D3DVECTOR cam=game->Camera.GetCameraPos();
	if ((cam.y<pos.y+einganghoehe)||(eingang->IsInside(cam))||(box->IsInside(cam)))
	{
		// Bilder malen
		lpDevice->SetMaterial(&bild);
		for (i=0;i<4;i++)if (bilder[i].was!=0)
		{
			if (game->lpTexture[bilder[i].was]==NULL)
				LoadPicture(bilder[i].was);

			lpDevice->SetTexture(0,game->lpTexture[bilder[i].was]);
			DrawPicture(lpDevice,i,m,bilder[i].ratio);
		}
	}

	// Stall malen
	lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	lpMesh->Render(lpDevice,FALSE);


	// Fahne malen
	D3DUtil_SetTranslateMatrix(m,0,hoehe,0);
	D3DUtil_SetRotateYMatrix(m2,-ang+winkel);
	lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m);
	lpDevice->MultiplyTransform(D3DTRANSFORMSTATE_WORLD,&m2);

	lpDevice->SetMaterial(&material);
	lpDevice->SetTexture(0,game->lpTexture[20]);
    lpDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLELIST, 
		                              lpFlag1, 0,
                                      NUM_FLAG_VERTICES, m_pFlagIndices,
                                      NUM_FLAG_INDICES, D3DDP_WAIT );


	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CW);
    lpDevice->DrawIndexedPrimitiveVB( D3DPT_TRIANGLELIST, 
		                              lpFlag2, 0,
                                      NUM_FLAG_VERTICES, m_pFlagIndices,
                                      NUM_FLAG_INDICES, D3DDP_WAIT );
	
	lpDevice->SetRenderState(D3DRENDERSTATE_CULLMODE,D3DCULL_CCW);
	lpDevice->SetTexture(0,NULL);
}

void CStall::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	DRAWNAME(hoehe+0.2f);
}

void CStall::SendData(DPID to)
{
	Send(to,NM_OBJECTDATA,id,1,FloatToLong(ang),0,&pos,sizeof(pos),TRUE);
}

void CStall::ReceiveData(PNETWORKDATA data)
{
	switch(data->p1)
	{
	case 1:
		{
			ang=LongToFloat(data->p2);
			pos=*(D3DVECTOR*)data->userdata;
			eingang->SetPos(pos);
			box->SetPos(D3DVECTOR(pos.x,pos.y+einganghoehe,pos.z));
		}
		break;
	}
}

void CStall::NewWorld()
{
	FitWorld();
	CreateFlag();
}

void CStall::FitWorld()
{
	{ // Stall auf Boden setzen
		D3DVECTOR v=D3DVECTOR(breite*0.5f,0,breite*0.5f);
		D3DMATRIX m;
		D3DUtil_SetRotateYMatrix(m,ang);
		D3DMath_VectorMatrixMultiply(v,v,m);
		D3DUtil_SetRotateYMatrix(m,g_PI_DIV_2);
		float h[4];
		for (int i=0;i<4;i++)
		{
			h[i]=world->HeightAtPoint(pos.x+v.x,pos.z+v.z);
			D3DMath_VectorMatrixMultiply(v,v,m);
		}

		pos.y=h[0];
		for (i=1;i<4;i++)if (h[i]<pos.y)pos.y=h[i];
	}

	eingang->SetPos(pos);
	box->SetPos(D3DVECTOR(pos.x,pos.y+einganghoehe,pos.z));
}

void CStall::CreateFlag()
{
#define faktor (0.68f)
	SaveRelease(lpFlag1);
	SaveRelease(lpFlag2);
	lpFlag1=CreateVertexBuffer(game->lpD3D,&game->d3ddesc,D3DFVF_VERTEX,NUM_FLAG_VERTICES,FALSE);
	lpFlag2=CreateVertexBuffer(game->lpD3D,&game->d3ddesc,D3DFVF_VERTEX,NUM_FLAG_VERTICES,FALSE);

    // Lock and fill the vertex buffer with Flag data
    D3DVERTEX* pFlagVertices1,*pFlagVertices2;
    lpFlag1->Lock( DDLOCK_WAIT, (VOID**)&pFlagVertices1,NULL);
    lpFlag2->Lock( DDLOCK_WAIT, (VOID**)&pFlagVertices2,NULL);
    {
        for( WORD ix=0; ix<FLAG_SIZE+1; ix++ )
        {
            for( WORD iy=0; iy<FLAG_SIZE+1; iy++ )
            {
                FLOAT tu = ix/(FLOAT)FLAG_SIZE;
                FLOAT tv = iy/(FLOAT)FLAG_SIZE;
                FLOAT x  = 0.0f + tu * 1.000f*faktor;
                FLOAT y  = -0.666f*faktor + tv * 0.666f*faktor - (tu/10.0f);

                pFlagVertices1[ix+iy*(FLAG_SIZE+1)] = D3DVERTEX( D3DVECTOR(x,y,0),
                                                                D3DVECTOR(0,0,-1),
                                                                tu, 1-tv );
                pFlagVertices2[ix+iy*(FLAG_SIZE+1)] = D3DVERTEX( D3DVECTOR(x,y,0),
                                                                D3DVECTOR(0,0,1),
                                                                tu, 1-tv );
            }
        }
    }
    lpFlag2->Unlock();
	lpFlag1->Unlock();
}

void CStall::Restore()
{
	CreateFlag();
}

void CStall::Invalidate()
{
	SaveRelease(lpFlag1);
	SaveRelease(lpFlag2);
}

const BOOLEAN CStall::LoadPicture(const int texturenr)
{
#define LP(tn,file) if (texturenr==tn)return (game->lpTexture[tn]=CreateTextureFromResource(game->lpDevice,game->lpDD,NULL,file,3,TRUE,FALSE,FALSE))!=NULL;

	LP(21,"image1.jpg");
	LP(22,"image2.jpg");
	LP(23,"image3.jpg");
	LP(24,"image4.jpg");
	LP(25,"image5.jpg");
	LP(26,"image6.jpg");
	LP(27,"image7.jpg");
	LP(28,"image8.jpg");

	LP(32,"image9.jpg");
	LP(33,"image10.jpg");
	LP(34,"image11.jpg");
	LP(39,"image12.jpg");
	LP(44,"image13.jpg");
	LP(45,"image14.jpg");
	LP(46,"image15.jpg");

	return FALSE;
}