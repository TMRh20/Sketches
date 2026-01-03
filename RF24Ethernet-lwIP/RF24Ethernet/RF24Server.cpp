/*
 RF24Server.cpp - Arduino implementation of a uIP wrapper class.
 Copyright (c) 2014 tmrh20@gmail.com, github.com/TMRh20
 Copyright (c) 2013 Norbert Truchsess <norbert.truchsess@t-online.de>
 All rights reserved.

 This program is free software: you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation, either version 3 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program.  If not, see <http://www.gnu.org/licenses/>.
  */
#include "RF24Ethernet.h"
#include "RF24Server.h"

extern "C" {
//#include "uip-conf.h"
}

/*************************************************************/
#if USE_LWIP != 1
RF24Server::RF24Server(uint16_t port) : _port(htons(port))
{
}
#else
uint16_t RF24Server::_port;
struct tcp_pcb* RF24Server::sPcb;
bool RF24Server::doOnce;

RF24Server::RF24Server(uint16_t port)
{
    _port = port;
    //RF24Client::sConnectionTimeout = 30000;
}

#endif
/*************************************************************/

RF24Client RF24Server::available()
{

    Ethernet.tick();
#if USE_LWIP != 1
    for (uip_userdata_t* data = &RF24Client::all_data[0]; data < &RF24Client::all_data[UIP_CONNS]; data++)
    {
        if (data->packets_in != 0 && (((data->state & UIP_CLIENT_CONNECTED) && uip_conns[data->state & UIP_CLIENT_SOCKETS].lport == _port) || ((data->state & UIP_CLIENT_REMOTECLOSED) && ((uip_userdata_closed_t*)data)->lport == _port)))
        {
            return RF24Client(data);
        }
    }
#else
    uint32_t data = 1;
    return RF24Client(data);
#endif
    return RF24Client();
}

void RF24Server::restart()
{

#if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    LOCK_TCPIP_CORE();
#endif
    if (sPcb) {
        if (sPcb->state == ESTABLISHED || sPcb->state == SYN_SENT || sPcb->state == SYN_RCVD) {
            tcp_close(sPcb);
        }
        Ethernet.tick();
    }

    if (RF24Client::delayedState == nullptr) {
        RF24Client::delayedState = new RF24Client::ConnectState;
    }
    sPcb = tcp_new();

    RF24Client::serverActive = true;
    tcp_err(sPcb, RF24Client::error_callback);

    if (!doOnce) {
        //Serial.print("bind to port ");
        //Serial.println(RF24Server::_port);
        //ip_set_option(sPcb, SOF_REUSEADDR);
        err_t err = tcp_bind(sPcb, IP_ADDR_ANY, RF24Server::_port);

        if (err != ERR_OK) {
            //Debug print
            Serial.println("unable to bind to port");
        }
    }
    doOnce = true;
    if (RF24Client::gState == nullptr) {
        RF24Client::gState = new RF24Client::ConnectState;
    }
    RF24Client::gState->finished = false;
    RF24Client::gState->connected = false;
    RF24Client::gState->result = 0;
    RF24Client::gState->waiting_for_ack = false;

    sPcb = tcp_listen_with_backlog(sPcb, 1);

    tcp_arg(sPcb, RF24Client::gState);
    tcp_accept(sPcb, RF24Client::accept);
#if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
    UNLOCK_TCPIP_CORE();
#endif

    RF24Ethernet.tick();
}
/*************************************************************/

void RF24Server::begin()
{
#if USE_LWIP != 1
    uip_listen(_port);
#else

    restart();
/*#if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
  LOCK_TCPIP_CORE();
#endif
if(sPcb){
    if( sPcb->state == ESTABLISHED || sPcb->state == SYN_SENT || sPcb->state == SYN_RCVD  ){
      tcp_close(sPcb);
    }
    Ethernet.tick();
}

sPcb = tcp_new();
RF24Client::serverActive = true;
tcp_err(sPcb, RF24Client::error_callback);


///if(!doOnce){
//Serial.print("bind to port ");
//Serial.println(RF24Server::_port);
err_t err = tcp_bind(sPcb, IP_ADDR_ANY, RF24Server::_port);

if (err != ERR_OK) {
    //Debug print
    Serial.println("unable to bind to port");
}
doOnce = true;
    if(!RF24Client::gState){
        RF24Client::gState = new RF24Client::ConnectState;
    }
    RF24Client::gState->finished = false;
    RF24Client::gState->connected = false;
    RF24Client::gState->result = 0;
    RF24Client::gState->waiting_for_ack = false;


sPcb = tcp_listen(sPcb);

tcp_arg(sPcb, RF24Client::gState);
tcp_accept(sPcb, RF24Client::accept);
#if defined RF24ETHERNET_CORE_REQUIRES_LOCKING
  UNLOCK_TCPIP_CORE();
#endif
*/
#endif
    RF24Ethernet.tick();
}

/*************************************************************/
#if defined(ESP32)
void RF24Server::begin(uint16_t port)
{
    _port = port;
    begin();
}
#endif

/*************************************************************/

size_t RF24Server::write(uint8_t c)
{
    return write(&c, 1);
}

/*************************************************************/

size_t RF24Server::write(const uint8_t* buf, size_t size)
{
    size_t ret = 0;
#if USE_LWIP != 1
    for (uip_userdata_t* data = &RF24Client::all_data[0]; data < &RF24Client::all_data[UIP_CONNS]; data++)
    {
        if ((data->state & UIP_CLIENT_CONNECTED) && uip_conns[data->state & UIP_CLIENT_SOCKETS].lport == _port)
            ret += RF24Client::_write(data, buf, size);
    }
#else
    uint8_t data;
    RF24Client::_write(&data, buf, size);
#endif
    return ret;
}

/*************************************************************/

void RF24Server::setTimeout(uint32_t timeout)
{
#if USE_LWIP != 1
    #if UIP_CONNECTION_TIMEOUT > 0
    for (uint8_t i = 0; i < UIP_CONNS; i++) {
        uip_userdata_t* data = &RF24Client::all_data[i];
        if (data) {
            data->connectTimeout = timeout;
        }
    }
    #endif
#else
    //RF24Client::sConnectionTimeout = timeout;
#endif
}