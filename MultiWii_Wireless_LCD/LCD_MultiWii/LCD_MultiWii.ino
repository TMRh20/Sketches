
//LCD MultiWii by TMRh20:
//For use with a basic LCD Module and Wireless Serial Data Transceiver (APC220, XBee, etc)
//View status: Voltage, Errors, Running Time, Flight Modes: Requires Modificatins to MultiWii
//Configuration: Tune PID and General MultiWii settings wirelessly using controller 
//Steps w/regular Transmitter:
// 1. Disarm MutliRotor
// 2. Move pitch stick up, yaw right to enter LCD Menu
// 3. Navigate menus using either option:
//        a: Serial Commands: MENU_PREV 'p', LCD_MENU_NEXT 'n', LCD_VALUE_UP 'u', DOWN 'd', MENU_SAVE_EXIT 's', LCD_MENU_ABORT 'x'
//        b: Stick Movements: Pitch Fwd/Back: Prev/Next  Pitch R/L: Value Up/Dn
//4. Pitch Fwd-Yaw Right = Abort/Exit, Pitch Fwd-Yaw Left = Save/Exit 
//Steps with my custom XBox controller:
// 1. Disarm MutliRotor
// 2. Hold Start button down
// 3. B = next, X = prev, A = Value Dn, Y = Value Up, Black = Save/Exit, Release Start = Abort/Exit

//Codes for use in MultiWii Serial Protocol (MSP)
#define MSP_MY_INFO              197
#define MSP_SET_RAW_RC           200
#define MSP_MY_RC                199

//Enable/Disable SD functionality
#define EN_SD

// include the LCD library and my Audio Library:
#include <LiquidCrystal.h>

#if defined EN_SD   
  #include <SD.h>
  #include <TMRpcm.h> 
#endif

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

int steps = 0;

#if defined EN_SD
    TMRpcm wav;
    char* fileName = "/Altitude/AltMx";
#endif

void setup() {
  
  Serial.begin(115200);
  Serial3.begin(57600); //APC220 Port
  //setAPC220(); //Uncomment for APC220 config
  
  pinMode(13,OUTPUT); // enable LED
  // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2); lcd.clear();
  lcd.print("MultiWii Remote"); lcd.setCursor(0,1); lcd.print("   LCD by TMRh20"); 

  #if defined EN_SD  
    if (!SD.begin()) { Serial.println("SD fail"); return;
    }else{             Serial.println("SD ok"); }  
    wav.speakerPin = 46;
    wav.pwmMode = 1;
    pinMode(45,OUTPUT);
    
    startAltLogging();
  #endif
  
}

unsigned long timer = 0, infoTimer = 0;
boolean getInfo = 0;
byte dBuff[36], pBuff[36], dataLen = 0;
boolean rxTog = 0, mspStarted = 0;
//float volts = 0.00;
byte Alt = 0,mode=0, Uptime = 0;
float Volts=0;
//byte mode = 0;

unsigned long aTimer = 0;

void loop() {  
  
  lcdDisplay(); 
  
  pollSerial(); //handle Serial data
  
  
  #if defined EN_SD
    if(millis() - aTimer > 100){ aTimer=millis(); pollAudio(); }
    doSD();
  #endif
  
 
  
//  if(Alt < 6){ //1Meter
//    if(!wav.isPlaying()){
//      wav.play("soc11");
//      wav.volume(0);
//    }
//  }else
//    if(wav.isPlaying()){
//      wav.stopPlayback();
//    }
}

