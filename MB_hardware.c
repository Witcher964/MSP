/*
 * MB_hardware.c
 *
 *  Created on: 22 нояб. 2018 г.
 *      Author: ASUS
 */




#include "MB_header.h"

#define EUSCI_A_UART_CLOCKSOURCE_SMCLK                  UCSSEL__SMCLK       /* These defines are fetched from MSP430 driverlib headers  */
#define EUSCI_A_UART_DEGLITCH_TIME_200ns                (UCGLIT0 + UCGLIT1)
#define EUSCI_A_UART_LSB_FIRST                          0x00
#define EUSCI_A_UART_MODE                               UCMODE_0
#define EUSCI_A_UART_ONE_STOP_BIT                       0x00
#define EUSCI_A_UART_OVERSAMPLING_BAUDRATE_GENERATION   0x01
#define EUSCI_A_UART_RECEIVE_INTERRUPT                  UCA0RXIE
#define EUSCI_A_UART_TRANSMIT_INTERRUPT                 UCA0TXIE
#define EUSCI_A_UART_RECEIVE_ERRONEOUSCHAR_INTERRUPT    UCRXEIE
#define EUSCI_A_UART_BREAKCHAR_INTERRUPT                UCBRKIE
#define EUSCI_A_UART_STARTBIT_INTERRUPT                 UCSTTIE
#define EUSCI_A_UART_TRANSMIT_COMPLETE_INTERRUPT        UCTXCPTIE
#define TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE              CCIE
#define TIMER_A_CLOCKSOURCE_DIVIDER_1                   0x01
#define TIMER_A_CLOCKSOURCE_SMCLK                       TASSEL_2
#define TIMER_A_UP_MODE                                 MC_1

#define FSMCLK_HZ   (1000000)               /* Look FQUARZ_HZ in main application */

const
uint16_t NBR[] = {  FSMCLK_HZ/1200/16,
                    FSMCLK_HZ/2400/16,
                    FSMCLK_HZ/4800/16,
                    FSMCLK_HZ/9600/16,
                    FSMCLK_HZ/19200/16,
                    FSMCLK_HZ/38400/16,
                    FSMCLK_HZ/57600/16,
                    FSMCLK_HZ/115200/16 };
const
uint16_t Nt35[] = { FSMCLK_HZ*38.5/1200-1,      // t3.5 is 11 bits * 3.5 = 38.5 clocks
                    FSMCLK_HZ*38.5/2400-1,
                    FSMCLK_HZ*38.5/4800-1,
                    FSMCLK_HZ*38.5/9600-1,
                    FSMCLK_HZ*38.5/19200-1,
                    1750,
                    1750,
                    1750 };
const
uint16_t Nt15[] = { FSMCLK_HZ*16.5/1200-1,      // t1.5 is 11 bits * 1.5
                    FSMCLK_HZ*16.5/2400-1,
                    FSMCLK_HZ*16.5/4800-1,
                    FSMCLK_HZ*16.5/9600-1,
                    FSMCLK_HZ*16.5/19200-1,
                    750,
                    750,
                    750 };


uint16_t MBHWinit( uint8_t baudrate_code, uint8_t parity, uint8_t numOfBytes, uint8_t numOfStopBytes)
{

    /*
     * Timer А1
     * Used to catch events t1.5 and t3.5
     * Source: SMCLK, divided by 1
     * UP mode, interrupt at CCR0 compare
     */

    TA1CTL = ((TIMER_A_CLOCKSOURCE_DIVIDER_1 - 1) << 6);
    //TA1EX0 = TAIDEX_0;
    TA1CTL |= TIMER_A_CLOCKSOURCE_SMCLK + ID_0;
    TA1CCTL0 |= TIMER_A_CCIE_CCR0_INTERRUPT_ENABLE; // CCIE bit = 1
    TA1CTL |= TIMER_A_UP_MODE;
    TA1CCR0  = 0;                                   // Look datasheet, 11.2.2 Starting the Timer: it's STOP

    /*
     * eUSCI as UART for MODBUS
     * Clock: SMCLK, divided by 1. It equals to 1000000 Hz in my project
     * All depended constants are precompiled values from tables
     * The recommended eUSCI_A initialization/reconfiguration process is:
     * 1. Set UCSWRST (BIS.B #UCSWRST,&UCAxCTL1).
     * 2. Initialize all eUSCI_A registers with UCSWRST = 1 (including UCAxCTL1).
     * 3. Configure ports.
     * 4. Clear UCSWRST via software (BIC.B #UCSWRST,&UCAxCTL1).
     * Enable interrupts (optional) via UCRXIE or UCTXIE.
     */

     DCOCTL  = 0;             // Select lowest DCOx and MODx settings
     BCSCTL1 = CALBC1_1MHZ;   // Set range
     DCOCTL  = CALDCO_1MHZ;   // Set DCO step + modulation


     //------------ Configuring MAX485 Control Lines ---------------//

     P2DIR  |= BIT0 + BIT1; // P2.0 -> DE,P2.1 -> ~RE output
     P2OUT   = 0x00;        // P2.0 and P2.1 = 0

     //--------- Setting the UART function for P1.1 & P1.2 ---------//

     P1SEL  |=  BIT1 + BIT2;              // P1.1 UCA0RXD input
     P1SEL2 |=  BIT1 + BIT2;              // P1.2 UCA0TXD output

     //------------ Configuring the UART(USCI_A0) ----------------//

     UCA0CTL1 |=  UCSSEL_2 + UCSWRST;  // USCI Clock = SMCLK,USCI_A0 disabled

     // 0(or other) - disable parity; 1 or 2 - enable parity
     if(parity == 1 || parity == 2)
         UCA0CTL0 |= UCPEN;
     else
         UCA0CTL0 &= ~UCPEN;

     // 2 - odd; 1(or other) - even
     if(parity == 2)
         UCA0CTL0 &= ~UCPAR;
     else
         UCA0CTL0 |= UCPAR;

     // 0 - 7 bit; 1(or other) - 8 bit
     if(numOfBytes == 0)
         UCA0CTL0 |= UC7BIT;
     else
         UCA0CTL0 &= ~UC7BIT;

     // 0(or other) - 1 stop bit; 1 - 2 stop bit
     if(numOfStopBytes == 1)
         UCA0CTL0 |= UCSPB;
     else
         UCA0CTL0 &= ~UCSPB;

     switch (baudrate_code)
     {
     //1200
     case 0:
         UCA0BR0 = 0x41;
         UCA0BR1 = 0x3;
         UCA0MCTL = 0x92;
         break;
     //2400
     case 1:
         UCA0BR0 = 0xa0;
         UCA0BR1 = 0x1;
         UCA0MCTL = 0x6d;
         break;
     //4800
     case 2:
         UCA0BR0 = 0xd0;
         UCA0BR1 = 0x00;
         UCA0MCTL = 0x92;
         break;
     //9600
     case 3:
         UCA0BR0 = 0x68;
         UCA0BR1 = 0x00;
         UCA0MCTL = 0x04;
         break;
     //19200
     case 4:
         UCA0BR0 = 0x34;
         UCA0BR1 = 0x00;
         UCA0MCTL = 0x20;
         break;
     //38400
     case 5:
         UCA0BR0 = 0x1a;
         UCA0BR1 = 0x00;
         UCA0MCTL = 0x00;
         break;
     //57600
     case 6:
         UCA0BR0 = 0x11;
         UCA0BR1 = 0x00;
         UCA0MCTL = 0x52;
         break;
     //115200
     case 7:
         UCA0BR0 = 0x8;
         UCA0BR1 = 0x00;
         UCA0MCTL = 0x6d;
         break;
     }

     UCA0CTL1 &= ~UCSWRST;             // Clear UCSWRST to enable USCI_A0

     P1DIR &= ~BIT3;
     P1REN |= BIT3;
     P1DIR &= ~BIT4;
     P1REN |= BIT4;
     P1DIR &= ~BIT5;
     P1REN |= BIT5;
     P1DIR &= ~BIT6;
     P1REN |= BIT6;

     // Enable the USCI after configuration

    UC0IE = EUSCI_A_UART_TRANSMIT_INTERRUPT |
             EUSCI_A_UART_RECEIVE_INTERRUPT;

    TA1CCR0 = Nt35[baudrate_code];          // Start Timer t3.5

    return  Nt15[baudrate_code];            // Timer value @ t1.5 after it's reset
}
