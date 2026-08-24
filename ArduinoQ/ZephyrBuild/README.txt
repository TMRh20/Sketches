Compile from the zephyrproject directory with the command:
west build -p always -b arduino_uno_q -d ~/ZephyrBuild/build ~/ZephyrBuild/

Copy the zephyr.elf file from the build/zephyr/ directory to the Arduino Q then run this from the same directory as the elf file
/opt/openocd/bin/openocd   -s /opt/openocd   -f openocd_gpiod.cfg   -c "program zephyr.elf verify reset exit"
