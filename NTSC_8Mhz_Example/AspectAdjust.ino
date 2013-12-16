
//The UART is used in Master SPI mode to draw the video pixels at up to 8mhz
//The BAUD rate is adjusted depending on the horizontal resolution to fill
//the screen as much as possible

void configUSART(){
  
 UCSR1C = _BV(UMSEL01) | _BV(UMSEL00); //Set USART to Master SPI mode

//This section defines the BAUD rate and screen adjustment
// (Left/Right) for different horizontal resolutions   

if(displayWidth > 208){ UBRR1 = 0; WAIT = 132;
  }else
  if(displayWidth > 144){ UBRR1 = 1; WAIT = 129;
  }else
  if(displayWidth > 112){ UBRR1 = 2; WAIT = 127;
  }else
  if(displayWidth > 88){  UBRR1 = 3; WAIT = 125;
  }else{                   UBRR1 = 4; WAIT = 124;
  }  
  //Double the number of horizontal lines printed to screen when zooming
  if(hZoom == 0){ dispLines = displayHeight;
  }else{          dispLines = displayHeight*2;
  }
  
  
#ifdef NTSC  
  
  if(displayHeight < 216 && hZoom == 0){
     dispStart = (216 - displayHeight) / 2;
  }else{
     dispStart = 13; 
  }
  
#endif

#ifdef PAL
   
  if(displayHeight < 260 && hZoom == 0){
     dispStart = (260 - displayHeight) / 2;
  }else{
     dispStart = 13; 
  }

#endif



}
