/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _OBJCHAIN_INCLUDED_
#define _OBJCHAIN_INCLUDED_


#include "objects.h"




class CObjChain:public CObject
{
private:
	CObject* first;
public:
	CObjChain() :first(NULL) {}
	virtual ~CObjChain() {RemoveAll();}

	virtual void Execute(float elapsed);
	void Add(CObject* obj);
	void RemoveAll();
	void CleanChain();
	CObject* GetFirst()const {return first;}

	virtual void Draw(LPDIRECTDRAWSURFACE7 lpDDSBack);
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice);
	virtual void DrawShadow(LPDIRECT3DDEVICE7 lpDevice);
	virtual void Draw(HDC dc);

	virtual void Invalidate();
	virtual void Restore();

	CObject* Find(const UID id);

	int Count();
	
#ifdef _DEBUG
	virtual int NumFaces();
#endif
};










#endif