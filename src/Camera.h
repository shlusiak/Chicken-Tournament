/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _CAMERA_INCLUDED_
#define _CAMERA_INCLUDED_



class CCamera
{
private:
	D3DMATRIX view;
public:
	void OptimizeCamera() { OptimizeCamera(&view); }
	const D3DVECTOR GetCameraPos() { return GetCameraPos(&view); }

	void SetViewMatrix(const D3DMATRIX m) { view=m; }
	D3DMATRIX* GetViewMatrix() { return &view; }

	static void OptimizeCamera(D3DMATRIX *view);
	static const D3DVECTOR GetCameraPos(D3DMATRIX *view);
};










#endif