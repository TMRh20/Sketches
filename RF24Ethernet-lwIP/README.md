This is an copy of the RF24Ethernet library designed to work with the lwIP IP stack.
The code should be considered experimental as it is currently in development. 

Requres:
1. Arduino lwIP library can be installed from Arduino Library Manager

To install:
1. Copy the RF24Ethernet folder into your Arduino Libraries folder at Documents/Arduino/libraries/
2. See the docs and getting started information at https://nrf24.github.io/RF24Ethernet/
3. The library is configured to detect CPU speeds over 50Mhz and use lwIP automatically, but users can edit RF24Ethernet.h and #define USE_LWIP
4. See lwipopts.h included with the lwIP library to configure the stack
5. For now users would need install RF24Network from zip and edit the RF24Network_config.h file and set #define MAX_PAYLOAD_SIZE 1514
