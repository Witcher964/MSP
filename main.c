#include <msp430g2553.h>


/**
 * main.c
 */

#include "stdint.h"

typedef enum
{
    FALSE,
    TRUE
} eLogical;

typedef struct
{
    uint16_t *pin;
    uint16_t *pwk;
    uint8_t  regsnumb;
    uint8_t  br_indx;
    uint8_t  id;
    uint8_t parity;
    uint8_t  numOfBits;
    uint8_t  numOfStopBits;
} mb_struct;

#define LASTINDEX   25

mb_struct   mbs;
uint16_t    ParsWk[LASTINDEX];
uint16_t    ParsIn[LASTINDEX];

extern uint8_t MBserve(void);
extern void MBinit( mb_struct *mbp);

uint8_t flag1,flag2,flag3,flag4;
uint16_t count1, count2, count3, count4;

void changeCounter (uint16_t* counter1_0, uint16_t* counter1_1, uint16_t* counter2_0, uint16_t* counter2_1, uint16_t* coef, uint16_t* count)
{
    if(*counter1_0 == 65535)
    {
         if(*counter1_1 == 65535)
             *counter1_1 = 0;
         else
             *counter1_1 ++;

         *counter1_0 = 0;
    }
    else
    {
        *counter1_0++;
    }

    if(*count % *coef == 0)
    {
        if(*counter2_0 == 65535)
        {
            if(*counter2_1 == 65535)
                *counter2_1 = 0;
            else
                *counter2_1 ++;

            *counter2_0 = 0;
        }
        else
        {
            *counter2_0++;
        }

        *count = 0;
    }
}

int main(void)
{
	WDTCTL = WDTPW | WDTHOLD;	// stop watchdog timer

	// start settings
	mbs.regsnumb = LASTINDEX;
	mbs.pin = ParsIn;
	mbs.pwk = ParsWk;
	mbs.br_indx  = 6;
	mbs.id  = 1;
	mbs.parity = 0;
	mbs.numOfBits = 1;
	mbs.numOfStopBits = 0;

	flag1=0;
	flag2=0;
	flag3=0;
	flag4=0;

	count1 = 0;
	count2 = 0;
	count3 = 0;
	count4 = 0;

	// init
	MBinit( &mbs);

	// counters init
    uint8_t i;
    for(i = 0; i < 16; i++)
    {
            ParsIn[i] = 0;
            ParsWk[i] = ParsIn[i];
    }

    // coefficient inint
    for(i = 16; i < 20; i++)
    {
            ParsIn[i] = 1;
            ParsWk[i] = ParsIn[i];
    }

    // baud rate init
    ParsIn[20] = mbs.br_indx;
    ParsWk[20] = ParsIn[20];

    // data bits init
    ParsIn[21] = mbs.numOfBits;
    ParsWk[21] = ParsIn[21];

    // stop bits init
    ParsIn[22] = mbs.numOfStopBits;
    ParsWk[22] = ParsIn[22];

    // parity init
    ParsIn[23] = mbs.parity;
    ParsWk[23] = ParsIn[23];

    // id init
    ParsIn[24] = mbs.id;
    ParsWk[24] = ParsIn[24];

    //interrupt enable
	__bis_SR_register(GIE);

	// main loop
	    do {

	         //MBserve();

	         // 1-st counter
             if ((P1IN & BIT3) == BIT3)
                 flag1 = 1;

             if (((P1IN & BIT3) != BIT3) && flag1)
             {
                 flag1 = 0;
                 count1 ++;

                 changeCounter(&ParsWk[0], &ParsWk[1], &ParsWk[8], &ParsWk[9], &ParsWk[16], &count1);

                 ParsIn[0] = ParsWk[0];
                 ParsIn[1] = ParsWk[1];
                 ParsIn[8] = ParsWk[8];
                 ParsIn[9] = ParsWk[9];
             }

             //2-nd counter
             if ((P1IN & BIT4) == BIT4)
                 flag2 = 1;

             if (((P1IN & BIT4) != BIT4) && flag2)
             {
                 flag2 = 0;
                 count2 ++;

                 changeCounter(&ParsWk[2], &ParsWk[3], &ParsWk[10], &ParsWk[11], &ParsWk[17], &count2);

                 ParsIn[2] = ParsWk[2];
                 ParsIn[3] = ParsWk[3];
                 ParsIn[10] = ParsWk[10];
                 ParsIn[11] = ParsWk[11];
             }

             //3-d counter
             if ((P1IN & BIT5) == BIT5)
                 flag3 = 1;

             if (((P1IN & BIT5) != BIT5) && flag3)
             {
                 flag3 = 0;
                 count3++;

                 changeCounter(&ParsWk[4], &ParsWk[5], &ParsWk[12], &ParsWk[13], &ParsWk[18], &count3);

                 ParsIn[4] = ParsWk[4];
                 ParsIn[5] = ParsWk[5];
                 ParsIn[12] = ParsWk[12];
                 ParsIn[13] = ParsWk[13];

             }

             //4-th counter
             if ((P1IN & BIT6) == BIT6)
                 flag4 = 1;

             if (((P1IN & BIT6) != BIT6) && flag4)
             {
                 flag4 = 0;
                 count4 ++;

                 changeCounter(&ParsWk[6], &ParsWk[7], &ParsWk[14], &ParsWk[15], &ParsWk[19], &count4);

                 ParsIn[6] = ParsWk[6];
                 ParsIn[7] = ParsWk[7];
                 ParsIn[14] = ParsWk[14];
                 ParsIn[15] = ParsWk[15];
             }

             // force change of counters values and coefficients
             uint8_t i;
             for(i = 0; i < 20; i++)
                 if(ParsIn[i] != ParsWk[i])
                     ParsWk[i] = ParsIn[i];

             // change baud rate
             if(ParsIn[20] != ParsWk[20])
             {
                 if(0 <= ParsIn[20] && ParsIn[20] < 8 )
                 {
                     ParsWk[20] = ParsIn[20];
                     mbs.br_indx = ParsWk[20];
                     MBinit( &mbs);
                 }
                 else
                     ParsIn[20] = ParsWk[20];
             }

             // change data bits count
             if(ParsIn[21] != ParsWk[21])
             {
                 if(0 <= ParsIn[21] && ParsIn[21] < 2 )
                 {
                     ParsWk[21] = ParsIn[21];
                     mbs.numOfBits = ParsWk[21];
                     MBinit( &mbs);
                 }
                 else
                     ParsIn[21] = ParsWk[21];
             }

             // change count of stop bits
             if(ParsIn[22] != ParsWk[22])
             {
                 if(0 <= ParsIn[22] && ParsIn[22] < 2 )
                 {
                     ParsWk[22] = ParsIn[22];
                     mbs.numOfStopBits = ParsWk[22];
                     MBinit( &mbs);
                 }
                 else
                     ParsIn[22] = ParsWk[22];
             }

             // change parity
             if(ParsIn[23] != ParsWk[23])
             {
                 if(0 <= ParsIn[23] && ParsIn[23] < 3 )
                 {
                     ParsWk[23] = ParsIn[23];
                     mbs.parity = ParsWk[23];
                     MBinit( &mbs);
                 }
                 else
                     ParsIn[23] = ParsWk[23];
             }

             // change id
             if(ParsIn[24] != ParsWk[24])
             {
                 if (0 < ParsIn[24] && ParsIn[24] < 255)
                 {
                     ParsWk[24] = ParsIn[24];
                     mbs.id = ParsWk[24];
                 }
                 else
                     ParsIn[24] = ParsWk[24];

             }

	    } while( 1);
	
	return 0;
}
