1. Install zephyrproject sdk etc
2. Edit zephyrproject/zephyr/west.yml
3. Under projects add:
```
    - name: arduino-core-zephyr
      path: modules/lib/arduinocore-zephyr
      revision: main
      url: https://github.com/arduino/ArduinoCore-zephyr
    - name: arduino-api
      path: modules/lib/arduinocore-zephyr/cores/arduino
      revision: master
      url: https://github.com/arduino/ArduinoCore-API
```

4. Run west update
5. Run west blobs fetch
6. Fix compilation errors (Fixes have been submitted via GitHub):
7. Edit the file zephyrproject/modules/lib/arduinocore-zephyr/cores/arduino/zephyrSerial.cpp line 82
8. Change the line to `uart_irq_update(uart);`
9. Edit the file zephyrproject/modules/lib/arduinocore-zephyr/cores/arduino/inlines.h lines 16 & 20
10. Change both functions to read `static inline __attribute__((always_inline))`
11. Edit the file zephyrproject/modules/lib/arduinocore-zephyr/cores/arduino/api/Common.h lines 124 & 125 where delay and delayMicroseconds are declared
12. Change both functions to `static void delay(unsigned long);` & `static void delayMicroseconds(unsigned int us);`
13. Download the ZephyrBuild sketch from this repo
14. Compile the project from the zephyrproject directory:

`west build -p always -b arduino_uno_q <path-to-your-project>`

15. Copy the zephyr.elf file from the build/zephyr/ directory to the Arduino Q then run this from the same directory as the elf file to upload the sketch

`/opt/openocd/bin/openocd   -s /opt/openocd   -f openocd_gpiod.cfg   -c "program zephyr.elf verify reset exit"`

Note: Required Arduino libraries (Install from source or the Arduino Library Manager): 

Arduino Library Manager:
`lwIP, RF24, RF24Mesh, Arduino Graphics`

From source:
```
RF24Network
RF24Ethernet : https://github.com/nRF24/RF24Ethernet/tree/ZephyrNetworking
```
