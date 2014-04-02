

boolean lcdCurs = 0, lcdClr =0, gotRtn = 0, wasWord = 0;
int spCnt = 0, printCnt = 0;
byte lastByte = 0;
char *modes[8] = {"ARM ","HRZ ","ANG ","HF ","MAG ","ALT "};


void lcdDisplay(){
  
 if(millis() - infoTimer > 1100){
   
   if(getInfo){
      infoTimer = millis(); getInfo=0;
      rxTog = !rxTog;
      lcd.clear(); lcd.setCursor(0, 0); //Clear LCD and print the data:
      //volts = float(Volts/10.0);
      lcd.print("V"); lcd.print(Volts);      
      lcd.print(" T:"); lcd.print(Uptime);
      lcd.print(" H:"); lcd.print(Alt);
      lcd.setCursor(15, 1);
      //lcd.print("E:"); lcd.print(errs); lcd.print(" "); 
      if(rxTog){ lcd.print("*");}else{lcd.print(" ");}
      lcd.setCursor(0, 1);
      
      lcd.setCursor(0, 1);
      for(int i=0; i<8; i++){
        if(bitRead(mode,i)){
          lcd.print(modes[i]);
        }
      }
   }
   
   
  }
  
  
}



//Function for handling data not associated to MSP commands

void lcdPrnt(byte bb){  
  
 if(gotRtn){ //If received a 10 (Return character), do the following
   
     if(bb < 58 && bb > 32  ){ //If this is an ASCII number character
       lcd.setCursor(0,1);     //Set LCD to second row 
       gotRtn = 0; wasWord = 0; printCnt = 0;
     }
     if(bb >= 58  && bb < 127 ){ //If this is an ASCII letter      
       if(!wasWord){
           tone(46,2500,15);
           lcd.clear(); lcd.setCursor(0,0); printCnt=0; //Set to home row
       } 
           gotRtn = 0; 
       if(printCnt > 2){ //If we got 2 words in a row, keep printing on the same line, but next will clear
         wasWord = 0; printCnt=0;
         if(lastByte != 32){lcd.print(" ");}
       }else{
         wasWord=1;
       }
       
     }
     //delay(25);
 } 
  
 if(bb > 32 && bb < 127){//If valid ASCII character
   if(lastByte == 32 ){  lcd.print(" "); printCnt++;} //If the last byte was a space, print it
   lcd.write(bb); ++printCnt; //delay(25); //Write the current character to screen
 }
 
 
 if(bb == 10){ gotRtn = 1; } //catch return characters
 lastByte = bb;
 
// Serial.print(bb,DEC); 
 Serial.write(bb); //For monitoring over a regular serial connection with no LCD
}
