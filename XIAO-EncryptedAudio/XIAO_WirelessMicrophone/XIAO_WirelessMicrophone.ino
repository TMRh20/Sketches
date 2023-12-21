/* XIAO 52840 Sense Example for Auto Analog Audio
 * Copyright (C) 2023  TMRh20 - tmrh20@gmail.com, github.com/TMRh20
*/


#include <AutoAnalogAudio.h>
#include <nrf_to_nrf.h>

AutoAnalog aaAudio;
nrf_to_nrf radio;

#define MAX_PAYLOAD_SIZE 110

/********************** User Config ***********************************/
const uint8_t pipe[2][5] = { { 0xCE, 0xC3, 0xE3, 0xCC, 0x3C }, { 0xC3, 0xC3, 0xE3, 0xCC, 0x3C } };
uint8_t myKey[16] = { 1, 2, 3, 5, 8, 13, 21, 34, 55, 89, 144, 233, 1, 2, 3, 5 };
/*********************************************************/

void setup() {

  Serial.begin(115200);
  Serial.println("Analog Audio Begin");

  aaAudio.begin(1, 0);  //Setup aaAudio using ADC only
  aaAudio.autoAdjust = 0;
  aaAudio.adcBitsPerSample = 16;
  aaAudio.setSampleRate(16500);

  radio.begin();
  radio.enableDynamicPayloads(MAX_PAYLOAD_SIZE + 12);
  radio.setDataRate(NRF_2MBPS);
  radio.setKey(myKey);
  radio.setCounter(12358);
  radio.enableEncryption = true;
  radio.setChannel(127);
  radio.setCRCLength(NRF_CRC_16);
  radio.setRetries(0, 1);
  radio.openWritingPipe(pipe[0]);
  radio.printDetails();
  radio.stopListening();

  NRF_RNG->CONFIG = 1;
  NRF_RNG->EVENTS_VALRDY = 0;
  NRF_RNG->TASKS_START = 1;
}

/*********************************************************/

uint32_t dispTimer = 0;
bool ok = 0;

void loop() {

  //Display the timer period variable for each channel every 3 seconds
  if (millis() - dispTimer > 30000) {
    dispTimer = millis();
    Serial.println("Rekeying...");
    //Serial.println(ok);
    Serial.print("NewKey: ");
    uint8_t newKey[32];
    for (int i = 0; i < 16; i++) {
      while (NRF_RNG->EVENTS_VALRDY == 0) {}
      NRF_RNG->EVENTS_VALRDY = 0;
      newKey[i] = NRF_RNG->VALUE;
      Serial.print(newKey[i]);
      Serial.print(":");
    }
    Serial.println();
    
    memcpy(&newKey[16],&myKey[0],16);
  
    radio.openWritingPipe(pipe[1]);
    while (!radio.write(&newKey, sizeof(newKey))) {delay(1);}
    Serial.println("ReKeying OK");
    memcpy(myKey,newKey,16);
    radio.setKey(myKey);
    radio.openWritingPipe(pipe[0]);
  }

  // getADC() will trigger once, then block until the ADC data is ready
  aaAudio.getADC(MAX_PAYLOAD_SIZE / 2);
  ok = radio.write(&aaAudio.adcBuffer16[0], MAX_PAYLOAD_SIZE);
}

/*********************************************************/
