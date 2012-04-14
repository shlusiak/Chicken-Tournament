#include "stdafx.h"
#include "..\\resources/resources.h"
#include "utils.h"


//#define MarkMipmap
int TextureBPP=16;


void SetTextureBPP(const int to)
{
	TextureBPP=to;
#ifdef _DEBUG
	CHAR c[100];
	sprintf(&c[0],"Texture BPP set to %d",TextureBPP);
	DebugOut(&c[0]);
#endif
}






struct TEXTURESEARCHINFO
{
    DWORD dwDesiredBPP;   // Input for texture format search
    BOOL  bUseAlpha;
    BOOL  bUsePalette;
	BOOL  bUseCompressed;
    BOOL  bFoundGoodFormat;

    DDPIXELFORMAT* pddpf; // Output of texture format search
};

HRESULT CALLBACK TextureSearchCallback( DDPIXELFORMAT* pddpf,
                                               VOID* param )
{
    if( NULL==pddpf || NULL==param )
        return DDENUMRET_OK;

    TEXTURESEARCHINFO* ptsi = (TEXTURESEARCHINFO*)param;

    // Skip any funky modes
    if( pddpf->dwFlags & (DDPF_LUMINANCE|DDPF_BUMPLUMINANCE|DDPF_BUMPDUDV) )
        return DDENUMRET_OK;

    // Check for palettized formats
    if( ptsi->bUsePalette )
    {
        if( !( pddpf->dwFlags & DDPF_PALETTEINDEXED8 ) )
            return DDENUMRET_OK;

        // Accept the first 8-bit palettized format we get
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }

    // Skip any FourCC formats
    if( pddpf->dwFourCC != 0 )
	{
		if (ptsi->bUseCompressed==FALSE)return DDENUMRET_OK;

		DebugOut("FOURCC Compressed PixelFormat for Texture found.");
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
	}
	if (ptsi->bUseCompressed)return DDENUMRET_OK;

    // Else, skip any paletized formats (all modes under 16bpp)
    if( pddpf->dwRGBBitCount < 16 )
        return DDENUMRET_OK;


    // Make sure current alpha format agrees with requested format type
    if( (ptsi->bUseAlpha==TRUE) && !(pddpf->dwFlags&DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;
    if( (ptsi->bUseAlpha==FALSE) && (pddpf->dwFlags&DDPF_ALPHAPIXELS) )
        return DDENUMRET_OK;

    // Check if we found a good match
    if( pddpf->dwRGBBitCount == ptsi->dwDesiredBPP )
    {
        memcpy( ptsi->pddpf, pddpf, sizeof(DDPIXELFORMAT) );
        ptsi->bFoundGoodFormat = TRUE;
        return DDENUMRET_CANCEL;
    }

    return DDENUMRET_OK;
}

LPDIRECTDRAWSURFACE7 CreateTexture(const LPDIRECT3DDEVICE7 lpDevice,PDWORD w,PDWORD h,const DWORD mipmaps,const BOOLEAN withalpha,const BOOLEAN compressed,const BOOLEAN RenderTarget)
{
    LPDIRECTDRAWSURFACE7 pddsTexture;
    HRESULT hr;

	// Get the device caps so we can check if the device has any constraints
	// when using textures (Voodoo cards for example, have a limit of 256x256 texture size)
	D3DDEVICEDESC7 ddDesc;
	if( FAILED( lpDevice->GetCaps( &ddDesc ) ) )
		return NULL;

	// Get the bitmap structure (to extract width, height, and bpp)
	DWORD dwWidth  = *w;
	DWORD dwHeight = *h;

	// Setup the new surface desc for the texture. Note how we are using the
	// texture manage attribute, so Direct3D does alot of dirty work for us
	DDSURFACEDESC2 ddsd;
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC2) );
	ddsd.dwSize          = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags         = DDSD_CAPS|DDSD_HEIGHT|DDSD_WIDTH|
	DDSD_PIXELFORMAT|((mipmaps!=0)?DDSD_MIPMAPCOUNT:0);
	ddsd.ddsCaps.dwCaps  = DDSCAPS_TEXTURE|((mipmaps!=0)?(DDSCAPS_MIPMAP|DDSCAPS_COMPLEX):0)|(RenderTarget?DDSCAPS_3DDEVICE|DDSCAPS_COMPLEX:0);
	ddsd.dwMipMapCount	 = mipmaps;
	ddsd.dwWidth         = dwWidth;
	ddsd.dwHeight        = dwHeight;

	if (!RenderTarget)
	{
		// Turn on texture management for hardware devices
		if( ddDesc.deviceGUID == IID_IDirect3DHALDevice )
	        ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
	    else if( ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice )
			ddsd.ddsCaps.dwCaps2 = DDSCAPS2_TEXTUREMANAGE;
		else
	        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
	}


    // Adjust width and height, if the driver requires it
    if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_POW2 )
    {
		for( ddsd.dwWidth=1;  dwWidth>ddsd.dwWidth;   ddsd.dwWidth<<=1 );
		for( ddsd.dwHeight=1; dwHeight>ddsd.dwHeight; ddsd.dwHeight<<=1 );
	}
	if( ddDesc.dpcTriCaps.dwTextureCaps & D3DPTEXTURECAPS_SQUAREONLY )
	{
        if( ddsd.dwWidth > ddsd.dwHeight ) ddsd.dwHeight = ddsd.dwWidth;
		else                               ddsd.dwWidth  = ddsd.dwHeight;
	}

	while (ddsd.dwWidth>ddDesc.dwMaxTextureWidth) ddsd.dwWidth>>=1;
	while (ddsd.dwHeight>ddDesc.dwMaxTextureHeight) ddsd.dwHeight>>=1;

	// Enumerate the texture formats, and find the closest device-supported
	// texture pixel format. The TextureSearchCallback function for this
	// tutorial is simply looking for a 16-bit texture. Real apps may be
	// interested in other formats, for alpha textures, bumpmaps, etc..
	TEXTURESEARCHINFO ti;
	ti.bFoundGoodFormat=FALSE;
	ti.bUseAlpha=withalpha;
	ti.bUsePalette=FALSE;
	ti.bUseCompressed=compressed;
	ti.dwDesiredBPP=TextureBPP;
	ti.pddpf=&ddsd.ddpfPixelFormat;
	lpDevice->EnumTextureFormats( TextureSearchCallback, &ti);
	if( ti.bFoundGoodFormat==FALSE )
		return NULL;

	// Get the device's render target, so we can then use the render target to
	// get a ptr to a DDraw object. We need the DirectDraw interface for
	// creating surfaces.
	LPDIRECTDRAWSURFACE7 pddsRender;
	LPDIRECTDRAW7        pDD;
	lpDevice->GetRenderTarget( &pddsRender );
	pddsRender->GetDDInterface( (VOID**)&pDD );
	pddsRender->Release();

	// Create a new surface for the texture
	if( FAILED( hr = pDD->CreateSurface( &ddsd, &pddsTexture, NULL ) ) )
	{
		pDD->Release();
		return NULL;
	}

	// Done with DDraw
	pDD->Release();

	*w=ddsd.dwWidth;
	*h=ddsd.dwHeight;

	return pddsTexture;
}

const BOOLEAN CopyBitsToSurface(const LPDIRECTDRAWSURFACE7 lpSurface,const DWORD w,const DWORD h,const DWORD bw,const DWORD bh,const HBITMAP hbm,const BYTE mipmaps,const BOOLEAN halftone)
{
    HDC hdcBitmap = CreateCompatibleDC( NULL );
    if( NULL == hdcBitmap )return FALSE;
    SelectObject( hdcBitmap, hbm );

    // Get a DC for the surface
	LPDIRECTDRAWSURFACE7 s=lpSurface;
    HDC hdcTexture;
	for (int i=0;(i<mipmaps)||(i==0);i++)
	{
		if( SUCCEEDED( s->GetDC( &hdcTexture ) ) )
		{
			SetStretchBltMode(hdcTexture,halftone?HALFTONE:COLORONCOLOR);
			StretchBlt(hdcTexture,0,0,w>>i,h>>i,hdcBitmap,0,0,bw,bh,SRCCOPY);

#ifdef MarkMipmap
			CHAR c[10];
			sprintf(&c[0],"%d",i);
			TextOut(hdcTexture,(bm.bmWidth>>i)/2,(bm.bmHeight>>i)/2,&c[0],strlen(&c[0]));
#endif
	        s->ReleaseDC( hdcTexture );
		}else{
			DebugOut("Konnte DC von Surface nicht erhalten...");
		}

        DDSCAPS2 ddsCaps;
        ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
        ddsCaps.dwCaps2 = 0;
        ddsCaps.dwCaps3 = 0;
        ddsCaps.dwCaps4 = 0;
        if( SUCCEEDED( s->GetAttachedSurface( &ddsCaps, &s ) ) )
            s->Release();
    }
    DeleteDC( hdcBitmap );

    return TRUE;
}

const BOOLEAN CopySurfaceToSurface(const LPDIRECTDRAWSURFACE7 lpSurface,const LPDIRECTDRAWSURFACE7 lpSurface2,const BYTE mipmaps)
{
	LPDIRECTDRAWSURFACE7 s=lpSurface;
	for (int i=0;(i<mipmaps)||(i==0);i++)
	{
        if( FAILED( s->Blt( NULL, lpSurface2, NULL, /*DDBLT_WAIT*/0, NULL ) ) )
		{
			DebugOut("Konnte nicht auf Texture Blitten!!!");
			return FALSE;
		}


        DDSCAPS2 ddsCaps;
        ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
        ddsCaps.dwCaps2 = 0;
        ddsCaps.dwCaps3 = 0;
        ddsCaps.dwCaps4 = 0;
        if( SUCCEEDED( s->GetAttachedSurface( &ddsCaps, &s ) ) )
            s->Release();
    }
    return TRUE;
}

LPDIRECTDRAWSURFACE7 CreateSurfaceFromBitmap(const LPDIRECTDRAW7 lpDD,const HBITMAP bm)
{
	BITMAP bi;
	GetObject(bm,sizeof(bi),&bi);
	LPDIRECTDRAWSURFACE7 lpSurface;
	lpSurface=CreateSurface(lpDD,bi.bmWidth,bi.bmHeight);
	if (lpSurface==NULL)return NULL;
	if (!CopyBitsToSurface(lpSurface,bi.bmWidth,bi.bmHeight,bi.bmWidth,bi.bmHeight,bm,0,TRUE))return NULL;
	return lpSurface;
}

LPDIRECTDRAWSURFACE7 CreateTextureFromBitmap(const LPDIRECT3DDEVICE7 pd3dDevice,const LPDIRECTDRAW7 lpDD,LPDIRECTDRAWSURFACE7 surface,
	const HBITMAP hbm,const BYTE mipmaps,const BOOLEAN halftone,const BOOLEAN withalpha,const BOOLEAN compressed)
{
	if (hbm==0)return NULL;
	LPDIRECTDRAWSURFACE7 lpTexture;

	BITMAP bi;
	GetObject(hbm,sizeof(bi),&bi);
	DWORD w=bi.bmWidth,h=bi.bmHeight;
	lpTexture=CreateTexture(pd3dDevice,&w,&h,mipmaps,withalpha,compressed,FALSE);

	if (lpTexture==NULL)return NULL;

	if (!compressed)
	{
		if (CopyBitsToSurface(lpTexture,w,h,bi.bmWidth,bi.bmHeight,hbm,mipmaps,halftone)==FALSE)
		{
			lpTexture->Release();
			return NULL;
		}
	}else{
		LPDIRECTDRAWSURFACE7 lpSurface;
		lpSurface=CreateSurfaceFromBitmap(lpDD,hbm);
		if (lpSurface==NULL)
		{
			lpTexture->Release();
			return NULL;
		}
		CopySurfaceToSurface(lpTexture,lpSurface,mipmaps);
		lpSurface->Release();
	}
	return lpTexture;
}


LPDIRECTDRAWSURFACE7 CreateSurfaceFromResource(const LPDIRECTDRAW7 lpDD,const PCHAR file_name,RECT* dims)
{
	HDC hdc;
	HBITMAP bit;
	LPDIRECTDRAWSURFACE7 surf;

	// load the interface bitmap

	bit=LoadPicture(file_name);
	if (NULL==bit) 
	{
		// failed to load, return failure to caller
		return NULL;
	}

	// get bitmap dimensions

	BITMAP bitmap;
    GetObject( bit, sizeof(BITMAP), &bitmap );
	int surf_width=bitmap.bmWidth;
	int surf_height=bitmap.bmHeight;

	// create surface

	HRESULT ddrval;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT ;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN|DDSCAPS_SYSTEMMEMORY; 
	ddsd.dwWidth=surf_width;
	ddsd.dwHeight=surf_height;

	// attempt to create surface

	ddrval=lpDD->CreateSurface(&ddsd,&surf,NULL);

	// created ok?

	if (ddrval!=DD_OK) {

		// no, release the bitmap and return failure to caller

		DeleteObject(bit);
		return NULL;
	} else {

		// yes, get a DC for the surface

		surf->GetDC(&hdc);

		// generate a compatible DC

		HDC bit_dc=CreateCompatibleDC(hdc);

		// blit the interface to the surface

		SelectObject(bit_dc,bit);
		SetStretchBltMode(hdc,COLORONCOLOR);
		StretchBlt(hdc,0,0,surf_width,surf_height,bit_dc,0,0,bitmap.bmWidth,bitmap.bmHeight,SRCCOPY);
//		BitBlt(hdc,0,0,surf_width,surf_height,bit_dc,0,0,SRCCOPY);

		// release the DCs

		surf->ReleaseDC(hdc);
		DeleteDC(bit_dc);

		// save the dimensions if rectangle pointer provided

		if (dims) {
			dims->left=0;
			dims->top=0;
			dims->right=surf_width;
			dims->bottom=surf_height;
		}
	}

	// clear bitmap 

	DeleteObject(bit);

	// return pointer to caller

	return surf;
}

LPDIRECTDRAWSURFACE7 CreateSurface(const LPDIRECTDRAW7 lpDD,const int width,const int height)
{
	LPDIRECTDRAWSURFACE7 surf;

	HRESULT ddrval;
	DDSURFACEDESC2 ddsd;
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize = sizeof(DDSURFACEDESC2);
	ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT ;
	ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN; 
	ddsd.dwWidth=width;
	ddsd.dwHeight=height;

	ddrval=lpDD->CreateSurface(&ddsd,&surf,NULL);

	if (ddrval!=DD_OK) return NULL;

	return surf;
}

LPDIRECTDRAWSURFACE7 CreateTextureFromResource(const LPDIRECT3DDEVICE7 dev,const LPDIRECTDRAW7 lpDD,const LPDIRECTDRAWSURFACE7 surface,const PCHAR ident,const BYTE mipmaps,const BOOLEAN halftone,const BOOLEAN withalpha,const BOOLEAN compressed)
{
	HBITMAP b=0;
	b=LoadPicture(ident);
	if (b==0)
	{
		// Versuche DDS-Datei zu laden
/*		DDSURFACEDESC2 ddsd;
		BYTE mips;

		LPDIRECTDRAWSURFACE7 lpSurface=ReadDDSTexture(ident,&ddsd,&mips);
		if (lpSurface==NULL)return 0;


		lpSurface->Release();*/
		return 0;
	}
	LPDIRECTDRAWSURFACE7 s=NULL;

	s=CreateTextureFromBitmap(dev,lpDD,surface,b,mipmaps,halftone,withalpha,compressed);

	if (s==NULL)
	{
		DebugOut("Error creating Compressed Texture!!!");
		s=CreateTextureFromBitmap(dev,lpDD,surface,b,mipmaps,halftone,withalpha,FALSE);
	}


	DeleteObject(b);
	return s;
}

void MakeTransparent(LPDIRECTDRAWSURFACE7 lpSurface,const BOOLEAN white)
{
	DDSURFACEDESC2 ddsd;
	DWORD dwAlphaMask,dwRGBMask,dwColorkey;
	DWORD x,y;
	PWORD p16;
	PDWORD p32;
	DWORD texturenum=0;
	DWORD dwRGBBitCount;

nochmal:
//	For textures with real alpha (not palettized), set transparent bits
	ZeroMemory(&ddsd,sizeof(ddsd));
	ddsd.dwSize=sizeof(ddsd);
	while (lpSurface->Lock(NULL, &ddsd, 0, 0) == DDERR_WASSTILLDRAWING)  ;

	if (texturenum==0)
	{
		dwAlphaMask = ddsd.ddpfPixelFormat.dwRGBAlphaBitMask;
		dwRGBMask   = ddsd.ddpfPixelFormat.dwRBitMask | ddsd.ddpfPixelFormat.dwGBitMask |
							ddsd.ddpfPixelFormat.dwBBitMask;
		dwColorkey  = 0x00000000;           // Colorkey on black
		if (white) 
			dwColorkey = dwRGBMask;     // Colorkey on white
		dwRGBBitCount=ddsd.ddpfPixelFormat.dwRGBBitCount;
	}

      // Add an opaque alpha value to each non-colorkeyed pixel
	if (dwRGBBitCount == 16)
	{
		for (y = 0;y<ddsd.dwHeight;y++)
		{
			p16 =  LPWORD(DWORD(ddsd.lpSurface) + y * DWORD(ddsd.lPitch));
			for (x = 0;x<ddsd.dwWidth;x++)
			{
				*p16 = *p16 & WORD(dwRGBMask);
				if (*p16!=dwColorkey) *p16 = *p16 | (WORD)(dwAlphaMask);
				p16++;
			}
		}
	}
	if (dwRGBBitCount == 32)
	{
        for (y = 0;y<ddsd.dwHeight;y++) 
		{
			p32=PDWORD(DWORD(ddsd.lpSurface)+y*DWORD(ddsd.lPitch));
			for (x=0;x<ddsd.dwWidth;x++)
			{
				*p32=*p32&dwRGBMask;
				if (*p32!=dwColorkey)*p32=*p32|DWORD(dwAlphaMask);
				p32++;
			}
		}
	}
	lpSurface->Unlock(NULL);

	if (ddsd.dwMipMapCount>1)
	{
		texturenum++;

        DDSCAPS2 ddsCaps;
        ddsCaps.dwCaps  = DDSCAPS_TEXTURE | DDSCAPS_MIPMAP;
        ddsCaps.dwCaps2 = 0;
        ddsCaps.dwCaps3 = 0;
        ddsCaps.dwCaps4 = 0;
        if (SUCCEEDED(lpSurface->GetAttachedSurface( &ddsCaps, &lpSurface )))
			lpSurface->Release();
		goto nochmal;
    }
}



LPDIRECTDRAWSURFACE7 ReadDDSTexture(const LPDIRECTDRAW7 lpDD,const CHAR* strTextureName,DDSURFACEDESC2* pddsdNew,BYTE *mipmaps)
{
    LPDIRECTDRAWSURFACE7 pdds = NULL;
    DDSURFACEDESC2       ddsd;
    DWORD   dwMagic;
    HRESULT hr;

    // Open the compressed texture file
    FILE* file = fopen( strTextureName, "rb" );
    if( file == NULL )return NULL;

    // Read magic number
    fread( &dwMagic, sizeof(DWORD), 1, file );
    if( dwMagic != MAKEFOURCC('D','D','S',' ') )
    {
        fclose( file );
        return NULL;
    }

    // Read the surface description
    fread( &ddsd, sizeof(DDSURFACEDESC2), 1, file );

    // Load DDS into system memory
    ddsd.ddsCaps.dwCaps  |= DDSCAPS_SYSTEMMEMORY;

    // Handle special case for hardware that doesn't support mipmapping
/*    if( !m_bSupportsMipmaps )
    {
        ddsd.dwMipMapCount   = 0;
        ddsd.dwFlags        &= ~DDSD_MIPMAPCOUNT;
        ddsd.ddsCaps.dwCaps &= ~( DDSCAPS_MIPMAP | DDSCAPS_COMPLEX );
    }*/

    // Does texture have mipmaps?
    *mipmaps = ( ddsd.dwMipMapCount > 0 ) ? TRUE : FALSE;

    // Clear unwanted flags
    ddsd.dwFlags &= (~DDSD_PITCH);
    ddsd.dwFlags &= (~DDSD_LINEARSIZE);

    // Store the return copy of this surfacedesc
    (*pddsdNew) = ddsd;

    // Create a new surface based on the surface description
    if( FAILED( hr = lpDD->CreateSurface( &ddsd, &pdds, NULL ) ) )
    {
        fclose( file );
        return NULL;
    }

	LPDIRECTDRAWSURFACE7 ppddsNew=pdds;
    // Store the return copy of the compressed surface
    ppddsNew = pdds;
    ppddsNew->AddRef();

    while( TRUE )
    {
        if( FAILED( hr = pdds->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL )))
        {
            fclose( file );
            return NULL;
        }

        if( ddsd.dwFlags & DDSD_LINEARSIZE )
        {
            fread( ddsd.lpSurface, ddsd.dwLinearSize, 1, file );
        }
        else
        {
            BYTE* pDest = (BYTE*)ddsd.lpSurface;
            DWORD dwBytesPerRow = ddsd.dwWidth * ddsd.ddpfPixelFormat.dwRGBBitCount / 8;
            
            for( DWORD yp = 0; yp < ddsd.dwHeight; yp++ )
            {
                fread( pDest, dwBytesPerRow, 1, file );
                pDest += ddsd.lPitch;
            }
        }

        pdds->Unlock( NULL );

/*        if( !m_bSupportsMipmaps )
        {
            // For mipless hardware, don't copy mipmaps
            pdds->Release();
            fclose( file );
            return ppddsNew;
        }*/

        // Get the next surface.
        LPDIRECTDRAWSURFACE7 pddsNext;
        DDSCAPS2 ddsCaps = { DDSCAPS_TEXTURE|DDSCAPS_MIPMAP|DDSCAPS_COMPLEX, 0, 0, 0 };
        
        if( FAILED( hr = pdds->GetAttachedSurface( &ddsCaps, &pddsNext ) ) )
        {
            // Failure means were done with the mipmaps
            pdds->Release();
            fclose( file );
            return ppddsNew;
        }

        pdds->Release();
        pdds = pddsNext;
    }
	return ppddsNew;
}

/*inline BOOLEAN CopySurfaceToSurface(const LPDIRECTDRAWSURFACE7 lpSurface,const LPDIRECTDRAWSURFACE7 lpSurface2)
{
/*	DDSURFACEDESC2 ddsd2;
	ZeroMemory(&ddsd2,sizeof(ddsd2));
	ddsd2.dwSize=sizeof(ddsd2);
	lpSurface2->GetSurfaceDesc(&ddsd2);

	return CopySurfaceToSurface(lpSurface,lpSurface2,0);
}*/

LPDIRECTDRAWSURFACE7 CreateCubeMap(const LPDIRECTDRAW7 lpDD,const LPDIRECT3DDEVICE7 lpDevice,const int width,const int height)
{
	LPDIRECTDRAWSURFACE7 lpCubeMap=NULL;
	HRESULT h;

    DDSURFACEDESC2 ddsd;
    ZeroMemory( &ddsd, sizeof(ddsd) );
    ddsd.dwSize         = sizeof(ddsd);
    ddsd.dwFlags        = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_PIXELFORMAT;
//    ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE|DDSCAPS_MIPMAP|DDSCAPS_3DDEVICE|DDSCAPS_COMPLEX;
    ddsd.dwWidth        = width;
    ddsd.dwHeight       = height;

    // Enumerate a good texture format. Search for a 16-bit format first
	TEXTURESEARCHINFO ti;
	ti.bFoundGoodFormat=FALSE;
	ti.bUseAlpha=FALSE;
	ti.bUsePalette=FALSE;
	ti.bUseCompressed=FALSE;
	ti.dwDesiredBPP=32;
	ti.pddpf=&ddsd.ddpfPixelFormat;
	lpDevice->EnumTextureFormats( TextureSearchCallback, &ti);
	if( ti.bFoundGoodFormat==FALSE )
	{
		ti.dwDesiredBPP=32;
		lpDevice->EnumTextureFormats( TextureSearchCallback, &ti);

		if (ti.bFoundGoodFormat==FALSE)return NULL;
	}

    ddsd.ddsCaps.dwCaps  = DDSCAPS_COMPLEX|DDSCAPS_3DDEVICE|DDSCAPS_TEXTURE;
    ddsd.ddsCaps.dwCaps2 = DDSCAPS2_CUBEMAP|DDSCAPS2_CUBEMAP_ALLFACES;
    ddsd.ddsCaps.dwCaps3 = 0;
    ddsd.ddsCaps.dwCaps4 = 0;

    // Force system memory for software devices
/*	D3DDEVICEDESC7 ddDesc;
	if( FAILED( lpDevice->GetCaps( &ddDesc ) ) )
		return NULL;

	// Turn on texture management for hardware devices
	if( ddDesc.deviceGUID == IID_IDirect3DHALDevice )
        ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
    else if( ddDesc.deviceGUID == IID_IDirect3DTnLHalDevice )
		ddsd.ddsCaps.dwCaps2 |= DDSCAPS2_TEXTUREMANAGE;
	else
        ddsd.ddsCaps.dwCaps |= DDSCAPS_SYSTEMMEMORY;
*/
    // Create the envmap surface and texture
    if( FAILED( h=lpDD->CreateSurface( &ddsd, &lpCubeMap, NULL ) ) )
        return NULL;

	return lpCubeMap;
}