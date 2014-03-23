

void clear(){
  //Empty interrupt for handling TX clear
    
}
  //Transmission sending interrupt
  ISR(TIMER1_COMPA_vect){                                    //This interrupt vector sends the samples when a buffer is filled
	if(buffEmpty[!whichBuff] == 0){              //If a buffer is ready to be sent
	 		a = !whichBuff;                      //Get the buffer # before allowing nested interrupts
   			TIMSK1 &= ~(_BV(OCIE1A));            //Disable this interrupt vector
   			sei();                               //Allow other interrupts to interrupt this vector (nested interrupts)
               
                        radio.startFastWrite(&buffer[a],32);
                        //radio.write(&buffer[a],32);
                        
             buffEmpty[a] = 1;                    //Mark the buffer as empty
   	    TIMSK1 |= _BV(OCIE1A);               //Re-enable this interrupt vector
            cntr++; 
        }
  }
  
  
  void txComplete(){
    bool tx,fail,rx;
    radio.whatHappened(tx,fail,rx);
    if(rx){
   byte tmp;
   radio.read(&tmp,1); 
  }
    detachInterrupt(0);
     
     if ( radio.isAckPayloadAvailable() )
  {
    radio.getDynamicPayloadSize();
  }

  // Yay, we are done.

  // Power down
  radio.powerDown();

  // Flush buffers (Is this a relic of past experimentation, and not needed anymore??)
  //flush_tx();
    TIMSK1 |= _BV(OCIE1A);    
  }
  
  //Transmission buffering interrupt
  ISR(TIMER1_CAPT_vect){                                    //This interrupt vector captures the ADC values and stores them in a buffer

    buffer[whichBuff][buffCount] = ADCH;                     //Read the high byte of the ADC register into the buffer
    
    #if defined (speakerTX)
	if(volMod < 0 ){  OCR1A = OCR1B = (ADCH >> volMod*-1);       //Output to speaker at a set volume if defined
	}else{  	  OCR1A = OCR1B = ADCH << volMod;            
	}
    #endif
    
	buffCount++;                                         //Keep track of how many samples have been loaded
	if(buffCount >= 32){                                 //Every 32 samples do this stuff
	  buffCount = 0;                                     //Reset the sample counter
          buffEmpty[!whichBuff] = 0;                         //Indicate which bufffer is ready to send
  	  whichBuff = !whichBuff;                            //Switch buffers and load the other one
  	}
  }


void TX(){
        
        radio.stopListening();
        recvMode = 0; streaming = 0;
  	buffCount = 0; buffEmpty[0] = 1; buffEmpty[1] = 1;
        digitalWrite(ledPin,HIGH); ledTimer = millis(); ledVal = 1000000;
        byte pin = ANALOG_PIN;
	/*** This section taken from wiring_analog.c to translate between pins and channel numbers ***/
	#if defined(analogPinToChannel)
	#if defined(__AVR_ATmega32U4__)
		if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	#endif
		pin = analogPinToChannel(pin);
	#elif defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
		if (pin >= 54) pin -= 54; // allow for channel or pin numbers
	#elif defined(__AVR_ATmega32U4__)
		if (pin >= 18) pin -= 18; // allow for channel or pin numbers
	#elif defined(__AVR_ATmega1284__) || defined(__AVR_ATmega1284P__) || defined(__AVR_ATmega644__) || defined(__AVR_ATmega644A__) || defined(__AVR_ATmega644P__) || defined(__AVR_ATmega644PA__)
		if (pin >= 24) pin -= 24; // allow for channel or pin numbers
	#else
		if (pin >= 14) pin -= 14; // allow for channel or pin numbers
	#endif

	#if defined(ADCSRB) && defined(MUX5)
		// the MUX5 bit of ADCSRB selects whether we're reading from channels
		// 0 to 7 (MUX5 low) or 8 to 15 (MUX5 high).
		ADCSRB = (ADCSRB & ~(1 << MUX5)) | (((pin >> 3) & 0x01) << MUX5);
	#endif

	#if defined(ADMUX)
		ADMUX = (pin & 0x07);
	#endif
	
        ICR1 = 10 * (1600000/SAMPLE_RATE);           //Timer counts from 0 to this value
	TIMSK1 = _BV(ICIE1) | _BV(OCIE1A);          //Enable the TIMER1 COMPA and COMPB interrupts
        
        #if !defined (speakerTX) //If disabling/enabling the speaker
        
            int current = OCR1A;
            if(current > 0){
                for(int i=0; i < ICR1; i++){
	        #if defined(rampMega)
	          OCR1B = constrain((current + i),0,ICR1);
	          OCR1A = constrain((current - i),0,ICR1);
	        #else
	          OCR1B = constrain((current - i),0,ICR1);
	          OCR1A = constrain((current - i),0,ICR1);
	        #endif
	        for(int i=0; i<10; i++){ while(TCNT1 < ICR1-50){} }
	      }
            }
            TCCR1A &= ~_BV(COM1A1);                    //Disable output to speaker
        
        #endif
        
	ADMUX |= _BV(REFS0) | _BV(ADLAR);            //Analog 5v reference, left-shift result so only high byte needs to be read
	ADCSRB |= _BV(ADTS0) | _BV(ADTS1) | _BV(ADTS2);           //Attach ADC start to TIMER1 Capture interrupt flag
	byte prescaleByte = 0;

	if(      SAMPLE_RATE < 8900){  prescaleByte = B00000111;} //128     
        else if( SAMPLE_RATE < 18000){ prescaleByte = B00000110;} //ADC division factor 64 (16MHz / 64 / 13clock cycles = 19230 Hz Max Sample Rate )
	else if( SAMPLE_RATE < 27000){ prescaleByte = B00000101;} //32  (38461Hz Max)
	else if( SAMPLE_RATE < 65000){ prescaleByte = B00000100;} //16  (76923Hz Max)
        else   {                       prescaleByte = B00000011;} //8  (fast as hell)
	
        ADCSRA = prescaleByte;                        //Adjust sampling rate of ADC depending on sample rate
	ADCSRA |= _BV(ADEN) | _BV(ADATE);             //ADC Enable, Auto-trigger enable

       timer=millis(); //prevent reset of transmission
}


