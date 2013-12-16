

byte steps = 0;
unsigned int dotByte;
byte dotBit;

int sBuffCnt = 0;

void SerialIn(){
  
//  byte *tmp = &data[0];
  byte dat;
  //if(Serial3.available() > 0){
    if((UCSR3A & (1<<RXC3))){   
    digitalWrite(13,HIGH);
    //Serial.println("g");
     dat = UDR3;
     
//      
//     if(dat == 254 && steps == 0){
//        steps = 1; 
//         //Serial.println("st0");
//     }else
//     if(steps == 1 && dat == 170){
//       // dotByte = dat;
//        steps = 2;
//         //Serial.println("st1");
//     }else
//     if(steps == 1 && dat != 170){ steps = 0; }
//     else
//     if(steps == 2){
        sBuffCnt++;
        if(sBuffCnt >= displayWidth-1){sBuffCnt = 1; steps = 0; redraw = 1; clearScreen();
              
        }
        if(dat > 128){steps = 1;}
        sBuffer[sBuffCnt-1]=dat;
//     }
     
     
//     if(steps == 2){       
//       dotByte |= dat << 8;      
//       steps = 3;
//       //Serial.println("st2"); 
//     }else
//     if(steps == 3){
//        bitSet(data[dotByte],dat);
//        //if(dotByte >= wLDiv){clearScreen();}
//        steps = 0;
//        cntr++;
//     }else
//      if( dat == 'C' && steps == 0){
//        clearScreen();
//     }
       
//     else
//     if(steps == 0){
//       constrain(dat,0,100);
//       printChar(dat,dat,dat); 
//     }
    //cntr++;
     
     digitalWrite(13,LOW);
  }
  
    
  
}
