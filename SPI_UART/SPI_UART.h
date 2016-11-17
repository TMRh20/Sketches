/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#ifndef _SPI_UART_H_INCLUDED
#define _SPI_UART_H_INCLUDED

#include <stdio.h>
#include <Arduino.h>
#include <avr/pgmspace.h>

#define SPI_CLOCK_DIV4 0x00
#define SPI_CLOCK_DIV16 0x01
#define SPI_CLOCK_DIV64 0x02
#define SPI_CLOCK_DIV128 0x03
#define SPI_CLOCK_DIV2 0x04
#define SPI_CLOCK_DIV8 0x05
#define SPI_CLOCK_DIV32 0x06
//#define SPI_CLOCK_DIV64 0x07

#define SPI_MODE0 0x00
#define SPI_MODE1 0x04
#define SPI_MODE2 0x08
#define SPI_MODE3 0x0C

#define SPI_MODE_MASK 0x0C  // CPOL = bit 3, CPHA = bit 2 on SPCR
#define SPI_CLOCK_MASK 0x03  // SPR1 = bit 1, SPR0 = bit 0 on SPCR
#define SPI_2XCLOCK_MASK 0x01  // SPI2X = bit 0 on SPSR

//#if !defined (PNTRS)
#if defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__)
	#define MEGA
#endif


class SPIUARTClass {
public:
  inline static byte transfer(byte _data);
  inline static void transfer(void *_buf, size_t _count);
  inline static byte fTransfer(byte _data);
  // SPI Configuration methods

  inline static void attachInterrupt();
  inline static void detachInterrupt(); // Default

  static void begin(); // Default
  static void end();

  static void setBitOrder(uint8_t);
  static void setDataMode(uint8_t);
  static void setClockDivider(uint8_t);
};

void SPIUARTClass::transfer(void *_buf, size_t _count){


  uint8_t *d = (uint8_t*)_buf;
  
  if(_count == 1){
      *d = transfer(*d);
  }
  #if defined (__arm__)

    USART0->US_RPR = USART0->US_TPR = (uint32_t)_buf;
    USART0->US_TCR = USART0->US_RCR = _count;
    while( USART0->US_RNCR > 0 || USART0->US_RCR > 0 ){;} 

  #else
    while(_count--){
      *d = transfer(*d);
      d++;
    }
  #endif
}

byte SPIUARTClass::fTransfer(byte _data) {

#if defined (__arm__)
  while( !(USART0->US_CSR & US_CSR_TXRDY) ){;} 
  USART0->US_THR = _data;
  while(  !(USART0->US_CSR & US_CSR_RXRDY) ){;} 
  return USART0->US_RHR;
#elif defined (MEGA)
  while ( !(UCSR1A & _BV(UDRE0))){ }
  UDR1 = _data;
  while( !(UCSR1A & _BV(RXC1) )){}
  return UDR1;
#else
    while ( !(UCSR0A & _BV(UDRE0))){ }
    UDR0 = _data;
    while( !(UCSR0A & _BV(RXC0) )){}
  return UDR0;
#endif


}

byte SPIUARTClass::transfer(byte _data) {

#if defined (__arm__)
  while( !(USART0->US_CSR & US_CSR_TXRDY) ){;} 
  USART0->US_THR = _data;
  while(  !(USART0->US_CSR & US_CSR_RXRDY) ){;} 
  return USART0->US_RHR;
#elif defined (MEGA)
  while ( !(UCSR1A & _BV(UDRE0))){ }
  UDR1 = _data;
  //while ( !(*UCSRNA & _BV(UDRE0))){ }                      //Wait until the USART is ready for more data
  while( !(UCSR1A & _BV(RXC0) )){}
  return UDR1;
#else
  while ( !(UCSR0A & _BV(UDRE0))){ }
  UDR0 = _data;
  //while ( !(*UCSRNA & _BV(UDRE0))){ }                      //Wait until the USART is ready for more data
  while( !(UCSR0A & _BV(RXC0) )){}
  return UDR0;
#endif

}

void SPIUARTClass::attachInterrupt() {
#if defined (__arm__)
  
#elif defined (MEGA)
  UCSR1A |= _BV(UDRE0);
#else
  UCSR0A |= _BV(UDRE0);
#endif

}

void SPIUARTClass::detachInterrupt() {
#if defined (__arm__)

#elif defined (MEGA)
  UCSR1A &= ~_BV(UDRE0);
#else
  UCSR0A &= ~_BV(UDRE0);
#endif

}

#endif