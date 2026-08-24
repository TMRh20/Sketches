cmake_minimum_required(VERSION 3.20.0)


set(ZEPHYR_EXTRA_MODULES 
    "/home/tmrh20/ArduinoProject/zephyrproject/modules/lib/ArduinoCore-zephyr"
)

set(BOARD_VARIANT "arduino_uno_q")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(myTestSketch)
set(ARDUINO_CORE_DIR "/home/tmrh20/.arduino15/packages/arduino/hardware/zephyr/0.90.0")



set(RF24_LIB_DIR "/home/tmrh20/Arduino/libraries/RF24")
set(ARDUINO_ZEPHYR_CORE "/home/tmrh20/.arduino15/packages/arduino/hardware/zephyr/0.90.0/")

target_sources(app PRIVATE 
    src/main.cpp
    ${RF24_LIB_DIR}/RF24.cpp
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/netif.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/tcp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/tcp_out.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/tcp_in.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/pbuf.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/mem.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/memp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ip.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/udp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/sys.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/altcp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/def.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/dns.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/raw.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/stats.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/timeouts.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/inet_chksum.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/icmp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/ip4.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/ip4_addr.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/acd.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/dhcp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/etharp.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/autoip.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/core/ipv4/ip4_frag.c
    /home/tmrh20/Arduino/libraries/lwIP/src/arch/sys_arch.c
    /home/tmrh20/Arduino/libraries/lwIP/src/arch/cc.h
    /home/tmrh20/Arduino/libraries/lwIP/src/arch/SerialWrapper.cpp
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/api/err.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/api/if_api.c
    /home/tmrh20/Arduino/libraries/lwIP/src/lwip/api/tcpip.c
    /home/tmrh20/Arduino/libraries/RF24Network/RF24Network.cpp
    /home/tmrh20/Arduino/libraries/RF24Mesh/RF24Mesh.cpp
    /home/tmrh20/Arduino/libraries/RF24Ethernet/RF24Ethernet.cpp
    /home/tmrh20/Arduino/libraries/RF24Ethernet/RF24Client.cpp
    /home/tmrh20/Arduino/libraries/RF24Ethernet/RF24Server.cpp
    /home/tmrh20/Arduino/libraries/RF24Ethernet/RF24Udp.cpp
    /home/tmrh20/Arduino/libraries/RF24Ethernet/Dns.cpp
)

target_include_directories(app PRIVATE
 ${RF24_LIB_DIR}
 ${ARDUINO_ZEPHYR_CORE}/cores/arduino
 )

target_include_directories(app PRIVATE
    "${ARDUINO_ZEPHYR_CORE}/cores/arduino"
    "${ARDUINO_ZEPHYR_CORE}/cores/arduino/api"
    "${ARDUINO_ZEPHYR_CORE}/variants/arduino_uno_q_stm32u585xx"
    "${ARDUINO_ZEPHYR_CORE}/libraries/SPI"
    "/home/tmrh20/Arduino/libraries/Arduino_RouterBridge/src"
    "/home/tmrh20/Arduino/libraries/Arduino_RPClite/src"
    "/home/tmrh20/Arduino/libraries/MsgPack"
    "/home/tmrh20/Arduino/libraries/DebugLog"
    "/home/tmrh20/Arduino/libraries/ArxTypeTraits"
    "/home/tmrh20/Arduino/libraries/ArxContainer"
    "/home/tmrh20/Arduino/libraries/RF24"
    "/home/tmrh20/Arduino/libraries/RF24Network"
    "/home/tmrh20/Arduino/libraries/RF24Mesh"
    "/home/tmrh20/Arduino/libraries/RF24Ethernet"
    "/home/tmrh20/Arduino/libraries/lwIP/src"
    "/home/tmrh20/.arduino15/packages/arduino/hardware/zephyr/0.90.0/libraries/Arduino_LED_Matrix/src"
)
zephyr_compile_definitions(
    ARDUINO
)

target_compile_definitions(app PRIVATE 
    -DCONFIG_BOARD_ARDUINO_UNO_Q
    -DARDUINO_VARIANT="arduino_uno_q_stm32u585xx"
    -DNUM_HW_SERIAL=1
    -DPIN_SERIAL1_RX=0
    -DPIN_SERIAL1_TX=1
    -DUSE_LWIP=1
    -DRF24ETHERNET_USE_UDP=1
 )
