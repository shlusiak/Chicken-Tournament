#include "stdafx.h"
#include "utils.h"




const HRESULT CreateSphere( D3DVERTEX** ppVertices,
                      DWORD* pdwNumVertices,
                      WORD** ppIndices, DWORD* pdwNumIndices,
                      const FLOAT fRadius, const DWORD dwNumRings, const float uxmul,const float uymul )
{
    // Allocate memory for the vertices and indices
//	if (dwNumRings>72)dwNumRings=72;
    DWORD      dwNumVertices = (dwNumRings*(2*dwNumRings+1)+2);
    DWORD      dwNumIndices  = 6*(dwNumRings*2)*((dwNumRings-1)+1);
    D3DVERTEX* pVertices     = new D3DVERTEX[dwNumVertices];
    WORD*      pIndices      = new WORD[dwNumIndices];

    (*ppVertices) = pVertices;
    (*ppIndices)  = pIndices;
    
    // Counters
    WORD x, y, vtx = 0, index = 0;

    // Angle deltas for constructing the sphere's vertices
    FLOAT fDAng   = g_PI / dwNumRings;
    FLOAT fDAngY0 = fDAng;

    // Make the middle of the sphere
    for( y=0; y<dwNumRings; y++ )
    {
        FLOAT y0 = (FLOAT)cos(fDAngY0);
        FLOAT r0 = (FLOAT)sin(fDAngY0);
//        FLOAT tv = (1.0f - y0)/2;
		float tv=(fDAngY0/g_PI);

        for( x=0; x<(dwNumRings*2)+1; x++ )
        {
            FLOAT fDAngX0 = x*fDAng;
        
            D3DVECTOR v( r0*(FLOAT)sin(fDAngX0), y0, r0*(FLOAT)cos(fDAngX0) );
            FLOAT tu = 1.0f - x/(dwNumRings*2.0f);

            *pVertices++ = D3DVERTEX( fRadius*v, v, tu*uxmul, tv*uymul);
            vtx ++;
        }
        fDAngY0 += fDAng;
    }

    for( y=0; y<dwNumRings-1; y++ )
    {
        for( x=0; x<(dwNumRings*2); x++ )
        {
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) );
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) );
            *pIndices++ = (WORD)( (y+0)*(dwNumRings*2+1) + (x+1) );
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+0) ); 
            *pIndices++ = (WORD)( (y+1)*(dwNumRings*2+1) + (x+1) );
            index += 6;
        }
    }
    // Make top and bottom
    D3DVECTOR vy( 0.0f, 1.0f, 0.0f );
    WORD wNorthVtx = vtx;
    *pVertices++ = D3DVERTEX( fRadius*vy, vy, 0.5f*uxmul, 0.0f );
    vtx++;
    WORD wSouthVtx = vtx;
    *pVertices++ = D3DVERTEX( -fRadius*vy,-vy, 0.5f*uxmul, 1.0f*uymul );
    vtx++;

    for( x=0; x<(dwNumRings*2); x++ )
    {
        WORD p1 = wSouthVtx;
        WORD p2 = (WORD)( (y)*(dwNumRings*2+1) + (x+1) );
        WORD p3 = (WORD)( (y)*(dwNumRings*2+1) + (x+0) );

        *pIndices++ = p1;
        *pIndices++ = p3;
        *pIndices++ = p2;
        index += 3;
    }

    for( x=0; x<(dwNumRings*2); x++ )
    {
        WORD p1 = wNorthVtx;
        WORD p2 = (WORD)( (0)*(dwNumRings*2+1) + (x+1) );
        WORD p3 = (WORD)( (0)*(dwNumRings*2+1) + (x+0) );

        *pIndices++ = p1;
        *pIndices++ = p3;
        *pIndices++ = p2;
        index += 3;
    }

    (*pdwNumVertices) = vtx;
    (*pdwNumIndices)  = index;

    return S_OK;
}

void TransformVertex(const LPDIRECT3DDEVICE7 lpDevice,D3DTLVERTEX* pvVertex)
{
    // Get the width and height of the viewport. This is needed to scale the
    // transformed vertices to fit the render window.
    D3DVIEWPORT7 vp;
    lpDevice->GetViewport(&vp);

    const float fClipWidth2  = (float) vp.dwWidth;
    const float fClipHeight2 = (float) vp.dwHeight;
    const float fClipWidth   = fClipWidth2 * 0.5f;
    const float fClipHeight  = fClipHeight2 * 0.5f;

    // Get the current matrix set. This is needed for the transformation.
    D3DMATRIX matWorld, matView, matProj, matSet;
    lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
    lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW, &matView);
    lpDevice->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

    D3DMath_MatrixMultiply(matSet, matWorld, matView);
    D3DMath_MatrixMultiply(matSet, matSet, matProj);

    // Get the untransformed vertex position
    const float fX = pvVertex->sx;
    const float fY = pvVertex->sy;
    const float fZ = pvVertex->sz;

    // Transform it through the current matrix set
    const float fXp = matSet.m[0][0]*fX + matSet.m[1][0]*fY + matSet.m[2][0]*fZ + matSet.m[3][0];
    const float fYp = matSet.m[0][1]*fX + matSet.m[1][1]*fY + matSet.m[2][1]*fZ + matSet.m[3][1];
    const float fZp = matSet.m[0][2]*fX + matSet.m[1][2]*fY + matSet.m[2][2]*fZ + matSet.m[3][2];
    const float fWp = matSet.m[0][3]*fX + matSet.m[1][3]*fY + matSet.m[2][3]*fZ + matSet.m[3][3];

    const float fWpInv = 1.0f / fWp;

    // Finally, scale the vertices to screen coords. This step first
    // "flattens" the coordinates from 3D space to 2D device coordinates,
    // by dividing each coordinate by the wp value. Then, the x- and
    // y-components are transformed from device coords to screen coords.
    // Note 1: device coords range from -1 to +1 in the viewport.
    // Note 2: the sz-coordinate will be used in the z-buffer.

    pvVertex->sx  = (1.0f + (fXp * fWpInv)) * fClipWidth;
    pvVertex->sy  = (1.0f - (fYp * fWpInv)) * fClipHeight;
    pvVertex->sz  = fZp * fWpInv;
    pvVertex->rhw = fWp;
}

const float TransformVector(const LPDIRECT3DDEVICE7 lpDevice,D3DVECTOR& v)
{
    // Get the width and height of the viewport. This is needed to scale the
    // transformed vertices to fit the render window.
    D3DVIEWPORT7 vp;
    lpDevice->GetViewport(&vp);

    const float fClipWidth2  = (float) vp.dwWidth;
    const float fClipHeight2 = (float) vp.dwHeight;
    const float fClipWidth   = fClipWidth2 * 0.5f;
    const float fClipHeight  = fClipHeight2 * 0.5f;

    // Get the current matrix set. This is needed for the transformation.
    D3DMATRIX matWorld, matView, matProj, matSet;
    lpDevice->GetTransform(D3DTRANSFORMSTATE_WORLD, &matWorld);
    lpDevice->GetTransform(D3DTRANSFORMSTATE_VIEW, &matView);
    lpDevice->GetTransform(D3DTRANSFORMSTATE_PROJECTION, &matProj);

    D3DMath_MatrixMultiply(matSet, matWorld, matView);
    D3DMath_MatrixMultiply(matSet, matSet, matProj);

    // Get the untransformed vertex position
    const float fX = v.x;
    const float fY = v.y;
    const float fZ = v.z;

    // Transform it through the current matrix set
    const float fXp = matSet.m[0][0]*fX + matSet.m[1][0]*fY + matSet.m[2][0]*fZ + matSet.m[3][0];
    const float fYp = matSet.m[0][1]*fX + matSet.m[1][1]*fY + matSet.m[2][1]*fZ + matSet.m[3][1];
    const float fZp = matSet.m[0][2]*fX + matSet.m[1][2]*fY + matSet.m[2][2]*fZ + matSet.m[3][2];
    const float fWp = matSet.m[0][3]*fX + matSet.m[1][3]*fY + matSet.m[2][3]*fZ + matSet.m[3][3];

    const float fWpInv = 1.0f / fWp;

    // Finally, scale the vertices to screen coords. This step first
    // "flattens" the coordinates from 3D space to 2D device coordinates,
    // by dividing each coordinate by the wp value. Then, the x- and
    // y-components are transformed from device coords to screen coords.
    // Note 1: device coords range from -1 to +1 in the viewport.
    // Note 2: the sz-coordinate will be used in the z-buffer.

    v.x  = (1.0f + (fXp * fWpInv)) * fClipWidth;
    v.y  = (1.0f - (fYp * fWpInv)) * fClipHeight;
    v.z  = fZp * fWpInv;

    if(v.x < 0.0f || v.x > fClipWidth2 ||
       v.y < 0.0f || v.y > fClipHeight2)
    {
       return 0.0f;
    }

    return fWp;
}

void MakeBillboardVertices(D3DTLVERTEX *v,const int x,const int y,const int w,const int h,const DWORD color)
{
	v[0]=D3DTLVERTEX(D3DVECTOR(float(x),float(y),0),0.5f,color,0,0,0);
	v[1]=D3DTLVERTEX(D3DVECTOR(float(x+w),float(y),0),0.5f,color,0,1,0);
	v[2]=D3DTLVERTEX(D3DVECTOR(float(x),float(y+h),0),0.5f,color,0,0,1);
	v[3]=D3DTLVERTEX(D3DVECTOR(float(x+w),float(y+h),0),0.5f,color,0,1,1);
}


LPDIRECT3DVERTEXBUFFER7 CreateVertexBuffer(const LPDIRECT3D7 lpD3D,const D3DDEVICEDESC7 *d3ddesc,const DWORD type,const int num,const BOOLEAN videomemory)
{
	LPDIRECT3DVERTEXBUFFER7 vb=NULL;

	D3DVERTEXBUFFERDESC d;
	d.dwSize=sizeof(d);
	d.dwCaps=D3DVBCAPS_WRITEONLY;
	if (d3ddesc->deviceGUID!=IID_IDirect3DTnLHalDevice)
		d.dwCaps|=D3DVBCAPS_SYSTEMMEMORY;

	if (!videomemory)
		d.dwCaps|=D3DVBCAPS_SYSTEMMEMORY;

	d.dwFVF=type;
	d.dwNumVertices=num;

	lpD3D->CreateVertexBuffer(&d,&vb,0);

	return vb;
}


void ConvertLastErrorToString(PCHAR dest,const int maxlen)
{
	LPVOID buf;
	FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER|FORMAT_MESSAGE_FROM_SYSTEM|FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		GetLastError(),
		MAKELANGID(LANG_NEUTRAL,SUBLANG_DEFAULT),
		(LPTSTR)&buf,
		0,
		NULL);
	strncpy(dest,reinterpret_cast<char*>(buf),maxlen);
	LocalFree(buf);
}