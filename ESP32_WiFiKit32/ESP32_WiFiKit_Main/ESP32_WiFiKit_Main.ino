
#include <WiFi.h>       //ESP32 Specific includes
#include <HTTPClient.h>
#include "lwip/inet.h"
#include "time.h"

#include <U8g2lib.h>    //OLED Includes
#include "DHTesp.h"     //DHT TEmperature sensor

#include "myOLED.h"     //Includes for attached sketch files
#include "myNTP.h"
                        //Note: This URL requires an API key and additional info on the end of it to work
const char* mySecretURL="https://api.openweathermap.org/data/2.5/weather";
#include "myInfo.h"

/************************ User Config ***********************************/

const char* ssid     = "your-ssid";
const char* password = "your-password";

uint8_t dhtPin = 17; //Configure the data pin for the DHT sensor

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -21600;
const int   daylightOffset_sec = 0;

char otherModule[35] = {"http://10.10.1.248/"}; //Enter the IP/Hostname of your other module here
/************************************************************************/

DHTesp dhtSensor;
TempAndHumidity sensorData;


void setup() {

  Serial.begin(57600);
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  //WiFi.disconnect();
  WiFi.mode(WIFI_STA);
  //WiFi.setHostname("TheCaterpillar");
  WiFi.begin(ssid, password);

  
  if(WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println("Connection Failed!");
    delay(5000);
    ESP.restart();
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  charPtr[0] = &charBuffer1[0]; //Pointers for graphics stuff
  charPtr[1] = &charBuffer2[0];
  charPtr[2] = &charBuffer3[0];
  charPtr[3] = &charBuffer4[0];
  charPtr[4] = &charBuffer5[0];
  u8g2.begin();
  
  dhtSensor.setup(dhtPin, DHTesp::DHT22);

  delay(1000);  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  
}


  uint32_t dhtTimer = millis();
  uint32_t timeTimer = millis();
  uint32_t weatherTimer = millis();
  bool weatherOrCovid = 0;

void loop() {

  if(Serial.available()){
    char c = Serial.read();
    if(c == 'R'){
      ESP.restart();
    }
  }

  if(millis() - weatherTimer > 10000){
    weatherTimer=millis();
    if(weatherOrCovid){
      getWeatherData();
    }else{
      getCovidData();
    }
    weatherOrCovid = !weatherOrCovid;
  }

  if(millis() - timeTimer > 800){
    timeTimer = millis();
    char myBuffer[25];
    if( getTheTime(myBuffer) ){
      printString(myBuffer,strlen(myBuffer)+1,1);
    }    
  }

  if(millis() - dhtTimer > 5000){
    dhtTimer=millis();
    Serial.print("Temp: ");
    Serial.print(sensorData.temperature);
    Serial.print(" Humidity: ");
    Serial.println(sensorData.humidity);

    sensorData = dhtSensor.getTempAndHumidity();
    char buffer[20];
    sprintf(buffer,"Temp: %.0f Hum: %.0f%%",sensorData.temperature,sensorData.humidity);    
    printString(buffer,strlen(buffer)+1,2); // Print the temp & humidity on line 2

    HTTPClient dhtClient;
    int httpCode = 0;

    char url[35];
    memcpy(url,otherModule,strlen(otherModule));
    //Send the temp and humidity readings to the other module
    sprintf(&url[strlen(otherModule)],"%.1f,%.1f\n\r",sensorData.temperature,sensorData.humidity);
    char* myPtr = &url[0];
    dhtClient.begin(myPtr);
    httpCode = dhtClient.GET();
    dhtClient.end();
    //Serial.println(url);
  }

  u8g2.firstPage();
  do {                                  //Font group is adobe x11
    u8g2.setFont(u8g2_font_courB08_tr); //Height is 10, width 7, mas chars/line is about 17 probly
    u8g2.drawStr(1,11,charPtr[0]);
    u8g2.drawStr(1,23,charPtr[1]);
    u8g2.drawStr(1,35,charPtr[2]);
    u8g2.drawStr(1,47,charPtr[3]);
    u8g2.drawStr(1,59,charPtr[4]);    
  } while ( u8g2.nextPage() );
  delay(250);
//  Serial.println("OK");
  

}
