
//The following function is run every loop to check the status of various external buttons



unsigned long volTime = 0;


void handleButtons(){
  
  boolean state = digitalRead(TX_PIN);         //Get the state of the transmitting pin                   
  
  if(!state){                                   //If the pin is low, start transmitting
    if(!transmitting){                         //Only do this if not already transmitting
      transmitting = 1;                        //Set the transmitting variable
      TX();                                    //Switch to TX mode
    } 
  }else{                                       //If the TX pin is low
    if(transmitting){                          //If still in transmitting mode
      RX(); transmitting = 0;                  //Start receiving/Stop transmitting      
    } 
  } 
  
  if(!digitalRead(VOL_UP_PIN)){                 //If the volume pin is high, raise the volume
    if(millis()-volTime > 200){                //De-bounce of buttons by 200ms
        volume(1); volTime = millis();
     }
  }else
  if(!digitalRead(VOL_DN_PIN)){
    if(millis()-volTime > 200){
        volume(0); volTime = millis(); 
    }     
  }
  
  if(!digitalRead(REMOTE_TX_PIN)){              //Start remote recording and transmission of audio
    if(recvMode){                              //If not transmitting already
        radio.stopListening();                 //Stop listening and write the request twice for good measure
        radio.write(&txCmd,2); 
        radio.write(&txCmd,2); 
        radio.startListening();}
        delay(200);                           //Delay as a simple button debounce
  }else
  if(!digitalRead(REMOTE_RX_PIN)){             //With TX timeout enabled, this will stop remote recording by stopping transmission for 2 seconds
        delay(2000);                          
  }
  
  
  
}
