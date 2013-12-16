

void configVideo(){
  
  ICR1 = hLineTime; //Set TIMER1 TOP value (period) to horizontal line width ( value / 16 = microseconds )
  OCR1B = hSync;    //Set TIIMER1 compare match to hSync pulse width (~5us by definition)
  OCR1A = vStart;   //Start the drawing interrupt upon compare match with vStart

  //WGM11,12,13 all set to 1 = fast PWM/w ICR TOP, Output on pin 12(Mega), no prescaling
  TCCR1A = _BV(WGM11) | _BV(COM1B1) | _BV(COM1B0); 
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);    

  //Enable Compare match A interrupt vector to trigger the COMPA ISR
  TIMSK1 = _BV(OCIE1A); 
  
  TCCR3A = _BV(WGM11) | _BV(COM1B1) | _BV(COM1B0); 
  TCCR3B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);
  ICR3 = 3200;
  OCR3B = 1600;
  
  
}


//This is the main display interrupt, triggered at about 15.75khz for NTSC, 15.63khz for PAL
ISR(TIMER1_COMPA_vect){

     
     if( isrCounter > 43 && isrCounter <= 43+displayHeight){     //Only draw video on certain lines specified by the resolution
        unsigned int start = heightCnt * wLDiv;                   //Find the starting point in the data array. Each new line starts at lineNumber*(width/8)
       
        wait_until(153);                                        //Wait a period of time to account for delays in interrupt triggering. Aligns things horizontally and removes video glitches
        UDR1 = data[start];                                       //Load the first byte into the buffer
        UCSR1B |= _BV(TXEN1);                                     //Enable the USART
        
        for (int x =1; x < wLDiv; x++){ // 0 to screen width/8    //Load bytes until we have drawn 1 horizontal line:
          while ( !(UCSR1A & _BV(UDRE1))){ }                      //Wait until the USART is ready for more data
          UDR1 = data[x+start];          //Load a byte
        }
   
        UCSR1B = 0 ;        //Disable the USART TX

        heightCnt+=togCnt;                                        //Increase horizontal display count depending on zoom
        
        if(hZoom){ togCnt = !togCnt;                                //Zoom stuff
        }else{     togCnt = 1; 
        }
      }
      
      ++isrCounter; //Count each horizontal line
     
        switch(isrCounter){          
          case vSync: OCR1B = hLineTime-hSync; heightCnt = 0; break;  //Invert the hSync pulse to provide the vertical sync every frame (262 lines for NTSC)             
          case vSync+3: OCR1B = hSync; break;                         //Undo the inversion after 3 vertical sync pulses
          case hLinesPerFrame:  isrCounter = 0; break;                //Reset the line count after each frame
        }
}




//This code is what allows the precise timing required to
//trigger the start of video display at up to 8Mhz

static void inline wait_until(uint8_t time) {

	__asm__ __volatile__ (
			"sub	%[time], %[tcnt1l]\n\t" //Subtract the low byte of TCNT1 from our designated time to wait until
                        "brmi   102f\n\t"               //If the result is negative, we already missed it. Branch to 102 and exit
		"100:\n\t"                      
                        "subi	%[time], 3\n\t"         //Subtract 3 from time
			"brcc	100b\n\t"               //Loop until we get a negative
			"subi	%[time], 0-3\n\t"       //Add 3 back to our value
			"breq	101f\n\t"               //If equal to 0, branch to 101 and delay 1/16 of a uS (No OPeration)
			"dec	%[time]\n\t"            //Decrement our value by 1
			"breq	102f\n\t"               //If equal to 0, branch to 102 and don't delay
			"rjmp	102f\n"                 //Else Jump to the end
		"101:\n\t"
			"nop\n"
		"102:\n"
		:                                 //Output variables (none)
		: [time] "a" (time),              //Input variables
		[tcnt1l] "a" (TCNT1L)
	);
}




