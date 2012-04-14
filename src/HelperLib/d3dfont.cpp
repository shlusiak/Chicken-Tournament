//-----------------------------------------------------------------------------
// File: D3DFont.cpp
//
// Desc: Texture-based font class
//
// Copyright (c) 1999-2000 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#include "stdafx.h"
#include "D3DFont.h"
#include "utils.h"




//-----------------------------------------------------------------------------
// Custom vertex types for rendering text
//-----------------------------------------------------------------------------
#define MAX_NUM_VERTICES 50*6

struct FONT2DVERTEX { D3DVECTOR p;   float rw; DWORD color;     FLOAT tu, tv; };
struct FONT3DVERTEX { D3DVECTOR p;   D3DVECTOR n;   FLOAT tu, tv; };

#define D3DFVF_FONT2DVERTEX (D3DFVF_XYZRHW|D3DFVF_DIFFUSE|D3DFVF_TEX1)
#define D3DFVF_FONT3DVERTEX (D3DFVF_XYZ|D3DFVF_NORMAL|D3DFVF_TEX1)

inline FONT2DVERTEX InitFont2DVertex( const D3DVECTOR& p, D3DCOLOR color,
                                      FLOAT tu, FLOAT tv )
{
    FONT2DVERTEX v;   v.p = p;   v.rw=1.0f; v.color = color;   v.tu = tu;   v.tv = tv;
    return v;
}

inline FONT3DVERTEX InitFont3DVertex( const D3DVECTOR& p, const D3DVECTOR& n,
                                      FLOAT tu, FLOAT tv )
{
    FONT3DVERTEX v;   v.p = p;   v.n = n;   v.tu = tu;   v.tv = tv;
    return v;
}




//-----------------------------------------------------------------------------
// Name: CD3DFont()
// Desc: Font class constructor
//-----------------------------------------------------------------------------
CD3DFont::CD3DFont( const TCHAR* strFontName, const DWORD dwHeight, const DWORD dwFlags )
{
    _tcscpy( m_strFontName, strFontName );
    m_dwFontHeight         = dwHeight;
    m_dwFontFlags          = dwFlags;

    m_pd3dDevice           = NULL;
    m_pTexture             = NULL;
    m_pVB                  = NULL;

    m_dwSavedStateBlock    = 0L;
    m_dwDrawTextStateBlock = 0L;
}




//-----------------------------------------------------------------------------
// Name: ~CD3DFont()
// Desc: Font class destructor
//-----------------------------------------------------------------------------
CD3DFont::~CD3DFont()
{
    InvalidateDeviceObjects();
    DeleteDeviceObjects();
}




//-----------------------------------------------------------------------------
// Name: InitDeviceObjects()
// Desc: Initializes device-dependent objects, including the vertex buffer used
//       for rendering text and the texture map which stores the font image.
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InitDeviceObjects( LPDIRECT3DDEVICE7 pd3dDevice )
{
    // Keep a local copy of the device
    m_pd3dDevice = pd3dDevice;

    // Establish the font and texture size
    m_fTextScale  = 1.0f; // Draw fonts into texture without scaling

    // Large fonts need larger textures
    if( m_dwFontHeight > 40 )
        m_dwTexWidth = m_dwTexHeight = 1024;
    else if( m_dwFontHeight > 20 )
        m_dwTexWidth = m_dwTexHeight = 512;
    else
        m_dwTexWidth  = m_dwTexHeight = 256;

    // If requested texture is too big, use a smaller texture and smaller font,
    // and scale up when rendering.
    D3DDEVICEDESC7 d3dCaps;
    m_pd3dDevice->GetCaps( &d3dCaps );

    if( m_dwTexWidth > d3dCaps.dwMaxTextureWidth )
    {
        m_fTextScale = (FLOAT)d3dCaps.dwMaxTextureWidth / (FLOAT)m_dwTexWidth;
        m_dwTexWidth = m_dwTexHeight = d3dCaps.dwMaxTextureWidth;
    }

    // Create a new texture for the font
	m_pTexture=CreateTexture(m_pd3dDevice,&m_dwTexWidth,&m_dwTexHeight,0,TRUE,FALSE,FALSE);
    if( m_pTexture==NULL )
        return -1;

    // Create a DC and a bitmap for the font
    HDC     hDC;
	m_pTexture->GetDC(&hDC);



    // Create a font.  By specifying ANTIALIASED_QUALITY, we might get an
    // antialiased font, but this is not guaranteed.
    INT nHeight    = -MulDiv( m_dwFontHeight, 
        (INT)(GetDeviceCaps(hDC, LOGPIXELSY) * m_fTextScale), 72 );
    DWORD dwBold   = (m_dwFontFlags&D3DFONT_BOLD)   ? FW_BOLD : FW_NORMAL;
    DWORD dwItalic = (m_dwFontFlags&D3DFONT_ITALIC) ? TRUE    : FALSE;
    HFONT hFont    = CreateFont( nHeight, 0, 0, 0, dwBold, dwItalic,
                          FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS,
                          CLIP_DEFAULT_PRECIS, ANTIALIASED_QUALITY,
                          VARIABLE_PITCH, m_strFontName );
    if( NULL==hFont )
        return E_FAIL;

    SelectObject( hDC, hFont );

    // Set text properties
    SetTextColor( hDC, RGB(255,255,255) );
    SetBkColor(   hDC, RGB(0,0,0));
    SetTextAlign( hDC, TA_TOP );

    // Loop through all printable character and output them to the bitmap..
    // Meanwhile, keep track of the corresponding tex coords for each character.
    DWORD x = 0;
    DWORD y = 0;
    TCHAR str[2] = _T("x");
    SIZE size;

    for( unsigned char c=32; c<255; c++ )
    {
        str[0] = c;
        GetTextExtentPoint32( hDC, str, 1, &size );

        if( (DWORD)(x+size.cx+1) > m_dwTexWidth )
        {
            x  = 0;
            y += size.cy+1;
        }

        ExtTextOut( hDC, x+0, y+0, ETO_OPAQUE, NULL, str, 1, NULL );

        m_fTexCoords[c-32][0] = ((FLOAT)(x+0))/m_dwTexWidth;
        m_fTexCoords[c-32][1] = ((FLOAT)(y+0))/m_dwTexHeight;
        m_fTexCoords[c-32][2] = ((FLOAT)(x+0+size.cx))/m_dwTexWidth;
        m_fTexCoords[c-32][3] = ((FLOAT)(y+0+size.cy))/m_dwTexHeight;

        x += size.cx+1;
    }

	m_pTexture->ReleaseDC(hDC);
    DeleteObject( hFont );

	MakeTransparent(m_pTexture,FALSE);

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: RestoreDeviceObjects()
// Desc:
//-----------------------------------------------------------------------------
HRESULT CD3DFont::RestoreDeviceObjects()
{
	LPDIRECT3D7 lpD3D;
	D3DDEVICEDESC7 dd;
	m_pd3dDevice->GetCaps(&dd);
	m_pd3dDevice->GetDirect3D(&lpD3D);


    // Create vertex buffer for the letters
	m_pVB=CreateVertexBuffer(lpD3D,&dd,D3DFVF_FONT2DVERTEX,MAX_NUM_VERTICES*sizeof(FONT2DVERTEX),FALSE);
	lpD3D->Release();

	if (m_pVB==NULL)return -1;

    // Create the state blocks for rendering text
    for( UINT which=0; which<2; which++ )
    {
        m_pd3dDevice->BeginStateBlock();
        m_pd3dDevice->SetTexture( 0, m_pTexture );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHABLENDENABLE, TRUE );
	    m_pd3dDevice->SetRenderState( D3DRENDERSTATE_SRCBLEND,   D3DBLEND_SRCALPHA );
	    m_pd3dDevice->SetRenderState( D3DRENDERSTATE_DESTBLEND,  D3DBLEND_INVSRCALPHA );
		m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHATESTENABLE,  TRUE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAREF,         0x08 );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ALPHAFUNC,  D3DCMP_GREATEREQUAL );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FILLMODE,   D3DFILL_SOLID );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE,   D3DCULL_CCW );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_ZENABLE,          FALSE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_STENCILENABLE,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CLIPPING,         TRUE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_EDGEANTIALIAS,    FALSE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CLIPPLANEENABLE,  FALSE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_VERTEXBLEND,      FALSE );
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_FOGENABLE,        FALSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLOROP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAOP,   D3DTOP_MODULATE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_POINT );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_POINT );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXCOORDINDEX, 0 );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_COLOROP,   D3DTOP_DISABLE );
        m_pd3dDevice->SetTextureStageState( 1, D3DTSS_ALPHAOP,   D3DTOP_DISABLE );

        if( which==0 )
            m_pd3dDevice->EndStateBlock( &m_dwSavedStateBlock );
        else
            m_pd3dDevice->EndStateBlock( &m_dwDrawTextStateBlock );
    }

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: InvalidateDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::InvalidateDeviceObjects()
{
    SAFE_RELEASE( m_pVB );

    // Delete the state blocks
    if( m_pd3dDevice )
    {
        if( m_dwSavedStateBlock )
            m_pd3dDevice->DeleteStateBlock( m_dwSavedStateBlock );
        if( m_dwDrawTextStateBlock )
            m_pd3dDevice->DeleteStateBlock( m_dwDrawTextStateBlock );
    }

    m_dwSavedStateBlock    = 0L;
    m_dwDrawTextStateBlock = 0L;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DeleteDeviceObjects()
// Desc: Destroys all device-dependent objects
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DeleteDeviceObjects()
{
    SAFE_RELEASE( m_pTexture );
    m_pd3dDevice = NULL;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: GetTextExtent()
// Desc: Get the dimensions of a text string
//-----------------------------------------------------------------------------
HRESULT CD3DFont::GetTextExtent( const TCHAR* strText, SIZE* pSize )
{
    if( NULL==strText || NULL==pSize )
        return E_FAIL;

    FLOAT fRowWidth  = 0.0f;
    FLOAT fRowHeight = (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
    FLOAT fWidth     = 0.0f;
    FLOAT fHeight    = fRowHeight;

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            fRowWidth = 0.0f;
            fHeight  += fRowHeight;
        }
        if( c < _T(' ') )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT tx2 = m_fTexCoords[c-32][2];

        fRowWidth += (tx2-tx1)*m_dwTexWidth;

        if( fRowWidth > fWidth )
            fWidth = fRowWidth;
    }

    pSize->cx = (int)fWidth;
    pSize->cy = (int)fHeight;

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: DrawTextScaled()
// Desc: Draws scaled 2D text.  Note that x and y are in viewport coordinates
//       (ranging from -1 to +1).  fXScale and fYScale are the size fraction 
//       relative to the entire viewport.  For example, a fXScale of 0.25 is
//       1/8th of the screen width.  This allows you to output text at a fixed
//       fraction of the viewport, even if the screen or window size changes.
//-----------------------------------------------------------------------------
/*HRESULT CD3DFont::DrawTextScaled( FLOAT x, FLOAT y, FLOAT z,
                                  FLOAT fXScale, FLOAT fYScale, DWORD dwColor,
                                  TCHAR* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Set up renderstate
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
//    m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
  //  m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    }

    D3DVIEWPORT7 vp;
    m_pd3dDevice->GetViewport( &vp );
    FLOAT sx  = (x+1.0f)*vp.dwWidth/2;
    FLOAT sy  = (y-1.0f)*vp.dwHeight/2;
    FLOAT sz  = z;
    FLOAT rhw = 1.0f;
    FLOAT fStartX = sx;

    FLOAT fLineHeight = ( m_fTexCoords[0][3] - m_fTexCoords[0][1] ) * m_dwTexHeight;

    // Fill vertex buffer
    FONT2DVERTEX* pVertices;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( DDLOCK_DISCARDCONTENTS, (void**)&pVertices, NULL);

    while( *strText )
    {
        TCHAR c = *strText++;

        if( c == _T('\n') )
        {
            sx  = fStartX;
            sy += fYScale*vp.dwHeight;
        }
        if( c < _T(' ') )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1)*m_dwTexWidth;
        FLOAT h = (ty2-ty1)*m_dwTexHeight;

        w *= (fXScale*vp.dwHeight)/fLineHeight;
        h *= (fYScale*vp.dwHeight)/fLineHeight;

        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+0-0.5f,sy+h-0.5f,sz), dwColor, tx1, ty2 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+0-0.5f,sy+0-0.5f,sz), dwColor, tx1, ty1 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+w-0.5f,sy+h-0.5f,sz), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+w-0.5f,sy+0-0.5f,sz), dwColor, tx2, ty1 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+w-0.5f,sy+h-0.5f,sz), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+0-0.5f,sy+0-0.5f,sz), dwColor, tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitiveVB( D3DPT_TRIANGLELIST, m_pVB, 0, dwNumTriangles,0 );
            m_pVB->Lock( DDLOCK_DISCARDCONTENTS, (void**)&pVertices, NULL);
            dwNumTriangles = 0L;
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
		m_pd3dDevice->DrawPrimitiveVB( D3DPT_TRIANGLELIST, m_pVB, 0, dwNumTriangles,0 );

    // Restore the modified renderstates
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );

    return S_OK;
}*/




//-----------------------------------------------------------------------------
// Name: DrawText()
// Desc: Draws 2D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::DrawText( FLOAT sx, FLOAT sy, const DWORD dwColor,
						   const char* strText, const DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    FONT2DVERTEX* pVertices = NULL;
    DWORD         dwNumTriangles = 0;
    m_pVB->Lock( DDLOCK_DISCARDCONTENTS, (void**)&pVertices, NULL );

	if (pVertices==NULL)return E_FAIL;

    // Setup renderstate
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
//    m_pd3dDevice->SetVertexShader( D3DFVF_FONT2DVERTEX );
//    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT2DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
		m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    }

    FLOAT fStartX = sx;

    // Fill vertex buffer

    while( *strText )
    {
        unsigned char c = *strText++;

        if( c == '\n' )
        {
            sx = fStartX;
            sy += (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight;
        }
        if( c < ' ' )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) *  m_dwTexWidth / m_fTextScale;
        FLOAT h = (ty2-ty1) * m_dwTexHeight / m_fTextScale;

        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+0-0.5f,sy+h-0.5f,0.9f), dwColor, tx1, ty2 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+0-0.5f,sy+0-0.5f,0.9f), dwColor, tx1, ty1 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+w-0.5f,sy+h-0.5f,0.9f), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+w-0.5f,sy+0-0.5f,0.9f), dwColor, tx2, ty1 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+w-0.5f,sy+h-0.5f,0.9f), dwColor, tx2, ty2 );
        *pVertices++ = InitFont2DVertex( D3DVECTOR(sx+0-0.5f,sy+0-0.5f,0.9f), dwColor, tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitiveVB( D3DPT_TRIANGLELIST, m_pVB,0, dwNumTriangles*3 ,0);
            pVertices = NULL;
			m_pVB->Lock( DDLOCK_DISCARDCONTENTS, (void**)&pVertices, NULL );
            dwNumTriangles = 0L;
        }

        sx += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
           m_pd3dDevice->DrawPrimitiveVB( D3DPT_TRIANGLELIST, m_pVB,0, dwNumTriangles*3 ,0);

    // Restore the modified renderstates
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );

    return S_OK;
}




//-----------------------------------------------------------------------------
// Name: Render3DText()
// Desc: Renders 3D text
//-----------------------------------------------------------------------------
HRESULT CD3DFont::Render3DText( TCHAR* strText, DWORD dwFlags )
{
    if( m_pd3dDevice == NULL )
        return E_FAIL;

    // Setup renderstate
    m_pd3dDevice->CaptureStateBlock( m_dwSavedStateBlock );
    m_pd3dDevice->ApplyStateBlock( m_dwDrawTextStateBlock );
//    m_pd3dDevice->SetVertexShader( D3DFVF_FONT3DVERTEX );
//    m_pd3dDevice->SetStreamSource( 0, m_pVB, sizeof(FONT3DVERTEX) );

    // Set filter states
    if( dwFlags & D3DFONT_FILTERED )
    {
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MINFILTER, D3DTFN_LINEAR );
        m_pd3dDevice->SetTextureStageState( 0, D3DTSS_MAGFILTER, D3DTFG_LINEAR );
    }

    // Position for each text element
    FLOAT x = 0.0f;
    FLOAT y = 0.0f;

    // Center the text block at the origin
    if( dwFlags & D3DFONT_CENTERED )
    {
        SIZE sz;
        GetTextExtent( strText, &sz );
        x = -((FLOAT)sz.cx)*0.05f;
        y = -((FLOAT)sz.cy)*0.05f;
    }

    // Turn off culling for two-sided text
    if( dwFlags & D3DFONT_TWOSIDED )
        m_pd3dDevice->SetRenderState( D3DRENDERSTATE_CULLMODE, D3DCULL_NONE );

    FLOAT fStartX = x;
    TCHAR c;

    // Fill vertex buffer
    FONT3DVERTEX* pVertices;
    DWORD         dwVertex       = 0L;
    DWORD         dwNumTriangles = 0L;
    m_pVB->Lock( DDLOCK_DISCARDCONTENTS, (void**)&pVertices, NULL);

    while( c = *strText++ )
    {
        if( c == '\n' )
        {
            x = fStartX;
            y -= (m_fTexCoords[0][3]-m_fTexCoords[0][1])*m_dwTexHeight/10.0f;
        }
        if( c < 32 )
            continue;

        FLOAT tx1 = m_fTexCoords[c-32][0];
        FLOAT ty1 = m_fTexCoords[c-32][1];
        FLOAT tx2 = m_fTexCoords[c-32][2];
        FLOAT ty2 = m_fTexCoords[c-32][3];

        FLOAT w = (tx2-tx1) * m_dwTexWidth  / ( 10.0f * m_fTextScale );
        FLOAT h = (ty2-ty1) * m_dwTexHeight / ( 10.0f * m_fTextScale );

        *pVertices++ = InitFont3DVertex( D3DVECTOR(x+0,y+0,0), D3DVECTOR(0,0,-1), tx1, ty2 );
        *pVertices++ = InitFont3DVertex( D3DVECTOR(x+0,y+h,0), D3DVECTOR(0,0,-1), tx1, ty1 );
        *pVertices++ = InitFont3DVertex( D3DVECTOR(x+w,y+0,0), D3DVECTOR(0,0,-1), tx2, ty2 );
        *pVertices++ = InitFont3DVertex( D3DVECTOR(x+w,y+h,0), D3DVECTOR(0,0,-1), tx2, ty1 );
        *pVertices++ = InitFont3DVertex( D3DVECTOR(x+w,y+0,0), D3DVECTOR(0,0,-1), tx2, ty2 );
        *pVertices++ = InitFont3DVertex( D3DVECTOR(x+0,y+h,0), D3DVECTOR(0,0,-1), tx1, ty1 );
        dwNumTriangles += 2;

        if( dwNumTriangles*3 > (MAX_NUM_VERTICES-6) )
        {
            // Unlock, render, and relock the vertex buffer
            m_pVB->Unlock();
            m_pd3dDevice->DrawPrimitiveVB( D3DPT_TRIANGLELIST, m_pVB, 0, dwNumTriangles ,0);
		    m_pVB->Lock( DDLOCK_DISCARDCONTENTS, (void**)&pVertices, NULL);
            dwNumTriangles = 0L;
        }

        x += w;
    }

    // Unlock and render the vertex buffer
    m_pVB->Unlock();
    if( dwNumTriangles > 0 )
		m_pd3dDevice->DrawPrimitiveVB( D3DPT_TRIANGLELIST, m_pVB, 0, dwNumTriangles ,0);

    // Restore the modified renderstates
    m_pd3dDevice->ApplyStateBlock( m_dwSavedStateBlock );

    return S_OK;
}




