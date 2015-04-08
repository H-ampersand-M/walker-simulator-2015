/**
 * \file main.c
 * \brief Main.
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

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <sysexits.h>
#include <stdbool.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>
#include <unistd.h>
#include <float.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>

#include "walker/version.h"
#include "walker/active.h"
#include "walker/request.h"

#define TIMER_SIG SIGRTMIN
#define NOTIFY_SIG SIGRTMAX

#define SECURITY_DURATION 3
#define MINIMUM_DURATION 3
#define OPTIONAL_DURATION 5

timer_t timer;
pid_t spammer = 0;
pid_t printer = 0;

static bool set_sigaction (int signal, void (* action) (int, siginfo_t *, void *))
{
    struct sigaction sa;
    sa.sa_sigaction = action;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset (& sa.sa_mask);

    int success = sigaction (signal, & sa, NULL);

    if (success == -1)
        perror ("sigaction");

    return success != -1;
}

static bool wlk_queue_notify (mqd_t queue, int signo)
{
    struct sigevent event;

    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = signo;
    event.sigev_value = (union sigval) { .sival_int = 0, };
    int success = mq_notify (queue, & event);

    if (success == -1)
        perror ("mq_notify");

    return success != -1;
}

static void init_timer (int signal)
{
    struct sigevent event;
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = signal;
    event.sigev_value = (union sigval) { .sival_int = 0, };

    timer_create (CLOCK_REALTIME, & event, & timer);
}

static void set_one_shot_timer (time_t seconds)
{
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = seconds;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;

    timer_settime (timer, 0, & timer_spec, NULL);
}

static void walker_interface (void)
{
    for (;;)
    {
        /* printf ("\x1B[1m%lu\x1B[0m \x1B[37m>> \x1B[0m", wlk_get_active_way ()); */

        unsigned int route = 0;
        errno = 0;
        int scan = scanf ("%u", & route); scanf ("%*[^\n]%*[\n]");

        if (errno == EINTR)
            #ifdef DEBUG
            fprintf (stderr, "\x1B[1m\x1B[38;5;220mwalker_interface (): "
                    "`scanf (\"%%u%%*[^\\n]%%*[\\n]\")` interrupted!\x1B[0m\n");
            #else
            printf ("\n");
            #endif

        if (scan == 1)
            wlk_send_request (route);
    }
}

static void security (void)
{
    wlk_set_active_way (0);

    set_one_shot_timer (SECURITY_DURATION);

    /* Wait for the SIGRTMIN signals. */
    sigset_t set;
    sigfillset (& set);
    sigdelset (& set, SIGRTMIN);
    sigsuspend (& set);
}

static void set_next_request (void)
{
    size_t next;

    wlk_debug_requests ();

    if (wlk_request_pending ())
        next = wlk_pop_request ();
    else
        next = wlk_round_robin ();

    wlk_set_active_way (next);
}

static void request_pending (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    set_one_shot_timer (0);
    wlk_queue_notify (wlk_request_queue (), NOTIFY_SIG);
    wlk_pull_requests (wlk_get_active_way ());
}

static void way (void)
{
    set_one_shot_timer (MINIMUM_DURATION);

    /* Wait for the SIGRTMIN signals. */
    sigset_t set;
    sigfillset (& set);
    sigdelset (& set, SIGRTMIN);
    sigdelset (& set, SIGINT);
    sigprocmask (SIG_SETMASK, & set, NULL);
    sigsuspend (& set);

    ///////////////////////////////////////////

    sigdelset (& set, NOTIFY_SIG);

    wlk_pull_requests (wlk_get_active_way ());
    if (! wlk_request_pending ())
    {
        set_one_shot_timer (OPTIONAL_DURATION);
        sigsuspend (& set);
    }

    sigprocmask (SIG_SETMASK, & set, NULL);
    wlk_reset_request (wlk_get_active_way ());
}

static void sigrtmin_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;
    return;
}

static void sigint_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    /* Erase the ^C. */
    if (printer)
        fprintf (stderr, "\b\b  \n");

    wlk_close_request_queue ();
    wlk_close_activity_memory ();

    if (printer)
    {
        kill (spammer, SIGINT);
        kill (printer, SIGINT);
        wait (NULL);
        wait (NULL);

        mq_unlink ("/walker_request_queue");
        wlk_unlink_activity_memory ();
    }

    exit (EXIT_SUCCESS);
}

/**
 * \brief Main function.
 * \param argc Command line argument count.
 * \param argv Command line arguments.
 * \retval EXIT_SUCCESS on success.
 * \retval EX_USAGE on user misbehaviour.
 */
int main (int argc, char ** argv)
{
    --argc; argv++;
    if (argc > 0)
    {
        fprintf (stderr, "Error: too many arguments.\n");
        exit (EX_USAGE);
    }

    fprintf_version (stderr, "Walker Simulator 2015");

    set_sigaction (SIGINT, sigint_action);

    wlk_open_request_queue ();
    wlk_open_activity_memory ();

    wlk_set_active_way (0);

    spammer = fork ();
    if (spammer)
    {
        printer = fork ();

        if (printer)
        {
            init_timer (TIMER_SIG);
            wlk_queue_notify (wlk_request_queue (), NOTIFY_SIG);
            set_sigaction (NOTIFY_SIG, request_pending);
            set_sigaction (TIMER_SIG, sigrtmin_action);

            for (;;)
            {
                set_next_request ();
                way ();
                security ();
            }
        }
        else
        {
            size_t last = wlk_get_active_way ();
            printf ("\r\x1B[1m%lu\x1B[0m \x1B[37m>> \x1B[0m", last);
            fflush (stdout);

            size_t current;
            for (;;)
            {
                current = wlk_get_active_way ();
                if (current != last)
                {
                    last = current;
                    printf ("\r\x1B[1m%lu\x1B[0m \x1B[37m>> \x1B[0m", last);
                    fflush (stdout);
                }
            }
        }
    }
    else
    {
        walker_interface ();
    }

    exit (EXIT_SUCCESS);
}
