/*
 RF24Client.cpp - Arduino implementation of a uIP wrapper class.
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

#define UIP_TCP_PHYH_LEN UIP_LLH_LEN + UIP_IPTCPH_LEN

#ifndef USE_LWIP
uip_userdata_t RF24Client::all_data[UIP_CONNS];
#else
#define LWIP_ERR_T uint32_t

    #include "lwip\include\lwip\tcp.h"
	#include "lwip\include\lwip\ip_addr.h"
#include "RF24Ethernet.h"


RF24Client::ConnectState RF24Client::gState;
static char incomingData[MAX_PAYLOAD_SIZE*2] __attribute__((aligned(4)));
static uint16_t dataSize = 0;
struct tcp_pcb* RF24Client::myPcb;
//static struct tcp_pcb* sPcb;
//bool RF24Client::serverActive;


// Called when the remote host acknowledges receipt of data
err_t RF24Client::sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {
myPcb = tpcb;
	Serial.println("sent cb");
    ConnectState* state = (ConnectState*)arg;
    state->waiting_for_ack = false; // Data is successfully out
    return ERR_OK;
}


err_t RF24Client::blocking_write(struct tcp_pcb* fpcb, ConnectState* fstate, const char* data, size_t len) {
    Serial.println("blk write");
myPcb = fpcb;
	if (!fpcb || !fstate->connected){
		Serial.println(fstate->connected);
		if(!fpcb){
			Serial.println("nofpbcb");
			}
		
		return ERR_CLSD;
	}
    fstate->waiting_for_ack = true;
Serial.println("blk write 1");
	err_t err = tcp_write(fpcb, data, len, TCP_WRITE_FLAG_COPY);

    if (err != ERR_OK) {
        fstate->waiting_for_ack = false;
		Serial.println("BLK Write fail 2");
        return err;
    }
Serial.println("blk write 2");
    tcp_output(fpcb);
    tcp_sent(fpcb, sent_callback);

    uint32_t timer = millis() + 10000;
    while (fstate->waiting_for_ack && !fstate->finished && millis() < timer) {
        sys_check_timeouts(); 
        Ethernet.update();
    }
	Serial.println("blk write 3");
    return fstate->result;
}

void RF24Client::error_callback(void *arg, err_t err) {
    ConnectState* state = (ConnectState*)arg;
    if (state) {
        state->result = err;
        state->connected = false;
        state->finished = true; // Break the blocking loop
		state->waiting_for_ack = false;
    }
}


err_t RF24Client::srecv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
Serial.println("recv cb");

	ConnectState* state = (ConnectState*)arg;
	myPcb = tpcb;
    if (p == nullptr) {
        state->connected = false;
        state->finished = true; // Break the loop
        tcp_close(tpcb);
	    myPcb = nullptr;
		
        return ERR_OK;
    }
    if (err != ERR_OK || state == nullptr) {
        if (p) pbuf_free(p);
        return ERR_OK;
    }

    const uint8_t* data = static_cast<const uint8_t*>(p->payload);
		
		memcpy(&incomingData[dataSize], data, p->len );
		dataSize += p->len;

    // Process data
    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}



err_t RF24Client::recv_callback(void *arg, struct tcp_pcb *tpcb, struct pbuf *p, err_t err) {
Serial.println("recv cb");

	ConnectState* state = (ConnectState*)arg;
    if (p == nullptr) {
        state->connected = false;
        state->finished = true; // Break the loop
        tcp_close(tpcb);
	    myPcb = nullptr;
        return ERR_OK;
    }
    if (err != ERR_OK || state == nullptr) {
        if (p) pbuf_free(p);
        return ERR_OK;
    }

    const uint8_t* data = static_cast<const uint8_t*>(p->payload);
		
		memcpy(&incomingData[dataSize], data, p->len );
		dataSize += p->len;

    // Process data
    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}


err_t RF24Client::accept(void *arg, struct tcp_pcb *tpcb, err_t err) {
	Serial.println("acc cb");
	ConnectState* state = (ConnectState*)arg;
	myPcb = tpcb;

	tcp_recv(tpcb, srecv_callback);
	tcp_sent(tpcb, sent_callback);
    state->result = err;
    state->finished = false;
    state->connected = true;
	state->waiting_for_ack = false;
    return ERR_OK;
}
	
// Callback triggered by lwIP when handshake completes

err_t RF24Client::on_connected(void *arg, struct tcp_pcb *tpcb, err_t err) {

	ConnectState* state = (ConnectState*)arg;
    state->result = err;
    state->finished = true;
    state->connected = true;
	state->waiting_for_ack = false;
    return ERR_OK;
}



#endif

/*************************************************************/
#ifndef USE_LWIP
RF24Client::RF24Client() : data(NULL) {}
#else
RF24Client::RF24Client() : data(0) {}

#endif
/*************************************************************/

#ifndef USE_LWIP
RF24Client::RF24Client(uip_userdata_t* conn_data) : data(conn_data) {}
#else
RF24Client::RF24Client(uint32_t data) : data(0) {}	
#endif
/*************************************************************/

uint8_t RF24Client::connected()
{
	#ifndef USE_LWIP
    return (data && (data->packets_in != 0 || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
	#else
	return gState.connected;
	#endif
}

/*************************************************************/



int RF24Client::connect(IPAddress ip, uint16_t port)
{

#ifndef USE_LWIP
#if UIP_ACTIVE_OPEN > 0

    // do{

    stop();
    uip_ipaddr_t ipaddr;
    uip_ip_addr(ipaddr, ip);

    struct uip_conn* conn = uip_connect(&ipaddr, htons(port));

    if (conn)
    {
    #if UIP_CONNECTION_TIMEOUT > 0
        uint32_t timeout = millis();
    #endif

        while ((conn->tcpstateflags & UIP_TS_MASK) != UIP_CLOSED)
        {
            RF24EthernetClass::tick();

            if ((conn->tcpstateflags & UIP_TS_MASK) == UIP_ESTABLISHED)
            {
                data = (uip_userdata_t*)conn->appstate;
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(millis()); Serial.print(F(" connected, state: ")); Serial.print(data->state); Serial.print(F(", first packet in: ")); Serial.println(data->packets_in););
                return 1;
            }

    #if UIP_CONNECTION_TIMEOUT > 0
            if ((millis() - timeout) > UIP_CONNECTION_TIMEOUT)
            {
                conn->tcpstateflags = UIP_CLOSED;
                break;
            }
    #endif
        }
    }
    // delay(25);
    // }while(millis()-timer < 175);

#endif // Active open enabled
#else

if(gState.connected == true){
	tcp_abort(myPcb);
	sys_check_timeouts(); 
    Ethernet.tick();
	gState.connected = false;
	return false;

}
		sys_check_timeouts(); 
        Ethernet.tick();
		if(myPcb != nullptr){
  		  tcp_close(myPcb);
		}
		myPcb = tcp_new();
		dataSize = 0;
	    memset(incomingData,0,sizeof(incomingData));
		Ethernet.RXQueue.nWrite = 0;
        Ethernet.RXQueue.nRead = 0;
		for(int i=0; i< Ethernet.MAX_RX_QUEUE; i++){
		  Ethernet.RXQueue.len[i] = 0;
		}
		gState.finished = false;
		gState.connected = false;
		gState.result = 0;
	    gState.waiting_for_ack = false;
		
    tcp_arg(myPcb, &gState);
    tcp_err(myPcb, error_callback);
    tcp_recv(myPcb, recv_callback);
	
	ip4_addr_t myIp;
	IP4_ADDR(&myIp, ip[0], ip[1], ip[2], ip[3]);
    // Start non-blocking connection
    err_t err = tcp_connect(myPcb, &myIp, port, on_connected);
	uint32_t *test = &ip4_addr_get_u32(&myIp);
	uint8_t test2 = *test & 0xFF;
	
	Serial.println(test2);
    if (err != ERR_OK) {
		tcp_abort(myPcb);
		gState.connected = false;
		gState.finished = true;
		myPcb = nullptr;
		return err;
	}
    uint32_t timeout = millis() + 5000;
    // Simulate blocking by looping until the callback sets 'finished'
    while (!gState.finished && millis() < timeout) {
         sys_check_timeouts(); 
         Ethernet.update();
    }
    return gState.connected;



#endif
    return 0;
}

/*************************************************************/

int RF24Client::connect(const char* host, uint16_t port)
{
    // Look up the host first
    int ret = 0;

#if UIP_UDP
    DNSClient dns;
    IPAddress remote_addr;

    dns.begin(RF24EthernetClass::_dnsServerAddress);
    ret = dns.getHostByName(host, remote_addr);

    if (ret == 1)
    {
    #if defined(ETH_DEBUG_L1) || #defined(RF24ETHERNET_DEBUG_DNS)
        Serial.println(F("*UIP Got DNS*"));
    #endif
        return connect(remote_addr, port);
    }
#else  // ! UIP_UDP
    // Do something with the input parameters to prevent compile time warnings
    if (host) {
    };
    if (port) {
    };
#endif // ! UIP_UDP

#if defined(ETH_DEBUG_L1) || defined(RF24ETHERNET_DEBUG_DNS)
    Serial.println(F("*UIP DNS fail*"));
#endif

    return ret;
}

/*************************************************************/

void RF24Client::stop()
{
#ifndef USE_LWIP
    if (data && data->state)
    {

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(millis()); Serial.println(F(" before stop(), with data")););

        data->packets_in = 0;
        data->dataCnt = 0;

        if (data->state & UIP_CLIENT_REMOTECLOSED)
        {
            data->state = 0;
        }
        else
        {
            data->state |= UIP_CLIENT_CLOSE;
        }

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("after stop()")););
    }
    else
    {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(millis()); Serial.println(F(" stop(), data: NULL")););
    }

    data = NULL;
    RF24Ethernet.tick();
#else
	
    uint32_t timeout = millis() + 10000;
	while(gState.waiting_for_ack && millis() < timeout){RF24Ethernet.tick();}
    if(myPcb != nullptr){
	  tcp_close(myPcb);
	}
	RF24Server::restart();
	RF24Ethernet.tick();
#endif
}

/*************************************************************/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs)
{
	#ifndef USE_LWIP
    return data && rhs.data && (data == rhs.data);
	#else
	return dataSize > 0 ? true : false;
	#endif
	
}

/*************************************************************/

RF24Client::operator bool()
{
    Ethernet.tick();
	#ifndef USE_LWIP
    return data && (!(data->state & UIP_CLIENT_REMOTECLOSED) || data->packets_in != 0);
	#else
	return dataSize > 0 ? true : false;
	#endif
}

/*************************************************************/

size_t RF24Client::write(uint8_t c)
{
    return _write(data, &c, 1);
}

/*************************************************************/

size_t RF24Client::write(const uint8_t* buf, size_t size)
{
    return _write(data, buf, size);
}

/*************************************************************/
#ifndef USE_LWIP
size_t RF24Client::_write(uip_userdata_t* u, const uint8_t* buf, size_t size)
#else
size_t RF24Client::_write(uint8_t* data, const uint8_t* buf, size_t size)

#endif

{

#ifndef USE_LWIP
    size_t total_written = 0;
    size_t payloadSize = rf24_min(size, UIP_TCP_MSS);

test2:

    RF24EthernetClass::tick();
    if (u && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)) && u->state & (UIP_CLIENT_CONNECTED))
    {

        if (u->out_pos + payloadSize > UIP_TCP_MSS || u->hold)
        {
            goto test2;
        }

        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient.write: writePacket(")); Serial.print(u->packets_out); Serial.print(F(") pos: ")); Serial.print(u->out_pos); Serial.print(F(", buf[")); Serial.print(size - total_written); Serial.print(F("]: '")); Serial.write((uint8_t*)buf + total_written, payloadSize); Serial.println(F("'")););

        memcpy(u->myData + u->out_pos, buf + total_written, payloadSize);
        u->packets_out = 1;
        u->out_pos += payloadSize;

        total_written += payloadSize;

        if (total_written < size)
        {
            size_t remain = size - total_written;
            payloadSize = rf24_min(remain, UIP_TCP_MSS);

            // RF24EthernetClass::tick();
            goto test2;
        }
        u->hold = false;
        return u->out_pos;
    }
    u->hold = false;
    return -1;
#else
	

    char buffer[size];
	memcpy(buffer, buf, size);	
	err_t write_err = blocking_write(myPcb, &gState, buffer, size);
	
	if (write_err == ERR_OK) {
	  return(size);
    }

	return -1;
#endif
}

/*************************************************************/

void uip_log(char* msg)
{
    // Serial.println();
    // Serial.println("** UIP LOG **");
    // Serial.println(msg);
    if (msg)
    {
    };
}

/*************************************************************/
#ifndef USE_LWIP
void serialip_appcall(void)
{
    uip_userdata_t* u = (uip_userdata_t*)uip_conn->appstate;

    /*******Connected**********/
    if (!u && uip_connected())
    {
        IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_connected")););

        u = (uip_userdata_t*)EthernetClient::_allocateData();

        if (u)
        {
            uip_conn->appstate = u;
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(F("UIPClient allocated state: ")); Serial.println(u->state, BIN););
        }
        else
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient allocation failed")););
        }
    }

#if UIP_CONNECTION_TIMEOUT > 0
    if (u && u->connectTimeout > 0) {
        if (millis() - u->connectTimer > u->connectTimeout) {
            u->state |= UIP_CLIENT_CLOSE;
            u->connectTimer = millis();
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println("UIP Client close(timeout)"););
        }
    }
#endif

    /*******User Data RX**********/
    if (u)
    {
        if (uip_newdata())
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.print(F(" UIPClient uip_newdata, uip_len:")); Serial.println(uip_len););
#if UIP_CONNECTION_TIMEOUT > 0
            u->connectTimer = millis();
#endif
            u->hold = (u->out_pos = (u->windowOpened = (u->packets_out = false)));

            if (uip_len && !(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                uip_stop();
                u->state &= ~UIP_CLIENT_RESTART;
                u->windowOpened = false;
                u->restartTime = millis();
                memcpy(&u->myData[u->in_pos + u->dataCnt], uip_appdata, uip_datalen());
                u->dataCnt += uip_datalen();

                u->packets_in = 1;
            }
            goto finish;
        }

        /*******Closed/Timed-out/Aborted**********/
        // If the connection has been closed, save received but unread data.
        if (uip_closed() || uip_timedout() || uip_aborted())
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_closed")););
            // drop outgoing packets not sent yet:
            u->packets_out = 0;

            if (u->packets_in)
            {
                ((uip_userdata_closed_t*)u)->lport = uip_conn->lport;
                u->state |= UIP_CLIENT_REMOTECLOSED;
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient close 1")););
            }
            else
            {
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("UIPClient close 2")););
                u->state = 0;
            }

            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("after UIPClient uip_closed")););
            uip_conn->appstate = NULL;
            goto finish;
        }

        /*******ACKED**********/
        if (uip_acked())
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient uip_acked")););
            u->state &= ~UIP_CLIENT_RESTART;
            u->hold = (u->out_pos = (u->windowOpened = (u->packets_out = false)));
            u->restartTime = millis();
#if UIP_CONNECTION_TIMEOUT > 0
            u->connectTimer = millis();
#endif
        }

        /*******Polling**********/
        if (uip_poll() || uip_rexmit())
        {
            if (uip_rexmit()) {
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.print(F("ReXmit, Len: ")););
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(u->out_pos));
                uip_len = u->out_pos;
                uip_send(u->myData, u->out_pos);
                u->hold = true;
                goto finish;
            }
            // IF_RF24ETHERNET_DEBUG_CLIENT( Serial.println(); Serial.println(F("UIPClient uip_poll")); );

            if (u->packets_out != 0 && !u->hold)
            {
                uip_len = u->out_pos;
                uip_send(u->myData, u->out_pos);
                u->hold = true;
                goto finish;
            }

            // Restart mechanism to keep connections going
            // Only call this if the TCP window has already been re-opened, the connection is being polled, but no data
            // has been acked
            if (!(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {

                if (u->windowOpened == true && u->state & UIP_CLIENT_RESTART && millis() - u->restartTime > u->restartInterval)
                {
                    u->restartTime = millis();
#if defined RF24ETHERNET_DEBUG_CLIENT || defined ETH_DEBUG_L1
                    Serial.println();
                    Serial.print(millis());
    #if UIP_CONNECTION_TIMEOUT > 0
                    Serial.print(F(" UIPClient Re-Open TCP Window, time remaining before abort: "));
                    Serial.println(UIP_CONNECTION_TIMEOUT - (millis() - u->connectTimer));
    #endif
#endif
                    u->restartInterval += 500;
                    u->restartInterval = rf24_min(u->restartInterval, 7000);
                    uip_restart();
                }
            }
        }

        /*******Close**********/
        if (u->state & UIP_CLIENT_CLOSE)
        {
            IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(); Serial.print(millis()); Serial.println(F(" UIPClient state UIP_CLIENT_CLOSE")););

            if (u->packets_out == 0)
            {
                u->state = 0;
                uip_conn->appstate = NULL;
                uip_close();
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("no blocks out -> free userdata")););
            }
            else
            {
                uip_stop();
                IF_RF24ETHERNET_DEBUG_CLIENT(Serial.println(F("blocks outstanding transfer -> uip_stop()")););
            }
        }
finish:;

        if (u->state & UIP_CLIENT_RESTART && !u->windowOpened)
        {
            if (!(u->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                uip_restart();
#if defined ETH_DEBUG_L1
                Serial.println();
                Serial.print(millis());
                Serial.println(F(" UIPClient Re-Open TCP Window"));
#endif
                u->windowOpened = true;
                u->restartInterval = UIP_WINDOW_REOPEN_DELAY; //.75 seconds
                u->restartTime = millis();
            }
        }
    }
}
#endif
/*******************************************************/
#ifndef USE_LWIP
uip_userdata_t* RF24Client::_allocateData()
{
    for (uint8_t sock = 0; sock < UIP_CONNS; sock++)
    {
        uip_userdata_t* data = &RF24Client::all_data[sock];
        if (!data->state)
        {
            data->state = sock | UIP_CLIENT_CONNECTED;
            data->packets_in = 0;
            data->packets_out = 0;
            data->dataCnt = 0;
            data->in_pos = 0;
            data->out_pos = 0;
            data->hold = 0;
            data->restartTime = millis();
            data->restartInterval = 5000;
#if (UIP_CONNECTION_TIMEOUT > 0)
            data->connectTimer = millis();
            data->connectTimeout = UIP_CONNECTION_TIMEOUT;
#endif
            return data;
        }
    }
    return NULL;
}
#endif

int RF24Client::waitAvailable(uint32_t timeout)
{
    uint32_t start = millis();
    while (available() < 1)
    {
        if (millis() - start > timeout)
        {
            return 0;
        }
        RF24Ethernet.tick();
    }
    return available();
}

/*************************************************************/

int RF24Client::available()
{
    RF24Ethernet.tick();
	#ifndef USE_LWIP
    if (*this)
    {
        return _available(data);
    }
	#else
		return _available(data);
	#endif
    return 0;
}

/*************************************************************/
#ifndef USE_LWIP
int RF24Client::_available(uip_userdata_t* u)
#else
int RF24Client::_available(uint8_t *data)
#endif
{
	#ifndef USE_LWIP
    if (u->packets_in)
    {
        return u->dataCnt;
    }
	#else 
	  return dataSize;
	#endif
    return 0;
}

int RF24Client::read(uint8_t* buf, size_t size)
{
#ifndef USE_LWIP
    if (*this)
    {
        if (!data->packets_in)
        {
            return -1;
        }

        size = rf24_min(data->dataCnt, size);
        memcpy(buf, &data->myData[data->in_pos], size);
        data->dataCnt -= size;

        data->in_pos += size;

        if (!data->dataCnt)
        {
            data->packets_in = 0;
            data->in_pos = 0;

            if (uip_stopped(&uip_conns[data->state & UIP_CLIENT_SOCKETS]) && !(data->state & (UIP_CLIENT_CLOSE | UIP_CLIENT_REMOTECLOSED)))
            {
                data->state |= UIP_CLIENT_RESTART;
                data->restartTime = 0;

                IF_ETH_DEBUG_L2(Serial.print(F("UIPClient set restart ")); Serial.println(data->state & UIP_CLIENT_SOCKETS); Serial.println(F("**")); Serial.println(data->state, BIN); Serial.println(F("**")); Serial.println(UIP_CLIENT_SOCKETS, BIN); Serial.println(F("**")););
            }
            else
            {
                IF_ETH_DEBUG_L2(Serial.print(F("UIPClient stop?????? ")); Serial.println(data->state & UIP_CLIENT_SOCKETS); Serial.println(F("**")); Serial.println(data->state, BIN); Serial.println(F("**")); Serial.println(UIP_CLIENT_SOCKETS, BIN); Serial.println(F("**")););
            }

            if (data->packets_in == 0)
            {
                if (data->state & UIP_CLIENT_REMOTECLOSED)
                {
                    data->state = 0;
                    data = NULL;
                }
            }
        }
        return size;
    }

    return -1;
#else
    if(available()){
		if(dataSize >= size){
		   memcpy(&buf[0],&incomingData[0],size);
		   memmove(&incomingData[0], &incomingData[size], dataSize-size); 
		   
		   dataSize -= size;
		   return size;
		}
	}
	return -1;
#endif
}

/*************************************************************/

int RF24Client::read()
{
    uint8_t c;
    if (read(&c, 1) < 0)
        return -1;
    return c;
}

/*************************************************************/

int RF24Client::peek()
{
    if (available())
    {
		#ifndef USE_LWIP
        return data->myData[data->in_pos];
		#else
		return incomingData[0];
		#endif
    }
    return -1;
}

/*************************************************************/

void RF24Client::flush()
{
	#ifndef USE_LWIP
    if (*this)
    {
		#ifndef USE_LWIP
        data->packets_in = 0;
        data->dataCnt = 0;
		#else
		data = 0;
		#endif
    }
	#else
		while(available()){
			read();
		}
	#endif
}
