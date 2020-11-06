
/**
ESP32 Audio DAC Test by TMRh20 - 2020
A simple test using i2S to feed the onboard DAC of ESP32

*/

/************************************/
#include "driver/i2s.h"

#include <SD.h>

/************************************/
/*********** USER CONFIG ************/
// Set to the number of samples in the sine wave
#define numSamples 32
const char *audioFileName = "/c32-16M.wav";
/************************************/

// Buffer for a sine wave
uint8_t dacBuffer[numSamples];
#include "audio.h"
// Boolean variable to control audio output based on Serial input
bool outputAudio = false;
/************************************/
// Function to load data into the sine wave buffer
void sineSetup();
/************************************/

void setup() {

  Serial.begin(57600);
  Serial.println("ESP32 Audio DAC Test");
  if(!SD.begin(17)){
    Serial.println("SD FAIL");
  }
  //sineSetup();

  // Set the I2S peripheral to MASTER, TX (DAC Output), using built in DAC
  i2s_mode_t myMode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_TX | I2S_MODE_DAC_BUILT_IN);

  // This configures the i2s peripheral, configure as necessary...
  i2s_config_t i2s_cfg = {
    .mode = (i2s_mode_t)myMode,
    .sample_rate =  32000,              // The format of the signal using ADC_BUILT_IN
    .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT, // is fixed at 12bit, stereo, MSB
    .channel_format = I2S_CHANNEL_FMT_RIGHT_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S_LSB,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = numSamples,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
  };

  i2s_driver_install(I2S_NUM_0, &i2s_cfg, 0, NULL);

  i2s_set_dac_mode(I2S_DAC_CHANNEL_BOTH_EN);
  i2s_set_clk(I2S_NUM_0, 32000, I2S_BITS_PER_SAMPLE_16BIT, I2S_CHANNEL_MONO);
  i2s_stop(I2S_NUM_0);
}

/************************************/

void loop() {

  if (outputAudio) {
    size_t bytesWritten = 0;
    //esp_err_t i2s_write_expand(i2s_port_t i2s_num, const void *src, size_t size, size_t src_bits, size_t aim_bits, size_t *bytes_written, TickType_t ticks_to_wait);
    i2s_write_expand(I2S_NUM_0, &dacBuffer[0], numSamples, 8, 16, &bytesWritten, 500 / portTICK_PERIOD_MS);
  }

  if (Serial.available()) {
    char c = Serial.read();
    if (c == '1') {
      outputAudio = !outputAudio;
      if (outputAudio) {
        Serial.println("Enable Output");
        sineSetup();
        i2s_start(I2S_NUM_0);
      } else {
        Serial.println("Disable Output");
        i2s_stop(I2S_NUM_0);
      }
    }else{
      if(c == '2'){
        audioPlay();
      }
    }
  }
  loadBuffer();
}

/************************************/

void sineSetup() {

  dacBuffer[0] = 0x40;
  dacBuffer[1] = 0x4c;
  dacBuffer[2] = 0x58;
  dacBuffer[3] = 0x63;
  dacBuffer[4] = 0x6c;
  dacBuffer[5] = 0x74;
  dacBuffer[6] = 0x7a;
  dacBuffer[7] = 0x7e;
  dacBuffer[8] = 0x7f;
  dacBuffer[9] = 0x7e;
  dacBuffer[10] = 0x7a;
  dacBuffer[11] = 0x74;
  dacBuffer[12] = 0x6c;
  dacBuffer[13] = 0x63;
  dacBuffer[14] = 0x58;
  dacBuffer[15] = 0x4c;
  dacBuffer[16] = 0x40;
  dacBuffer[17] = 0x33;
  dacBuffer[18] = 0x27;
  dacBuffer[19] = 0x1c;
  dacBuffer[20] = 0x13;
  dacBuffer[21] = 0xb;
  dacBuffer[22] = 0x5;
  dacBuffer[23] = 0x1;
  dacBuffer[24] = 0x0;
  dacBuffer[25] = 0x1;
  dacBuffer[26] = 0x5;
  dacBuffer[27] = 0xb;
  dacBuffer[28] = 0x13;
  dacBuffer[29] = 0x1c;
  dacBuffer[30] = 0x27;
  dacBuffer[31] = 0x33;
}
