
//This is just a simple function to get the time and return the values

bool getTheTime(char* buf){

  struct tm timeInfo;
  if(!getLocalTime(&timeInfo)){
    Serial.println("Fail to get time");
    return false;
  }else{
    strftime(buf,25,"%a %b %d %T",&timeInfo);
    //Serial.println(buf);
  }
  return true;
}
