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

// tile.h: tile and derived class headers

#ifndef __TILE_H
#define __TILE_H


/**
 *  Enumeration of available tile types.
 */
typedef enum
{
	TILE_WALL       = '#',
	TILE_SAND       = '.',
	TILE_BOULDER    = '@',
	TILE_DIAMOND    = '$',
	TILE_PLAYER     = '~',
	TILE_EXIT       = ';'
} TILETYPE;


/**
 *  Abstract class representing map tile.
 */
class Tile
{
private:
	TILETYPE type;
	bool steppable;
	bool locked;        // doors e.g.
	bool falling;       // anything that can fall

protected:

public:
	Tile(TILETYPE type, bool steppable, bool locked);
	void setType(TILETYPE type);
	TILETYPE getType();
	void setSteppable(bool steppable);
	bool isSteppable();
	void setLocked(bool locked);
	bool isLocked();
	void setFalling(bool falling);
	bool isFalling();
};


#endif /* __TILE_H */
