cmake_minimum_required(VERSION 3.20.0)


set(ZEPHYR_EXTRA_MODULES 
    "/home/tmrh20/ArduinoProject/zephyrproject/modules/lib/ArduinoCore-zephyr"
)

set(BOARD_VARIANT "arduino_uno_q")

find_package(Zephyr REQUIRED HINTS $ENV{ZEPHYR_BASE})

project(myTestSketch)

set(ARDUINO_CORE_DIR "~/.arduino15/packages/arduino/hardware/zephyr/0.90.0")
set(LIB_DIR "~/Arduino/libraries")

target_sources(app PRIVATE 
    src/main.cpp
    ${LIB_DIR}/RF24/RF24.cpp
    ${LIB_DIR}/lwIP/src/lwip/core/netif.c
    ${LIB_DIR}/lwIP/src/lwip/core/tcp.c
    ${LIB_DIR}/lwIP/src/lwip/core/tcp_out.c
    ${LIB_DIR}/lwIP/src/lwip/core/tcp_in.c
    ${LIB_DIR}/lwIP/src/lwip/core/pbuf.c
    ${LIB_DIR}/lwIP/src/lwip/core/mem.c
    ${LIB_DIR}/lwIP/src/lwip/core/memp.c
    ${LIB_DIR}/lwIP/src/lwip/core/ip.c
    ${LIB_DIR}/lwIP/src/lwip/core/udp.c
    ${LIB_DIR}/lwIP/src/lwip/core/sys.c
    ${LIB_DIR}/lwIP/src/lwip/core/altcp.c
    ${LIB_DIR}/lwIP/src/lwip/core/def.c
    ${LIB_DIR}/lwIP/src/lwip/core/dns.c
    ${LIB_DIR}/lwIP/src/lwip/core/raw.c
    ${LIB_DIR}/lwIP/src/lwip/core/stats.c
    ${LIB_DIR}/lwIP/src/lwip/core/timeouts.c
    ${LIB_DIR}/lwIP/src/lwip/core/inet_chksum.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/icmp.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/ip4.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/ip4_addr.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/acd.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/dhcp.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/etharp.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/autoip.c
    ${LIB_DIR}/lwIP/src/lwip/core/ipv4/ip4_frag.c
    ${LIB_DIR}/lwIP/src/arch/sys_arch.c
    ${LIB_DIR}/lwIP/src/arch/cc.h
    ${LIB_DIR}/lwIP/src/arch/SerialWrapper.cpp
    ${LIB_DIR}/lwIP/src/lwip/api/err.c
    ${LIB_DIR}/lwIP/src/lwip/api/if_api.c
    ${LIB_DIR}/lwIP/src/lwip/api/tcpip.c
    ${LIB_DIR}/RF24Network/RF24Network.cpp
    ${LIB_DIR}/RF24Mesh/RF24Mesh.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Ethernet.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Client.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Server.cpp
    ${LIB_DIR}/RF24Ethernet/RF24Udp.cpp
    ${LIB_DIR}/RF24Ethernet/Dns.cpp
    
)

target_include_directories(app PRIVATE
 ${RF24_LIB_DIR}
 ${ARDUINO_CORE_DIR}/cores/arduino
 )

target_include_directories(app PRIVATE
    "${LIB_DIR}/Arduino_RouterBridge/src"
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
    "${LIB_DIR}/lwIP/src"
    "${ARDUINO_CORE_DIR}/libraries/Arduino_LED_Matrix/src"
    "${LIB_DIR}/ArduinoGraphics/src"
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
