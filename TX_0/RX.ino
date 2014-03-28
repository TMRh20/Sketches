



void handleRadio(){
   boolean n=!whichBuff;                      //Grab the changing value of which buffer is not being read
  
  if(buffEmpty[n] && streaming){              //If in RX mode and a buffer is empty, load it
      if(radio.available() ){
          radio.read(&buffer[n],32);
          buffEmpty[n] = 0;                   //Indicate that a buffer is now full and ready to play
          cntr++;                             //For display of transmission speed
      }                                       //Indicate that a buffer is now full and ready to play
  }else
  if(recvMode && !streaming){                  //If not actively reading a stream, read commands
    if(radio.available() ){  
        byte cmd[32];
        radio.read(&cmd,32);
        switch(cmd[0]){                       //Additional commands can be added here for controlling other things via radio command 
          case 'r': if(cmd[1] == 'R' && RADIO_NO < 2){        //Switch to TX mode if we recived the remote tx command and this is radio 0 or 1
                      TX();
                    }
                    break;
          default: streaming= 1;              //If not a command, treat as audio data
                   ledVal = 125;              //Set the delay for LED flashing during reception  

        }
    }
  } 
}


void RX(){ //Start Receiving
        digitalWrite(ledPin,LOW);
	ADCSRA = 0; ADCSRB = 0;                //Disable Analog to Digital Converter (ADC)
        buffEmpty[0] = 1; buffEmpty[1] = 1;    //Set the buffers to empty
        TCCR1A |= _BV(COM1A1) | _BV(COM1B1) | _BV(COM1B0);  //Enable output to speaker pin
        #if defined (oversampling)
          ICR1 = 10 * (800000/SAMPLE_RATE);    //Set timer top for 2X oversampling
        #else
          ICR1 = 10 * (1600000/SAMPLE_RATE);
        #endif   
        TIMSK1 = _BV(TOIE1);                   //Enable the overflow vector (receive mode)
        
        radio.openWritingPipe(pipes[0]);       //Set up reading and writing pipes
        radio.openReadingPipe(1,pipes[1]);        
        radio.startListening();                //Exit sending mode
        recvMode = 1;                          //Enable buffering of recvd data
        
}


// Receiving interrupt
ISR(TIMER1_OVF_vect){                                      //This interrupt vector loads received audio samples into the timer register
    
 
  if(buffEmpty[whichBuff] ){ whichBuff=!whichBuff; }else{  //Return if both buffers are empty
      #if defined (oversampling)  
        if(lCntr){lCntr = !lCntr;return;}   lCntr=!lCntr;
      #endif

#if !defined (tenBit)      
  if(volMod < 0 ){                                         //Load an audio sample into the timer compare register
    OCR1A = OCR1B = (buffer[whichBuff][intCount] >> volMod*-1);    //Output to speaker at a set volume
  }else{ 
    OCR1A = OCR1B = buffer[whichBuff][intCount] << volMod;            
  }
  intCount++;                                              //Keep track of how many samples have been loaded
  
  if(intCount >= buffSize){                                //If the buffer is empty, do the following
      intCount = 0;                                        //Reset the sample count
      buffEmpty[whichBuff] = true;                         //Indicate which buffer is empty
      whichBuff = !whichBuff;                              //Switch buffers to read from
  }
#else  
    sampl = buffer[whichBuff][intCount];
    bitWrite(  sampl, 8,  bitRead( buffer[whichBuff][bytePos],bitPos));    
    bitWrite(sampl, 9, bitRead(buffer[whichBuff][bytePos],bitPos+1));
    bitPos+=2;
    
  if(volMod < 0 ){                                         //Load an audio sample into the timer compare register
    OCR1A = OCR1B = sampl >> (volMod*-1);                 //Output to speaker at a set volume
  }else{ 
    OCR1A = OCR1B = sampl << volMod;            
  }
    
    
    if(bitPos >=8){
      bitPos = 0;
      bytePos = bytePos+1; 
    }
    
    intCount++;                                            //Keep track of how many samples have been loaded
  
  if(intCount >= 25){                                      //If the buffer is empty, do the following
      bytePos = 25;
      bitPos = 0;
      intCount = 0;                                        //Reset the sample count
      buffEmpty[whichBuff] = true;                         //Indicate which buffer is empty
      whichBuff = !whichBuff;                              //Switch buffers to read from
  }    
    
    
#endif  
  }  
}

