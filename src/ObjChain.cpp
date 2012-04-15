/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "objchain.h"



void CObjChain::Execute(float elapsed)
{
	CObject* akt=first;
	while (akt)
	{
		if (!akt->deleting)akt->Execute(elapsed);
		
		akt=akt->next;
	}
	CleanChain();
}

void CObjChain::Add(CObject *obj)
{
	if (first==NULL)
	{
		first=obj;
		obj->prev=obj->next=NULL;
	}else{
		CObject* akt=first;
		while (akt->next!=NULL)akt=akt->next;

		akt->next=obj;
		obj->prev=akt;
	}
	obj->chain=this;
	if (!obj->Init())
	{
#ifdef _DEBUG
		DebugOut("Objektinitialisierung fehlgeschlagen!!!");
__asm int 3
#endif
	}
}

void CObjChain::CleanChain()
{
	CObject *akt=first;
	while (akt!=NULL)
	{
		if (akt->deleting)
		{
			if (akt->next!=NULL)akt->next->prev=akt->prev;
			if (akt->prev!=NULL)akt->prev->next=akt->next;else first=akt->next;
			CObject *n=akt->next;
			akt->next=akt->prev=NULL;
			delete akt;
			akt=n;
		}else akt=akt->next;
	}
}

void CObjChain::Draw(LPDIRECTDRAWSURFACE7 lpDDSBack)
{
	CObject* akt=first;
	while (akt)
	{
		akt->Draw(lpDDSBack);
		akt=akt->next;
	}
}

void CObjChain::Draw(LPDIRECT3DDEVICE7 lpDevice)
{
	CObject* akt=first;
	while (akt)
	{
		akt->Draw(lpDevice);
		akt=akt->next;
	}
}

void CObjChain::DrawUI(LPDIRECT3DDEVICE7 lpDevice)
{
	CObject* akt=first;
	while (akt)
	{
		akt->DrawUI(lpDevice);
		akt=akt->next;
	}
}

void CObjChain::DrawShadow(LPDIRECT3DDEVICE7 lpDevice)
{
	CObject* akt=first;
	while (akt)
	{
		akt->DrawShadow(lpDevice);
		akt=akt->next;
	}
}

void CObjChain::Draw(HDC dc)
{
	CObject* akt=first;
	while (akt)
	{
		akt->Draw(dc);
		akt=akt->next;
	}
}

CObject* CObjChain::Find(const UID id)
{
	if (id==0)return NULL;
	CObject* akt=GetFirst();
	while (akt)
	{
		if (akt->id==id)return akt;
		akt=akt->next;
	}
	return NULL;
}

void CObjChain::RemoveAll()
{
	CObject* akt=first;
	while (akt)
	{
		akt->Remove();
		akt=akt->next;
	}
	CleanChain();
}

void CObjChain::Invalidate()
{
	CObject* akt=first;
	while (akt)
	{
		akt->Invalidate();
		akt=akt->next;
	}
}

void CObjChain::Restore()
{
	CObject* akt=first;
	while (akt)
	{
		akt->Restore();
		akt=akt->next;
	}
}

int CObjChain::Count()
{
	CObject* akt=first;
	int i=0;
	while (akt)
	{
		i++;
		akt=akt->next;
	}
	return i;
}

#ifdef _DEBUG
int CObjChain::NumFaces()
{
	int num=0;
	CObject* akt=first;
	while (akt)
	{
		num+=akt->NumFaces();
		akt=akt->next;
	}
	return num;
}
#endif

