#if defined EN_SD



boolean sdStarted = 0;
File file;
unsigned long logTimer = 0;
unsigned long filePos = 0;
byte lastMaxAlt = 0;
byte lastUptime = 0;
boolean logNow = 0;

void doSD(){ 
  
  sdSerial();//handle incoming Serial commands for displaying serial data
  
  //Altitude Logging
  if(Alt > lastMaxAlt || Uptime > lastUptime){
     logNow = 1;   
  }
  
  if(millis() - logTimer > 1000 && sdStarted){
    logTimer = millis();
    if(logNow){ //if current Altitude > last logged altitude
        logNow = 0;
        while(wav.isPlaying()){}
        file = SD.open(fileName,FILE_WRITE); //open the file
        if(!file){ Serial.println("fail"); return; }
        file.seek(filePos); //go to the right place
        //Serial.print(Alt);
        file.print("|"); file.print(Uptime); file.print(" "); file.println(Alt);
        lastMaxAlt = Alt; lastUptime = Uptime;
        file.close();
    }
  }
}



void startAltLogging(){
    
  //UnComment to reset the file
  //if(SD.exists(fileName)){ SD.remove(fileName); }
    
    sdStarted = 1;
    file = SD.open(fileName,FILE_WRITE);
    
    if(file){
      //if(file.size() < 1){ file.print("|"); } 
      filePos = file.size();
      file.close();
      Serial.println("Alt Begun");
    }
    
  
}


void sdSerial(){
  if(Serial.available() > 0){
    int cntThree = 0;
    unsigned long cnt = 0;
    File F;
    
    switch(Serial.read()){
      
      case 'r': 
                Serial.println("Max Altitude Data:");
                F =SD.open(fileName,FILE_WRITE);
                F.setTimeout(33);
                F.seek(0);
                
                
                if(!F){Serial.println("fail1"); return;}
                while(F.available() > 0){                  
                  ++cnt;
                  if(F.findUntil("|","")){
                    int tim = F.parseInt();
                    int alt = F.parseInt();
                    Serial.print("Flight: "); Serial.print(cnt);
                    Serial.print(" Time: "); Serial.print(tim);
                    Serial.print(" Max Alt: ");Serial.println(alt);
                  }   
                }
                F.close();
            
      break;
      
      case 'd': wav.play("warning.wav"); break;
      case '=': wav.volume(1); break;
      case '-': wav.volume(0); break;
     
     
    }  
  
}
}

#endif

