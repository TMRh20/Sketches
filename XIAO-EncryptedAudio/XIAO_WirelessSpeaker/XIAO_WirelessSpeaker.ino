/* XIAO 52840 Sense Example for Auto Analog Audio
 * Copyright (C) 2023  TMRh20 - tmrh20@gmail.com, github.com/TMRh20
*/


#include <AutoAnalogAudio.h>
#include <nrf_to_nrf.h>

AutoAnalog aaAudio;
nrf_to_nrf radio;

/********************** User Config ***********************************/
const uint8_t pipe[2][5] = {{0xCE,0xC3,0xE3,0xCC,0x3C},{0xC3,0xC3,0xE3,0xCC,0x3C}};
uint8_t myKey[16] = {1,2,3,5,8,13,21,34,55,89,144,233,1,2,3,5};
/*********************************************************/

#define MAX_PAYLOAD_SIZE 110
uint32_t sampleRate = 14850;

void setup() {
  aaAudio.begin(0, 1);
  aaAudio.autoAdjust = 0;
  //aaAudio.adcBitsPerSample = 16; // 16-bit audio at 16khz is the default on NRF52 and cannot be modified currently (in progress)
  aaAudio.dacBitsPerSample = 16;
  aaAudio.setSampleRate(sampleRate);

  radio.begin();
  radio.enableDynamicPayloads(MAX_PAYLOAD_SIZE + 12);
  radio.setDataRate(NRF_2MBPS);
  radio.setKey(myKey);
  radio.setCounter(12358);
  radio.enableEncryption = true;
  radio.setChannel(127);
  radio.setCRCLength(NRF_CRC_16);
  radio.setRetries(0,1);
  radio.openReadingPipe(1, pipe[0]);
  radio.openReadingPipe(2, pipe[1]);
  radio.startListening();
}

uint8_t buffer[2][MAX_PAYLOAD_SIZE];
uint32_t timer = micros();
bool whichBuffer = 0;
uint32_t counter = 0;

void loop() {



  if (millis() - timer > 3000) {
    timer = millis();
    Serial.println(counter);
    counter = 0;
  }
  
  uint8_t pipe = 0;
  if (radio.available(&pipe)) { 
    if(pipe == 1){
      counter++;
      uint16_t buffer[MAX_PAYLOAD_SIZE / 2];
      radio.read(buffer, MAX_PAYLOAD_SIZE);
      for (int i = 0; i < (MAX_PAYLOAD_SIZE / 2); i++) {
        buffer[i] += 0x8000;
      }
      memcpy(&aaAudio.dacBuffer16[0],&buffer[0],MAX_PAYLOAD_SIZE);
      aaAudio.feedDAC(MAX_PAYLOAD_SIZE / 2);
    }else
    if(pipe == 2){
      uint8_t bothKeys[32];
      radio.read(&bothKeys,sizeof(bothKeys));
      for(int i=0; i<16; i++){
        if(myKey[i] != bothKeys[i+16]){
          Serial.println("Keys not match");
          return;
        }
      }
      memcpy(&myKey[0], &bothKeys[0], 16);
      radio.setKey(myKey);
      Serial.println("rekey");
    }else{
      radio.read(0,0);
      Serial.println("wtf");
    }
  }
}
