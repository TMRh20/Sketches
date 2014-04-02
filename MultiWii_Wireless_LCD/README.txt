************************************************************************************
 MultiWii Wireless LCD
 by TMRh20
************************************************************************************

 The MultiWii flight control software has the built-in functionality of using an
attached LCD to view status and adjust configuration settings using either buttons,
or via RC controller/transmitter. This takes it one step further by sending the
data wirelessly to a remote LCD screen, which can be very handy for adjusting PID
settings and other configuration options while out flying.  

For use with a basic LCD Module (using Arduino LCD Library) and Wireless Serial Data
Transceiver (APC220, XBee, etc)

View status: Voltage, Errors, Running Time, Flight Modes: -Requires Modifications to MultiWii-

Sketch can use my Arduino WAV/PCM Audio library: https://github.com/TMRh20
Audio requires an SD card and some sort of output device (Speaker, amplifier, etc)

************************************************************************************

Requirements:

- Arduino based FC Board capable of using APC220 or XBee module
- DIY Wireless LCD: Arduino Mega + LCD + APC220/XBee

************************************************************************************

Useage:

Steps w/regular Transmitter:

1. Disarm MultiRotor
2. Move pitch stick up, yaw right to enter LCD Menu
3. Navigate menus using either option:	
	a: Serial Commands: MENU_PREV 'p', LCD_MENU_NEXT 'n', LCD_VALUE_UP 'u', DOWN 'd', MENU_SAVE_EXIT 's', LCD_MENU_ABORT 'x'
	b: Stick Movements: Pitch Fwd/Back: Prev/Next  Pitch R/L: Value Up/Dn
4. Pitch Fwd-Yaw Right = Abort/Exit, Pitch Fwd-Yaw Left = Save/Exit


Steps with my custom XBox controller:

1. Disarm MultiRotor
2. Hold Start button down to enter LCD Menu
3. B = next, X = prev, A = Value Dn, Y = Value Up, Black = Save/Exit, Release Start = Abort/Exit

************************************************************************************

Setup Steps:

Configure MultiWii for LCD (Config.h)
1. Un-Comment: #define LCD_CONF, #define LCD_TTY, #define LCD_SERIAL_PORT 3
2. Set LCD_SERIAL_PORT to the correct port for your transceiver

Configure the LCD:
1. Connect LCD to Arduino Mega and ensure it is working
1. Attach APC220 or other Serial data module to Arduino Mega
3. Comment out #define EN_SD in this sketch if not using an SD card
4. Upload this sketch to your Arduino with LCD
5. Enter LCD Menu via instructions above

************************************************************************************
