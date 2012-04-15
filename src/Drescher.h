/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _DRESCHER_INCLUDED_
#define _DRESCHER_INCLUDED_

#include "objects.h"
#include "weapon.h"
#include "player.h"
#include "collision.h"
#include "chicken.h"
#include "multiplayer.h"


class CDrescher:public CWeapon
{
private:
	D3DVECTOR Normale,IstNormale;
	CD3DFileObject *mesh1,*mesh2;
	float rolle;
	D3DVECTOR speed;
	float alpha,beta;
	float schaukelphase;
	CSound* sound;
	CMessageChain *statusmessage;

	const D3DVECTOR GetCenter1()const;
	const D3DVECTOR GetCenter2()const;
	const D3DVECTOR PrivotFromCenter2(const D3DVECTOR p)const;
	const BOOLEAN PointInRolle(D3DVECTOR point,const float radius)const;
	inline void UpdateBox();
	const float schaukelwinkel()const { return sinf(schaukelphase)/15.0f; }
	void UpdateWinkel();
public:
	D3DVECTOR ang,acc;
	float rot;
	CBoundingTube *box1; // Innerer Radius (ohne Rolle)
	CBoundingTube *box2; // Äußerer Radius (mit Rolle)

	CDrescher();
	~CDrescher();
	BOOLEAN Init();
	virtual void Draw(LPDIRECT3DDEVICE7 lpDevice);

	const BOOLEAN GetIn(CPlayer* p);
	void GetOut();

	void HandleInput(const int mx,const int my,const int mz);
	virtual void Execute(float elapsed);
	const BOOLEAN HitRolle(CBoundingTube* tube) {return PointInRolle(tube->GetPos(),tube->GetRadius());}
	virtual void SendData(DPID to);
	virtual void ReceiveData(PNETWORKDATA data);
	virtual void Notify(PNETWORKDATA data);
	virtual void Unbind();
	virtual void NewWorld();
	void GetViewMatrix(D3DMATRIX &view)const;
	const D3DVECTOR GetOutput()const;
};



// inline Funktionen

inline void CDrescher::UpdateBox()
{
	box1->SetPos(GetCenter1());
	box2->SetPos(GetCenter2());
	UpdateWinkel();
}


// -----




#endif