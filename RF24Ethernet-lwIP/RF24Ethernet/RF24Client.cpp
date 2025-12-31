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

#if USE_LWIP != 1
uip_userdata_t RF24Client::all_data[UIP_CONNS];
#else
#define LWIP_ERR_T uint32_t
//
#if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_ARCH_ESP8266
  #include "lwip\tcp.h"
  #include "lwip/tcpip.h"
  #include "lwip/timeouts.h"
#else
  #include "lwip\include\lwip\tcp.h"
  #include "lwip\include\lwip\tcpip.h"
#endif

#include "RF24Ethernet.h"


RF24Client::ConnectState RF24Client::gState;
char RF24Client::incomingData[INCOMING_DATA_SIZE];
uint16_t RF24Client::dataSize =0;
struct tcp_pcb* RF24Client::myPcb;
bool RF24Client::serverActive;


// Called when the remote host acknowledges receipt of data
err_t RF24Client::sent_callback(void *arg, struct tcp_pcb *tpcb, u16_t len) {


	Serial.println("sent cb");
    ConnectState* state = (ConnectState*)arg;
	if(state){
      state->waiting_for_ack = false; // Data is successfully out
	  state->finished = true;
    }else{
		
		Serial.println("^^^^^^^^^ NO STATE ^^^^^^^^^^^^^^^^^");
	}
	//state->finished = true;
    return ERR_OK;
}



err_t RF24Client::blocking_write(struct tcp_pcb* fpcb, ConnectState* fstate, const char* data, size_t len) {
    

	
	//Serial.println("blk write");

	if (!fpcb || !fstate->connected){
		Serial.print("tx with no connection: ");
		Serial.println(fstate->connected);
		bool isConnected = false;
		if(!fpcb){
			Serial.println("nofpbcb");
		}else{
			uint32_t timr = millis() + 1000;
			
			while(!fstate->connected){
				Ethernet.tick();
				if(millis() > timr){
			      fpcb = nullptr;
		          myPcb = nullptr;
				  break;
				}
				if(fstate->connected){
					isConnected = true;
				}
			}			

		}
		if(!isConnected){
		  return ERR_CLSD;
		}
	}
 
Serial.print("blk write 1: ");
	uint16_t available_space = tcp_sndbuf(fpcb);
	//Serial.println(available_space);
	//Serial.println(len);
	uint32_t timeout = millis() + 1000;
	while(len > available_space){
		Ethernet.tick();
		
		if(millis() > timeout){
			Serial.println("********** tx timeout *******");
			return ERR_BUF;
		}
		//Ethernet.tick();
	}

fstate->waiting_for_ack = true;   
	err_t err = tcp_write(fpcb, data, len, TCP_WRITE_FLAG_COPY);
//Ethernet.tick();
    if (err != ERR_OK) {
        fstate->waiting_for_ack = false;
		fstate->finished = true;
	    Serial.println("BLK Write fail 2");		
        return err;
    }
	
//	Serial.println("blk write 2.2");
    if(fpcb && fpcb->state != CLOSED && fstate->connected){
      tcp_sent(fpcb, sent_callback);
	}else{
	  Serial.print(" TCP OUT FAIL 2: ");
	  return ERR_BUF;
	}
	

//Serial.println("blk write 0.1");
//	Serial.println("wait");
	volatile uint32_t timer = millis() + 5000;
    while (fstate->waiting_for_ack && !fstate->finished) {
		if(millis() > timer){ //Serial.println("blk write 0.2");
			//fstate->waiting_for_ack = false;
			fstate->finished = true;
			fstate->result = -1;
			break;			
		}
		Ethernet.tick();
    }
//	Serial.println("blk write 3");
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
//Serial.println("srecv cb");

	ConnectState* state = (ConnectState*)arg;
	
    if (p == nullptr) {
        state->connected = false;
        //state->finished = true; // Break the loop
        tcp_close(tpcb);
	    myPcb = nullptr;
		
        return ERR_OK;
    }
    if (err != ERR_OK || state == nullptr) {
        if (p) pbuf_free(p);
        return ERR_OK;
    }

    const uint8_t* data = static_cast<const uint8_t*>(p->payload);
		
	if(dataSize + p->len < INCOMING_DATA_SIZE){
        memcpy(&incomingData[dataSize], data, p->len );
	    dataSize += p->len;
	}else{
		Serial.println("srecv: Out of incoming buffer space");
	}

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
	
	if(dataSize + p->len < INCOMING_DATA_SIZE){
        memcpy(&incomingData[dataSize], data, p->len );
	    dataSize += p->len;
	}else{
		Serial.println("recv: Out of incoming buffer space");
	}

    // Process data
    tcp_recved(tpcb, p->len);
    pbuf_free(p);
    return ERR_OK;
}


err_t RF24Client::accept(void *arg, struct tcp_pcb *tpcb, err_t err) {
	//Serial.println("acc cb");
	ConnectState* state = (ConnectState*)arg;


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
Serial.println("conn cb");

	ConnectState* state = (ConnectState*)arg;
	if(state){
      state->result = err;
      state->finished = true;
      state->connected = true;
	  state->waiting_for_ack = false;
	}
    return ERR_OK;
}



#endif

/*************************************************************/
#if USE_LWIP != 1
RF24Client::RF24Client() : data(NULL) {}
#else
RF24Client::RF24Client() : data(0) {}

#endif
/*************************************************************/

#if USE_LWIP != 1
RF24Client::RF24Client(uip_userdata_t* conn_data) : data(conn_data) {}
#else
RF24Client::RF24Client(uint32_t data) : data(0) {}	
#endif
/*************************************************************/

uint8_t RF24Client::connected()
{
	#if USE_LWIP != 1
    return (data && (data->packets_in != 0 || (data->state & UIP_CLIENT_CONNECTED))) ? 1 : 0;
	#else
	return gState.connected;
	#endif
}

/*************************************************************/



int RF24Client::connect(IPAddress ip, uint16_t port)
{

#if USE_LWIP != 1
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

if( myPcb != nullptr ){
	if( myPcb->state == ESTABLISHED || myPcb->state == SYN_SENT || myPcb->state == SYN_RCVD  ){
	  tcp_close(myPcb);
	  Ethernet.tick();
      return false;
	}
}
		
		myPcb = tcp_new();
		if(!myPcb){
			return 0;			
		}
		dataSize = 0;
	    memset(incomingData,0,sizeof(incomingData));
		gState.finished = false;
		gState.connected = false;
		gState.result = 0;
	    gState.waiting_for_ack = false;
		
    tcp_arg(myPcb, &gState);
    tcp_err(myPcb, error_callback);
    tcp_recv(myPcb, recv_callback);
	
	
	err_t err = ERR_OK;
	ip4_addr_t myIp;
	#if defined ARDUINO_ARCH_ESP32 || defined ARDUINO_ARCH_ESP8266
	IP4_ADDR(&myIp, ip[0], ip[1], ip[2], ip[3]);
	ip_addr_t generic_addr;
	ip_addr_copy_from_ip4(generic_addr, myIp);
    err = tcp_connect(myPcb, &generic_addr, port, on_connected);
	#else
	IP4_ADDR(&myIp, ip[0], ip[1], ip[2], ip[3]);
    // Start non-blocking connection
    err = tcp_connect(myPcb, &myIp, port, on_connected);
    #endif

	//uint32_t *test = &ip4_addr_get_u32(&myIp);
	//uint8_t test2 = *test & 0xFF;
	
	//Serial.println(test2);
    if (err != ERR_OK) {
		if(myPcb){
  		  tcp_abort(myPcb);
		  myPcb = nullptr;
		}
		gState.connected = false;
		gState.finished = true;

		return err;
	}
    uint32_t timeout = millis() + 5000;
    // Simulate blocking by looping until the callback sets 'finished'
    while (!gState.finished && millis() < timeout) {
         Ethernet.tick();
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
#if USE_LWIP != 1
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
	
if(serverActive){
    uint32_t timeout = millis() + 1000;
	while(!gState.finished && gState.waiting_for_ack && millis() < timeout){ RF24Ethernet.tick();}
	if(myPcb){
    if( myPcb->state == ESTABLISHED || myPcb->state == SYN_SENT || myPcb->state == SYN_RCVD  ){
	  gState.connected = false;
	  tcp_close(myPcb);	  
	}
	}
	RF24Server::restart();
}else{
	gState.connected = false;
	if(myPcb){
	if( myPcb->state == ESTABLISHED || myPcb->state == SYN_SENT || myPcb->state == SYN_RCVD  ){
      tcp_close(myPcb);
	}
	}
}
	
	RF24Ethernet.tick();
#endif
}

/*************************************************************/

// the next function allows us to use the client returned by
// EthernetServer::available() as the condition in an if-statement.
bool RF24Client::operator==(const RF24Client& rhs)
{
	#if USE_LWIP != 1
    return data && rhs.data && (data == rhs.data);
	#else
	return dataSize > 0 ? true : false;
	#endif
	
}

/*************************************************************/

RF24Client::operator bool()
{
    Ethernet.tick();
	#if USE_LWIP != 1
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
#if USE_LWIP != 1
size_t RF24Client::_write(uip_userdata_t* u, const uint8_t* buf, size_t size)
#else
size_t RF24Client::_write(uint8_t* data, const uint8_t* buf, size_t size)

#endif

{

#if USE_LWIP != 1
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
	
	
	if(myPcb == nullptr){
		return -1;
	}
    char buffer[size];
	uint32_t position = 0;
	uint32_t timeout1 = millis() + 1000;
	
	while(size > MAX_PAYLOAD_SIZE-14 && millis() < timeout1){
		uint32_t timeout = millis() + 1000;
	  while(myPcb->snd_queuelen >= TCP_SND_QUEUELEN && millis() < timeout){
	    Ethernet.tick();
	  }
      memcpy(buffer, &buf[position], MAX_PAYLOAD_SIZE-14);
	  err_t write_err = blocking_write(myPcb, &gState, buffer, MAX_PAYLOAD_SIZE-14);
	  position += MAX_PAYLOAD_SIZE-14;
	  size -= MAX_PAYLOAD_SIZE-14;
	  Ethernet.tick();

    }
	timeout1 = millis() + 1000;
	while(myPcb->snd_queuelen >= TCP_SND_QUEUELEN && millis() < timeout1){
	    Ethernet.tick();
	}
	memcpy(buffer, &buf[position], size);
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
#if USE_LWIP != 1
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
#if USE_LWIP != 1
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
	#if USE_LWIP != 1
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
#if USE_LWIP != 1
int RF24Client::_available(uip_userdata_t* u)
#else
int RF24Client::_available(uint8_t *data)
#endif
{
	#if USE_LWIP != 1
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
#if USE_LWIP != 1
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
		#if USE_LWIP != 1
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
	#if USE_LWIP != 1
    if (*this)
    {
		#if USE_LWIP != 1
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
