

#include <Arduino_LED_Matrix.h>
#include <matrix.inc>
#include "Arduino_RouterBridge.h"

Arduino_LED_Matrix matrix;

const uint8_t FRAME_ROWS = 8;                        // Rows
const uint8_t FRAME_COLS = 13;                       // Columns
const uint8_t FRAME_SIZE = FRAME_ROWS * FRAME_COLS;  // Total number of pixels in the frame

uint8_t frame[FRAME_SIZE] = {  // Define a X shaped frame as an array with just 2 brightness levels (0 and 7)
  7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7,
  0, 0, 7, 7, 0, 0, 0, 0, 0, 7, 7, 0, 0,
  0, 0, 0, 0, 7, 7, 0, 7, 7, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 7, 7, 7, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 7, 7, 0, 7, 7, 0, 0, 0, 0,
  0, 0, 7, 7, 0, 0, 0, 0, 0, 7, 7, 0, 0,
  7, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 7, 7
};

uint8_t frame_gradient[FRAME_SIZE] = {  // Define a X shaped gradient frame as an array with brightness levels from 0 to 7
  1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1,
  0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0,
  0, 0, 0, 0, 4, 5, 0, 5, 4, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 6, 7, 6, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 6, 7, 6, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 4, 5, 0, 5, 4, 0, 0, 0, 0,
  0, 0, 3, 3, 0, 0, 0, 0, 0, 3, 3, 0, 0,
  1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1
};



void setup() {

  Bridge.begin();
  Bridge.notify("on_tick", 12);

  pinMode(LED3_R, OUTPUT);  // Red LED
  pinMode(LED3_G, OUTPUT);  // Green LED
  pinMode(LED3_B, OUTPUT);  // Blue LED

  digitalWrite(LED3_R, HIGH);
  digitalWrite(LED3_G, HIGH);
  digitalWrite(LED3_B, HIGH);


  Serial.begin(115200);
  Serial1.begin(115200);
  Serial2.begin(115200);
  delay(500);
  Serial.println("Start0");
  Serial1.println(F("Start1"));
  Serial2.println(F("Start2"));

  matrixBegin();
}

uint32_t counter = 0;

void loop() {

  Serial.println("Testing");
  
  digitalWrite(LED3_R, LOW);
  digitalWrite(LED3_G, HIGH);
  matrix.draw(frame);
  delay(1000);
  matrix.draw(frame_gradient);
  digitalWrite(LED3_R, HIGH);
  digitalWrite(LED3_G, LOW);
  delay(1000);

  Bridge.notify("on_tick", counter++);
}
