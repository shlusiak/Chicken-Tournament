/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _STALL_INCLUDED_
#define _STALL_INCLUDED_


#include "objects.h"
#include "collision.h"



const FLAG_SIZE=6;
const NUM_FLAG_VERTICES=((FLAG_SIZE+1)*(FLAG_SIZE+1));
const NUM_FLAG_INDICES=(FLAG_SIZE*FLAG_SIZE*6);
const NUM_POLE_VERTICES=8*2;



class CStall:public CObject
{
private:
	LPDIRECT3DVERTEXBUFFER7 lpFlag1,lpFlag2;
	CD3DFileObject* lpMesh;

	float ang;
	float time;
	D3DMATERIAL7 material,bild;

    WORD        m_pFlagIndices[NUM_FLAG_INDICES];
    D3DVERTEX   m_pPoleVertices[NUM_POLE_VERTICES];

	struct BILD
	{
		BYTE was;
		float ratio;
	}bilder[4];

	void FitWorld();
	void DrawFlag(LPDIRECT3DDEVICE7 lpDevice)const;
	void CreateFlag();
	void DrawPicture(LPDIRECT3DDEVICE7 lpDevice,const int wo,D3DMATRIX matrix,const float ratio)const;
	static const BOOLEAN LoadPicture(const int texturenr);
public:
	CBoundingTube *box,*eingang;
	D3DVECTOR pos;

	CStall();
	virtual ~CStall();

	virtual BOOLEAN Init();
	virtual void Execute(float elapsed);

	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void NewWorld();
	virtual void Invalidate();
	virtual void Restore();
};












#endif