//-----------------------------------------------------------------------------
// File: D3DFile.h
//
// Desc: Support code for loading DirectX .X files.
//
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved.
//-----------------------------------------------------------------------------
#ifndef D3DFILE_H
#define D3DFILE_H




//-----------------------------------------------------------------------------
// Name: struct MeshMaterialData
// Desc: Internal structure for holding material data for within a mesh. This
//       is used because multiple materials can be used in the same mesh.
//-----------------------------------------------------------------------------
#define MAX_MATERIAL        16
#define MAX_TEXTURE_NAME    80

struct MeshMaterialData
{
    D3DMATERIAL7 m_mtrl;
    CHAR         m_strTexture[MAX_TEXTURE_NAME];
    DWORD        m_dwNumIndices;
};




//-----------------------------------------------------------------------------
// Name: class CD3DFileObject
// Desc: Internal class for objects in a .X file
//-----------------------------------------------------------------------------
class CD3DFileObject
{
    // Common data
    CHAR             m_strName[80];
    CD3DFileObject*  m_pNext;
    CD3DFileObject*  m_pChild;

    // For file frames
    D3DMATRIX        m_mat;

    // For file meshes
    BOOL             m_bHasMeshData;
    DWORD            m_dwNumVertices;
    D3DVERTEX*       m_pVertices;
    DWORD            m_dwNumIndices;
    WORD*            m_pIndices;
    DWORD            m_dwNumMaterials;
    MeshMaterialData m_Material[MAX_MATERIAL];
    BOOL             m_bHasAlpha;

public:
    // Initializing functions
    VOID    AddNext( CD3DFileObject* );
    VOID    AddChild( CD3DFileObject* );
    VOID    SetName( const CHAR* strName )     { strcpy( m_strName, strName ); }
    VOID    SetMatrix( const D3DMATRIX* pmat ) { m_mat = *pmat; }
    VOID    SetNormals( D3DVECTOR* pNormals );
    VOID    SetTextureCoords( FLOAT* pTexCoords );
    VOID    SetMaterialData( const DWORD dwMaterial, D3DMATERIAL7* pmtrl, CHAR*strName );
    VOID    AddFace( DWORD dwMaterial, DWORD* pFaceData, DWORD dwNumFaces );
    const HRESULT ComputeNormals();
    const HRESULT SetMeshGeometry( D3DVECTOR* pvVertices, DWORD dwNumVertices,
                             DWORD* pFaces, DWORD dwNumFaces );
	
	// Access functions
    CHAR*           GetName()   { return m_strName; }
    CD3DFileObject* GetNext()   { return m_pNext; }
    CD3DFileObject* GetChild()  { return m_pChild; }
    D3DMATRIX*      GetMatrix() { return &m_mat; }
    HRESULT         GetMeshGeometry( D3DVERTEX** ppVertices,
		                             DWORD* pdwNumVertices, WORD** ppIndices,
									 DWORD* pdwNumIndices );

    // Common functions
    VOID    Render( const LPDIRECT3DDEVICE7 pd3dDevice , const BOOL bAlpha ,const LPDIRECTDRAWSURFACE7 lpForceTexture=NULL);
    BOOL    EnumObjects( BOOL (*fnCallback)(CD3DFileObject*,D3DMATRIX*,VOID*),
                         D3DMATRIX* pmat, VOID* pContext );

	// Constuctor / destructor
    CD3DFileObject( CHAR* strName );
    ~CD3DFileObject();
};




//-----------------------------------------------------------------------------
// Name: class CD3DFile
// Desc: 
//-----------------------------------------------------------------------------
class CD3DFile
{
    CD3DFileObject*   m_pRoot;

public:
    HRESULT    GetMeshVertices( CHAR* strName, D3DVERTEX** ppVertices,
                                DWORD* pdwNumVertices );
    HRESULT    GetMeshIndices( CHAR* strName, WORD** ppIndices,
                               DWORD* pdwNumIndices );
    
    CD3DFileObject* FindObject( const CHAR* strName );
    VOID            EnumObjects( BOOL (*fnCallback)(CD3DFileObject*,D3DMATRIX*,VOID*),
                                 D3DMATRIX* pmat, VOID* pContext );
    VOID            Scale( FLOAT fScale );

    
    
    HRESULT Load( CHAR* strFilename ,const HMODULE hModule);
    const HRESULT Render( const LPDIRECT3DDEVICE7 )const;

    CD3DFile();
    ~CD3DFile();
};




#endif




