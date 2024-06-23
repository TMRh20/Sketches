/*
* Single channel scanner for XIAO 52840 Sense
*
* Monitoring of a single channel using the built-in radio of the NRF52840
* Scans a single channel for noise using the RSSI measurement feature
* Can also listen for received packets
*
* Used for monitoring level of traffic and noise separately for a given RF24Mesh master node
* Default is RSSI level monitoring, uncomment USE_RX to enable monitoring of received packets also
*
* ** LED Indicators: **
* Blue: Minimal traffic, 
* Green: Medium traffic, 15/100 signals > -65dBm
* Red: Heavy traffic,  25/100 signals > -65dBm
* Orange: Very heavy traffic, 35/100 signals > -65dBm
* 
*/
#include "nrf_to_nrf.h"
#include <RF24Network.h>
#include <RF24Mesh.h>

/**********************************************************/
#define CHANNEL 3   // What channel to scan on
#define MIN_DBM 65  // The minimum RSSI (dBm) for counting signals. Higher value == greater sensitivity.
#define USE_RX      // Also listen for and count received packets
/**********************************************************/

// Set up nRF24L01 radio on SPI bus plus pins 7 & 8

nrf_to_nrf radio;
RF52Network network(radio);
RF52Mesh mesh(radio, network);
//
// Channel info
//

const uint8_t num_channels = 1;
uint8_t values = 0;
uint8_t valuesR = 0;

//
// LED Setup
//
void red() {
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, HIGH);
}
void green() {
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, LOW);
}
void blue() {
  digitalWrite(LED_BLUE, LOW);
  digitalWrite(LED_RED, HIGH);
  digitalWrite(LED_GREEN, HIGH);
}
void orange() {
  digitalWrite(LED_BLUE, HIGH);
  digitalWrite(LED_RED, LOW);
  digitalWrite(LED_GREEN, LOW);
}



void setup(void) {
  Serial.begin(115200);
  Serial.println(F("\n\rRF24/examples/scanner/"));

  // Setup and configure rf radio
  mesh.setNodeID(0);
  mesh.begin(CHANNEL);
  radio.setAutoAck(false);
  radio.setCRCLength(NRF_CRC_DISABLED);
  radio.setAddressWidth(2);
  radio.setChannel(CHANNEL);
  radio.startListening();
}


const int num_reps = 100;  // Max 255

void loop(void) {


  // Clear measurement values
  values = 0;
  valuesR = 0;

  // Scan channel num_reps times
  int rep_counter = num_reps;
  while (rep_counter--) {

    if (radio.testCarrier(MIN_DBM)) {
      ++values;
    }
    delayMicroseconds(256);

#if defined USE_RX
    if (radio.available()) {
      radio.read(0, 0);
      valuesR++;
    }
#endif
    delayMicroseconds(256);
  }

  // Print out channel measurements, clamped to a single hex digit
  Serial.print("Low:");
  Serial.println(15);  // To freeze the lower limit
  Serial.print("Med:");
  Serial.println(25);  // To freeze the upper limit
  Serial.print("High:");
  Serial.println(35);  // To freeze the upper limit
  Serial.print("Values:");
  Serial.println(values);
  if (values >= 15 && values < 25) {
    green();
  } else if (values >= 25 && values < 35) {
    red();
  } else if (values >= 35) {
    orange();
  } else {
    blue();
  }
#if defined USE_RX
  Serial.print("RX:");
  if (valuesR) {
    Serial.println(valuesR);
  } else {
    Serial.println(0);
  }
#endif
}
