/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * SPI Master library for arduino.
 *
 * This file is free software; you can redistribute it and/or modify
 * it under the terms of either the GNU General Public License version 2
 * or the GNU Lesser General Public License version 2.1, both as
 * published by the Free Software Foundation.
 */

#include "pins_arduino.h"
#include "SPI_UART.h"


void SPIUARTClass::begin() {

  // Set SS to high so a connected chip will be "deselected" by default
  digitalWrite(SS, HIGH);

  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).
  pinMode(SS, OUTPUT);

  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
  // automatically switches to Slave, so the data direction of
  // the SS pin MUST be kept as OUTPUT.
  //SPCR |= _BV(MSTR);
  //SPCR |= _BV(SPE);


  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // By doing this AFTER enabling SPI, we avoid accidentally
  // clocking in a single bit since the lines go directly
  // from "input" to SPI control.
  // http://code.google.com/p/arduino/issues/detail?id=888
  //pinMode(SCK, OUTPUT);
  //pinMode(MOSI, OUTPUT);
  #if defined (MEGA)
  	pinMode(18, OUTPUT); //Serial 2
  //pinMode(52, OUTPUT); //SCK
  //PORTD |= _BV(PORTD5); //xck1 as output
  	DDRD |= _BV(DDD5);
  #else
  	pinMode(1,OUTPUT);
  	pinMode(4,OUTPUT);
  #endif

    //Set USART to Master mode
  #if defined (MEGA)
    UCSR1C = _BV(UMSEL01) | _BV(UMSEL00);
    UCSR1B = _BV(RXEN0) | _BV(TXEN0);
    UCSR1A = 0;
  #else
    UCSR0C = _BV(UMSEL01) | _BV(UMSEL00);
    UCSR0B = _BV(RXEN0) | _BV(TXEN0);
    UCSR0A = 0;
#endif
}


void SPIUARTClass::end() {
  //SPCR &= ~_BV(SPE);
#if defined (MEGA)
  UCSR1C &= ~(_BV(UMSEL01) | _BV(UMSEL00));
#else
  UCSR0C &= ~(_BV(UMSEL01) | _BV(UMSEL00));
#endif

}

void SPIUARTClass::setBitOrder(uint8_t bitOrder)
{
//  if(bitOrder == LSBFIRST) {
//    SPCR |= _BV(DORD);
//  } else {
//    SPCR &= ~(_BV(DORD));
//  }
#if defined (MEGA)
  if(bitOrder == LSBFIRST) {
    UCSR1C |= _BV(2);  //UDORD
  } else {
    UCSR1C &= ~(_BV(2)); //_BV(UDORD1);
  }
#else
if(bitOrder == LSBFIRST) {
    UCSR0C |= _BV(2);  //UDORD
  } else {
    UCSR0C &= ~(_BV(2)); //_BV(UDORD1);
  }
#endif

}

void SPIUARTClass::setDataMode(uint8_t mode)
{
  //SPCR = (SPCR & ~SPI_MODE_MASK) | mode;

#if defined (MEGA)
  if(mode == 0){
	  UCSR1C &= ~(_BV(1) | _BV(UCPOL0));
  }else
  if(mode == 4){
	  UCSR1C &= ~(_BV(1));
	  UCSR1C |= (_BV(UCPOL0));
  }else
  if(mode == 8){
	  UCSR1C &= ~(_BV(UCPOL0));
	  UCSR1C |= (_BV(1));
  }else
  if(mode == 12){
	  UCSR1C |= (_BV(1) | _BV(UCPOL0));
  }
#else
  if(mode == 0){
	  UCSR0C &= ~(_BV(1) | _BV(UCPOL0));
  }else
  if(mode == 4){
	  UCSR0C &= ~(_BV(1));
	  UCSR0C |= (_BV(UCPOL0));
  }else
  if(mode == 8){
	  UCSR0C &= ~(_BV(UCPOL0));
	  UCSR0C |= (_BV(1));
  }else
  if(mode == 12){
	  UCSR0C |= (_BV(1) | _BV(UCPOL0));
  }
#endif
}

void SPIUARTClass::setClockDivider(uint8_t rate)
{
  //SPCR = (SPCR & ~SPI_CLOCK_MASK) | (rate & SPI_CLOCK_MASK);
  //SPSR = (SPSR & ~SPI_2XCLOCK_MASK) | ((rate >> 2) & SPI_2XCLOCK_MASK);
#if defined (MEGA)
  if(rate == 0){ //4Mhz - div4
  	//UBRR1 = 1;
  	UBRR1 = 1; 
  	UCSR1A &= ~_BV(U2X0);
  }else
  if(rate == 1){ //1Mhz - div16
  	UBRR1 = 7;
  	UCSR1A &= ~_BV(U2X0);
  }else
  if(rate == 2){  //250khz
	  UBRR1 = 31;
	  UCSR1A &= ~_BV(U2X0);
  }else
  if(rate == 4){ //8mhz
	  UBRR1 = 0;
	  UCSR1A &= ~_BV(U2X0);
  }else
  if(rate == 5){ //2mhz
	  UBRR1 = 3;
	  UCSR1A &= ~_BV(U2X0);
  }
#else
  if(rate == 0){ //4Mhz
  	//UBRR1 = 1;
  	UBRR0 = 1; 
  	UCSR0A &= ~_BV(U2X0);
  }else
  if(rate == 1){ //16Mhz
  	UBRR0 = 7;
  	UCSR0A &= ~_BV(U2X0);
  }else
  if(rate == 2){
	  UBRR0 = 31;
	  UCSR0A &= ~_BV(U2X0);
  }else
  if(rate == 4){ //8mhz
	  UBRR0 = 0;
	  UCSR0A &= ~_BV(U2X0);
  }else
  if(rate == 8){
	  UBRR0 = 3;
	  UCSR0A &= ~_BV(U2X0);
  }
#endif
}

