/***********************************************************
 * This file is part of the Chicken Tournament source code *
 *                                                         *
 * Copyright (c) by Sascha Hlusiak, 2003.                  *
 * All rights reserved                                     *
 *                                                         *
 ***********************************************************/

#ifndef _MUSIC_INCLUDED_
#define _MUSIC_INCLUDED_



class CMusic
{
private:
	IDirectMusic *lpDirectMusic;
	IDirectMusicPerformance *lpDM;
	IDirectMusicLoader *lpLoader;
	IDirectMusicComposer *lpComposer;
	IDirectMusicSegment *lpSegment;
	IDirectMusicSegment *lpTemplate;
	IDirectMusicStyle *lpStyle;
	IDirectMusicChordMap *lpChordMap;
	IDirectMusicBand *lpBand;
	IDirectMusicSegment* lpBandSegment;
public:
	BOOLEAN playing;

	CMusic();
	~CMusic();

	const BOOLEAN LoadMusic();
	const BOOLEAN InitMusic(const BOOLEAN play);
	const BOOLEAN SetPort();

	const BOOLEAN ToggleMusic();
	const BOOLEAN IsMusic()const;
	void PlayModif(const PCHAR name);
};








#endif