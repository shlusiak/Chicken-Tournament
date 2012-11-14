/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

typedef DWORD UID;


const int IDUnknown=0;
const int IDSound=1;
const int IDEffect=2;
const int IDGartenkralle=3;
const int IDChicken=4;
const int IDBauer=5;
const int IDDrescher=6;
const int IDGolfer=7;
const int IDWorld=8;
const int IDTree=9;
const int IDEi=10;
const int IDStall=11;
const int IDPlasmaCannon=12;
const int IDItem=13;




const UID MakeUID(const BYTE type);
inline const BYTE GetType(const UID id){ return BYTE(id&0xFF); }

const PCHAR GetTypeName(const UID id);