
//********Radio Defines ****************************
// Radio pipe addresses for the 2 nodes to communicate.
const uint64_t pipes[4] = { 0xABCDABCD71LL, 0x544d52687CLL, 0x544d526832LL };


void setRadio(){

  radio.begin();
  
  radio.setChannel(1);  
  radio.setAutoAck(0);
  radio.setPALevel(RF24_PA_MAX);
  radio.setDataRate(RF_SPEED);     //or RF24_1MBPS
  radio.setCRCLength(RF24_CRC_8);  
  //radio.disableCRC();                //Disable CRC for extra speed
  if(!RADIO_NO){
    radio.setRetries(2,3);            //Timeout, No of retries
    radio.openWritingPipe(pipes[0]);   //Set up reading and writing pipes
    radio.openReadingPipe(1,pipes[1]);
  }else{
    radio.setRetries(3,5);            //Timeout, No of retries
    radio.openWritingPipe(pipes[1]);   //Set up reading and writing pipes
    radio.openReadingPipe(1,pipes[0]);
  }
  radio.startListening();            //NEED to start listening after opening a reading pipe  
  radio.printDetails();              //Print info via printf
  TX();
}
