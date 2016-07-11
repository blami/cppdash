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

// cppdash.cpp: application main

using namespace std;

#include <cstdio>
#include <cstdlib>
#include "tile.h"
#include "map.h"
#include "ui_sdl.h"
#include "config.h"
#include "debug.h"


int main(int argc, char** argv)
{
	int done = 0;

	printf("C++dash (%s) - Yet another `Boulder Dash' clone\n"
		"Author: Ondrej Balaz <ondra@blami.net>\n"
		"This software is distributed under BSD style license. See LICENSE.\n"
		"\n",
		VERSION);

	// process arguments
	if(argc < 2)
	{
		fprintf(stderr,
			"error: path to map file is missing!\n"
			"Usage: %s /path/to/map.txt\n",
			argv[0]
		);

		return EXIT_FAILURE;
	}

	debug("init");

	// initialize map
	Map* map = new Map();
	map->load(argv[1]);

	// initialize ui
	SDLUI* ui = new SDLUI();

	while(!done)
	{
		// handle input and move player
		switch(ui->input())
		{
		case INPUT_UP:
			map->movePlayer(0, -1);
			break;
		case INPUT_DOWN:
			map->movePlayer(0, 1);
			break;
		case INPUT_LEFT:
			map->movePlayer(-1, 0);
			break;
		case INPUT_RIGHT:
			map->movePlayer(1, 0);
			break;
		case INPUT_QUIT:
			done = -1;
			break;
		}

		// tasks done once upon time (not every tick)
		map->doGravity();

		// redraw map
		ui->draw(map);

		// check map state
		switch(map->getState())
		{
		case MAP_WON:
			printf("---------------\n" \
				"    PERFECT!\n" \
				"---------------\n");
			done = -1;
			break;
		case MAP_LOST:
			printf("---------------\n" \
				"   BAD LUCK!\n" \
				"---------------\n");
			done = -1;
			break;

			break;
		}
	}

	debug("exit");

	// cleanup
	delete ui;
	delete map;

	return EXIT_SUCCESS;
}
