/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#include "stdafx.h"
#include "game.h"
#include "shadows.h"



#if (SHADOWS)


void RenderShadow(LPDIRECT3DDEVICE7 lpDevice,D3DVERTEX* vertices,int numvertices,WORD* indices,DWORD numindices)
{
    // Turn depth buffer off, and stencil buffer on
    lpDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE,  FALSE );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE, TRUE );

    // Set up stencil compare fuction, reference value, and masks
    // Stencil test passes if ((ref & mask) cmpfn (stencil & mask)) is true
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC,     D3DCMP_ALWAYS );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,      0x1 );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILMASK,     0xffffffff );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILWRITEMASK,0xffffffff );

    // If ztest passes, write 1 into stencil buffer
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILZFAIL, D3DSTENCILOP_KEEP );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILFAIL,  D3DSTENCILOP_KEEP );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS,  D3DSTENCILOP_REPLACE );

    // Make sure that no pixels get drawn to the frame buffer
    lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
    lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,  D3DBLEND_ZERO );
    lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_ONE );

	lpDevice->SetTexture(0,NULL);
    // Draw front-side of shadow volume in stencil/z only
	if (indices==NULL)
	{
		lpDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                        vertices, numvertices,
                                        NULL );
	}
	else
		lpDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                        vertices, numvertices,
                                        indices, numindices, NULL );

    // Now reverse cull order so back sides of shadow volume are written,
    // writing 0's into stencil. Result will be any pixel which still has a bit
    // set in the stencil buffer, is inside the shadow.
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILREF, 0x0 );

    // Draw back-side of shadow volume in stencil/z only
    lpDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CW );
	if (indices==NULL)
	{
		lpDevice->DrawPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                        vertices, numvertices,
                                        NULL );
	}
	else
		lpDevice->DrawIndexedPrimitive( D3DPT_TRIANGLELIST, D3DFVF_VERTEX,
                                        vertices, numvertices,
                                        indices, numindices, NULL );

    // Restore render states
    lpDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_CCW );
    lpDevice->SetRenderState( D3DRENDERSTATE_ZWRITEENABLE,     TRUE );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE );
    lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
}



void RenderSquareShadow(LPDIRECT3DDEVICE7 lpDevice,float x,float y,float z,float w,float h)
{
    WORD wIndices[] = { 
		0,1,2,
		3,2,1,
		1,5,3,
		3,5,7, 
		0,4,1,
		1,4,5, 
		0,2,4,
		4,2,6, 
		2,3,6,
		6,3,7,
	};

	D3DLIGHT7 light;
	lpDevice->GetLight(0,&light);
	D3DVECTOR length=Normalize(light.dvDirection)*(-100);

	D3DVERTEX v[8];

    v[0] = D3DVERTEX( D3DVECTOR( x-w/2, y, z-h/2 ),D3DVECTOR(0,1,0), 0, 1 );
    v[1] = D3DVERTEX( D3DVECTOR( x-w/2, y,  z+h/2 ),D3DVECTOR(0,1,0), 0, 0 );
    v[2] = D3DVERTEX( D3DVECTOR( x+w/2, y, z-h/2 ),D3DVECTOR(0,1,0), 1, 1 );
    v[3] = D3DVERTEX( D3DVECTOR( x+w/2, y,  z+h/2 ),D3DVECTOR(0,1,0), 1, 0 );

    v[4] = D3DVERTEX( D3DVECTOR( x-w/2, y, z-h/2 )-length,D3DVECTOR(0,1,0), 0, 0 );
    v[5] = D3DVERTEX( D3DVECTOR( x-w/2, y,  z+h/2 )-length,D3DVECTOR(0,1,0), 0, 0 );
    v[6] = D3DVERTEX( D3DVECTOR( x+w/2, y, z-h/2 )-length,D3DVECTOR(0,1,0), 0, 0 );
    v[7] = D3DVERTEX( D3DVECTOR( x+w/2, y,  z+h/2 )-length,D3DVECTOR(0,1,0), 0, 0 );

	RenderShadow(lpDevice,v,8,wIndices,sizeof(wIndices)/sizeof(wIndices[0]));
}

// Runden Schatten malen
void RenderRoundShadow(LPDIRECT3DDEVICE7 lpDevice,float x,float y,float z,float r)
{
#define EDGES 12
	static D3DVERTEX v[(EDGES+1)*2];

	D3DLIGHT7 light;
	lpDevice->GetLight(0,&light);
	D3DVECTOR length=Normalize(light.dvDirection)*(-100.0f);

	for (int i=0;i<=EDGES;i++)
	{
		v[i]=v[i+EDGES+1]=D3DVERTEX(D3DVECTOR(x+sinf(float(i)/float(EDGES)*g_PI*2.0f)*r,y,z+cosf(float(i)/float(EDGES)*g_PI*2.0f)*r),D3DVECTOR(0,1,0),0,0);
		v[i+EDGES+1].x-=length.x;
		v[i+EDGES+1].y-=length.y;
		v[i+EDGES+1].z-=length.z;
	}

	static WORD wIndices[(EDGES-2)*3+EDGES*6];
	const PWORD Cap=&wIndices[0];
	const PWORD Side=&wIndices[(EDGES-2)*3];

	for (i=0;i<EDGES-2;i++)
	{
		Cap[i*3]=0;
		Cap[i*3+1]=i+1;
		Cap[i*3+2]=i+2;
	}
	for (i=0;i<EDGES;i++)
	{
		Side[i*6+0]=i;
		Side[i*6+1]=i+EDGES+2;
		Side[i*6+2]=i+1;
		Side[i*6+3]=i;
		Side[i*6+4]=i+EDGES+1;
		Side[i*6+5]=i+EDGES+2;
	}

	RenderShadow(lpDevice,v,EDGES*2+2,wIndices,sizeof(wIndices)/sizeof(wIndices[0]));
//	RenderShadow(lpDevice,v,8,wIndices,(EDGES-2)*3);
}


/*
void AddEdge( WORD* pEdges, DWORD& dwNumEdges, WORD v0, WORD v1 )
{
    // Remove interior edges (which appear in the list twice)
    for( DWORD i=0; i < dwNumEdges; i++ )
    {
        if( ( pEdges[2*i+0] == v0 && pEdges[2*i+1] == v1 ) ||
            ( pEdges[2*i+0] == v1 && pEdges[2*i+1] == v0 ) )
        {
            if( dwNumEdges > 1 )
            {
                pEdges[2*i+0] = pEdges[2*(dwNumEdges-1)+0];
                pEdges[2*i+1] = pEdges[2*(dwNumEdges-1)+1];
            }
            dwNumEdges--;
            return;
        }
    }

    pEdges[2*dwNumEdges+0] = v0;
    pEdges[2*dwNumEdges+1] = v1;
    dwNumEdges++;
}

inline D3DVERTEX Vector2Vertex(D3DVECTOR &v)
{
	return D3DVERTEX(v,D3DVECTOR(0,1,0),0,0);
}

void RenderModelShadow(LPDIRECT3DDEVICE7 lpDevice,CD3DFileObject* lpMesh)
{
	D3DVECTOR vLight;
	{
		D3DLIGHT7 light;
		lpDevice->GetLight(0,&light);
		vLight=Normalize(light.dvDirection);
		vLight=D3DVECTOR(0,-1,0);
	}
	D3DVERTEX * pVertices;
    WORD*       pIndices;
	DWORD dwNumVertices;
	DWORD dwNumFaces;

	lpMesh->GetMeshGeometry(&pVertices,&dwNumVertices,&pIndices,&dwNumFaces);
	dwNumFaces=dwNumFaces/3;

	DWORD m_dwNumVertices=0;

    // Allocate a temporary edge list
    WORD* pEdges = new WORD[dwNumFaces*6];
    DWORD dwNumEdges = 0;

    // For each face
    for( DWORD i=0; i<dwNumFaces; i++ )
    {
        WORD wFace0 = pIndices[3*i+0];
        WORD wFace1 = pIndices[3*i+1];
        WORD wFace2 = pIndices[3*i+2];

        D3DVECTOR v0 = D3DVECTOR(pVertices[wFace0].x,pVertices[wFace0].y,pVertices[wFace0].z);
        D3DVECTOR v1 = D3DVECTOR(pVertices[wFace1].x,pVertices[wFace1].y,pVertices[wFace1].z);
        D3DVECTOR v2 = D3DVECTOR(pVertices[wFace2].x,pVertices[wFace2].y,pVertices[wFace2].z);

        // Transform vertices or transform light?
        D3DVECTOR vNormal;
        vNormal=CrossProduct( (v2-v1), (v1-v0) );

        if( DotProduct( vNormal, vLight ) >= 0.0f )
        {
            AddEdge( pEdges, dwNumEdges, wFace0, wFace1 );
            AddEdge( pEdges, dwNumEdges, wFace1, wFace2 );
            AddEdge( pEdges, dwNumEdges, wFace2, wFace0 );
        }
    }

	D3DVERTEX* m_pVertices=new D3DVERTEX[dwNumEdges*6];

    for( i=0; i<dwNumEdges; i++ )
    {
#define ex (-40)
        D3DVECTOR v1 = D3DVECTOR(pVertices[pEdges[2*i+0]].x,pVertices[pEdges[2*i+0]].y,pVertices[pEdges[2*i+0]].z);
        D3DVECTOR v2 = D3DVECTOR(pVertices[pEdges[2*i+1]].x,pVertices[pEdges[2*i+1]].y,pVertices[pEdges[2*i+1]].z);
        D3DVECTOR v3 = v1 - vLight*(ex);
        D3DVECTOR v4 = v2 - vLight*(ex);

        // Add a quad (two triangles) to the vertex list
        m_pVertices[m_dwNumVertices++] = Vector2Vertex(v1);
        m_pVertices[m_dwNumVertices++] = Vector2Vertex(v3);
        m_pVertices[m_dwNumVertices++] = Vector2Vertex(v2);

        m_pVertices[m_dwNumVertices++] = Vector2Vertex(v2);
        m_pVertices[m_dwNumVertices++] = Vector2Vertex(v3);
        m_pVertices[m_dwNumVertices++] = Vector2Vertex(v4);
    }
    // Delete the temporary edge list
    delete[] pEdges;

	RenderShadow(lpDevice,m_pVertices,m_dwNumVertices,NULL,0);


	delete[] m_pVertices;
}


*/

void DrawShadow(LPDIRECT3DDEVICE7 lpDevice)
{
    lpDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,       FALSE );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE, TRUE );

    // Turn on alphablending
    lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );

    lpDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND, D3DBLEND_SRCALPHA );
    lpDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND, D3DBLEND_INVSRCALPHA );

	lpDevice->SetTextureStageState(0,D3DTSS_ALPHAARG1,D3DTA_DIFFUSE);

    // Only write where the stencil value == 1
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILREF,  0x1 );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILFUNC, D3DCMP_EQUAL );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILPASS, D3DSTENCILOP_KEEP );

    // Get viewport dimensions for big, gray square
    D3DVIEWPORT7 vp;
    lpDevice->GetViewport(&vp);
    FLOAT sx = (FLOAT)vp.dwWidth;
    FLOAT sy = (FLOAT)vp.dwHeight;

	D3DCOLOR color=D3DRGBA(0,0,0,0.5f);
    // Draw a big, gray square
    D3DTLVERTEX vBigGraySquare[4];
    vBigGraySquare[0] = D3DTLVERTEX( D3DVECTOR( 0,sy,0.0f),1.0f,color,0,0,0 );
    vBigGraySquare[1] = D3DTLVERTEX( D3DVECTOR( 0, 0,0.0f),1.0f,color,0,0,0 );
    vBigGraySquare[2] = D3DTLVERTEX( D3DVECTOR(sx,sy,0.0f),1.0f,color,0,0,0 );
    vBigGraySquare[3] = D3DTLVERTEX( D3DVECTOR(sx, 0,0.0f),1.0f,color,0,0,0 );
    lpDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, D3DFVF_TLVERTEX,
                                 vBigGraySquare, 4, NULL );

    // Restore render states
    lpDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,          TRUE );
    lpDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE );
    lpDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, FALSE );
}

  
#endif