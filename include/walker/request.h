/**
 * \file request.h
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

#ifndef __WALKER_REQUEST_H__
#define __WALKER_REQUEST_H__

#ifndef _GNU_SOURCE
    #define _GNU_SOURCE
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sysexits.h>
#include <float.h>
#include <time.h>
#include <mqueue.h>
#include <string.h>

#include "walker/tools.h"

////////////////////////////////////////////////////////////////////////////////
// Request queue initialization.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Open the message queue for requests.
 * \retval true in case of success.
 * \retval false otherwise.
 */
bool wlk_open_request_queue (void);

/**
 * \brief Close the message queue for requests.
 * \retval true in case of success.
 * \retval false otherwise.
 */
bool wlk_close_request_queue (void);

////////////////////////////////////////////////////////////////////////////////
// Request exchanges.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Send a request for the ith way.
 * \param i The way id.
 * \retval true in case of success.
 * \retval false otherwise.
 */
bool wlk_send_request (size_t i);

/**
 * \brief Pull pending requests.
 * \param ignore The way id to ignore.
 * \retval true if requests were pulled.
 * \retval false otherwise.
 */
bool wlk_pull_requests (size_t ignore);

////////////////////////////////////////////////////////////////////////////////
// Selection.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Get the current request queue descriptor.
 * \return The request queue descriptor.
 */
mqd_t wlk_request_queue (void);

/**
 * \brief Determine whether there are pending requests.
 * \retval true if there are pending requests.
 * \retval false otherwise.
 */
bool wlk_request_pending (void);

/**
 * \brief Select a request.
 * \return The requested way.
 *
 * If more than one request are pending, the most urgent will be selected.
 */
size_t wlk_select_request (void);

////////////////////////////////////////////////////////////////////////////////
// Modification.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Reset the ith request.
 */
void wlk_reset_request (size_t i);

////////////////////////////////////////////////////////////////////////////////
// Input / Output.
////////////////////////////////////////////////////////////////////////////////

/**
 * \brief Print all requests.
 */
void wlk_debug_requests (void);

#endif /* __WALKER_REQUEST_H__ */
