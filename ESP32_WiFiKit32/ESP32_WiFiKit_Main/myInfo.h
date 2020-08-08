

const char* root_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIElDCCA3ygAwIBAgIQAf2j627KdciIQ4tyS8+8kTANBgkqhkiG9w0BAQsFADBh\n" \
"MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n" \
"d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n" \
"QTAeFw0xMzAzMDgxMjAwMDBaFw0yMzAzMDgxMjAwMDBaME0xCzAJBgNVBAYTAlVT\n" \
"MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxJzAlBgNVBAMTHkRpZ2lDZXJ0IFNIQTIg\n" \
"U2VjdXJlIFNlcnZlciBDQTCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEB\n" \
"ANyuWJBNwcQwFZA1W248ghX1LFy949v/cUP6ZCWA1O4Yok3wZtAKc24RmDYXZK83\n" \
"nf36QYSvx6+M/hpzTc8zl5CilodTgyu5pnVILR1WN3vaMTIa16yrBvSqXUu3R0bd\n" \
"KpPDkC55gIDvEwRqFDu1m5K+wgdlTvza/P96rtxcflUxDOg5B6TXvi/TC2rSsd9f\n" \
"/ld0Uzs1gN2ujkSYs58O09rg1/RrKatEp0tYhG2SS4HD2nOLEpdIkARFdRrdNzGX\n" \
"kujNVA075ME/OV4uuPNcfhCOhkEAjUVmR7ChZc6gqikJTvOX6+guqw9ypzAO+sf0\n" \
"/RR3w6RbKFfCs/mC/bdFWJsCAwEAAaOCAVowggFWMBIGA1UdEwEB/wQIMAYBAf8C\n" \
"AQAwDgYDVR0PAQH/BAQDAgGGMDQGCCsGAQUFBwEBBCgwJjAkBggrBgEFBQcwAYYY\n" \
"aHR0cDovL29jc3AuZGlnaWNlcnQuY29tMHsGA1UdHwR0MHIwN6A1oDOGMWh0dHA6\n" \
"Ly9jcmwzLmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RDQS5jcmwwN6A1\n" \
"oDOGMWh0dHA6Ly9jcmw0LmRpZ2ljZXJ0LmNvbS9EaWdpQ2VydEdsb2JhbFJvb3RD\n" \
"QS5jcmwwPQYDVR0gBDYwNDAyBgRVHSAAMCowKAYIKwYBBQUHAgEWHGh0dHBzOi8v\n" \
"d3d3LmRpZ2ljZXJ0LmNvbS9DUFMwHQYDVR0OBBYEFA+AYRyCMWHVLyjnjUY4tCzh\n" \
"xtniMB8GA1UdIwQYMBaAFAPeUDVW0Uy7ZvCj4hsbw5eyPdFVMA0GCSqGSIb3DQEB\n" \
"CwUAA4IBAQAjPt9L0jFCpbZ+QlwaRMxp0Wi0XUvgBCFsS+JtzLHgl4+mUwnNqipl\n" \
"5TlPHoOlblyYoiQm5vuh7ZPHLgLGTUq/sELfeNqzqPlt/yGFUzZgTHbO7Djc1lGA\n" \
"8MXW5dRNJ2Srm8c+cftIl7gzbckTB+6WohsYFfZcTEDts8Ls/3HB40f/1LkAtDdC\n" \
"2iDJ6m6K7hQGrn2iWZiIqBtvLfTyyRRfJs8sjX7tN8Cp1Tm5gr8ZDOo0rwAhaPit\n" \
"c+LJMto4JQtV05od8GiG7S5BNO98pVAdvzr508EIDObtHopYJeS4d60tbvVS3bR0\n" \
"j6tJLp07kzQoH3jOlOrHvdPJbRzeXDLz\n" \
"-----END CERTIFICATE-----\n";

const char* root_weather_ca= \
"-----BEGIN CERTIFICATE-----\n" \
"MIIF3jCCA8agAwIBAgIQAf1tMPyjylGoG7xkDjUDLTANBgkqhkiG9w0BAQwFADCB\n" \
"iDELMAkGA1UEBhMCVVMxEzARBgNVBAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0pl\n" \
"cnNleSBDaXR5MR4wHAYDVQQKExVUaGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNV\n" \
"BAMTJVVTRVJUcnVzdCBSU0EgQ2VydGlmaWNhdGlvbiBBdXRob3JpdHkwHhcNMTAw\n" \
"MjAxMDAwMDAwWhcNMzgwMTE4MjM1OTU5WjCBiDELMAkGA1UEBhMCVVMxEzARBgNV\n" \
"BAgTCk5ldyBKZXJzZXkxFDASBgNVBAcTC0plcnNleSBDaXR5MR4wHAYDVQQKExVU\n" \
"aGUgVVNFUlRSVVNUIE5ldHdvcmsxLjAsBgNVBAMTJVVTRVJUcnVzdCBSU0EgQ2Vy\n" \
"dGlmaWNhdGlvbiBBdXRob3JpdHkwggIiMA0GCSqGSIb3DQEBAQUAA4ICDwAwggIK\n" \
"AoICAQCAEmUXNg7D2wiz0KxXDXbtzSfTTK1Qg2HiqiBNCS1kCdzOiZ/MPans9s/B\n" \
"3PHTsdZ7NygRK0faOca8Ohm0X6a9fZ2jY0K2dvKpOyuR+OJv0OwWIJAJPuLodMkY\n" \
"tJHUYmTbf6MG8YgYapAiPLz+E/CHFHv25B+O1ORRxhFnRghRy4YUVD+8M/5+bJz/\n" \
"Fp0YvVGONaanZshyZ9shZrHUm3gDwFA66Mzw3LyeTP6vBZY1H1dat//O+T23LLb2\n" \
"VN3I5xI6Ta5MirdcmrS3ID3KfyI0rn47aGYBROcBTkZTmzNg95S+UzeQc0PzMsNT\n" \
"79uq/nROacdrjGCT3sTHDN/hMq7MkztReJVni+49Vv4M0GkPGw/zJSZrM233bkf6\n" \
"c0Plfg6lZrEpfDKEY1WJxA3Bk1QwGROs0303p+tdOmw1XNtB1xLaqUkL39iAigmT\n" \
"Yo61Zs8liM2EuLE/pDkP2QKe6xJMlXzzawWpXhaDzLhn4ugTncxbgtNMs+1b/97l\n" \
"c6wjOy0AvzVVdAlJ2ElYGn+SNuZRkg7zJn0cTRe8yexDJtC/QV9AqURE9JnnV4ee\n" \
"UB9XVKg+/XRjL7FQZQnmWEIuQxpMtPAlR1n6BB6T1CZGSlCBst6+eLf8ZxXhyVeE\n" \
"Hg9j1uliutZfVS7qXMYoCAQlObgOK6nyTJccBz8NUvXt7y+CDwIDAQABo0IwQDAd\n" \
"BgNVHQ4EFgQUU3m/WqorSs9UgOHYm8Cd8rIDZsswDgYDVR0PAQH/BAQDAgEGMA8G\n" \
"A1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQEMBQADggIBAFzUfA3P9wF9QZllDHPF\n" \
"Up/L+M+ZBn8b2kMVn54CVVeWFPFSPCeHlCjtHzoBN6J2/FNQwISbxmtOuowhT6KO\n" \
"VWKR82kV2LyI48SqC/3vqOlLVSoGIG1VeCkZ7l8wXEskEVX/JJpuXior7gtNn3/3\n" \
"ATiUFJVDBwn7YKnuHKsSjKCaXqeYalltiz8I+8jRRa8YFWSQEg9zKC7F4iRO/Fjs\n" \
"8PRF/iKz6y+O0tlFYQXBl2+odnKPi4w2r78NBc5xjeambx9spnFixdjQg3IM8WcR\n" \
"iQycE0xyNN+81XHfqnHd4blsjDwSXWXavVcStkNr/+XeTWYRUc+ZruwXtuhxkYze\n" \
"Sf7dNXGiFSeUHM9h4ya7b6NnJSFd5t0dCy5oGzuCr+yDZ4XUmFF0sbmZgIn/f3gZ\n" \
"XHlKYC6SQK5MNyosycdiyA5d9zZbyuAlJQG03RoHnHcAP9Dc1ew91Pq7P8yF1m9/\n" \
"qS3fuQL39ZeatTXaw2ewh0qpKJ4jjv9cJ2vhsE/zB+4ALtRZh8tSQZXq9EfX7mRB\n" \
"VXyNWQKV3WKdwrnuWih0hKWbt5DHDAff9Yk2dDLWKMGwsAvgnEzDHNb842m1R0aB\n" \
"L6KCq9NjRHDEjf8tM7qtj3u1cIiuPhnPQCjY/MiQu12ZIvVS5ljFH4gxQ+6IHdfG\n" \
"jjxDah2nGN59PRbxYvnKkKj9\n" \
"-----END CERTIFICATE-----\n";


uint32_t covUpdateTimer = 0;
char covBuf1[21];
char covBuf2[21];
char covBuf3[21];

uint32_t wUpdateTimer = 0;

char weatherBuf1[21];
char weatherBuf2[21];
char weatherBuf3[21];

void getCovidData(){

  if(millis() - covUpdateTimer > (60000 * 60 * 6) || strlen(covBuf1) < 2) { //Only fetch new data every 6hrs

    covUpdateTimer = millis();
    Serial.println("Update Covid data from http");

    // First we need to get the file name,since it changes daily
    HTTPClient myHttp0;
    int httpCode = 0;
    char fileNameBuffer[20];
    myHttp0.begin("https://dashboard.saskatchewan.ca/health-wellness/covid-19/cases",root_ca);
    httpCode = myHttp0.GET();
    if(httpCode == HTTP_CODE_OK){
      WiFiClient * stream = myHttp0.getStreamPtr();
      stream->setTimeout(1000);
      stream->find("/export/cases/");      
      stream->readBytesUntil('"',fileNameBuffer, sizeof(fileNameBuffer));
      //Serial.println(fileNameBuffer);
      //printf("%s\n",fileNameBuffer);
    }
    myHttp0.end();

    //Then add the filename to the known file URL and grab the information                      
    char myBuffer[65] = {"https://dashboard.saskatchewan.ca/export/cases/1381.csv"}; //47 characters including last /
    printf("%s\n",myBuffer);
    memcpy(&myBuffer[47],fileNameBuffer,4);
    printf("%s\n",myBuffer);

    
    char *myPtr = &myBuffer[0];
    HTTPClient myHttp;
    myHttp.begin(myPtr);

    httpCode = myHttp.GET();
    if(httpCode == HTTP_CODE_OK) {
      WiFiClient * stream = myHttp.getStreamPtr();
      stream->setTimeout(1000);
      struct tm timeInfo;
      char myBuffer[21];
      if(getLocalTime(&timeInfo)){
        timeInfo.tm_mday -= 1; // We want yesterdays days because data is not updated right at midnight
        strftime(myBuffer,20,"%Y/%m/%d",&timeInfo);
      }else{Serial.println("HTTP Covid Time FAIL"); return;}
      //Serial.println(myBuffer);
      stream->find(myBuffer);
      stream->find("Regina,");
      uint16_t newCases = stream->parseInt();
      stream->parseInt();
      stream->parseInt();
      uint16_t activeCases = stream->parseInt();
      stream->parseInt();
      uint16_t inICU = stream->parseInt();
      
      stream->parseInt();
      uint16_t deaths = stream->parseInt();
    
      printf("Local Covid19 Stats: New:%d Active:%d InICU:%d Deaths:%d\n",newCases,activeCases,inICU,deaths);
      sprintf(covBuf1,"Local Cov19: New %d",newCases);
      sprintf(covBuf2,"Active %d InICU: %d",activeCases,inICU);
      sprintf(covBuf3,"Deaths %d",deaths);
    
    }else{
      Serial.print("HTTP GET FAIL covid code:");
      Serial.println(httpCode);
    }
    myHttp.end();
  }
  
  if(strlen(covBuf1) > 1){
    printString(covBuf1,strlen(covBuf1)+1,3);
    printString(covBuf2,strlen(covBuf2)+1,4);
    printString(covBuf3,strlen(covBuf3)+1,5);
  }
}




void getWeatherData(){
  
  if( ((millis() - wUpdateTimer) > (60000 * 60) ) || (strlen(weatherBuf1) < 2) ){ //Only update weather every hour
    Serial.println("Update Weather from http timer: ");
    wUpdateTimer = millis();
  
    HTTPClient weatherClient;
    float temp = 0;
    float tempLow = 0;
    float tempHigh = 0;
    float windSpeed = 0;
    const char* no_ca="";
    weatherClient.begin(mySecretURL,root_weather_ca); //API Key is in URL
    int httpCode = weatherClient.GET();
    if (httpCode > 0) { //Check for the returning code
      if(httpCode == HTTP_CODE_OK) { Serial.println("Get Weather");
        WiFiClient * stream = weatherClient.getStreamPtr();
        stream->setTimeout(1000);
        stream->find("iption\":\"");
        char main[32] = "";
        char tempArray[8] = "";
        stream->readBytesUntil('"',main, sizeof(main));
        stream->find("temp\":");
        stream->readBytesUntil('"',tempArray,sizeof(tempArray));
        temp = atof(tempArray);
        temp -= 273.15; //Celcius = Kelvin - 273.15
        stream->find("temp_min\":");
        stream->readBytesUntil('"',tempArray,sizeof(tempArray));
        tempLow = atof(tempArray);
        tempLow -= 273.15;
        stream->find("temp_max\":");
        stream->readBytesUntil('"',tempArray,sizeof(tempArray));
        tempHigh = atof(tempArray);
        tempHigh -= 273.15;
        stream->find("speed\":");
        memset(tempArray,0,sizeof(tempArray));
        stream->readBytesUntil('"',tempArray,sizeof(tempArray));
        windSpeed = atof(tempArray);
        windSpeed *= 3.6; //meters per second to KM/h

        printf("Weather: %s, Temp: %.0f, Low: %.0f, High: %.0f, Wind: %.0f KMh\n",main,temp,tempLow,tempHigh,windSpeed);

        sprintf(weatherBuf1,"Weather %s",main);
        sprintf(weatherBuf2,"Temp %.0f Wind %.0fKM/h",temp,windSpeed);
        sprintf(weatherBuf3,"High %.0f Low %.0f",tempHigh,tempLow);
      }
    }else{
      Serial.print("HTTP Code ");
      Serial.println(httpCode);
    }
    weatherClient.end();
  }
  if(strlen(weatherBuf1) > 1){
    printString(weatherBuf1,strlen(weatherBuf1)+1,3);
    printString(weatherBuf2,strlen(weatherBuf2)+1,4);
    printString(weatherBuf3,strlen(weatherBuf3)+1,5);
  }  

  
}
