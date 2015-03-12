/**
 * \file version.h
 * \brief Version.
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

#ifndef __WALKER_VERSION_H__
#define __WALKER_VERSION_H__

/**
 * \brief WALKER major version.
 */
static const unsigned int WALKER_VERSION_MAJOR = 1;

/**
 * \brief WALKER minor version.
 */
static const unsigned int WALKER_VERSION_MINOR = 0;

/**
 * \brief WALKER patch version.
 */
static const unsigned int WALKER_VERSION_PATCH = 0;

static inline void fprintf_version (FILE * const stream, const char * const name)
{
    fprintf (stream, "%s %u.%u.%u\n", name, WALKER_VERSION_MAJOR,
            WALKER_VERSION_MINOR, WALKER_VERSION_PATCH);
}

#endif /* __WALKER_VERSION_H__ */
