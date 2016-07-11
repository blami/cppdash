###############################################################################
#   C++dash                                                                   #
#   Copyright (c) 2008-2009 Ondrej Balaz <ondra@blami.net>                    #
#   All rights reserved.                                                      #
#                                                                             #
#   Redistribution and use in source and binary forms, with or without        #
#   modification, are permitted provided that the following conditions        #
#   are met:                                                                  #
#                                                                             #
#   1. Redistributions of source code must retain the above copyright         #
#      notice, this list of conditions and the following disclaimer.          #
#   2. Redistributions in binary form must reproduce the above copyright      #
#      notice, this list of conditions and the following disclaimer in the    #
#      documentation and/or other materials provided with the distribution.   #
#   3. Neither the name of the University nor the names of its contributors   #
#      may be used to endorse or promote products derived from this software  #
#      without specific prior written permission.                             #
###############################################################################

CMAKE_MINIMUM_REQUIRED(VERSION 2.6)
SET(CMAKE_COLOR_MAKEFILE ON)
SET(CMAKE_VERBOSE_MAKEFILE ON)
SET(CMAKE_INCLUDE_CURRENT_DIR TRUE)

PROJECT(cppash CXX C)

#########################################################################
#                            PROJECT FILES                              #
#########################################################################
# common sources:
SET(SOURCES
	cppdash.cpp
	map.cpp
	tile.cpp
	ui_sdl.cpp
)

# includes: (always include from root directory)
INCLUDE_DIRECTORIES(${CMAKE_SOURCE_DIR})

# link libraries:
SET(LIBS)
FIND_PACKAGE(SDL REQUIRED)

#########################################################################
#                        CONFIGURATION OPTIONS                          #
#########################################################################
# debug:
OPTION(DEBUG            "Build with debug code"                         OFF)

# preset options:
# version
FILE(READ "${CMAKE_SOURCE_DIR}/.gitversion" VERSION)
SET(VERSION             "git-${VERSION}")
# build environment
SET(BUILD_OS            "${CMAKE_SYSTEM}")
SET(BUILD_ARCH          "${CMAKE_SYSTEM_PROCESSOR}")
# compile-time configuration (config.h)
CONFIGURE_FILE(
	${CMAKE_SOURCE_DIR}/config.h.cmake
	${CMAKE_BINARY_DIR}/config.h
	@ONLY
)


#########################################################################
#                             EXECUTABLE                                #
#########################################################################

# gdb debugging
#ADD_DEFINITIONS(-g)
# g++ 32 bit executable
SET(LINK_FLAGS "-m32")

# final executable
LINK_LIBRARIES(${LIBS} ${SDL_LIBRARY} SDLmain)
ADD_EXECUTABLE(cppdash ${SOURCES})
SET_TARGET_PROPERTIES(cppdash PROPERTIES LINK_FLAGS "${LINK_FLAGS}")

# binary properties:
#SET_TARGET_PROPERTIES(vgce PROPERTIES COMPILE_FLAGS ${COMPILE_FLAGS})
