/*
 * mb_header.h
 *
 *  Created on: 22 но€б. 2018 г.
 *      Author: ASUS
 */

#ifndef MB_HEADER_H_
#define MB_HEADER_H_


#include <msp430.h>
#include "MB_hardware.h"    // Hardware depended defines

#define PARSE_IN_INT        /* Parse command inside TIMER1_A0_ISR_HOOK ISR. Quick response but 124 us in interrupt subroutine   */

#define MB_FRAME_MIN            4       /* Minimal size of a Modbus RTU frame   */
#define MB_FRAME_MAX            256     /* Maximal size of a Modbus RTU frame   */
#define MB_ADDRESS_BROADCAST    00      /* MBBuff[0] analysis                   */

#define MB_FUNC_NONE                            00
#define MB_FUNC_READ_COILS                      01
#define MB_FUNC_READ_DISCRETE_INPUTS            02
#define MB_FUNC_WRITE_SINGLE_COIL               05
#define MB_FUNC_WRITE_MULTIPLE_COILS            15
#define MB_FUNC_READ_HOLDING_REGISTER           03  /* implemented now  */
#define MB_FUNC_READ_INPUT_REGISTER             04  /* implemented now  */
#define MB_FUNC_WRITE_REGISTER                  06  /* implemented now  */
#define MB_FUNC_WRITE_MULTIPLE_REGISTERS        16  /* implemented now  */
#define MB_FUNC_READWRITE_MULTIPLE_REGISTERS    23
#define MB_FUNC_ERROR                           0x80

typedef enum {
    MBE_NONE                    = 0x00,
    MBE_ILLEGAL_FUNCTION        = 0x01,
    MBE_ILLEGAL_DATA_ADDRESS    = 0x02,
    MBE_ILLEGAL_DATA_VALUE      = 0x03
} eMBExcep;

typedef enum
{
    STATE_IDLE,         // Ready to get a frame from Master
    STATE_RCVE,         // Frame is being received
    STATE_PARS,         // Frame is being parsed (may take some time)
    STATE_SEND,         // Response frame is being sent
    STATE_SENT          // Last byte sent to shift register. Waiting "Last Bit Sent" interrupt
} eMBState;

// Actually only 1 variable uses this type: ER_frame_bad
typedef enum
{
    EV_NOEVENT,
    EV_HAPPEND
} eMBEvents;

// Boolean
typedef enum
{
    FALSE,
    TRUE
} eLogical;

// Main program passes interface data to Modbus stack.
typedef struct
{
    uint16_t *pin;           // Pointer to the begin of ParsIn array. Modbus writes data in the array
    uint16_t *pwk;           // Pointer to the begin of ParsWk array. Modbus takes data from the array
    uint8_t  regsnumb;       // Total number of registers (=pars), the size of ParsIn and ParsWk arrays
    uint8_t  br_indx;        // Index in tables of baudrate-depended values
    uint8_t  id;             // SlaveID and compare with MBBuff[0] in parsing
    uint8_t  parity;         // 0 - no parity; 1 - Odd; 2 - Even
    uint8_t  numOfBits;     // 7 or 8
    uint8_t  numOfStopBits; // 1 or 2
} mb_struct;


#endif /* MB_HEADER_H_ */
