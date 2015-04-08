/**
 * \file tools.h
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

#ifndef __WALKER_TOOLS_H__
#define __WALKER_TOOLS_H__

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>
#include <sysexits.h>
#include <stdarg.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <mqueue.h>
#include <time.h>

////////////////////////////////////////////////////////////////////////////////
// Debugging formatted printers.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief printf(3) when DEBUG is defined.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see printf(3)
 */
int debug (const char * format, ...);

/**
 * \brief fprintf(3) when DEBUG is defined.
 * \param stream The destination stream.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see fprintf(3)
 */
int fdebug (FILE * stream, const char * format, ...);

/**
 * \brief sprintf(3) when DEBUG is defined.
 * \param str The destination string.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see sprintf(3)
 */
int sdebug (char * str, const char * format, ...);

/**
 * \brief sprintf(3) when DEBUG is defined.
 * \param str The destination string.
 * \param size The maximum number of characters to write.
 * \param format The format string.
 * \retval 0 if DEBUG is not defined
 * \return The number of written characters.
 * \see sprintf(3)
 */
int sndebug (char * str, size_t size, const char * format, ...);

////////////////////////////////////////////////////////////////////////////////
// Error printing.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Print an error message in red.
 * \param stream Destination stream.
 * \param location The function where the error occurred.
 * \param function_name The function that produced the error.
 * \return The number of printed characters.
 */
int fperror (FILE * stream, const char * location, const char * function_name);

////////////////////////////////////////////////////////////////////////////////
// Various utilities.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Set a sigaction.
 * \param signal Signal.
 * \param action Function to call when the signal is received.
 * \retval true on success.
 * \retval false otherwise.
 */
bool set_sigaction (int signal, void (* action) (int, siginfo_t *, void *));

/**
 * \brief Make the queue notify the caller with a signal on message reception.
 * \param queue The notifying queue.
 * \param signo The signal to send.
 * \retval true on success.
 * \retval false otherwise.
 */
bool wlk_queue_notify (mqd_t queue, int signo);

/**
 * \brief Initialize a timer.
 * \param signal signal.
 * \param timer timer.
 */
void init_timer (int signal, timer_t * timer);

/**
 * \brief Set a one shot timer.
 * \param timer timer.
 * \param seconds duration.
 */
void set_one_shot_timer (timer_t timer, time_t seconds);

#endif /* __WALKER_TOOLS_H__ */
