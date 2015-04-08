/**
 * \file active.c
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

#include "walker/active.h"

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief The name of the shared memory dedicated to the activity.
 */
static const char * ACTIVITY_MEMORY_NAME = "/walker_activity_memory";

/**
 * \brief The total number of ways.
 */
static const size_t WAY_NUMBER = 3;

////////////////////////////////////////////////////////////////////////////////
// Static variables.
////////////////////////////////////////////////////////////////////////////////

size_t last_way = 3;                    /**< The last active way. */
static int activity_memory;             /**< Descriptor for the shared memory. */
static size_t * active_way = NULL;      /**< The location of the shared memory. */

////////////////////////////////////////////////////////////////////////////////
// Active way queue initialization.
////////////////////////////////////////////////////////////////////////////////

bool wlk_open_activity_memory (void)
{
    activity_memory = shm_open (ACTIVITY_MEMORY_NAME,
            O_RDWR | O_CREAT | O_TRUNC,
            S_IRUSR | S_IWUSR);

    int success = ftruncate (activity_memory, sizeof (size_t));

    if (success == -1)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_open_activity_memory (): ");
        perror ("ftruncate");
        fprintf (stderr, "\x1B[0m");
        exit (EX_OSERR);
    }

    if (activity_memory == -1)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_open_activity_memory (): ");
        perror ("shm_open");
        fprintf (stderr, "\x1B[0m");
        exit (EX_OSERR);
    }

    active_way = mmap (NULL, sizeof (size_t), PROT_READ | PROT_WRITE,
            MAP_SHARED, activity_memory, 0);

    if (active_way == MAP_FAILED)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_open_activity_memory (): ");
        perror ("mmap");
        fprintf (stderr, "\x1B[0m");
        exit (EX_OSERR);
    }

    return true;
}

bool wlk_close_activity_memory (void)
{
    int success = munmap (active_way, sizeof (size_t));

    if (success == -1)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_close_activity_memory (): ");
        perror ("munmap");
        fprintf (stderr, "\x1B[0m");
    }

    return success != -1;
}

bool wlk_unlink_activity_memory (void)
{
    int success = shm_unlink (ACTIVITY_MEMORY_NAME);

    if (success == -1)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_unlink_activity_memory (): ");
        perror ("shm_unlink");
        fprintf (stderr, "\x1B[0m");
    }

    return success != -1;
}

////////////////////////////////////////////////////////////////////////////////
// Activity status.
////////////////////////////////////////////////////////////////////////////////

size_t wlk_get_active_way (void)
{
    return * active_way;
}

void wlk_set_active_way (size_t way)
{
    * active_way = way;
}

size_t wlk_round_robin (void)
{
    last_way++;
    if (last_way > WAY_NUMBER)
        last_way -= WAY_NUMBER;
    return last_way;
}

////////////////////////////////////////////////////////////////////////////////
// Input / Output.
////////////////////////////////////////////////////////////////////////////////

void wlk_debug_activity (void)
{
    #ifdef DEBUG
    fprintf (stderr, "\x1B[37mActive way: %lu\x1B[0m\n\n", * active_way);
    #endif
}
