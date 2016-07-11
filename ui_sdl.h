/*
 *  Boulder Dash
 *  Copyright (c) 2008-2009 Ondrej Balaz <ondra@blami.net>
 *  All rights reserved.
 *
 *  Redistribution and use in source and binary forms, with or without 
 *  modification, are permitted provided that the following conditions 
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright 
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the University nor the names of its contributors
 *     may be used to endorse or promote products derived from this software
 *     without specific prior written permission.
 */

// sdl.h: SDL user interface class

#ifndef __SDL_UI_H
#define __SDL_UI_H

#include "SDL/SDL.h"        // libSDL
#include "ui.h"
#include "ui_sdl.xpm"


class Tile;                 // tile.h


/**
 *  SDL UI class.
 */
class SDLUI : public UI
{
private:
	SDL_Surface* screen;
	SDL_Surface* sprite;
	SDL_Event event;

	static SDL_Surface* xpmLoad(char** xpm);
	static int xpmColorToRgb(char* spec, int speclen, Uint32* rgb);

	void drawSprite(int spriteX, int spriteY, int x, int y);
public:
	SDLUI();
	~SDLUI();
	UIINPUT input();
	void draw(Map* map);
};

// XPM utils

/**
 *  Returns next line of XPM source.
 *  \param lines        XPM source lines
 *  \return             pointer to next source line
 */
#define XPM_LINE(lines) \
	*(*lines)++;

/**
 *  Skips spaces in pointer to char* and seeks to next nonspace character.
 *  \param ptr          pointer to char*
 */
#define XPM_SKIPSPACE(ptr) \
do \
{ \
	while(isspace((unsigned char)*(ptr))) \
		++(ptr); \
} \
while(0)

/**
 *  Skips non-spaces in pointer to char* and seeks to next space character.
 *  \param ptr          pointer to char*
 */
#define XPM_SKIPNONSPACE(ptr) \
do \
{ \
	while(!isspace((unsigned char)*(ptr)) && *ptr) \
		++(ptr); \
} \
while(0)


#endif /* __UI_H */
