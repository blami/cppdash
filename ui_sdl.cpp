/*
 *  C++dash
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

// ui_sdl.cpp: SDL user interface main class

using namespace std;

#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cassert>
#include <map> // STL map
#include <string>
#include "ui_sdl.h"
#include "map.h"
#include "tile.h"
#include "config.h"
#include "debug.h"


/**
 *  Constructor. Initializes SDL video and input, creates window and loads
 *  sprites XPM.
 */
SDLUI::SDLUI()
{
	debug("SDL: init");

	// initialize SDL
	if(SDL_Init(SDL_INIT_VIDEO) < 0)
		error(true, "SDL: couldn't initialize SDL");

	// setup window
	if(!(this->screen = SDL_SetVideoMode(640, 480, 8, SDL_DOUBLEBUF)))
	{
		SDL_Quit();
		error(true, "SDL: couldn't initialize video surface");
	}
	SDL_WM_SetCaption("C++dash", NULL);

	// load sprites
	this->sprite = SDLUI::xpmLoad(ui_sdl_xpm);
}

/**
 *  Destructor.
 */
SDLUI::~SDLUI()
{
	debug("SDL: exit");

	// free surfaces
	if(this->sprite)
		SDL_FreeSurface(this->sprite);
	if(this->screen)
		SDL_FreeSurface(this->screen);

	SDL_Quit();
}

/**
 *  Draw entire map to screen surface.
 *  \see Tile
 */
void SDLUI::draw(Map* map)
{
	assert(this->screen);
	assert(map);

	int x, y;
	Tile* tile = NULL;

	// blank screen
	SDL_FillRect(screen, &screen->clip_rect, SDL_MapRGB(screen->format, 0, 0, 0));

	// FIXME dirty bunch of code. Scrolling will need not-so-tied coordinates.
	// As long as scrolling isn't supported this will work perfectly.

	// draw map tiles
	for(y = 0; y < map->getHeight(); y++)
		for(x = 0; x < map->getWidth(); x++)
		{
			tile = map->getTileXY(x, y);

			if(!tile)
				continue; // empty tile

			switch(tile->getType())
			{
			case TILE_SAND:
				this->drawSprite(1, 0, x * 16, y * 16);
				break;
			case TILE_WALL:
				this->drawSprite(2, 0, x * 16, y * 16);
				break;
			case TILE_BOULDER:
				this->drawSprite(3, 0, x * 16, y * 16);
				break;
			case TILE_DIAMOND:
				this->drawSprite(4, 0, x * 16, y * 16);
				break;
			case TILE_PLAYER:
				this->drawSprite(5, 0, x * 16, y * 16);
				break;
			case TILE_EXIT:
				if(tile->isLocked())
					// locked door
					this->drawSprite(6, 0, x * 16, y * 16);
				else
					// unlocked door
					this->drawSprite(7, 0, x * 16, y * 16);
				break;
			}
		}

	// flip buffers
	SDL_Flip(this->screen);
}

/**
 *  Process input events.
 *  \return         key identificator
 *  \see UIINPUT
 */
UIINPUT SDLUI::input()
{
	UIINPUT r = INPUT_UNKNOWN;

	while(SDL_PollEvent(&this->event))
	{
		switch(this->event.type)
		{
		// non-keyboard events
		case SDL_QUIT:
			debug("SDL: SDL_QUIT event");
			r = INPUT_QUIT;
			break;

		// keyboard events
		case SDL_KEYDOWN:
			debug("SDL: SDL_KEYDOWN event (%x)", this->event.key.keysym.scancode);
			r = INPUT_UNKNOWN;

			switch(this->event.key.keysym.sym)
			{
			case SDLK_q:
			case SDLK_ESCAPE:
				debug("SDL: q/ESC, Exiting.");
				r = INPUT_QUIT;
				break;
			case SDLK_UP:
				debug("SDL: up");
				r = INPUT_UP;
				break;
			case SDLK_DOWN:
				debug("SDL: down");
				r = INPUT_DOWN;
				break;
			case SDLK_LEFT:
				debug("SDL: left");
				r = INPUT_LEFT;
				break;
			case SDLK_RIGHT:
				debug("SDL: right");
				r = INPUT_RIGHT;
				break;
			default:
				r = INPUT_UNKNOWN;
			}

			break;
		}
	}

	return r;
}

// ---------------------------------------------------------------------------
// Private methods
// ---------------------------------------------------------------------------

/**
 *  Draw sprite to specified coords on screen.
 *  \param spriteX  sprite X in 16x16 grid (0ish indexing)
 *  \param spriteY  sprite Y in 16x16 grid (0ish indexing)
 *  \param x        destination sprite absolute X coord
 *  \param y        destination sprite absolute Y coord
 *  \return         result of SDL_BlitSurface operation (0 if success)
 */
void SDLUI::drawSprite(int spriteX, int spriteY, int x, int y)
{
	assert(this->sprite && this->screen);

	SDL_Rect clip, offset;

	// clipping
	clip.x = spriteX * 16;
	clip.y = spriteY * 16;
	clip.w = 16;
	clip.h = 16;

	// offset
	offset.x = x;
	offset.y = y;

	SDL_BlitSurface(this->sprite, &clip, this->screen, &offset);
}

// XPM

/**
 *  Convert color from XPM spec. (0xrrggbb) to RGB. Returns 1 if success
 *  otherwise 0.
 *  \param spec     color spec (0xrrggbb)
 *  \param speclen  length of color spec
 *  \param rgb      pointer to result
 *  \returns        1 if success, otherwise 0
 */
int SDLUI::xpmColorToRgb(char* spec, int speclen, Uint32* rgb)
{
	if(spec[0] != '#')
	{
		// special 'None' color (transparent)
		if(toupper((unsigned char)*spec) == toupper((unsigned char)*"none"))
		{
			*rgb = 0xffffffff;
			return 1;
		}
		return 0;
	}

	char buf[7];
	switch(speclen)
	{
	case 4:
		buf[0] = buf[1] = spec[1];
		buf[2] = buf[3] = spec[2];
		buf[4] = buf[5] = spec[3];
		break;
	case 7:
		memcpy(buf, spec + 1, 6);
		break;
	case 13:
		buf[0] = spec[1];
		buf[1] = spec[2];
		buf[2] = spec[5];
		buf[3] = spec[6];
		buf[4] = spec[9];
		buf[5] = spec[10];
		break;
	}
	buf[6] = '\0';

	*rgb = strtol(buf, NULL, 16);
	return 1;
}

/**
 *  Load entire XPM image from char 2D array to SDL Surface.
 *  \param xpm      xpm static data (array)
 *  \returns        SDL_Surface
 */
SDL_Surface* SDLUI::xpmLoad(char** xpm)
{
	SDL_Surface *r = NULL;
	int i;
	int x, y;
	int ncolors;
	int w, h, cpp;
	SDL_Color* colors;
	char* line;
	char*** lines = NULL;
	char* keystrings = NULL;
	char* nextkey;
	int pixels_len;
	Uint8* dst;
	// colormap
	map<string, Uint32> colormap; // STL

	debug("SDL: load xpm");
	if(xpm)
		lines = &xpm;

	// read header
	line = XPM_LINE(lines);
	if(!line)
		goto done;

	if(sscanf(line, "%d %d %d %d", &w, &h, &ncolors, &cpp) != 4
		|| w != 256 || h != 256 || ncolors <= 0 || ncolors > 256 || cpp <= 0)
	{
		error(false, "SDL: invalid xpm format");
		goto done;
	}
	debug("SDL: xpm header: %dx%d %d colors", w, h, ncolors);

	keystrings = (char*)malloc(ncolors * cpp);
	if(!keystrings)
		error(true, "SDL: out of memory!");
	nextkey = keystrings;

	// prepare surface
	r = SDL_CreateRGBSurface(SDL_SWSURFACE, w, h, 8, 0, 0, 0, 0);
	colors = r->format->palette->colors;
	r->format->palette->ncolors = ncolors;
	if(!r)
		error(true, "SDL: out of memory/can't create surface!");

	// read colors
	for(i = 0; i < ncolors; i++)
	{
		char *ptr;

		line = XPM_LINE(lines)
		if(!line)
			goto done;

		ptr = line + cpp + 1;

		// parse line
		for(;;)
		{
			char type;
			char *spec;
			char key[cpp];
			Uint32 rgb, pixel;

			XPM_SKIPSPACE(ptr);
			if(!*ptr)
			{
				error(false, "SDL: invalid xpm color");
				goto done;
			}
			type = *ptr;
			XPM_SKIPNONSPACE(ptr);
			XPM_SKIPSPACE(ptr);
			spec = ptr;
			XPM_SKIPNONSPACE(ptr);

			if(type == 's')
				continue;   // skip symbolic colors
			if(!SDLUI::xpmColorToRgb(spec, ptr - spec, &rgb))
				continue;   // skip unknown colors
			memcpy(nextkey, line, cpp);
			SDL_Color *c = colors + i;
			c->r = (Uint8)(rgb >> 16);
			c->g = (Uint8)(rgb >> 8);
			c->b = (Uint8)(rgb);
			pixel = i;

			// store color in colormap
			strncpy(key, nextkey, cpp);
			colormap[key] = pixel;

			nextkey += cpp;

			// transparent color
			if(rgb == 0xffffffff)
				SDL_SetColorKey(r, SDL_SRCCOLORKEY, pixel);
			break;
		}
	}

	// read pixels
	pixels_len = w * cpp;
	dst = (Uint8*)r->pixels;
	for(y = 0; y < h; y++)
	{
		line = XPM_LINE(lines);

		// parse line
		for(x = 0; x < w; x++)
		{
			char key[cpp];
			strncpy(key, (line + x * cpp), cpp);
			dst[x] = (Uint8)colormap[key];
		}

		dst += r->pitch;
	}
	debug("SDL: xpm loaded");

done:
	if(keystrings)
		free(keystrings);
	return r;
}
