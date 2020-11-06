
File myFile;
bool filePlaying = false;

void audioPlay(){

  i2s_start(I2S_NUM_0);
  if (myFile) {
    Serial.println("Close Current");
    myFile.close();
  }
  myFile = SD.open(audioFileName);

  myFile.seek(44);
  Serial.print("file playing ");
  Serial.println(myFile.position());
  
  filePlaying = true;
  
}

void loadBuffer(){
  
  size_t bytesWritten = 0;
  
  if(myFile){
    
    i2s_write_expand(I2S_NUM_0, &dacBuffer[0], numSamples, 8, 16, &bytesWritten, 500 / portTICK_PERIOD_MS);
  if(myFile.available()){
    uint16_t tmpBuffer[64];    
    myFile.read((byte*)&tmpBuffer[0],64 );
    for(int i=0; i<32; i++){
      dacBuffer[i] = (uint8_t)((tmpBuffer[i] + 0x8000) >> 8);
    }
  }else{
    Serial.println("File close");
    myFile.close();
    filePlaying = false;
    i2s_stop(I2S_NUM_0); 
  }
  
  }
}
