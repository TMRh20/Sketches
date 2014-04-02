


void setAPC220(){
  
   //Used for setting APC220 configuration parameters: Baud, Air-Rate, Power, etc.
  pinMode(31,OUTPUT); digitalWrite(31,HIGH); //write SET pin HIGH
  delay(250); digitalWrite(31,LOW); //Begin SET
    Serial3.end(); Serial3.begin(9600); //default UART speed for setting parameters
  delay(220); char* tesst = "WR 450000 4 0 6 0"; //RTFM
  Serial3.println(tesst);
  delay(250);
  Serial3.end(); Serial3.begin(57600); //back to regular UART speed
  digitalWrite(31,HIGH);  //disable SET
}
