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

// ui.h: User interface interface base class

#ifndef __UI_H
#define __UI_H


class Map;      // map.h


/**
 *  Enum containing possible input commands. UI class should proccess input
 *  layer (keyboard/joystick) and map input to one of these "commands".
 */
typedef enum
{
	INPUT_UP        = 0,
	INPUT_DOWN      = 1,
	INPUT_LEFT      = 2,
	INPUT_RIGHT     = 3,
	INPUT_QUIT      = 4,

	INPUT_UNKNOWN   = -1
} UIINPUT;


/**
 *  Interface for UI classes.
 */
class UI
{
public:
	virtual UIINPUT input() { return INPUT_UNKNOWN; };
	virtual void draw(Map* map) {};
};


#endif /* __UI_H */
