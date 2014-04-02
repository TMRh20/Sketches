


char buffCnt = 0, toSend[42];

byte checksum = 0,  dBuffCnt = 0, crc = 0, code = 0;

boolean lcdNull = 0;
byte lastD = 0;



 //*Armed = &pBuff[0];


// Valid command was receive, see what it was and take appropriate action:
void evaluateCommand(byte data[36]){

  if(code == MSP_MY_RC || code == MSP_SET_RAW_RC){ //Do nothing if this is a control command
      //MSP_MY_RC and MSP_SET_RAW_RC
  }
  
  if(code == MSP_MY_INFO){ //MSP_MY_INFO recvd: Print the data
    getInfo = 1;
    mspStarted = 1;
//    for(int i=0; i< dataLen; i++){
//      pBuff[i] = dBuff[i];
//    }
    Alt = dBuff[3];
    mode = dBuff[2]; //Serial.println(Mode,BIN);
    Uptime = dBuff[1];
    Volts = dBuff[0]/10.0;
    
//      rxTog = !rxTog;
//      
//      int errs = dBuff[1]; errs |= dBuff[2] << 7; //Load bytes into 16-bit integer
//      boolean hF = bitRead(dBuff[4],1); //Read the bits: 2nd bit = HeadFree (1) or Mag Modes (0)
//      boolean ang = bitRead(dBuff[4],0); //1st bit: Angle/Level = 0, Horizon = 1;
//
//      lcd.clear(); lcd.setCursor(0, 0); //Clear LCD and print the data:
//      lcd.print("V "); lcd.print(float(dBuff[0]/10.0));
//      lcd.setCursor(7, 0);
//      lcd.print(" E:"); lcd.print(errs); lcd.print(" "); 
//      if(rxTog){ lcd.print("RX");}else{lcd.print(" ");}
//      lcd.setCursor(0, 1);
//      lcd.print("Tim:"); lcd.print(dBuff[3]);
//      lcd.setCursor(8, 1);
//      if(hF){ lcd.print("HF "); }else if(!hF) { lcd.print("MAG "); }
//      if(ang){ lcd.print("HRZ "); }else if(!ang) { lcd.print("ANG"); }
  }
}


void pollSerial(){
  
  if(Serial3.available() > 0){
    byte dd = Serial3.read();
    
    if(steps == 3){ //Received start of MSP packet, this should be the length:
      if(dd < 1 || dd > 31){ steps = 0; } //If out of range, restart steps
      else{
        dataLen = dd; //capture length of the data
        crc ^= dd; //start calculaing the checksum
        steps = 4; //go to next step
      }
    }else
    if(steps == 4){ 
      code = dd; //This is the MSP code, tells us what we received
      steps = 5;
      crc^= dd;
    }else
    if(steps == 5){ //Buffer the payload data
      crc ^= dd;
      dBuff[dBuffCnt] = dd;      
      ++dBuffCnt;
      if(dBuffCnt >= dataLen){ dBuffCnt = 0; steps = 6; } 
    }else
    if(steps == 6){ //Compare checksum to validate the data
      if(dd == crc){ evaluateCommand(dBuff); } //If valid, see what to do with it
      steps = 0;
      lcdNull = 1; 
    }else{
    
      switch(dd){ //For handling incoming data
        case '*': steps = 0; break;
        case '$': steps = 1;  break;
        case 'M': if(steps == 1){ steps = 2; }else{steps=0; if(!lcdNull){lcdPrnt(dd);} } break;
        case '<': if(steps == 2){ steps = 3; crc = 0;}else{steps=0; if(!lcdNull){lcdPrnt(dd);}} break;
        default:   if(steps==0 && dd < 127){ //If data is in range of what we are looking for
                      if(lcdNull){ //If this is not leftover data from MSP commands                        
                          if(dd == 'i' && lastD == 'i' ){ lcdNull = 0; } 
                      }else{
                          lcdPrnt(dd); //If not junk, pass to LCD
                      } 
                   }                   
                   lastD = dd;
                   steps = 0;
                   break;
      }
    }
  }
  
}



//Functions for converting and loading data into a sending buffer
//based on multiwii RCSerial protocol

//load two 8-bit values (16 bits) into sending array
void buffer16(int a){
  //convert 16-bit to 8Bit
  buffer8((a) & 0xFF); //strip the 'most significant bit' (MSB) and buffer
  buffer8((a>>8) & 0xFF); //move the MSB to LSB, strip the MSB and buffer
}

//load an 8-bit value into sending array
void buffer8(byte a){  
    toSend[buffCnt] = a;
    checksum ^= a;
    buffCnt++;
}


//Function to send the data once loaded
void sendBuffer(){
  toSend[buffCnt] = checksum; //checksum
  Serial3.print("$M<");  
  for(int u=0;u<buffCnt+2;u++){
    Serial3.write(toSend[u]);
  }
  checksum=0;
  buffCnt = 0;
}
