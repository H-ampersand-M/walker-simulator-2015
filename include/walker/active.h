/**
 * \file active.h
 * \brief Active way.
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

#ifndef __WALKER_ACTIVE_H__
#define __WALKER_ACTIVE_H__

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sysexits.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>

////////////////////////////////////////////////////////////////////////////////
// Active way queue initialization.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Open the shared memory for activity.
 * \retval true in case of success.
 * \retval false otherwise.
 */
bool wlk_open_activity_memory (void);

/**
 * \brief Close the shared memory for activity.
 * \retval true in case of success.
 * \retval false otherwise.
 */
bool wlk_close_activity_memory (void);

/**
 * \brief Unlink the shared memory for activity.
 * \retval true in case of success.
 * \retval false otherwise.
 */
bool wlk_unlink_activity_memory (void);

////////////////////////////////////////////////////////////////////////////////
// Activity status.
////////////////////////////////////////////////////////////////////////////////

size_t wlk_get_active_way (void);
void wlk_set_active_way (size_t way);
size_t wlk_round_robin (void);

////////////////////////////////////////////////////////////////////////////////
// Input / Output.
////////////////////////////////////////////////////////////////////////////////

void wlk_debug_activity (void);

#endif /* __WALKER_ACTIVE_H__ */
