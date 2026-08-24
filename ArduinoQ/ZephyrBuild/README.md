These files allow users to build custom files for the Uno Q MCU using Zephyr
The main.cpp file can be modified to any example from the RF24Ethernet, RF24Mesh, RF24Network or RF24 libraries, just make sure to include myCompat.h

See google for instructions on installing west and the zephyrproject files

Compile from the zephyrproject directory with the command:
`west build -p always -b arduino_uno_q -d ~/ZephyrBuild/build ~/ZephyrBuild/`

Copy the zephyr.elf file from the build/zephyr/ directory to the Arduino Q then run this from the same directory as the elf file
`/opt/openocd/bin/openocd   -s /opt/openocd   -f openocd_gpiod.cfg   -c "program zephyr.elf verify reset exit"`

Note: Serial output is confined to the TX/RX pins 0 & 1 on the board.

Needed changes to core files:

Edit the file `~/.arduino15/packages/arduino/hardware/zephyr/0.90.0/cores/arduino/inlines.h`

```cpp
static inline __attribute__((always_inline)) void delay(unsigned long ms) {
	k_sleep(K_MSEC(ms));
}

static inline __attribute__((always_inline)) void delayMicroseconds(unsigned int us) {
	if (us == 0) {
		return;
	}
	k_busy_wait(us - 1);
}
```

Edit the file `~/.arduino15/packages/arduino/hardware/zephyr/0.90.0/cores/arduino/api/Common.h`

```cpp
static inline void delay(unsigned long);
static inline void delayMicroseconds(unsigned int us);
```

