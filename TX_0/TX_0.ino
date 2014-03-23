
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include "printf.h"
#include <avr/sleep.h>
#include <avr/power.h>
/*

 **** Wireless Microphone/Speaker/Radio or Intercom by TMRh20 - 2011-2014 ****

 This is an extension of the wireless audio add-on library I published in early 2012 for use with my audio playback library, but
 in sketch form. Users can easily configure it to work as a two-way radio, intercom, or auto-triggered recording device. These 
 features may be fully incorporated into the main audio library at some point.
 
 *What it does:*
 Audio is captured by the on-board Analog-to-Digital Converter (ADC) into a digital (PCM/WAV) format. The audio is then transmitted wirelessly
 to a remote device using NRF24L01 radio modules. Recording can be started remotely, by external buttons, or automatically by sensors etc. 

 *Requirements:*
 2 X Arduino Uno, Nano, Mega, etc.
 2 X NRF24L01 Wireless Radio Modules
 1 | 2 Output Device(s) (Speaker, Amplifer, etc)
 1 | 2 Input Device(s) (Microphone, Recorded Audio etc)
 Optional:
 1 x LED for status indicator
 1 or more buttons for external control
 1 or more sensors for automated recording

 Audio format: 4kHz-29kHz 8-bit Mono
 Range: Up to 100m with PA module. 1000m with PA+LNA  at 12kHz or lower sample rate, 250kps data rate

 *Usage:*
 1. Configure the options below in User Configurable Variables
    a: Choose the analog pin to record on
    b: Set RADIO_NO 0 for one device, RADIO_NO 1 for the other
    c: Choose other options such as sample rate and data speed

 2. Upload this sketch to both devices with all options the same, except for RADIO_NO and pin selections
 3. Use serial commands to control either device and test
 4. Connect buttons or sensors as desired


 Notes & Troubleshooting:
 If the Arduino or radio modules seem to be resetting etc, it is likely due to a shortage of power. Disconnect other devices to test.
 Arduino Mega seems to need the device run off 5v. Arduino Nano etc. seem to be ok at 3v or 5v for modules that support both.
 Data speed has to be set correctly for the selected sample rate. See the notes below
 
*/

/***********************************************
//******* MANDATORY User Variables **************/

#define RADIO_NO 0                            //Needs to be changed for each device. Controls which pipes are used.

RF24 radio(48,49);                            //Choose the CE, CS Pins for the NRF24L01+ radio module 
#define SAMPLE_RATE 32000                     //The sample rate to use for transferring audio samples
#define RF_SPEED RF24_1MBPS                   //RF24_250KBPS will do 13-14khz sample rate max, RF24_1MBPS up to 24-25khz, RF24_2MBPS for higher. These are not limits, just a guide.
#define speakerPin 11                         //The pin to output audio on
#define speakerPin2 12
#define ANALOG_PIN A0                         //The pin that analog readings will be taken from (microphone pin)

//***** Optional user variables ********************
#define ledPin 4                              //Indicator pin
#define TX_PIN A1                             //Button pin to trigger recording & transmission
#define VOL_UP_PIN A2                         //Pin for external volume control
#define VOL_DN_PIN A3                         //Pin for external volume control
#define REMOTE_TX_PIN A4                      //Pin for externally triggering remote recording
#define REMOTE_RX_PIN A5                      //Pin for externally stopping remote recording
#define buffSize 32                          //The size of the memory buffer to use
//#define speakerTX                           //Whether to output to speaker while transmitting
//#define oversampling                        //Oversampling is recommended for low sample rates only

#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) || (__AVR_ATmega32U4__) || (__AVR_AT90USB646__) || defined(__AVR_AT90USB1286__) || (__AVR_ATmega128__) ||defined(__AVR_ATmega1281__)||defined(__AVR_ATmega2561__)
  #define rampMega
#endif

//******* General Variables ************************
volatile boolean buffEmpty[2] = {true,true}, whichBuff = false, recvMode = 1, a, lCntr=0, streaming = 0, transmitting = 0;
volatile byte buffCount = 0;
unsigned long timer, cntr, recvSt, ledTimer = 0, ledVal;
unsigned int intCount = 0;
byte txCmd[2] = {'r','R'};
byte buffer[2][buffSize+1];
char volMod = -1;

//****** Initialization and setup *****************
void setup(){
  
  pinMode(ledPin,OUTPUT);                    //Set LED pin as output
  pinMode(TX_PIN,INPUT_PULLUP);              //Set input pins as pullup. See http://arduino.cc/en/Tutorial/InputPullupSerial  
  pinMode(VOL_UP_PIN,INPUT_PULLUP);
  pinMode(VOL_DN_PIN,INPUT_PULLUP);
  pinMode(REMOTE_TX_PIN,INPUT_PULLUP);
  pinMode(REMOTE_RX_PIN,INPUT_PULLUP);
  
  pinMode(speakerPin,OUTPUT);                 //Enable output on the speaker pin  
  pinMode(speakerPin2,OUTPUT);
  Serial.begin(115200);                       //Enable Serial
  printf_begin();                             //Printf.h comes with the examples in the RF24 library. Used for printing radio details.
  setRadio();                                 //Setup the radio with required settings (rfSetup tab)  
  //SPI.setClockDivider(SPI_CLOCK_DIV2);
  timerStart();
  RX();                                       //Enable reception of audio streams
  
  if(SAMPLE_RATE < 16000){
    volMod = 2;
  }

}

//************* MAIN LOOP ************************
void loop(){      
    
  handleButtons();                              //Pretty much everything happens in these two functions
  handleRadio();
  
  //This section can be commented out
  if(Serial.available()){                       //Read inputs from Serial for testing etc.
      
      switch(Serial.read()){
        case 'r': TX(); break;                  //Enable transmission of audio. Setting the TX pin to low will do the same.
        case 's': RX(); break;                  //Enable reception of audio. Setting the RX pin low will do the same.
        case '-': volume(0); break;
        case '=': volume(1); break;
        case 't': if(recvMode){                 //Start recording remotely
                    radio.stopListening(); 
                    radio.write(&txCmd,2);
                    radio.startListening();
                  } 
                  break;
        case 'R': delay(2000); break;           //Force timeout on remote recording
        default: break;
      }
  } 
  
  
  //Timing functions to control an attached LED and calculate the transfer rate
  if(millis() - ledTimer > ledVal){
     ledTimer = millis();     
     if(streaming){ digitalWrite(ledPin,!digitalRead(ledPin)); 
     }else{ digitalWrite(ledPin,LOW); }
     
  }
  
  //Transfer rate calculator. This can be commented out
  if(millis() - timer > 3000){                 //Timer for indication of transfer speed
    timer = millis();  
    if(cntr < 80 ){ streaming = 0; RX(); }    //Disable TX if packets are not being sent
    Serial.print("Transfer Rate: ");    
    Serial.print((float)cntr*32/3000);         //The count is 1 for every 32 bytes. Cntr*32 = Total bytes sent or received
    Serial.println(" KB/s");
    cntr = 0; 
  }  
}




//General functions for volume control
void volume(boolean upDn){
    if(upDn==1){ volMod++;}
    else{ volMod--; }
}

void setVolume(char vol) {
    volMod = vol - 4 ;
}

void timerStart(){
        ICR1 = 10 * (800000/SAMPLE_RATE);                //Timer will count up to this value from 0;
        TCCR1A = _BV(COM1A1) ; //Enable the timer port/pin as output
	TCCR1A |= _BV(WGM11);                            //WGM11,12,13 all set to 1 = fast PWM/w ICR TOP
	TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);    //CS10 = no prescaling  
}





