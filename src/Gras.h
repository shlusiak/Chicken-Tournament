/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _GRAS_INCLUDED_
#define _GRAS_INCLUDED_


#include "objects.h"



class CGras:public CObject
{
private:
	int vertexbuffers;
	LPDIRECT3DVERTEXBUFFER7 *lpVertices;
	WORD *vertexnum;
	WORD *Indices;

	void CreateGras(int index,int anzahl);
public:
	CGras();
	~CGras();

	void InitGras(int anzahl);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Invalidate();
	virtual void Restore();
	const BOOLEAN IsCreated()const {return lpVertices!=NULL;}
#ifdef _DEBUG
	virtual int NumFaces();
#endif
};







#endif