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
6. Download the ZephyrBuild sketch from this repo
7. Compile the project from the zephyrproject directory:

`west build -p always -b arduino_uno_q <path-to-your-project>`

15. Copy the zephyr.elf file from the build/zephyr/ directory to the Arduino Q then run this from the same directory as the elf file to upload the sketch

`/opt/openocd/bin/openocd   -s /opt/openocd   -f openocd_gpiod.cfg   -c "program zephyr.elf verify reset exit"`

Note: Required Arduino libraries (Install from source or the Arduino Library Manager): 

Arduino Library Manager:
`RF24Ethernet, Arduino Graphics`

