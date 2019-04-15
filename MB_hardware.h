/*
 * MB_hardware.h
 *
 *  Created on: 22 но€б. 2018 г.
 *      Author: ASUS
 */

#ifndef MB_HARDWARE_H_
#define MB_HARDWARE_H_

#include "stdint.h"     /* Only int8_t, uint8_t, int16_t, uint16_t are needed   */

/*
 * Interrupt vector of my MCU (MSP430), extracted from driverlib
 */

/*
 * Some sort of MACROs - to make functions' texts "hardware independent"
 */
#define TO_TRANSMT  do { P2OUT &= ~BIT0; P2OUT |= BIT1; }   while(0)
#define TO_RECEIVE  do { P2OUT |= BIT0; P2OUT &= ~BIT1; }   while(0)
#define STOP_TIMER  do { TA1CTL &= ~MC_1; }     while(0)
#define RESET_TIMER do { TA1CTL |= TACLR; \
                         TA1CTL |= MC_1; }      while(0)
#define TIMER_COUNT     TA1R
#define BYTE_RECEIVED   UCA0RXBUF
#define BYTE_TO_SEND    UCA0TXBUF

/*
 * If you don't intend to output MBState at MCU pins - use only "MBState = s"
 */
#define SET_ST(s)   do {                                        \
                                                     \
        MBState = s; }  while(0)



#endif /* MB_HARDWARE_H_ */
