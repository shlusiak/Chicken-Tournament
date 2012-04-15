/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

typedef DWORD UID;


const IDUnknown=0;
const IDSound=1;
const IDEffect=2;
const IDGartenkralle=3;
const IDChicken=4;
const IDBauer=5;
const IDDrescher=6;
const IDGolfer=7;
const IDWorld=8;
const IDTree=9;
const IDEi=10;
const IDStall=11;
const IDPlasmaCannon=12;
const IDItem=13;




const UID MakeUID(const BYTE type);
inline const BYTE GetType(const UID id){ return BYTE(id&0xFF); }

const PCHAR GetTypeName(const UID id);