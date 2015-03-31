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

static const long int ACTIVITY_QUEUE_MAX = 8;
static const char * ACTIVITY_QUEUE_NAME = "/walker_activity_queue";

////////////////////////////////////////////////////////////////////////////////
// Static variables.
////////////////////////////////////////////////////////////////////////////////

static const size_t WAY_NUMBER = 3;
size_t active_way = 0;
size_t last_way = 1;
static mqd_t activity_queue;

////////////////////////////////////////////////////////////////////////////////
// Active way queue initialization.
////////////////////////////////////////////////////////////////////////////////

bool wlk_open_activity_queue (void)
{
    struct mq_attr attributes;

    attributes.mq_flags = 0;
    attributes.mq_msgsize = sizeof active_way;
    attributes.mq_maxmsg = ACTIVITY_QUEUE_MAX;
    attributes.mq_curmsgs = 0;

    mq_unlink (ACTIVITY_QUEUE_NAME);
    activity_queue = mq_open (ACTIVITY_QUEUE_NAME,
            O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK,
            S_IRUSR | S_IWUSR,
            & attributes);
    if (activity_queue == (mqd_t) - 1)
    {
        perror ("mq_open");
        exit (EX_OSERR);
    }

    return true;
}

bool wlk_close_activity_queue (void)
{
    int success = mq_close (activity_queue);

    if (success == -1)
        perror ("mq_close");

    return success != -1;
}

////////////////////////////////////////////////////////////////////////////////
// Activity exchanges.
////////////////////////////////////////////////////////////////////////////////

mqd_t wlk_activity_queue (void)
{
    return activity_queue;
}

bool wlk_send_activity (void)
{
    int success = mq_send (activity_queue, (char *) & active_way,
            sizeof active_way, 0);

    if (success == -1)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_send_activity (): ");
        perror ("mq_send");
        fprintf (stderr, "\x1B[0m");
    }

    return success != - 1;
}

bool wlk_receive_activity (void)
{
    struct mq_attr attributes;
    int attr_succ = mq_getattr (activity_queue, & attributes);

    if (attr_succ == - 1)
    {
        fprintf (stderr, "\x1B[1m\31mwlk_receive_activity (): ");
        perror ("mq_getattr");
        fprintf (stderr, "\x1B[0m");

        return false;
    }

    #ifdef DEBUG
    if (attributes.mq_curmsgs > 1)
        fprintf (stderr, "\x1B[1m\x1B[38;5;214mActivity count: %ld\x1B[0m\n", attributes.mq_curmsgs);
    #endif

    ssize_t success = -1;;
    for (long i = 0; i < attributes.mq_curmsgs; i++)
    {
        size_t new_way = 0;
        success = mq_receive (activity_queue, (char *) & new_way,
                sizeof new_way, 0);

        if (success == -1)
        {
            fprintf (stderr, "\x1B[1m\x1B[31mwlk_receive_activity (): ");
            perror ("mq_receive");
            fprintf (stderr, "\x1B[0m");
        }
        else
            wlk_set_active_way (new_way);
    }

    return success != -1;
}

////////////////////////////////////////////////////////////////////////////////
// Activity status.
////////////////////////////////////////////////////////////////////////////////

size_t wlk_get_active_way (void)
{
    return active_way;
}

void wlk_set_active_way (size_t way)
{
    active_way = way;
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
    fprintf (stderr, "\x1B[37mActive way: %lu\x1B[0m\n\n", active_way);
    #endif
}
