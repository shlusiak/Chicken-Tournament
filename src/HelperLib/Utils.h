#ifndef _UTILS_INCLUDED_
#define _UTILS_INCLUDED_



// Texturen.cpp
// ------------

// Erstellt eine Surface aus einer Resource
LPDIRECTDRAWSURFACE7 CreateSurfaceFromResource(const PCHAR file_name,RECT* dims=NULL);

// Erstelt eine Surface aus einem Bitmaps
LPDIRECTDRAWSURFACE7 CreateSurfaceFromBitmap(const LPDIRECTDRAW7 lpDD,const HBITMAP bm);

// Erstellt eine leere Surface
LPDIRECTDRAWSURFACE7 CreateSurface(const LPDIRECTDRAW7 lpDD,const int width,const int height);

// Erstellt eine Textur aus einer Resource
LPDIRECTDRAWSURFACE7 CreateTextureFromResource(const LPDIRECT3DDEVICE7 dev,const LPDIRECTDRAW7 lpDD,const LPDIRECTDRAWSURFACE7 surface,const PCHAR ident,const BYTE mipmaps=0,const BOOLEAN halftone=TRUE,const BOOLEAN withalpha=FALSE,const BOOLEAN compressed=FALSE);

// Erstellt eine Textur aus einem Bitmap
LPDIRECTDRAWSURFACE7 CreateTextureFromBitmap(const LPDIRECT3DDEVICE7 pd3dDevice,const LPDIRECTDRAW7 lpDD,const LPDIRECTDRAWSURFACE7 surface,const HBITMAP hbm,const BYTE mipmaps,const BOOLEAN halftone,const BOOLEAN withalpha,const BOOLEAN compressed);

// Erstellt eine leere Textur
LPDIRECTDRAWSURFACE7 CreateTexture(const LPDIRECT3DDEVICE7 lpDevice,PDWORD w,PDWORD h,const DWORD mipmaps,const BOOLEAN withalpha,const BOOLEAN compressed,const BOOLEAN RenderTarget);

// Macht eine Textur mit Alpha-Kanal transparent
void MakeTransparent(const LPDIRECTDRAWSURFACE7 lpSurface,const BOOLEAN white=TRUE);

// Kopiert ein Bitmap in eine Surface
const BOOLEAN CopyBitsToSurface(const LPDIRECTDRAWSURFACE7 lpSurface,const DWORD w,const DWORD h,const DWORD bw,const DWORD bh,const HBITMAP hbm,const BYTE mipmaps,const BOOLEAN halftone);

// Kopiert den Inhalt einer Surface in eine andere Surface
const BOOLEAN CopySurfaceToSurface(const LPDIRECTDRAWSURFACE7 lpSurface,const LPDIRECTDRAWSURFACE7 lpSurface2,const BYTE mipmaps);

// Liest eine DDS-Datei in eine Surface ein
LPDIRECTDRAWSURFACE7 ReadDDSTexture(const LPDIRECTDRAW7 lpDD, const CHAR* strTextureName,DDSURFACEDESC2* pddsdNew,BYTE *mipmaps);

// Erstellt eine CUBE Map
LPDIRECTDRAWSURFACE7 CreateCubeMap(const LPDIRECTDRAW7 lpDD,const LPDIRECT3DDEVICE7 lpDevice,const int width,const int height);

// Setzt die Farbtiefe für zu erstellende Texturen
void SetTextureBPP(const int to);

// ---------





// Utils.cpp
// ---------


// Erzeugt die Vertices und Indices einer Kugel (mit Textur)
const HRESULT CreateSphere( D3DVERTEX** ppVertices,DWORD* pdwNumVertices,WORD** ppIndices, DWORD* pdwNumIndices,const FLOAT fRadius, const DWORD dwNumRings, const float uxmul=1.0f,const float uymul=1.0f);

// Verwandelt ein 3D Vertex in ein 2D Vertex
void TransformVertex(const LPDIRECT3DDEVICE7 lpDevice,D3DTLVERTEX* pvVertex);

// Verwandelt einen 3D Vector in einen 2D Vector
const float TransformVector(const LPDIRECT3DDEVICE7 lpDevice,D3DVECTOR& pvVector);

// Zufallszahl zwischen 0.0f und 1.0f
inline const float randf() {return (float(rand())/float(RAND_MAX));}

// Erstellt 4 2-Dimensionale Vertices für ein Billboard (mit Textur)
void MakeBillboardVertices(D3DTLVERTEX *v,const int x,const int y,const int w,const int h,const DWORD color=D3DRGB(1,1,1));

// Erstellt einen Vertexbuffer
LPDIRECT3DVERTEXBUFFER7 CreateVertexBuffer(const LPDIRECT3D7 lpD3D,const D3DDEVICEDESC7 *d3ddesc,const DWORD type,const int num,const BOOLEAN videomemory);

// Transformiert ein RECT von Clientkoordinaten in Screenkoordinaten
inline void ClientToScreen(const HWND hWnd,const LPRECT r)
{
	LPPOINT p=(LPPOINT)&r->left;
	ClientToScreen(hWnd,p);
	p=LPPOINT(&r->right);
	ClientToScreen(hWnd,p);
}


// Gibt eine Farbe zurück
inline const D3DCOLORVALUE Color(const float r,const float g,const float b,const float a=1.0f)
{	D3DCOLORVALUE c; c.r=r; c.g=g; c.b=b; c.a=a; return c; }


// Gibt eine formatierte Fehlermeldung zurück
void ConvertLastErrorToString(PCHAR dest,const int maxlen);


// Macht einen 3D Vektor 2D, indem die Y-Koordinate gelöscht wird
inline D3DVECTOR & MakePlanar(D3DVECTOR &v) { v.y=0.0f; return v; }

inline void Line(const HDC dc,const int x1,const int y1,const int x2,const int y2)
{ MoveToEx(dc,x1,y1,NULL); LineTo(dc,x2,y2); LineTo(dc,x1,y1);}

template<class T> inline T abs(const T x) {return((x)>=0?(x):-(x));}
template<class T> inline const float sqr(const T t) {return (float(t)*float(t)); }

inline const long FloatToLong(const float x) {return (*PDWORD(&x));}
inline const float LongToFloat(const long x) {return (*PFLOAT(&x));}

#ifdef _D3D_H_
inline const BOOLEAN operator != (const D3DVECTOR &v1,const D3DVECTOR &v2)  {	return !(v1==v2); }
#endif

inline const float sqrtaf(const float x) { return (x<0.0f)?(-sqrtf(-x)):sqrtf(x); }


inline void SetVector(D3DLVERTEX& vertex,const D3DVECTOR &vector)
{
	vertex.x=vector.x;
	vertex.y=vector.y;
	vertex.z=vector.z;
}



class CZeitMesser
{
private:
	const PCHAR text;
	const DWORD zeit;
public:

#ifdef _DEBUG
	CZeitMesser(const PCHAR format):text(format),zeit(GetTickCount()) {}
	~CZeitMesser() { Dump(); }
	void Dump() { CHAR c[200]; sprintf(&c[0],text,GetTickCount()-zeit); DebugOut(&c[0]); }
#else
	CZeitMesser(const PCHAR format):text(NULL),zeit(0) {}
	~CZeitMesser() {}
	void Dump() {}
#endif
};



// r=1/p
#define FP_INV2(r,p)                     \
{                                        \
    __asm { mov     eax,0x7F000000    }; \
    __asm { sub     eax,dword ptr [p] }; \
    __asm { mov     dword ptr [r],eax }; \
    __asm { fld     dword ptr [p]     }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fsubr   [two]             }; \
    __asm { fmul    dword ptr [r]     }; \
    __asm { fstp    dword ptr [r]     }; \
}




// --------

#endif