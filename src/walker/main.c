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

/* #ifndef _POSIX_PRIORITY_SCHEDULING */
/*     #error "No POSIX priority scheduling." */
/* #endif */

#include "walker/version.h"

#define NOTIFY_SIG SIGRTMAX

static const int MAX_MESSAGES = 256;
timer_t timer;
static pid_t other;
static mqd_t queue;
static const char * QUEUE_NAME = "/walker_texas_ranger";

typedef struct way_request
{
    int integer;
    struct timespec date;
} way_request;

static int current = 0;
static mqd_t current_queue;

static way_request requests[3] =
{
    {0, {0,0}},
    {0, {0,0}},
    {0, {0,0}},
};

static void current_changed (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;
    int prio = 0;
    if (mq_receive (current_queue, & current, sizeof current, & prio) == -1)
    {
        perror ("mq_receive");
        fprintf (stderr, "current_changed eats ass!!!!\n");
    }

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    sev.sigev_value.sival_ptr = & current_queue;
    mq_notify (current_queue, & sev);

    printf ("Current: %d\n", current);
}

static void show_requests (void)
{
    for (size_t i = 0; i < 3; ++i)
    {
        way_request r = requests[i];
        fprintf (stderr, "[%u]: %d %d %d\n", i, r.integer, r.date.tv_sec, r.date.tv_nsec);
    }
}

static void walker_interface (void)
{
    int route = 0;

    struct sigevent sev;
    sev.sigev_notify = SIGEV_SIGNAL;
    sev.sigev_signo = NOTIFY_SIG;
    sev.sigev_value.sival_ptr = & current_queue;
    mq_notify (current_queue, & sev);

    struct sigaction sa;
    sa.sa_sigaction = current_changed;
    sa.sa_flags = SA_SIGINFO;
    sigemptyset (& sa.sa_mask);
    sigaction (NOTIFY_SIG, & sa, NULL);

    for (;;)
    {
        printf ("> ");
        if (scanf ("%d", & route) == 1)
        {
            struct timespec now;
            clock_gettime (CLOCK_REALTIME, & now);

            struct way_request r = { route, now };

            mq_send (queue, (char *) & r, sizeof r, 0);
        }
    }
}

static void security (void)
{
    /* Sigevent. */
    struct sigevent event;
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGRTMIN;
    event.sigev_value = (union sigval) { .sival_int = 0, };

    /* Timer information. */
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 1;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;

    /* Set the timer. */
    timer_create (CLOCK_REALTIME, & event, & timer);
    timer_settime (timer, 0, & timer_spec, NULL);

    /* Wait for the SIGRTMIN signals. */
    sigset_t set;
    sigfillset (& set);
    sigdelset (& set, SIGRTMIN);
    sigsuspend (& set);
}

static void peek_queue (void)
{
    struct mq_attr attr;
    way_request r;
    int prio = 0;
    mq_getattr (queue, & attr);
    for (long i = 0; i < attr.mq_curmsgs; ++i)
    {
        mq_receive (queue, (char *) & r, sizeof r, NULL);
        int j = r.integer;
        if (! requests[j].integer && current != j)
        {
            requests[r.integer].date = r.date;
            requests[r.integer].integer = 1;
        }
    }
}

static bool has_request (void)
{
    bool has = false;
    for (size_t i = 0; ! has && i < 3; ++i)
    {
        if (requests[i].integer)
        {
            fprintf (stderr, "has_requests: %lu, %d\n", i, requests[i].integer);
            has = true;
        }
    }
    return has;
}

static void set_next_request (void)
{
    /* show_requests (); */
    if (has_request ())
    {
        fprintf (stderr, "set_next: has_request ?!\n");
        struct timespec now;
        clock_gettime (CLOCK_REALTIME, & now);

        int selected = 0;
        double min = DBL_MAX;
        double now_d = ((double) now.tv_sec * 1e9 + (double) now.tv_nsec);
        for (size_t i = 0; i < 3; ++i)
        {
            if (requests[i].integer)
            {
                double t = ((double) requests[i].date.tv_sec * 1e9 + (double) requests[i].date.tv_nsec);
                if (now_d - t < min)
                {
                    min = now_d - t;
                    selected = i;
                }
            }
        }

        current = selected;
        printf ("selected: %d; current: %d\n", selected, current);
    }
    else
        current = (current + 1) % 3;

    fprintf (stderr, "||| current %d\n", current);
    int prio = SIGRTMIN;
    if (mq_send (current_queue, & current, sizeof current, prio) == -1)
    {
        {
            perror ("mq_send");
            fprintf (stderr, "alqsdfjlqsdfj\n");
        }
    }
    fprintf (stderr, "||| current %d\n", current);
}

static void request_pending (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 0;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;

    timer_settime (timer, 0, & timer_spec, NULL);

    peek_queue ();
}

static void way (void)
{
    fprintf (stderr, "way start\n");

    /* Sigevent. */
    struct sigevent event;
    event.sigev_notify = SIGEV_SIGNAL;
    event.sigev_signo = SIGRTMIN;
    event.sigev_value = (union sigval) { .sival_int = 0, };

    /* Timer information. */
    struct itimerspec timer_spec;
    timer_spec.it_value.tv_sec = 3;
    timer_spec.it_value.tv_nsec = 0;
    timer_spec.it_interval.tv_sec = 0;
    timer_spec.it_interval.tv_nsec = 0;

    /* Set the timer. */
    timer_create (CLOCK_REALTIME, & event, & timer);
    timer_settime (timer, 0, & timer_spec, NULL);

    /* Wait for the SIGRTMIN signals. */
    sigset_t set;
    sigfillset (& set);
    sigdelset (& set, SIGRTMIN);
    sigprocmask (SIG_SETMASK, & set, NULL);
    sigsuspend (& set);

    fprintf (stderr, "way 7sec end\n");
    ///////////////////////////////////////////

    sigdelset (& set, NOTIFY_SIG);
    peek_queue ();
    if (! has_request ())
    {
        fprintf (stderr, "way 23sec start\n");
        timer_spec.it_value.tv_sec = 5;
        timer_settime (timer, 0, & timer_spec, NULL);
        sigsuspend (& set);
        fprintf (stderr, "way 23sec end (why ??????????)\n");
    }
    sigprocmask (SIG_SETMASK, & set, NULL);

    requests[current].integer = 0;
}

static void sigrtmin_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;
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
    /* fprintf (stderr, "This program does not do anything yet.\n"); */

    /* Open the queue. */
    struct mq_attr attrs;
    attrs.mq_flags = 0;
    attrs.mq_msgsize = sizeof (way_request);
    attrs.mq_maxmsg = 5;
    attrs.mq_curmsgs = 0;
    mq_unlink (QUEUE_NAME);
    queue = mq_open(QUEUE_NAME, O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,
            & attrs);

    if (queue == -1)
    {
        fprintf (stderr, "%s\n", QUEUE_NAME);
        perror ("mq_open");
        exit (1);
    }

    attrs.mq_flags = 0;
    attrs.mq_msgsize = sizeof current;
    attrs.mq_maxmsg = 5;
    attrs.mq_curmsgs = 0;
    mq_unlink ("/walker_current");
    current_queue = mq_open("/walker_current", O_RDWR | O_CREAT | O_EXCL, S_IRUSR | S_IWUSR,
            & attrs);

    if (current_queue == -1)
    {
        fprintf (stderr, "%s\n", "/walker_current");
        perror ("mq_open");
        exit (1);
    }


    other = fork ();
    if (other)
    {
        struct sigevent sev;
        sev.sigev_notify = SIGEV_SIGNAL;
        sev.sigev_signo = NOTIFY_SIG;
        sev.sigev_value.sival_ptr = & queue;
        mq_notify (queue, & sev);

        struct sigaction sa;
        sa.sa_sigaction = request_pending;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset (& sa.sa_mask);
        sigaction (NOTIFY_SIG, & sa, NULL);

        /* Set the SIGRTMIN behaviour. */
        sa.sa_sigaction = sigrtmin_action;
        sa.sa_flags = SA_SIGINFO;
        sigemptyset (& sa.sa_mask);
        sigaction (SIGRTMIN, & sa, NULL);

        for (;;)
        {
            way ();
            security ();
            set_next_request ();
        }
    }
    else
    {
        walker_interface ();
    }

    exit (EXIT_SUCCESS);
}
