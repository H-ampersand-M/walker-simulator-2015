/**
 * \file tools.h
 * \brief Tools
 * \author RAZANAJATO RANAIVOARIVONY Harenome
 * \author SCHMITT Maxime
 * \date 2015
 * \copyright WTFPLv2
 */

/* Copyright © 2015
 *      Harenome RAZANAJATO <razanajato@etu.unistra.fr>
 *      Maxime SCHMITT <maxime.schmitt@etu.unistra.fr>
 *
 * This program is free software. It comes without any warranty, to
 * the extent permitted by applicable law. You can redistribute it
 * and/or modify it under the terms of the Do What The Fuck You Want
 * To Public License, Version 2, as published by Sam Hocevar.
 *
 * See http://www.wtfpl.net/ or read below for more details.
 */

/*        DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *                    Version 2, December 2004
 *
 * Copyright (C) 2004 Sam Hocevar <sam@hocevar.net>
 *
 * Everyone is permitted to copy and distribute verbatim or modified
 * copies of this license document, and changing it is allowed as long
 * as the name is changed.
 *
 *            DO WHAT THE FUCK YOU WANT TO PUBLIC LICENSE
 *   TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
 *
 *  0. You just DO WHAT THE FUCK YOU WANT TO.
 */

#ifndef __WALKER_TOOLS_H__
#define __WALKER_TOOLS_H__

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sysexits.h>
#include <stdarg.h>

////////////////////////////////////////////////////////////////////////////////
// Debugging formatted printers.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief printf(3) when DEBUG is defined.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see printf(3)
 */
int debug (const char * format, ...);

/**
 * \brief fprintf(3) when DEBUG is defined.
 * \param stream The destination stream.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see fprintf(3)
 */
int fdebug (FILE * stream, const char * format, ...);

/**
 * \brief sprintf(3) when DEBUG is defined.
 * \param str The destination string.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see sprintf(3)
 */
int sdebug (char * str, const char * format, ...);

/**
 * \brief sprintf(3) when DEBUG is defined.
 * \param str The destination string.
 * \param size The maximum number of characters to write.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see sprintf(3)
 */
int sndebug (char * str, size_t size, const char * format, ...);

#endif /* __WALKER_TOOLS_H__ */
