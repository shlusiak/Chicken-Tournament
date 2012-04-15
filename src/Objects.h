/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _OBJECTS_INCLUDED_
#define _OBJECTS_INCLUDED_

#include "ids.h"
#include "multiplayer.h"





class CObjChain;

class CObject
{
public:
	CObject* next,*prev;
	CObjChain* chain;
	BOOLEAN deleting;
	UID id;

	CObject():next(NULL),prev(NULL),deleting(FALSE),chain(NULL),id(MakeUID(IDUnknown)) {}

	virtual ~CObject() {}

	virtual BOOLEAN Init() { return TRUE; }
	virtual void Execute(float elapsed) {}
	void Remove() { deleting=TRUE; }
	virtual void Draw(LPDIRECTDRAWSURFACE7 lpDDSBack) {}
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice) {}
	virtual void DrawUI(LPDIRECT3DDEVICE7 lpDevice) {} // Draw UI Objects (after physical objects)
	virtual void Draw(HDC dc) {}
	virtual void DrawShadow(LPDIRECT3DDEVICE7 lpDevice) {}	// Schatten rendern, wenn einer da
//	virtual void HandleInput(int mx,int my,int mz) {}
	virtual void Notify(PNETWORKDATA data) {}
	inline const DWORD SendNotify(const DWORD code,const DWORD param1=0,const DWORD param2=0,const LPVOID data=0,const DWORD datasize=0,const BOOLEAN guaranteed=TRUE)
	{ return Send(DPID_ALLPLAYERS,NM_NOTIFY,id,code,param1,param2,data,datasize,guaranteed); }

	virtual void SendData(DPID to) {}
	virtual void ReceiveData(PNETWORKDATA data) {}
	virtual void Unbind() {}
	virtual void NewWorld() {}
	virtual void Invalidate() {}
	virtual void Restore() {}

	inline const BOOLEAN IsType(const UID type) { return GetType(id)==type; }

#ifdef _DEBUG
	virtual int NumFaces() { return 0; }
#endif
};



// inline Definitions


#ifdef _DEBUG

#define DRAWNAME(offset)\
if (Config.ShowPlayernames)\
{\
	CHAR c[20];\
	sprintf(&c[0],"UID #%d",id);\
	game->DrawName(pos+D3DVECTOR(0,offset,0),&c[0],D3DRGB(0,0.7f,1.0f));\
}


#else

#define DRAWNAME(offset)

#endif



#endif