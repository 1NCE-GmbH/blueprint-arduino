/*
 * MIT License
 *
 * Copyright (c) 2023 1NCE
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

/**
 * @file udp_interface_arduino.h
 * @brief UDP interface definitions to send and receive data over the
 * network via UDP in Arduino with Mbed OS.
 * @author  Hatim Jamali & Mohamed Abdelmaksoud
 * @date 01 August 2023
 */


#include "interface/udp_interface.h"
#include <string.h>

/**
 * @typedef OSNetwork_t
 */
struct OSNetwork
{
    int os_socket;
};

int nce_os_udp_connect( OSNetwork_t osnetwork,
                     OSEndPoint_t nce_oboarding );

int nce_os_udp_send( OSNetwork_t osnetwork,
                     void * pBuffer,
                     size_t bytesToSend );

int nce_os_udp_recv( OSNetwork_t osnetwork,
                     void * pBuffer,
                     size_t bytesToRecv );

int nce_os_udp_disconnect( OSNetwork_t osnetwork );

