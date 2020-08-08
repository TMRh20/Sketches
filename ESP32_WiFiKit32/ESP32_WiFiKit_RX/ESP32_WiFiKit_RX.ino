
#include <WiFi.h>
#include <U8g2lib.h> 
#include "myOLED.h"
#include "myNTP.h"
#include "time.h"

/************** USER CONFIG ***********************/
const char* ssid     = "your-ssid";
const char* password = "your-pwd";

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = -21600;
const int   daylightOffset_sec = 0;
/**************************************************/

WiFiServer server(80);

void setup() {
  Serial.begin(115200);
  pinMode(25, OUTPUT);      // set the LED pin mode
  Serial.print("\n\nConnecting to ");
  Serial.println(ssid);
  WiFi.mode(WIFI_STA);
  //WiFi.setHostname("Dormouse");
  WiFi.begin(ssid, password);
  if(WiFi.waitForConnectResult() != WL_CONNECTED){
    Serial.println("Connection Failed!");
    delay(5000);
    ESP.restart();
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
  server.begin();

  charPtr[0] = &charBuffer1[0]; //Pointers for graphics stuff
  charPtr[1] = &charBuffer2[0];
  charPtr[2] = &charBuffer3[0];
  charPtr[3] = &charBuffer4[0];
  charPtr[4] = &charBuffer5[0];
  u8g2.begin();  

  digitalWrite(25,HIGH);
  delay(1000);  
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  digitalWrite(25,LOW);
}

float temperature = 25;
float humidity = 50;
uint32_t timeTimer = millis();
uint32_t tempTimer = millis();
bool ledState = HIGH;
bool ledAlert = true;

void loop() {

  if(millis()-tempTimer>125){
    tempTimer=millis();
    if(temperature < 17 || temperature > 29 || humidity > 65 || humidity < 40){
      if(ledAlert == true){
        digitalWrite(25,ledState);
        ledState = !ledState;
      }
      printString("Temp/Humidity Alert!",20,5);
    }else{
      digitalWrite(25,LOW);
      printString(" ",2,5);
    }
  }
  
  if(millis()-timeTimer > 900){
    timeTimer=millis();
    char timeBuf[25];
    if(getTheTime(timeBuf)){
      printString(timeBuf,strlen(timeBuf)+1,1);
    }
  }

  WiFiClient client = server.available();
   if (client) {
     Serial.println("Client Connected");
     while (client.connected()) {
       if(client.available()){
         client.find("/");
         if( !(isDigit(client.peek())) ){
           if(client.read() == 's'){
            ledAlert = false;
            digitalWrite(25,LOW);
           }else{
            ledAlert = true;
           }
           client.stop();
           break;
         }
         temperature = client.parseFloat();
         humidity = client.parseFloat();
         Serial.print("Temp: ");
         Serial.println(temperature);
         Serial.print("Hum: ");
         Serial.println(humidity);
         client.stop();
         Serial.println("Client Disconnected.");
         char dataBuffer[70];
         sprintf(dataBuffer,"Temperature %.1f",temperature);
         printString(dataBuffer,strlen(dataBuffer)+1,2);
         sprintf(dataBuffer,"Humidity %.1f",humidity);
         printString(dataBuffer,strlen(dataBuffer)+1,3);
         
       }
     }
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

  delay(10);
}
