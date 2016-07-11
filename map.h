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

// map.h: map class headers

#ifndef __MAP_H
#define __MAP_H

#include "tile.h"

class Tile;


/**
 *  Enumeration of all map states.
 */
typedef enum
{
	MAP_NONE,
	MAP_WON,
	MAP_LOST
} MAPSTATE;


/**
 *  Class representing game map.
 */
class Map
{
private:
	Tile*** tiles;
	MAPSTATE state;
	int width;
	int height;
	bool loaded;
	int diamonds;   // total of diamonds to collect

public:
	Map();
	~Map();
	int load(const char* filename);
	int getWidth();
	int getHeight();
	int getDiamonds();
	MAPSTATE getState();
	Tile* getTileXY(int x, int y);
	void setTileXY(int srcX, int srcY, int dstX, int dstY);
	bool movePlayer(int xSteps, int ySteps);
	void doGravity();
	bool findTileType(TILETYPE type, int* x, int* y);
	void free();
};

#endif /* __MAP_H */
