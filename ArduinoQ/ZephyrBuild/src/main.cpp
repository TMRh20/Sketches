#include "myCompat.h"

#include <RF24.h>
#include <RF24Network.h>
#include "RF24Mesh.h"
#include <RF24Ethernet.h>
#include <RF24Client.h>
#include <RF24Server.h>
#include <Dns.h>


RF24 radio(7, 8);
RF24Network network(radio);
RF24Mesh mesh(radio, network);
RF24EthernetClass RF24Ethernet(radio, network, mesh);

EthernetClient client;

IPAddress ascii(10, 1, 3, 1);
IPAddress host(ascii);
void connect();

void setup() {

 
    pinMode(LED3_R, OUTPUT); // Red LED (Usually LED3_R under the hood)
    pinMode(LED3_G, OUTPUT);      // Green LED
    pinMode(LED3_B, OUTPUT);      // Blue LED
    
    digitalWrite(LED3_R, HIGH);
    digitalWrite(LED3_G, HIGH);
    digitalWrite(LED3_B, HIGH);

  Serial.begin(115200);
  while (!Serial) {}

  Serial.begin(115200);
  while (!Serial1) {}
  
  Serial2.begin(115200);
  while (!Serial2) {}
  
  Serial.println(F("Start0"));
  Serial1.println(F("Start1"));
  Serial2.println(F("Start2"));
  
  // Set the IP address we'll be using. The last octet mast match the nodeID (9)
  IPAddress myIP(10, 1, 3, 98);
  // If you'll be making outgoing connections from the Arduino to the rest of
  // the world, you'll need a gateway set up.
  IPAddress gwIP(10, 1, 3, 1);
  Ethernet.set_gateway(gwIP);

  Ethernet.begin(myIP);
  radio.begin();
  radio.setPALevel(RF24_PA_LOW,0);
  mesh.begin(60);
  
}

uint32_t counter = 0;
uint32_t reqTimer = 0;
uint32_t mesh_timer = 0;
uint32_t clientTimeoutTimer = 0;

void loop() {


  // Optional: If the node needs to move around physically, or using failover nodes etc.,
  // enable address renewal
  if (millis() - mesh_timer > 12000) {  //Every 12 seconds, test mesh connectivity

    mesh_timer = millis();
    if (!mesh.checkConnection()) {
      Serial.println("Renew");
      //refresh the network address
      if (mesh.renewAddress() == MESH_DEFAULT_ADDRESS) {
        mesh.begin(60);
      }
    }
  }

  size_t size;

  while ((size = client.available()) > 0) {
    char c = client.read();
    Serial.print(c);
    counter++;
    clientTimeoutTimer = millis();
  }

  // if the server's disconnected, stop the client:
  if (!client.connected()) {
    Serial.println();
    Serial.println(F("Disconnect. Waiting for disconnect timeout"));
    client.stop();

    // Wait 5 seconds between requests
    // Calling client.available(); or Ethernet.update(); is required during delays
    // to keep the stack updated
    reqTimer = millis();
    while (millis() - reqTimer < 5000 && !client.available()) {
    }
    connect();
  }

  // We need to implement a disconnection timer, in case we lose connection
  // and the server stops responding
  // Note: If using the uIP stack (not lwIP), there is internal timeout functionality
  if (client.connected()) {
    if (millis() - clientTimeoutTimer > 60000) {
      client.stop();
    }
  }

  // We can do other things in the loop, but be aware that the loop will
  // briefly pause while IP data is being processed.
}

void connect() {
  digitalWrite(LED3_R, LOW);
  digitalWrite(LED3_G, HIGH);
  Serial.println(F("connecting"));

  if (client.connect(host, 80)) {
    
    digitalWrite(LED3_R, HIGH);
    digitalWrite(LED3_G, LOW);
    Serial.println(F("connected"));
    clientTimeoutTimer = millis();

    // Make an HTTP request:
    if (host == ascii) {
      client.println("GET /ST HTTP/1.1");
      client.println("Host: 10.1.3.1");
    }

    client.println("Connection: close");
    client.println();

  } else {
    // if you didn't get a connection to the server:
    Serial.println(F("connection failed"));
    mesh.renewAddress();
  }
}
