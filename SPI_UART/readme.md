
**SPI_UART Library**

Enables a secondary SPI bus using UART or USART

Arduino Due Pins:

MOSI    TX1
MISO    RX1
SCK     SDA1
CS&CE   User selected

Arduino Mega:
MOSI    TX1
MISO    RX1
SCK     See https://github.com/TMRh20/RF24/issues/24#issuecomment-57693347
CS&CE   User selected

Arduino Uno, Nano, etc:
MOSI	TX(pin 0)
MISO	RX(pin 1)
SCK	    XCK(pin 4)