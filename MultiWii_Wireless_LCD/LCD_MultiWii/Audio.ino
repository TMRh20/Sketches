
#if defined EN_SD

unsigned long audioTimer = 0;
byte voltSteps = 0;
boolean linkOK = 0, audioArmed = 0;
byte lastMode = 0;


void pollAudio(){
  
  //byte mode = dBuff[2];
  
  if(!bitRead(mode,0)){ audioArmed = 0; }
  
  if(bitRead(mode,6)){
    while(millis()){
      wav.play("warning.wav"); wav.volume(0);wav.volume(0);
      lcd.clear(); lcd.print("   WARNING:");  
      lcd.setCursor(0,1); lcd.print(mode,BIN); //lcd.print("I2C ERRORS");
      while(wav.isPlaying()){};
      delay(250);
      wav.play("destruct.wav"); wav.volume(1);
      while(wav.isPlaying()){};
      
      //lcd.clear();
      delay(1500);
    }
  }else
  
  if(Volts > 4.5 && Volts < 10.6){
    if(millis() - audioTimer > 10000){
      audioTimer=millis();
      if(!wav.isPlaying()){ 
        if(voltSteps == 0){
          wav.play("WARNING.WAV"); //wav.volume(0); wav.volume(0);       
          lcd.clear(); lcd.print("   WARNING:");
          lcd.setCursor(0,1); lcd.print("BATTERY LOW");
          while(wav.isPlaying()){};
          wav.play("EMERGPWR.WAV"); }
          //wav.volume(0); wav.volume(0);
          while(wav.isPlaying()){};
          lcd.clear();
      }
    }
    
  }else
  if(bitRead(mode,0) && !audioArmed){ lastMode=mode; audioArmed = 1; wav.play("calibrat.wav");
  }else
  if(mspStarted && !linkOK){
      linkOK = 1;
      wav.play("link.wav");
      while(wav.isPlaying()){}
      lastMode=mode;
      linkOK = 1;    
  }else
  if(linkOK && mode != lastMode ){
    lastMode=mode;
    wav.play("verified.wav");
  }
  
  
  
  
  
  
}

#endif
