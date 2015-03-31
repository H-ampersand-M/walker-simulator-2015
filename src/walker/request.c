/**
 * \file request.c
 * \brief Requests.
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

#include "walker/request.h"

////////////////////////////////////////////////////////////////////////////////
// Private typedefs, enum, etc.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Walker requests.
 *
 * A request value can be used in two fashions :
 * - a way request boolean
 * - a way selector
 */
typedef struct wlk_request
{
    struct timespec date;   /**< The request's date. */
    size_t value;           /**< The request value. */
} wlk_request;

////////////////////////////////////////////////////////////////////////////////
// Constants
////////////////////////////////////////////////////////////////////////////////

#define WAY_NUMBER 3
static const long int REQUEST_QUEUE_MAX = 8;
static const char * REQUEST_QUEUE_NAME = "/walker_request_queue";

////////////////////////////////////////////////////////////////////////////////
// Static variables.
////////////////////////////////////////////////////////////////////////////////

static mqd_t request_queue;

static wlk_request requests[WAY_NUMBER] =
{
    { .date = { 0, 0 }, .value = 0 },
    { .date = { 0, 0 }, .value = 0 },
    { .date = { 0, 0 }, .value = 0 },
};

////////////////////////////////////////////////////////////////////////////////
// Request queue initialization.
////////////////////////////////////////////////////////////////////////////////

bool wlk_open_request_queue (void)
{
    struct mq_attr attributes;

    attributes.mq_flags = 0;
    attributes.mq_msgsize = sizeof (wlk_request);
    attributes.mq_maxmsg = REQUEST_QUEUE_MAX;
    attributes.mq_curmsgs = 0;

    mq_unlink (REQUEST_QUEUE_NAME);
    request_queue = mq_open (REQUEST_QUEUE_NAME,
            O_RDWR | O_CREAT | O_EXCL | O_NONBLOCK,
            S_IRUSR | S_IWUSR,
            & attributes);
    if (request_queue == (mqd_t) - 1)
    {
        perror ("mq_open");
        exit (EX_OSERR);
    }

    return true;
}

bool wlk_close_request_queue (void)
{
    int success = mq_close (request_queue);
    if (success == -1)
        perror ("mq_close");
    return success != -1;
}

////////////////////////////////////////////////////////////////////////////////
// Request exchanges.
////////////////////////////////////////////////////////////////////////////////

bool wlk_send_request (size_t i)
{
    struct timespec now;
    clock_gettime (CLOCK_REALTIME, & now);

    struct wlk_request request = { .value = i - 1, .date = now, };
    int success = mq_send (request_queue, (char *) & request, sizeof request, 0);

    if (success == -1)
    {
        fprintf (stderr, "\x1B[1m\x1B[31mwlk_send_request (%lu): ", i);
        perror ("mq_send");
        fprintf (stderr, "\x1B[0m");
    }

    return success != - 1;
}

bool wlk_pull_requests (size_t ignore)
{
    bool pulled = false;
    wlk_request request;
    struct mq_attr attributes;

    int get_attr_success = mq_getattr (request_queue, & attributes);
    if (get_attr_success == -1)
    {
        perror ("mq_getattr");
        return false;
    }

    --ignore;
    for (long i = 0; i < attributes.mq_curmsgs; ++i)
    {
        ssize_t success = mq_receive (request_queue, (char *) & request, sizeof request, NULL);
        if (success == -1)
        {
            fprintf (stderr, "\x1B[1m\x1B[31mwlk_pull_requests (%lu): ", ignore);
            perror ("mq_receive");
            fprintf (stderr, "\x1B[0m");
            continue;
        }

        size_t place = request.value;
        if (place != ignore && ! requests[place].value)
        {
            requests[place].date = request.date;
            requests[place].value = 1;
        }
    }

    return pulled;
}

////////////////////////////////////////////////////////////////////////////////
// Selection.
////////////////////////////////////////////////////////////////////////////////

mqd_t wlk_request_queue (void)
{
    return request_queue;
}

bool wlk_request_pending (void)
{
    bool pending = false;

    for (size_t i = 0; ! pending && i < WAY_NUMBER; ++i)
        pending = requests[i].value;

    return pending;
}

size_t wlk_pop_request (void)
{
#define timespec_to_double(t) (double) (t).tv_sec * 1e9 + (double) (t).tv_nsec

    struct timespec now;
    clock_gettime (CLOCK_REALTIME, & now);

    size_t selected = 0;
    double max = 0.0;

    double now_date = timespec_to_double (now);

    for (size_t i = 0; i < WAY_NUMBER; ++i)
    {
        if (requests[i].value)
        {
            double request_date = timespec_to_double (requests[i].date);
            double difference = now_date - request_date;
            if ((difference) > max)
            {
                max = difference;
                selected = i;
            }
        }
    }

    return selected + 1;
#undef timespec_to_double
}

////////////////////////////////////////////////////////////////////////////////
// Modification.
////////////////////////////////////////////////////////////////////////////////

void wlk_reset_request (size_t i)
{
    --i;
    if (i < WAY_NUMBER)
        requests[i] = (wlk_request) { .date = { 0, 0 }, .value = 0 };
}

////////////////////////////////////////////////////////////////////////////////
// Input / Output.
////////////////////////////////////////////////////////////////////////////////

void wlk_debug_requests (void)
{
    #ifdef DEBUG
    static char buffer[2048];

    sprintf (buffer, "\x1B[37mRequests\n========\n");
    for (size_t i = 0; i < WAY_NUMBER; ++i)
    {
        wlk_request r = requests[i];
        sprintf (& buffer[strlen (buffer)], "[%02lu]\t%8s\t(%10ld, %10ld)\n",
                i + 1, r.value ? "pending" : "none",
                r.date.tv_sec, r.date.tv_nsec);
    }
    sprintf (& buffer[strlen (buffer)], "\x1B[0m\n");

    fprintf (stderr, "%s", buffer);
    #endif
}
