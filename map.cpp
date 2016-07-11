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

// map.cpp: map class

using namespace std;

#include <cstdio>
#include <cctype>
#include <cassert>
#include "map.h"
#include "tile.h"
#include "config.h"
#include "debug.h"

// buffer size for map file loading
#define MAX_BUF_SIZE 255

/**
 *  Constructor.
 */
Map::Map()
{
	this->tiles = NULL;

	this->width = 0;
	this->height = 0;
	this->diamonds = 0;
	this->loaded = false;

	this->state = MAP_NONE;
}

/**
 *  Destructor.
 */
Map::~Map()
{
	this->free();
}

/**
 *  Loads map from plaintext storage which is further described in
 *  documentation. One ASCII character in file simply means one tile in map.
 *  \param filename     map filename
 *  \return             number of read tiles or -1 for error 
 */
int Map::load(const char* filename)
{
	FILE* f = fopen(filename, "r");
	int i;
	int total;
	char buffer[MAX_BUF_SIZE];  // line buffer
	int done = 0;

	this->free();

	if(!f)
	{
		error(false, "couldn't open map file: %s", filename);
		return -1;
	}
	debug("map file opened: %s", filename);

	// read file through buffer and decide map size
	int line_width = 0;
	int check[2] = {0, 0};
	while(fgets(buffer, MAX_BUF_SIZE, f) != NULL && !done)
	{
		i = 0;
		while(buffer[i] != '\0' && i < MAX_BUF_SIZE)
		{
			// skip non-printable characters
			if(!isprint(buffer[i]) && buffer[i] != '\n')
			{
				i++;
				continue;
			}

			if(buffer[i] == '\n')
			{
				if(line_width != 0)
				{
					// ordinary end of map line
					this->height++;
					if(this->width < line_width)
						this->width = line_width;

					line_width = 0;
					i++;
					continue;
				}
				else
				{
					done = 1;
					i++;
					continue;
				}
			}

			// entrance/exit check
			if(buffer[i] == '~' && !check[0])
				check[0]++;
			if(buffer[i] == ';')
				check[1]++;

			line_width++;
			i++;
		}
	}
	debug("map rectangle size: %ix%i", this->width, this->height);

	// failed checks
	if(check[0] != 1)
	{
		error(false, "map must have exactly one player symbol!");
		return -1;
	}
	if(!check[1])
	{
		error(false, "map must have at least one exit symbol!");
		return -1;
	}

	// load map data into memory
	int x = 0;
	int y = 0;

	this->tiles = new Tile** [this->height];    // alloc pointers to rows
	this->tiles[y] = new Tile* [this->width];   // alloc first row
	// initialize
	for(i = 0; i < this->width; i++)
		this->tiles[y][i] = NULL;

	rewind(f);
	done = 0;
	total = 0;

	// read file through buffer and fill tiles structure
	while(fgets(buffer, MAX_BUF_SIZE, f) != NULL && !done)
	{
		i = 0;
		while(buffer[i] != '\0' && i < MAX_BUF_SIZE)
		{
			// skip non-printable characters
			if(!isprint(buffer[i]) && buffer[i] != '\n')
			{
				i++;
				continue;
			}

			if(buffer[i] == '\n')
			{
				if(y < this->height)
				{
					y++;
					if(y < this->height)
					{
						// allocate and initialize next row
						this->tiles[y] = new Tile* [this->width];
						int j;
						for(j = 0; j < this->width; j++)
							this->tiles[y][j] = NULL;
					}

					x = 0;
					i++;
					continue;
				}
				else
				{
					// end of map tile data
					done = 1;
					i++;
					continue;
				}
			}

			switch(buffer[i])
			{
			case '#':
				this->tiles[y][x] = new Tile(TILE_WALL, false, false);
				break;
			case '.':
				this->tiles[y][x] = new Tile(TILE_SAND, true, false);
				break;
			case '@':
				this->tiles[y][x] = new Tile(TILE_BOULDER, false, false);
				break;
			case '$':
				this->diamonds++;
				this->tiles[y][x] = new Tile(TILE_DIAMOND, true, false);
				break;
			case '~':
				this->tiles[y][x] = new Tile(TILE_PLAYER, false, false);
				break;
			case ';':
				this->tiles[y][x] = new Tile(TILE_EXIT, true, false);
				break;
			default:
				// unknown tile is same as empty tile
#ifdef DEBUG
				if(buffer[i] != ' ')
					debug("unknown tile type '%c'", buffer[i]);
#endif /* DEBUG */
				this->tiles[y][x] = NULL;
			}
			x++;
			i++;
			total++;
		}
	}

	this->loaded = true;
	debug("%i map tiles loaded", total);

	this->state = MAP_NONE;

	// if there are any diamonds, be sure to lock exit
	x = 0;
	y = 0;
	if(this->findTileType(TILE_EXIT, &x, &y) && this->diamonds > 0)
		this->tiles[y][x]->setLocked(true);

	fclose(f);
	return total;
}

/**
 *  Returns width of map in tiles.
 *  \return             width of map in tiles
 */
int Map::getWidth()
{
	assert(this->loaded);
	return this->width;
}

/**
 *  Returns height of map in tiles.
 *  \return             height of map in tiles
 */
int Map::getHeight()
{
	assert(this->loaded);
	return this->height;
}

/**
 *  Returns number of remaining diamonds.
 *  \return             number of remaining diamonds
 */
int Map::getDiamonds()
{
	assert(this->loaded);
	return this->diamonds;
}

/**
 *  Returns current map state.
 *  \return             map state
 */
MAPSTATE Map::getState()
{
	return this->state;
}

/**
 *  Returns pointer to Tile object at coordinates x,y.
 *  \return             pointer to Tile object at x,y
 */
Tile* Map::getTileXY(int x, int y)
{
	assert(this->loaded);
	assert(this->tiles && this->width > x && this->height > y);

	return this->tiles[y][x];
}

/**
 *  Moves title from coords srcX, srcY to coords dstX, dstY vanishing original
 *  title placed on dstX, dstY. Does not apply any game rules.
 *  \param srcX         source X coordinate
 *  \param srcY         source Y coordinate
 *  \param dstX         destination X coordinate
 *  \param dstY         destination Y coordinate
 */
void Map::setTileXY(int srcX, int srcY, int dstX, int dstY)
{
	assert(this->loaded);
	assert(this->tiles);
	assert(srcX >= 0 && this->width > srcX && srcY >= 0 && this->height > srcY);
	assert(dstX >= 0 && this->width > dstX && dstY >= 0 && this->height > dstY);

	if(this->tiles[dstY][dstX] != NULL)
		delete this->tiles[dstY][dstX];

	this->tiles[dstY][dstX] = this->tiles[srcY][srcX];
	this->tiles[srcY][srcX] = NULL;
}

/**
 *  Move player by xStep tiles right and yStep tiles down. Accepts negative
 *  values. Check possibility of move and apply rules (vanish sand, collect
 *  diamonds).
 *  \param xStep        move by this number of tiles right (negative means
 *                      left)
 *  \param yStep        move by this number of tiles down (negative means up)
 *  \return             true if move possible, otherwise false
 */
bool Map::movePlayer(int xStep, int yStep)
{
	assert(this->loaded);
	assert(this->tiles);

	int x = 0, y = 0;
	if(!findTileType(TILE_PLAYER, &x, &y))
	{
		debug("oops! TILE_PLAYER not found!");
		return false;
	}

	assert(x+xStep >= 0 && this->width > x+xStep &&
		y+yStep >= 0 && this->height > y+yStep);

	debug("from x=%d y=%d to x=%d y=%d", x, y, x+xStep, y+yStep);

	// empty tile
	if(this->tiles[y+yStep][x+xStep] == NULL)
	{
		this->setTileXY(x, y, x+xStep, y+yStep);
		return true;
	}
	// non-empty tile
	else
		if(this->tiles[y+yStep][x+xStep]->isSteppable() == false)
		{
			// non-steppable tile. Move cannot be performed.
			debug("invalid move (tile not steppable)");
			return false;
		}

		switch(this->tiles[y+yStep][x+xStep]->getType())
		{
		case TILE_DIAMOND:
			this->diamonds--;
			debug("diamonds left: %d", this->diamonds);

			// unlock exit in case all diamonds has been collected
			if(this->diamonds == 0)
			{
				int ex = 0, ey = 0;
				this->findTileType(TILE_EXIT, &ex, &ey);
				this->tiles[ey][ex]->setLocked(false);
				debug("exit unlocked");
			}

			this->setTileXY(x, y, x+xStep, y+yStep);
			break;

		case TILE_SAND:
			this->setTileXY(x, y, x+xStep, y+yStep);
			break;

		case TILE_EXIT:
			// if exit tile is steppable, it's also unlocked
			assert(this->tiles[y+yStep][x+xStep]->isLocked() == false);

			// vanish player (this may cause oops in this method unless handled
			// by lifecycle through map->status == MAP_WON
			delete this->tiles[y][x];
			this->tiles[y][x] = NULL;

			this->state = MAP_WON;
			break;

		default:
			debug("invalid move!");
			return false;
		}

	return true;
}

/**
 *  Applies game-based gravity rules to entire map.
 */
void Map::doGravity()
{
	assert(this->loaded);
	assert(this->tiles);

	int x, y;
	for(y = 0; y < this->height; y++)
		for(x = 0; x < this->width; x++)
		{
			if(this->tiles[y][x] == NULL)
				continue;

			switch(this->tiles[y][x]->getType())
			{
			case TILE_BOULDER:
			case TILE_DIAMOND:
				if(y+1 < this->height)
				{
					// fall into empty space
					if(this->tiles[y+1][x] == NULL)
					{
						this->tiles[y][x]->setFalling(true);
						this->setTileXY(x, y, x, y+1);

						debug("falling object from x=%d y=%d to x=%d y=%d",
							x, y, x, y+1);
					}
					// fall on player and kill him
					else if(this->tiles[y+1][x]->getType() == TILE_PLAYER &&
						this->tiles[y][x]->isFalling())
					{
						this->setTileXY(x, y, x, y+1);
						this->state = MAP_LOST;

						debug("player killed by object at x=%d y=%d", x, y+1);
					}
					// stop falling
					else if(this->tiles[y][x]->isFalling())
					{
						this->tiles[y][x]->setFalling(false);

						debug("object stopped falling at x=%d y=%d", x, y);
					}
				}
				break;
			}
		}
}

/**
 *  Finds first occurence of specified type and fills x and y references with
 *  its coordinates. Those references are also starting coords of search. So
 *  this function can be used to iterate through map (remember that changes to
 *  map made in one step will be produced to further steps unlike 'real world
 *  iterator').
 *  \param type         type of searched Tile object
 *  \param x            starting x coordinate (if find is successfull also x
 *                      coordinate of found Tile object)
 *  \param y            starting y coordinate (if find is successfull also y
 *                      coordinate of found Tile object)
 *  \return             true if success (x, y contains position of found Tile
 *                      object) otherwise false.
 */
bool Map::findTileType(TILETYPE type, int* x, int* y)
{
	assert(this->loaded);
	assert(this->tiles);
	assert((*x) >= 0 && (*x) < this->width && (*y) >= 0 && (*y) < this->height);

	int ix, iy;
	for(iy = (*y); iy < this->height; iy++)
		for(ix = (*x); ix < this->width; ix++)
		{
			if(this->tiles[iy][ix] == NULL)
				continue;

			if(this->tiles[iy][ix]->getType() == type)
			{
				// found, set references to new coordinates
				(*x) = ix;
				(*y) = iy;
				debug("tile '%c' found at coords x=%d y=%d", type, ix, iy);
				return true;
			}
		}

	// not found
	return false;
}

/**
 *  Cleanup all map information.
 */
void Map::free()
{
	int x, y;

	// free tiles and pointer arrays
	if(this->tiles)
	{
		debug("freeing map data");

		for(y = 0; y < this->height; y++)
		{
			// delete row if exists
			if(this->tiles[y])
			{
				for(x = 0; x < this->width; x++)
				{
					if(this->tiles[y][x])
						delete this->tiles[y][x];
				}
			}
			delete[] this->tiles[y];
		}
		delete[] this->tiles;
	}

	this->width = 0;
	this->height = 0;
	this->loaded = false;
}
