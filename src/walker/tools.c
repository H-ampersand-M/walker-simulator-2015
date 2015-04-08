/**
 * \file tools.c
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

#include "walker/tools.h"

////////////////////////////////////////////////////////////////////////////////
// Debugging formatted printers.
////////////////////////////////////////////////////////////////////////////////

int debug (const char * format, ...)
{
    #ifdef DEBUG
        va_list ap;
        va_start (ap, format);
        int result = vprintf (format, ap);
        va_end (ap);
        return result;
    #else
        return 0;
    #endif
}

int fdebug (FILE * stream, const char * format, ...)
{
    #ifdef DEBUG
        va_list ap;
        va_start (ap, format);
        int result = vfprintf (stream, format, ap);
        va_end (ap);
        return result;
    #else
        return 0;
    #endif
}

int sdebug (char * str, const char * format, ...)
{
    #ifdef DEBUG
        va_list ap;
        va_start (ap, format);
        int result = vsprintf (str, format, ap);
        va_end (ap);
        return result;
    #else
        return 0;
    #endif
}

int sndebug (char * str, size_t size, const char * format, ...)
{
    #ifdef DEBUG
        va_list ap;
        va_start (ap, format);
        int result = vsnprintf (str, size, format, ap);
        va_end (ap);
        return result;
    #else
        return 0;
    #endif
}


