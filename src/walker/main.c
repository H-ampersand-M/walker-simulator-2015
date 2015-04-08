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

#include <readline/readline.h>
#include <readline/history.h>

#include <getopt.h>

#include "walker/version.h"
#include "walker/active.h"
#include "walker/request.h"
#include "walker/tools.h"

#define TIMER_SIG SIGRTMIN
#define NOTIFY_SIG SIGRTMAX

#define SECURITY_DURATION 1
#define MINIMUM_DURATION 7
#define OPTIONAL_DURATION 23

timer_t timer;
pid_t interface = 0;

static char prompt[128];

static bool set_sigaction (int signal, void (* action) (int, siginfo_t *, void *))
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

static bool wlk_queue_notify (mqd_t queue, int signo)
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
        sprintf (prompt, "%lu >> ", wlk_get_active_way ());
        char * read_string = readline (prompt);

        unsigned int route = 0;
        int scan = sscanf (read_string, "%u", & route);

        if (scan == 1)
            wlk_send_request (route);
    }
}

static void security (void)
{
    wlk_set_active_way (0);
    sigqueue (interface, SIGRTMAX, (union sigval) { .sival_int = 0 });

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
    sigqueue (interface, SIGRTMAX, (union sigval) { .sival_int = 0 });
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

static void sigrtmax_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    sprintf (prompt, "%lu >> ", wlk_get_active_way ());

    int saved_point;
    char * saved_line;

    bool save = (rl_readline_state & RL_STATE_READCMD) > 0;

    if (save)
    {
        saved_point = rl_point;
        saved_line = rl_copy_text (0, rl_end);
        rl_replace_line ("", 0);
        rl_redisplay ();
    }

    rl_set_prompt (prompt);

    if (save)
    {
        rl_replace_line (saved_line, 0);
        rl_point = saved_point;
    }

    rl_redisplay ();

    if (save)
        free (saved_line);

    return;
}


static void sigint_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    /* Erase the ^C. */
    fprintf (stderr, "\b\b  \n");

    wlk_close_request_queue ();
    wlk_close_activity_memory ();

    if (interface)
    {
        kill (interface, SIGINT);
        wait (NULL);

        mq_unlink ("/walker_request_queue");
        wlk_unlink_activity_memory ();
    }

    exit (EXIT_SUCCESS);
}

static void __print_help (void)
{
    fprintf (stderr, "\x1B[1mwalker-simulator-2015\x1B[0m [OPTIONS]\n\n");
    fprintf (stderr, "\x1B[1mOPTIONS\x1B[0m\n\n");

    fprintf (stderr, "\t\x1B[1m-h\x1B[0m, \x1B[1m--help\x1B[0m\n");
    fprintf (stderr, "\t\tPrint this help.\n");

    fprintf (stderr, "\t\x1B[1m-v\x1B[0m, \x1B[1m--version\x1B[0m\n");
    fprintf (stderr, "\t\tPrint the version.\n");

    fprintf (stderr, "\n");

    fprintf_version (stderr, "Walker Simulator 2015");
}
static void __parse_args (int argc, char ** argv)
{
    static const struct option walker_options[] =
    {
        { "version",    no_argument, NULL, 'v', },
        { "help",       no_argument, NULL, 'h', },
        { 0, 0, 0, 0, },
    };

    bool unknown_option = false;
    int val = 1;
    opterr = 0;

    do
    {
        int longindex;
        val = getopt_long (argc, argv, "vh", walker_options,
                & longindex);

        switch (val)
        {
            case 'v':
                fprintf_version (stderr, "Walker Simulator 2015");
                exit (EXIT_SUCCESS);
                break;
            case 'h':
                __print_help ();
                exit (EXIT_SUCCESS);
                break;
            case '?':
                fprintf (stderr, "\x1B[1m\x1B[31mError: Unknown option \"%s\".\x1B[0m\n",
                        argv[optind-1]);
                unknown_option = true;
            default:
                break;
        }
    }
    while (val != - 1);

    if (unknown_option)
        exit (EX_USAGE);
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
    __parse_args (argc, argv);

    printf ("\x1B[1m\x1B[32mWelcome!\x1B[0m\n");

    memset (prompt, 0, sizeof prompt);

    set_sigaction (SIGINT, sigint_action);
    set_sigaction (SIGRTMAX, sigrtmax_action);

    wlk_open_request_queue ();
    wlk_open_activity_memory ();

    wlk_set_active_way (0);

    interface = fork ();
    if (interface)
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
        walker_interface ();
    }

    exit (EXIT_SUCCESS);
}

