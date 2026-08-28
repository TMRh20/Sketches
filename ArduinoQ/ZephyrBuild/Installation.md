1. Install zephyrproject sdk etc
2. Edit zephyrproject/zephyr/west.yml
3. Under projects add:
```
    - name: arduino-core-zephyr
      path: modules/lib/arduinocore-zephyr
      revision: main
      url: https://github.com/arduino/ArduinoCore-zephyr
```
4. Run west update
5. Run west blobs fetch
6. Edit the file /zephyrproject/modules/lib/arduinocore-zephyr/cores/arduino/zephyrSerial.cpp line 82
7. Change the line to uart_irq_update(uart);
8. Compile the project from the zephyrproject/zephyr directory:
`west build -p always -b arduino_uno_q <path-to-your-project>`
