cmake_minimum_required(VERSION 3.20.0)

set(BOARD_VARIANT "arduino_uno_q")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})


#set(ARDUINO_CORE_DIR "~/.arduino15/packages/arduino/hardware/zephyr/0.90.0")
#set(ZEPHYR_EXTRA_MODULES ${ARDUINO_CORE_DIR} )

set(ZEPHYR_EXTRA_MODULES 
    "${ZEPHYR_BASE}/../modules/lib/arduinocore-zephyr"
)

project(myTestSketch)

#set(ARDUINO_CORE_DIR "~/.arduino15/packages/arduino/hardware/zephyr/0.90.0")
set(ARDUINO_CORE_DIR ${ZEPHYR_EXTRA_MODULES})
set(LIB_DIR "~/Arduino/libraries")

target_sources(app PRIVATE 
    src/main.cpp
    ${LIB_DIR}/RF24Ethernet/drivers/net/rf24_netif.c
    ${ARDUINO_CORE_DIR}/cores/arduino/Arduino.h
    ${LIB_DIR}/Arduino_RouterBridge/src/singletons.cpp
    ${LIB_DIR}/Arduino_RouterBridge/src/Arduino_RouterBridge.h
    ${LIB_DIR}/Arduino_RouterBridge/src/routerbridge_provides_serial.h
    ${LIB_DIR}/Arduino_RouterBridge/src/bridge.h
    ${LIB_DIR}/Arduino_RouterBridge/src/monitor.h
    ${LIB_DIR}/ArduinoGraphics/src/ArduinoGraphics.cpp
    ${LIB_DIR}/ArduinoGraphics/src/Image.cpp
    ${LIB_DIR}/RF24/RF24.cpp
    ${LIB_DIR}/RF24Network/RF24Network.cpp
    ${LIB_DIR}/RF24Mesh/RF24Mesh.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Ethernet.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Client.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Server.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Udp.cpp
    ${LIB_DIR}/RF24Ethernet/Dns.cpp
    ${LIB_DIR}/Arduino_RPClite/src/Arduino_RPClite.h
    ${ARDUINO_CORE_DIR}/cores/arduino/new.cpp
)

target_include_directories(app PRIVATE
    #drivers/net
    "${ARDUINO_CORE_DIR}/cores/arduino"
    "${ARDUINO_CORE_DIR}/cores/arduino/api"
    "${ARDUINO_CORE_DIR}/variants/arduino_uno_q_stm32u585xx"
    "${ARDUINO_CORE_DIR}/libraries/SPI"
    "${LIB_DIR}/Arduino_RPClite/src"
    "${LIB_DIR}/MsgPack"
    "${LIB_DIR}/DebugLog"
    "${LIB_DIR}/ArxTypeTraits"
    "${LIB_DIR}/ArxContainer"
    "${LIB_DIR}/RF24"
    "${LIB_DIR}/RF24Network"
    "${LIB_DIR}/RF24Mesh"
    "${LIB_DIR}/RF24Ethernet"
    "${ARDUINO_CORE_DIR}/libraries/Arduino_LED_Matrix/src"
    "${LIB_DIR}/ArduinoGraphics/src"
    "${LIB_DIR}/Arduino_RouterBridge/src"
    ${ZEPHYR_BASE}/../modules/lib/arduinocore-zephyr/loader/
)

target_compile_definitions(app PRIVATE 
    -DCONFIG_BOARD_ARDUINO_UNO_Q
    -DARDUINO_VARIANT="arduino_uno_q_stm32u585xx"
    -DUSE_LWIP=2
    -DRF24ETHERNET_USE_UDP=1
    -DARDUINO
 )
