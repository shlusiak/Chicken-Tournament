/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _WORLD_INCLUDED_
#define _WORLD_INCLUDED_

#include "objects.h"
#include "multiplayer.h"
#include "gras.h"


const float worldgrenze=0.5f;



struct WALLVF
{
	D3DVECTOR pos;
	D3DVECTOR normal;
	float tu;
	float tv;
};



class CWorld:public CObject
{
private:
	LPDIRECT3DVERTEXBUFFER7 lpVertexBuffer; // Vertexbuffer der Welt
	LPDIRECT3DVERTEXBUFFER7 lpSkyBuffer;	// Vertexbuffer des Himmels
	LPDIRECT3DVERTEXBUFFER7 lpWallBuffer;	// Vertexbuffer der Wand

	D3DVERTEX *vertices;	// Roh-Daten
	int numvertices;
	struct TRow
	{
		D3DVERTEX *vertices;
		int numvertices;
	}*ground;	// Vertices nach Spalten und Reihen sortiert

	D3DVERTEX* wasservertices;
	WORD wassernumvertices;

	int rows,cols;
	float width,height;
	D3DMATERIAL7 mat,wasser;
	CGras* lpGras;

	float time;

	void CreateWalls();
	void CreateWater();
	void CreateSky();
	void MakeVertexBuffer(); 
	inline void AnimateWater(const float elapsed);
	void MakeWallVertices(WALLVF* v,const float miny,const float maxy,const float width,const float height,const float maxth,const float ratio,const BOOLEAN flipu=FALSE)const;

	inline const WALLVF WALLVERTEX(const D3DVECTOR pos,const D3DVECTOR normal,const float tu,const float tv)const 
	{
		WALLVF v;
		v.pos=pos;
		v.normal=normal;
		v.tu=tu;
		v.tv=tv;
		return v;
	}

public:
	CWorld();
	virtual ~CWorld();

	virtual BOOLEAN Init();

	void CreateScene(const int rows,const int cols,const float sizex,const float sizey);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	void DrawSky(const LPDIRECT3DDEVICE7 lpDevice,const D3DVECTOR PlayerPos);
	void DrawUnderwater(const LPDIRECT3DDEVICE7 lpDevice);
	virtual void Execute(float elapsed);

	// Gibt die Höhe über 0 des angegebenen Punkts in der Welt zurück
	inline const float HeightAtPoint(const float x,const float y)const;
	const float HeightAtPoint(const float x,const float y,D3DVECTOR* normale)const;
	inline void Kippe(D3DMATRIX &matrix,const float x,const float z)const;

	const float minx()const {return -width*0.5f+worldgrenze;}
	const float maxx()const {return +width*0.5f-worldgrenze;}
	const float minz()const {return -height*0.5f+worldgrenze;}
	const float maxz()const {return +height*0.5f-worldgrenze;}
	const float RandX(const float abstand)const {return minx()+abstand+(randf()*(maxx()-minx()-abstand*2.0f));}
	const float RandY(const float abstand)const {return minz()+abstand+(randf()*(maxz()-minz()-abstand*2.0f));}
	const float GetWidth()const {return width;}
	const float GetHeight()const {return height;}
	const D3DVECTOR Rand(const float abstand)const { float x=RandX(abstand),y=RandY(abstand); return D3DVECTOR(x,HeightAtPoint(x,y),y); }
	const BOOLEAN IsInside(const D3DVECTOR p,const float radius)const {return ((p.x-radius>minx())&&(p.x+radius<maxx())&(p.z-radius>minz())&(p.z+radius<maxz())); }
	const BOOLEAN IsInside(const D3DVECTOR p)const { return ((p.x>minx())&&(p.x<maxx())&&(p.z>minz())&&(p.z<maxz())); }
	const float GetDistanceFromWall(const D3DVECTOR p)const { return Min(D3DVECTOR(maxx()-abs(p.x),maxx()-abs(p.x),maxz()-abs(p.z))); }

	virtual void Invalidate();
	virtual void Restore();

	void MakeInside(D3DVECTOR &p,const float radius=0.0f)const { if (p.x-radius<minx()) p.x=minx()+radius;	if (p.x>maxx()-radius) p.x=maxx()-radius;	if (p.z<minz()+radius) p.z=minz()+radius;	if (p.z>maxz()-radius) p.z=maxz()-radius; }
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA);
	virtual void NewWorld();

#ifdef _DEBUG
	virtual int NumFaces();
#endif
};




// inline Definitions
inline const float CWorld::HeightAtPoint(const float x,const float y)const
{
	if ((abs(x)>=width/2.0f-0.05f)||(abs(y)>=height/2.0f-0.05f))return 0;

	int n=int(floorf((y+height/2.0f)/height*(cols-1)))*6;
	const int m=int(floorf((x+width/2.0f)/width*(rows-1)));

	D3DVERTEX *v1=&ground[m].vertices[n],*v2=&ground[m].vertices[n+1],*v3=&ground[m].vertices[n+2];
	D3DVECTOR 
		l1= Normalize(D3DVECTOR(v1->x - x, 0, v1->z - y)),
		l2= Normalize(D3DVECTOR(v2->x - x, 0, v2->z - y)),
		l3= Normalize(D3DVECTOR(v3->x - x, 0, v3->z - y));
	float a= acosf(DotProduct(l1, l2)),b= acosf(DotProduct(l2, l3)),c= acosf(DotProduct(l1, l3)),d;
	if (abs(a + b + c - g_2_PI) >= 0.01f)n+=3;
	
	v1=&ground[m].vertices[n];
	v2=&ground[m].vertices[n+1];
	v3=&ground[m].vertices[n+2];

	const D3DVECTOR norm=CrossProduct(
		D3DVECTOR(v2->x-v1->x,v2->y-v1->y,v2->z-v1->z),
		D3DVECTOR(v3->x-v1->x,v3->y-v1->y,v3->z-v1->z));
	a=norm.x;
	b=norm.y;
	c=norm.z;
	d=-(a*v1->x+b*v1->y+c*v1->z);
	return (a*x+c*y+d) / (-b) ;
}	// Ende HeightAtPoint

inline void CWorld::Kippe(D3DMATRIX &matrix,const float x,const float z)const
{
	D3DVECTOR N;
	D3DMATRIX m;
	HeightAtPoint(x,z,&N);
	const float alpha=acosf(DotProduct(Normalize(N),D3DVECTOR(0,1,0)));
	const float beta=atan2f(N.x,N.z);

	D3DUtil_SetRotateYMatrix(m,beta);
	D3DMath_MatrixMultiply(matrix,m,matrix);
	D3DUtil_SetRotateXMatrix(m,alpha);
	D3DMath_MatrixMultiply(matrix,m,matrix);
	D3DUtil_SetRotateYMatrix(m,-beta);
	D3DMath_MatrixMultiply(matrix,m,matrix);
}




#endif