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
 * @file udp_interface_arduino.c
 * @brief Implements a UDP interface for Arduino with Mbed OS.
 * @author  Hatim Jamali & Mohamed Abdelmaksoud
 * @date 01 August 2023
 */


#ifndef NCE_IOT_C_SDK_H_
    #include <nce_iot_c_sdk.h>
#endif


#include "udp_interface_arduino.h"
#include "GSM.h"
#include "MbedUdp.h"


GSMUDP sdkclient;


int nce_os_udp_connect( OSNetwork_t osnetwork,
                         OSEndPoint_t nce_oboarding  )
{
    int ret;
    ret = sdkclient.begin(nce_oboarding.port);
    ret = sdkclient.beginPacket(nce_oboarding.host, nce_oboarding.port);

    if (ret == 1) {
        return 0;
    }

    return ret;
}
int nce_os_udp_send( OSNetwork_t osnetwork,
                     void * pBuffer,
                     size_t bytesToSend )
{
    int ret;
    ret = sdkclient.write( (uint8_t const*) pBuffer, bytesToSend);
    ret = sdkclient.endPacket();
    return ret;
}


int nce_os_udp_recv( OSNetwork_t osnetwork,
                     void * pBuffer,
                     size_t bytesToRecv )
{
    int ret ;
    for (int i = 0; i <= 10; i++) {
    ret = sdkclient.parsePacket();
        if (ret > 0 ) {
            break;
        }
        delay(1000);     
    }

  if (ret) {
    sdkclient.read((unsigned char*)pBuffer, bytesToRecv);
  }
    return ret;
}

int nce_os_udp_disconnect( OSNetwork_t osnetwork )
{
    sdkclient.stop();
    return 0;
}
