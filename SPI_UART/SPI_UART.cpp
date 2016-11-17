/*
 * Copyright (c) 2010 by Cristian Maglie <c.maglie@bug.st>
 * Copyright (c) 2016 TMRh20 <tmrh20@gmail.com>
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

  // When the SS pin is set as OUTPUT, it can be used as
  // a general purpose output port (it doesn't influence
  // SPI operations).
  pinMode(SS, OUTPUT);

  // Set SS to high so a connected chip will be "deselected" by default
  digitalWrite(SS, HIGH);
  
  // Warning: if the SS pin ever becomes a LOW INPUT then SPI
  // automatically switches to Slave, so the data direction of
  // the SS pin MUST be kept as OUTPUT.

  // Set direction register for SCK and MOSI pin.
  // MISO pin automatically overrides to INPUT.
  // By doing this AFTER enabling SPI, we avoid accidentally
  // clocking in a single bit since the lines go directly
  // from "input" to SPI control.
  // http://code.google.com/p/arduino/issues/detail?id=888

  #if defined (__arm__)
    //Arduino Due Pins: 18: MOSI, 19: MISO, SDA1: SCK, CS: User Selected, CE: User Selected
    PIOA->PIO_ABSR |= (1u << 17);   // SCK: Assign A16 I/O to the Peripheral B function
    PIOA->PIO_PDR |= (1u << 17);    // SCK: Disable PIO control, enable peripheral control
    PIOA->PIO_ABSR |= (0u << 10);   // MOSI: Assign PA13 I/O to the Peripheral A function
    PIOA->PIO_PDR |= (1u << 10);    // MOSI: Disable PIO control, enable peripheral control
    PIOA->PIO_ABSR |= (0u << 11);   // MISO: Assign A12 I/O to the Peripheral A function
    PIOA->PIO_PDR |= (1u << 11);    // MISO: Disable PIO control, enable peripheral control
  #elif defined (MEGA)
  	pinMode(18, OUTPUT); //Serial 2
  	DDRD |= _BV(DDD5);
  #else
  	pinMode(1,OUTPUT);
  	pinMode(4,OUTPUT);
  #endif

    //Set USART to Master mode
  #if defined(__arm__)
    pmc_enable_periph_clk(ID_USART0);
    USART0->US_MR = 0x409CE;
    USART0->US_BRGR = 9;
    USART0->US_CR = US_CR_RSTRX | US_CR_RSTTX;
    USART0->US_CR = US_CR_RXEN;
    USART0->US_CR = US_CR_TXEN;
    USART0->US_PTCR = US_PTCR_RXTEN;
    USART0->US_PTCR = US_PTCR_TXTEN;
  #elif defined (MEGA)
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

#if defined (__arm__)
  USART0->US_CR = US_CR_RXDIS;
  USART0->US_CR = US_CR_TXDIS;
  PIOA->PIO_PDR &= ~(1u << 11);
  PIOA->PIO_PDR &= ~(1u << 10);
  PIOA->PIO_PDR &= ~(1u << 17);
#elif defined (MEGA)
  UCSR1C &= ~(_BV(UMSEL01) | _BV(UMSEL00));
#else
  UCSR0C &= ~(_BV(UMSEL01) | _BV(UMSEL00));
#endif

}

void SPIUARTClass::setBitOrder(uint8_t bitOrder)
{

#if defined (__arm__)
  
#elif defined (MEGA)
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

#if defined (__arm__)
  if(mode == 0){
    USART0->US_MR |= 1 << 8;
  }else
  if(mode == 4){
    USART0->US_MR &= ~(1 << 8);  
  }
  
#elif defined (MEGA)
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

#if defined (__arm__)
  USART0->US_BRGR = rate;
#elif defined (MEGA)
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
