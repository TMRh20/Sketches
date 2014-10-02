Various Arduino sketches.

The code found here should be considered proof of concept only, and is very likely incomplete to some extent.


Arduino TV - Functional but incomplete - Based on TV Out library and other code - see tmrh20.blogspot.com
NTSC_8Mhz_Example - An example of producing video pixels on an NTSC display. Uses the UART
to draw the video at up to 8mhz, producing up to approx 400x216 video

SPI_UART - This is a little library that I used to drive nrf24l01 radio modules using the UART on Arduino boards.

Arduino Radio/Wireless Audio - Functional - near completion

TX_0 - TX/RX of reasonable quality wireless audio using two or more Arduinos and two or more NRF24L01 modules. 
Easily configurable development sketch. Looking at creating a library for wireless audio via Arduino and NRF24L01 modules. This is one of the initial mockup sketches as the code is nearing completion.
**Requires my fork of RF24 library found at https://github.com/TMRh20/RF24