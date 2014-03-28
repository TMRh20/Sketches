

void setRadio(){

  radio.begin();
  
  radio.setChannel(1);                 //Set RF channel to 1
  radio.setAutoAck(0);                 //Disable ACKnowledgement packets
  radio.setPALevel(RF24_PA_MAX);       //PA level to output
  radio.setDataRate(RF_SPEED);         //Set data rate as specified in user options
  radio.setCRCLength(RF24_CRC_8);  
  //radio.disableCRC();                //Disable CRC for extra speed
  
  radio.openWritingPipe(pipes[0]);     //Set up reading and writing pipes
  radio.openReadingPipe(1,pipes[1]);
  
  radio.startListening();              //NEED to start listening after opening a reading pipe  
  radio.printDetails();                //Print info via printf
}
