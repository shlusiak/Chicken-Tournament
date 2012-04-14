//-----------------------------------------------------------------------------
// File: D3DMath.h
//
// Desc: Math functions and shortcuts for Direct3D programming.
//
// Copyright (c) 1997-1999 Microsoft Corporation. All rights reserved
//-----------------------------------------------------------------------------
#ifndef D3DMATH_H
#define D3DMATH_H
#include <ddraw.h>
#include <d3d.h>


//-----------------------------------------------------------------------------
// Useful Math constants
//-----------------------------------------------------------------------------
const FLOAT g_PI       =  3.14159265358979323846f; // Pi
const FLOAT g_2_PI     =  6.28318530717958623200f; // 2 * Pi
const FLOAT g_PI_DIV_2 =  1.57079632679489655800f; // Pi / 2
const FLOAT g_PI_DIV_4 =  0.78539816339744827900f; // Pi / 4
const FLOAT g_INV_PI   =  0.31830988618379069122f; // 1 / Pi
const FLOAT g_DEGTORAD =  0.01745329251994329547f; // Degrees to Radians
const FLOAT g_RADTODEG = 57.29577951308232286465f; // Radians to Degrees
const FLOAT g_HUGE     =  1.0e+38f;                // Huge number for FLOAT
const FLOAT g_EPSILON  =  1.0e-5f;                 // Tolerance for FLOATs




//-----------------------------------------------------------------------------
// Fuzzy compares (within tolerance)
//-----------------------------------------------------------------------------
inline BOOL D3DMath_IsZero( FLOAT a, FLOAT fTol = g_EPSILON )
{ return ( a <= 0.0f ) ? ( a >= -fTol ) : ( a <= fTol ); }




//-----------------------------------------------------------------------------
// Matrix functions
//-----------------------------------------------------------------------------
inline VOID D3DMath_MatrixMultiply( D3DMATRIX& q, const D3DMATRIX& a, const D3DMATRIX& b )
{
    const FLOAT* pA = (FLOAT*)&a;
    const FLOAT* pB = (FLOAT*)&b;
    FLOAT pM[16];

    ZeroMemory(&pM, sizeof(D3DMATRIX) );

    for( WORD i=0; i<4; i++ ) 
        for( WORD j=0; j<4; j++ ) 
            for( WORD k=0; k<4; k++ ) 
                pM[4*i+j] +=  pA[4*i+k] * pB[4*k+j];

    memcpy( &q, pM, sizeof(D3DMATRIX) );
}

HRESULT D3DMath_MatrixInvert( D3DMATRIX& q, D3DMATRIX& a );




//-----------------------------------------------------------------------------
// Vector functions
//-----------------------------------------------------------------------------
inline const HRESULT D3DMath_VectorMatrixMultiply( D3DVECTOR& vDest, D3DVECTOR& vSrc,
                                      D3DMATRIX& mat)
{
    const float x = vSrc.x*mat._11 + vSrc.y*mat._21 + vSrc.z* mat._31 + mat._41;
    const float y = vSrc.x*mat._12 + vSrc.y*mat._22 + vSrc.z* mat._32 + mat._42;
    const float z = vSrc.x*mat._13 + vSrc.y*mat._23 + vSrc.z* mat._33 + mat._43;
    const float w = vSrc.x*mat._14 + vSrc.y*mat._24 + vSrc.z* mat._34 + mat._44;
    
    if( fabs( w ) < g_EPSILON )
        return E_INVALIDARG;

    vDest.x = x/w;
    vDest.y = y/w;
    vDest.z = z/w;

    return S_OK;
}



HRESULT D3DMath_VertexMatrixMultiply( D3DVERTEX& vDest, D3DVERTEX& vSrc,
                                      D3DMATRIX& mat );




//-----------------------------------------------------------------------------
// Quaternion functions
//-----------------------------------------------------------------------------
VOID D3DMath_QuaternionFromRotation( FLOAT& x, FLOAT& y, FLOAT& z, FLOAT& w,
                                     D3DVECTOR& v, FLOAT fTheta );
VOID D3DMath_RotationFromQuaternion( D3DVECTOR& v, FLOAT& fTheta,
                                     FLOAT x, FLOAT y, FLOAT z, FLOAT w );
VOID D3DMath_QuaternionFromAngles( FLOAT& x, FLOAT& y, FLOAT& z, FLOAT& w,
                                   FLOAT fYaw, FLOAT fPitch, FLOAT fRoll );
VOID D3DMath_MatrixFromQuaternion( D3DMATRIX& mat, FLOAT x, FLOAT y, FLOAT z,
                                   FLOAT w );
VOID D3DMath_QuaternionFromMatrix( FLOAT &x, FLOAT &y, FLOAT &z, FLOAT &w,
                                   D3DMATRIX& mat );
VOID D3DMath_QuaternionMultiply( FLOAT& Qx, FLOAT& Qy, FLOAT& Qz, FLOAT& Qw,
                                 FLOAT Ax, FLOAT Ay, FLOAT Az, FLOAT Aw,
                                 FLOAT Bx, FLOAT By, FLOAT Bz, FLOAT Bw );
VOID D3DMath_QuaternionSlerp( FLOAT& Qx, FLOAT& Qy, FLOAT& Qz, FLOAT& Qw,
                              FLOAT Ax, FLOAT Ay, FLOAT Az, FLOAT Aw,
                              FLOAT Bx, FLOAT By, FLOAT Bz, FLOAT Bw,
                              FLOAT fAlpha );


#endif // D3DMATH_H
