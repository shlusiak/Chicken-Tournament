/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "animation.h"



typedef CD3DFileObject *PD3DFileObject;

CD3DFileObject* CMorpher::SetTarget(const PCHAR p)
{
	target.mesh=file->FindObject(p);

	WORD *indices;
	DWORD numindices;
	target.mesh->GetMeshGeometry(&target.vertices,&numvertices,&indices,&numindices);
	return target.mesh;
}

void CMorpher::SetFrame(const BYTE index,const PCHAR p) const
{
	objects[index].mesh=file->FindObject(p);

	WORD *indices;
	DWORD numindices,numvertices;
	objects[index].mesh->GetMeshGeometry(&objects[index].vertices,&numvertices,&indices,&numindices);
}

void CMorpher::BlendMeshes(D3DVERTEX* pDstMesh,const D3DVERTEX* pSrcMesh1,const D3DVERTEX* pSrcMesh2,const DWORD dwNumVertices,const FLOAT fWeight ) const
{
    FLOAT fInvWeight = 1.0f - fWeight;

    // LERP positions and normals
    for( DWORD i=0; i<dwNumVertices; i++ )
    {
        pDstMesh->x  = fWeight*pSrcMesh1->x  + fInvWeight*pSrcMesh2->x;
        pDstMesh->y  = fWeight*pSrcMesh1->y  + fInvWeight*pSrcMesh2->y;
        pDstMesh->z  = fWeight*pSrcMesh1->z  + fInvWeight*pSrcMesh2->z;
        pDstMesh->nx = fWeight*pSrcMesh1->nx + fInvWeight*pSrcMesh2->nx;
        pDstMesh->ny = fWeight*pSrcMesh1->ny + fInvWeight*pSrcMesh2->ny;
        pDstMesh->nz = fWeight*pSrcMesh1->nz + fInvWeight*pSrcMesh2->nz;

        pDstMesh++;
        pSrcMesh1++;
        pSrcMesh2++;
    }
}

void CMorpher::LinearMorph(float phase,const int min,const int max) const
{
	while (phase>=1.0f)phase-=1.0f;
	while (phase<0.0f)phase+=1.0f;

	const int a=((int)floorf(phase*(max-min+1)));

	Morph(a%(max-min+1)+min,(a+1)%(max-min+1)+min,1.0f-phase*(max-min+1)+(float)a);
}

