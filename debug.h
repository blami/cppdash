/*
 * C++dash
 * Copyright (c) 2008-2009 Ondrej Balaz <ondra@blami.net>
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without 
 * modification, are permitted provided that the following conditions 
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright 
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 */

// debug.h: debugging macros

#ifndef __DEBUG_H
#define __DEBUG_H

#include <cstdio>
#include <cstdlib>
#include <cerrno>


#ifdef DEBUG
/** Debug macro. If is macro DEBUG defined outputs debug messages, otherwise
 *  does nothing.
 *  \param format       debug message format
 *  \param arg...       n arguments used as wildcards in format message */
#define debug(format, arg...) \
{ \
	fprintf(stderr, "debug: [%s:%.4d] %s(): " format "\n",\
		__FILE__, __LINE__, __func__, ##arg); \
}
#else
	#define debug(format, arg...)
#endif /* DEBUG */

/** Error macro. Macro prints formated error and if it's error bit set also
 *  calls perror().
 *  \param fatal        if set to 'true' then exits with EXIT_ERROR
 *  \param format       error message format
 *  \param arg...       n arguments used as wildcards in format message */
#define error(fatal, format, arg...) \
{ \
	fprintf(stderr, "error: [%s:%.4d] %s(): " format "\n",\
		__FILE__, __LINE__, __func__, ##arg); \
	if(errno) \
		perror("error"); \
	if(fatal) \
		exit(-1); \
}


#endif /* __DEBUG_H */
