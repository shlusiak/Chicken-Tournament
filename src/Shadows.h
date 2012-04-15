/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _SHADOWS_INCLUDED_
#define _SHADOWS_INCLUDED_



void RenderSquareShadow(LPDIRECT3DDEVICE7 lpDevice,float x,float y,float z,float w,float h);
void RenderRoundShadow(LPDIRECT3DDEVICE7 lpDevice,float x,float y,float z,float r);
void RenderModelShadow(LPDIRECT3DDEVICE7 lpDevice,CD3DFileObject* mesh);
void DrawShadow(LPDIRECT3DDEVICE7 lpDevice);






#define SHADOWS FALSE


#endif