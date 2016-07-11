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

// tile.cpp: tile and derived classes

using namespace std;

#include <cstdio>
#include "tile.h"
#include "config.h"
#include "debug.h"


/**
 *  Abstract Tile class constructor. Sets type and steppable attributes.
 *  \param type             desired tile type identifier
 *  \param steppable        sets whether is tile steppable or not
 */
Tile::Tile(TILETYPE type, bool steppable, bool locked)
{
	this->setType(type);
	this->setLocked(locked);
	// override steppable value set using setLocked();
	this->setSteppable(steppable);

	// wait for first map->doGravity()
	this->setFalling(false);
}

/**
 *  Sets tile type.
 *  \param type             desired tile type identifier
 */
void Tile::setType(TILETYPE type)
{
	this->type = type;
}

/**
 *  Gets tile type.
 *  \return                 tile type identifier
 */
TILETYPE Tile::getType()
{
	return this->type;
}

/**
 *  Sets tile steppable field. Player can enter those fields with steppable set
 *  to true.
 *  \param steppable        signals whether is tile steppable or not
 */
void Tile::setSteppable(bool steppable)
{
	this->steppable = steppable;
}

/**
 *  Returns true if tile is steppable. Otherwise false.
 *  \return                 steppable field of tile (true if steppable)
 */
bool Tile::isSteppable()
{
	return this->steppable;
}

/**
 *  Sets tile locked field (also toggles steppable). Locked literally means
 *  nonsteppable field with different texture than unlocked opossite.
 *  \param locked           signals whether is tile locked or not
 */
void Tile::setLocked(bool locked)
{
	this->locked = locked;
	this->setSteppable(!locked);
}

/**
 *  Returns true if tile is locked. Otherwise false.
 *  \return                 locked field of tile (true if locked)
 */
bool Tile::isLocked()
{
	return this->locked;
}

/**
 *  Sets tile falling state. Items that are falling can be collected in falling
 *  (e.g. diamond) or they can kill player (e.g. boulder). Because movement of
 *  anything by gravity is one tile at tick, tiles need falling state indicator
 *  \param falling          signals whether is tile falling or not
 */
void Tile::setFalling(bool falling)
{
	this->falling = falling;
}

/**
 *  Returns true if tile is falling. Otherwise false.
 *  \return                 falling field of tile (true if falling)
 */
bool Tile::isFalling()
{
	return this->falling;
}
