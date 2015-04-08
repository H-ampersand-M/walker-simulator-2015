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

////////////////////////////////////////////////////////////////////////////////
// Error printing.
////////////////////////////////////////////////////////////////////////////////

int fperror (FILE * stream, const char * location, const char * function_name)
{
    char buffer[256];

    /* Warning: There are two versions strerror_r() ! We are using the GNU
     * version (we defined _GNU_SOURCE in the tools.h header).
     */
    char * error_string = strerror_r (errno, buffer, 256);

    /* Print... in RED ! */
    return fprintf (stream, "\x1B[1m\x1B[31m%s: %s: %s\n\x1B[0m", location,
            function_name, error_string);
}

////////////////////////////////////////////////////////////////////////////////
// Various utilities.
////////////////////////////////////////////////////////////////////////////////


bool set_sigaction (int signal, void (* action) (int, siginfo_t *, void *))
{
    struct sigaction sa;
    sa.sa_sigaction = action;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset (& sa.sa_mask);

    int success = sigaction (signal, & sa, NULL);

    if (success == -1)
        fperror (stderr, "set_sigaction", "sigaction");

    return success != -1;
}

bool wlk_queue_notify (mqd_t queue, int signo)
{
    struct sigevent event;

    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = signo;
    event.sigev_value = (union sigval) { .sival_int = 0, };
    int success = mq_notify (queue, & event);

    if (success == -1)
        fperror (stderr, "wlk_queue_notify", "mq_notify");

    return success != -1;
}


void init_timer (int signal, timer_t * timer)
{
    struct sigevent event;
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = signal;
    event.sigev_value = (union sigval) { .sival_int = 0, };

    timer_create (CLOCK_REALTIME, & event, timer);
}

void set_one_shot_timer (timer_t timer, time_t seconds)
{
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = seconds;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;

    timer_settime (timer, 0, & timer_spec, NULL);
}

