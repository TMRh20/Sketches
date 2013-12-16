#include <SPI.h>


#include "progMem.h"

//Code details and info at http://TMRh20.blogspot.com
//Based on https://instruct1.cit.cornell.edu/courses/ee476/video/
// and http://code.google.com/p/arduino-tvout/

//****************** USER CONFIG *****************

//Notice: (Width * Height) / 8 = Memory Used. Max 7500 for Arduino Mega 
#define displayWidth 400  //must be divisible by 8  NTSC Optimal: 88, 112, 144, 208, 400
#define displayHeight 144 //                        NTSC Optimal: 108/w hZoom,216
#define hZoom 0           //Doubles the height if set to 1. Width must be 108 or lower

#define NTSC
//#define PAL

//****************** VIDEO FRAME CONFIG *****************

#define hSync 25              //Define the length of the hSync pulse in timer counts (about 2uS only at 16Mhz)
#define vStart 1              //When in the timing cycle to start the video display timer
  
#ifdef NTSC
  #define hLinesPerFrame  262  //262 Horizontal lines per frame for NTSC, about 216 are visible
  #define hLineTime 1019       //Timer count between each hSync pulse. 16Mhz CPU: 1019 * 19 = 63.6875 uS per line       
  #define vSync 247            //Start vertical sync on line 247
#endif

#ifdef PAL
  #define hLinesPerFrame  312  //312 Horizontal lines per frame for PAL, about 260 are visible
  #define hLineTime 1026        //Timer count between each hSync pulse. 16Mhz CPU: 1019 * 19 = 64.125 uS per line
  #define vSync 292           //Start vertical sync on line 292
#endif


//#define FOSC 1843200 // Clock Speed
#define BAUD 9600
#define MYUBRR F_CPU/16/BAUD-1

//**********************************************************

byte dispLines = displayHeight, WAIT = 137, heightCnt = 0, dispStart = 0;
const unsigned int wLDiv = displayWidth/8, totalBytes = displayHeight*wLDiv;
volatile unsigned int isrCounter = 0;
volatile boolean togCnt = 0;

byte data[totalBytes+1];
boolean draw = 1;

void setup(){
         
  pinMode(18,OUTPUT); pinMode(12,OUTPUT); // The UART and Timer1 pins need to be set as outputs
  pinMode(2,OUTPUT); pinMode(51,OUTPUT); pinMode(13,OUTPUT);
  digitalWrite(18,LOW); digitalWrite(53,HIGH);
  configVideo();
  configUSART(); //Enable and configure the USART, see USART tab


  printChar("Simple",displayWidth/2.4,displayHeight/2.5);
  printChar("Arduino",displayWidth/2.4,displayHeight/2.5 + 8);
  printChar("TV",displayWidth/2.4, displayHeight/2.5 + 16);
  printChar("by TMRh20",displayWidth/2.4, displayHeight/2.5 + 24);
  
  //Serial3.begin(2400);
  unsigned int ubr = MYUBRR;
  //UBRR3H = (unsigned char)(ubr>>8);
  //UBRR3L = (unsigned char)ubr;
  UBRR3 = 51;
  //UCSR3C = _BV(UMSEL30) | _BV(UCSZ31) | _BV(UCSZ30);
  //Enable receiver*/
  UCSR3B = (1<<RXEN0);
/* Set frame format: 8data, 2stop bit */
  //UCSR0C = (1<<USBS0)|(3<<UCSZ00);
  //Serial.begin(57600);
  randomSeed(analogRead(0));
}


unsigned long timr = 0;
unsigned long cntr = 0;
unsigned int dotCntr = 0;
byte sBuffer[400];
boolean clCube = 1; 

boolean redraw = 0;

boolean doCbe = 0;

void loop(){
  
  if(millis() - timr > 1500){
    
    //printChar(cntr,displayWidth/2,displayHeight/2);
   cntr = 0;
   doCbe = 1;
   //draw_line(10,10,25,25,1);
  }
  if(millis() - timr > 3000){
    //timr = millis();
    //printChar(cntr,displayWidth/2,displayHeight/2);
   cntr = 0;
   //doCbe = 0;
   clCube = 0;
   
   //draw_line(10,10,25,25,1);
  }
  if(millis() - timr > 3500){
    //timr = millis();
    //printChar(cntr,displayWidth/2,displayHeight/2);
   cntr = 0;
   doCbe = 0; clCube = 1;
   //draw_line(10,10,25,25,1);
  }
  if(millis() - timr > 4100){
   // timr = millis();
    //printChar(cntr,displayWidth/2,displayHeight/2);
   cntr = 0;
   set_pixel(random(0,displayWidth-1),random(0,displayHeight-2),1);
  }
   if(millis() - timr > 5500 ){
    //clearScreen();
     timr = millis();
    
   }
  

  if(doCbe){
    doCube();
    
  }


}



