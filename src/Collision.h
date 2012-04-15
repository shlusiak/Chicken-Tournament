/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _COLLISION_INCLUDED_
#define _COLLISION_INCLUDED_

#include "game.h"

class CBoundingTube
{
private:
	D3DVECTOR pos;
	const float radius,height;
public:
	CBoundingTube(const D3DVECTOR &vpos,const float &vradius,const float &vheight):pos(vpos),radius(vradius),height(vheight) {}

	void SetPos(const D3DVECTOR &vpos) {pos=vpos;}
//	void SetSize(const float &vradius,const float &vheight);
	const float GetRadius()const {return radius;}
	const float GetHeight()const {return height;}
	const D3DVECTOR GetPos()const {return pos;}
	const BOOLEAN IsInside(const D3DVECTOR &point)const;
	const BOOLEAN IsInside(const D3DVECTOR &point,const float r)const;
	const BOOLEAN Collidate(const CBoundingTube* b)const;
	const D3DVECTOR MoveOut(const CBoundingTube* b);	// Bewegt this aus b
	const D3DVECTOR MoveOut(const D3DVECTOR &v)const;	// Bewegt v aus this
	const BOOLEAN IsVisible();
	static const BOOLEAN IsSphereVisible(D3DVECTOR &center,float radius);
};


// inline Definitions


inline const BOOLEAN CBoundingTube::IsInside(const D3DVECTOR &point)const
{
	return ((point.y>pos.y)&&(point.y<pos.y+height)&&(sqr(point.x-pos.x)+sqr(point.z-pos.z)<sqr(radius)));
}

inline const BOOLEAN CBoundingTube::IsInside(const D3DVECTOR &point,const float r)const
{
	return ((point.y+r>pos.y)&&(point.y-r<pos.y+height)&&(sqr(point.x-pos.x)+sqr(point.z-pos.z)<sqr(radius+r)));
}

inline const BOOLEAN CBoundingTube::Collidate(const CBoundingTube* b)const
{
	return ((b->pos.y+b->height>pos.y)&&(pos.y+height>b->pos.y)&&(sqr(b->pos.x-pos.x)+sqr(b->pos.z-pos.z)<sqr(radius+b->radius)));
}

inline const D3DVECTOR CBoundingTube::MoveOut(const CBoundingTube* b)
{
	const D3DVECTOR dir=(D3DVECTOR(pos.x-b->pos.x,0,pos.z-b->pos.z));

	if ((dir.x==0.0f)&&(dir.z==0.0f))return pos+D3DVECTOR(0,0,radius+b->radius);

	return (pos+=-dir+dir*(radius+b->radius)/sqrtf(sqr(dir.x)+sqr(dir.z)));
}

inline const D3DVECTOR CBoundingTube::MoveOut(const D3DVECTOR &v)const
{
	const D3DVECTOR dir=(D3DVECTOR(v.x-pos.x,0,v.z-pos.z));
	if ((dir.x==0.0f)&&(dir.z==0.0f))return v;

	return (v-dir+(dir*(radius/sqrtf(sqr(dir.x)+sqr(dir.z)))));
}

inline const BOOLEAN CBoundingTube::IsVisible()
{
	return IsSphereVisible(pos,(radius<height?height:radius));
}

const inline BOOLEAN CBoundingTube::IsSphereVisible(D3DVECTOR &center,float radius)
{
	D3DMATRIX m;
	D3DUtil_SetIdentityMatrix(m);
	game->lpDevice->SetTransform(D3DTRANSFORMSTATE_WORLD,&m);

	DWORD ret;

	if (FAILED(game->lpDevice->ComputeSphereVisibility(&center,&radius,1,0,&ret)))return TRUE;

    return  !(ret&D3DSTATUS_CLIPINTERSECTIONALL);
}



// -----


#endif