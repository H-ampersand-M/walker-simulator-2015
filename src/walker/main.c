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
bool compact = false;
bool pretty = false;

static char prompt[128];

////////////////////////////////////////////////////////////////////////////////
// Static utility functions.
////////////////////////////////////////////////////////////////////////////////

static void walker_interface (void);
static void security (void);
static void set_next_request (void);
static void way (void);

////////////////////////////////////////////////////////////////////////////////
// Handlers.
////////////////////////////////////////////////////////////////////////////////

static void request_pending (int a, siginfo_t * b, void * c);
static void sigrtmin_action (int a, siginfo_t * b, void * c);
static void sigrtmax_action (int a, siginfo_t * b, void * c);
static void sigint_action (int a, siginfo_t * b, void * c);

////////////////////////////////////////////////////////////////////////////////
// Main utilities.
////////////////////////////////////////////////////////////////////////////////

static void print_end (void);
static void print_help (void);
static void parse_args (int argc, char ** argv);

////////////////////////////////////////////////////////////////////////////////
// Main.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Main function.
 * \param argc Command line argument count.
 * \param argv Command line arguments.
 * \retval EXIT_SUCCESS on success.
 * \retval EX_USAGE on user misbehaviour.
 */
int main (int argc, char ** argv)
{
    parse_args (argc, argv);

    printf ("\x1B[1m\x1B[34mWelcome to Walker Simulator 2015!\x1B[0m\n");

    memset (prompt, 0, sizeof prompt);

    set_sigaction (SIGINT, sigint_action);
    set_sigaction (SIGRTMAX, sigrtmax_action);

    wlk_open_request_queue ();
    wlk_open_activity_memory ();

    wlk_set_active_way (0);

    interface = fork ();
    if (interface)
    {
        init_timer (TIMER_SIG, & timer);
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

////////////////////////////////////////////////////////////////////////////////
// Static utility functions implementations.
////////////////////////////////////////////////////////////////////////////////

void walker_interface (void)
{
    for (;;)
    {
        /* Set the prompt with the currently active way, if any. */
        char * read_string = NULL;
        if (compact)
        {
            sprintf (prompt, "%lu >> ", wlk_get_active_way ());
            read_string = readline (prompt);
        }
        else
        {
            read_string = readline ("texas_ranger >> ");
        }

        if (! strcmp ("ragequit", read_string))
            sigqueue (getppid (), SIGINT, (union sigval) { .sival_int = 0 });

        unsigned int route = 0;
        int scan = sscanf (read_string, "%u", & route);

        /* If the input is correct, send the request. */
        if (scan == 1)
            wlk_send_request (route);

        /* Lines returned by readline(3) are allocated with malloc(3) ! */
        free (read_string);
    }
}

void security (void)
{
    /* Switch off all ways and notify the interface. */
    wlk_set_active_way (0);
    sigqueue (interface, SIGRTMAX, (union sigval) { .sival_int = 0 });

    /* Set the timer. */
    set_one_shot_timer (timer, SECURITY_DURATION);

    /* Wait for the timer to expire. */
    sigset_t set;
    sigfillset (& set);
    sigdelset (& set, SIGRTMIN);
    sigsuspend (& set);
}

void set_next_request (void)
{
    size_t next;

    wlk_debug_requests ();

    /* When we arrive here, no way is active. */
    if (wlk_request_pending (-1))
        next = wlk_select_request ();
    else
        next = wlk_round_robin ();

    /* Set the new active way and notify the interface. */
    wlk_set_active_way (next);
    sigqueue (interface, SIGRTMAX, (union sigval) { .sival_int = 0 });
}

void way (void)
{
    /* Set the timer for the minimum duration. */
    set_one_shot_timer (timer, MINIMUM_DURATION);

    /* Wait for the timer to expire. */
    sigset_t set;
    sigfillset (& set);
    sigdelset (& set, SIGRTMIN);
    sigdelset (& set, SIGINT);
    sigprocmask (SIG_SETMASK, & set, NULL);
    sigsuspend (& set);

    /* The timer has expired, this way can, if needed, be closed. */
    sigdelset (& set, NOTIFY_SIG);

    /* Check whether there are requests to pull. */
    size_t active = wlk_get_active_way ();
    wlk_pull_requests (active);
    if (! wlk_request_pending (active - 1))
    {
        /* If this way does not need to be closed, set a new timer for the
         * remaining time and wait for it (or for a request). */
        set_one_shot_timer (timer, OPTIONAL_DURATION);
        sigsuspend (& set);
    }

    sigprocmask (SIG_SETMASK, & set, NULL);
    wlk_reset_request (wlk_get_active_way ());
}

////////////////////////////////////////////////////////////////////////////////
// Handlers.
////////////////////////////////////////////////////////////////////////////////

void request_pending (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    set_one_shot_timer (timer, 0);
    wlk_queue_notify (wlk_request_queue (), NOTIFY_SIG);
    wlk_pull_requests (wlk_get_active_way ());
}

void sigrtmin_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;
    return;
}

void sigrtmax_action (int a, siginfo_t * b, void * c)
{
    (void) a; (void) b; (void) c;

    if (compact)
        sprintf (prompt, "%lu >> ", wlk_get_active_way ());

    int saved_point;
    char * saved_line;

    bool save = (rl_readline_state & RL_STATE_READCMD) > 0;

    if (save)
    {
        saved_point = rl_point;
        saved_line = rl_copy_text (0, rl_end);
        if (! compact)
            rl_save_prompt ();
        rl_replace_line ("", 0);
        rl_redisplay ();
    }

    if (compact)
        rl_set_prompt (prompt);
    else
        wlk_print_ways (pretty);

    if (save)
    {
        if (! compact)
            rl_restore_prompt ();
        rl_replace_line (saved_line, 0);
        rl_point = saved_point;
    }

    rl_redisplay ();

    if (save)
        free (saved_line);

    return;
}

void sigint_action (int a, siginfo_t * b, void * c)
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
        print_end();
    }


    exit (EXIT_SUCCESS);
}

////////////////////////////////////////////////////////////////////////////////
// Main utilities.
////////////////////////////////////////////////////////////////////////////////

void parse_args (int argc, char ** argv)
{
    static const struct option walker_options[] =
    {
        { "pretty",    no_argument, NULL, 'p', },
        { "chuck-norris",    no_argument, NULL, 'c', },
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
        val = getopt_long (argc, argv, "pcvh", walker_options,
                & longindex);

        switch (val)
        {
            case 'p':
                pretty = true;
                break;
            case 'c':
                compact = true;
                break;
            case 'v':
                fprintf_version (stderr, "Walker Simulator 2015");
                exit (EXIT_SUCCESS);
                break;
            case 'h':
                print_help ();
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


void print_help (void)
{
    fprintf (stderr, "\x1B[1mwalker-simulator-2015\x1B[0m [OPTIONS]\n\n");
    fprintf (stderr, "\x1B[1mOPTIONS\x1B[0m\n\n");

    fprintf (stderr, "\t\x1B[1m-h\x1B[0m, \x1B[1m--help\x1B[0m\n");
    fprintf (stderr, "\t\tPrint this help.\n");

    fprintf (stderr, "\t\x1B[1m-v\x1B[0m, \x1B[1m--version\x1B[0m\n");
    fprintf (stderr, "\t\tPrint the version.\n");

    fprintf (stderr, "\t\x1B[1m-c\x1B[0m, \x1B[1m--chuck-norris\x1B[0m\n");
    fprintf (stderr, "\t\tChuck Norris does not need detailed information.\n");

    fprintf (stderr, "\n");

    fprintf_version (stderr, "Walker Simulator 2015");
}

void print_end (void)
{
    printf ("%s",
            "\n    Hasta la vista baby\n\n"
            "     _.-^^---....,,--\n"
            " _--                  --_\n"
            "<                        >)\n"
            "|                         |\n"
            " \\._                   _./\n"
            "    ```--. . , ; .--'''\n"
            "          | |   |\n"
            "       .-=||  | |=-.\n"
            "       `-=#$%&%$#=-'\n"
            "          | ;  :|\n"
            " _____.,-#%&$@%#&#~,._____ \n");
}
