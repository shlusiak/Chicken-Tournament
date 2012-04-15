/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _ANIMATION_INCLUDED_
#define _ANIMATION_INCLUDED_



class CMorpher
{
private:
	CD3DFile* file;
	struct TMORPHFRAME
	{
		CD3DFileObject* mesh;
		D3DVERTEX* vertices;
	}*objects,target;
	DWORD numvertices;
	const BYTE num;

	int ref;
public:
	CMorpher(CD3DFile* vfile,const BYTE vnum):file(vfile),num(vnum),objects(new TMORPHFRAME[vnum]),ref(0) {}
	~CMorpher() { if (objects) delete[] objects; }

	CD3DFileObject* SetTarget(const PCHAR p);
	CD3DFileObject* GetTarget() const { return target.mesh; }
	void SetFrame(const BYTE index,const PCHAR p) const;
	
	void BlendMeshes(D3DVERTEX* pDstMesh, const D3DVERTEX* pSrcMesh1, const D3DVERTEX* pSrcMesh2, const DWORD dwNumVertices, const FLOAT fWeight) const;

	void Morph(const BYTE state1,const BYTE state2,const float weight) const { BlendMeshes(target.vertices,objects[state1].vertices,objects[state2].vertices,numvertices,weight); }
	void LinearMorph(float phase,const int min,const int max) const;

	void AddRef() { ref++; }
	const BOOLEAN Release() { return ((--ref)==0); }
};



#endif