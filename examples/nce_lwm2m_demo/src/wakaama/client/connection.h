/*******************************************************************************
 *
 * Copyright (c) 2013, 2014 Intel Corporation and others.
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v2.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v20.html
 * The Eclipse Distribution License is available at
 *    http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    David Navarro, Intel Corporation - initial API and implementation
 *
 *******************************************************************************/

#ifndef CONNECTION_H_
#define CONNECTION_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "stddef.h"
#include <stdint.h>

#define LWM2M_STANDARD_PORT_STR "5683"
#define LWM2M_STANDARD_PORT      5683
#define LWM2M_DTLS_PORT_STR     "5684"
#define LWM2M_DTLS_PORT          5684
#define LWM2M_BSSERVER_PORT_STR "5683"
#define LWM2M_BSSERVER_PORT      5683

typedef struct connection_t
{
    struct connection_t *  next;
    int                sock;
} connection_t;


connection_t * connection_find(connection_t * connList, char * addr, size_t addrLen);
connection_t * connection_new_incoming(connection_t * connList, int sock, char * addr, size_t addrLen);
void connection_free(connection_t * connList);

/**
 *  @brief network_interface: The operations to be implemented.
 */
struct network_interface
{
void (* connection_close)(void);
connection_t * (* connection_create)(connection_t * connList, int sock, char * host, char * port, int addressFamily);
int (* connection_send)(connection_t *connP, uint8_t * buffer, size_t length);

};

typedef struct network_interface network_interface_t;

#ifdef __cplusplus
}
#endif

#endif
