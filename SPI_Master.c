
#include <p24FJ128GA010.h>
#include "LCD.h"

// RB2 as SS for SPI slave MCU
#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2

#define TFLY 500 //500 times 16 us = 8 ms
#define DELAY() TMR1=0; while(TMR1<TFLY)

unsigned short spiBufR[]={0,0,0,0};	// SPI buffer for Receiving

void SPI1Init(void)// SPI Master
{
    //config SPI1
    SPI1STATbits.SPIEN 		= 0;	// disable SPI port
    SPI1STATbits.SPISIDL 	= 0; 	// Continue module operation in Idle mode
    SPI1BUF 				= 0;   	// clear SPI buffer data Master (SPI1BUF==SSPBUF as in the powerpoint)
    IFS0bits.SPI1IF 		= 0;	// clear interrupt flag
    IEC0bits.SPI1IE 		= 0;	// disable interrupt
    SPI1CON1bits.DISSCK		= 0;	// Internal SPIx clock is enabled
    SPI1CON1bits.DISSDO		= 0;	// SDOx pin is controlled by the module (enable SDO)
    SPI1CON1bits.MODE16 	= 1;	// set in 16-bit mode, clear in 8-bit mode
    SPI1CON1bits.SMP		= 0;	// Input data sampled at middle of data output time
    SPI1CON1bits.CKP 		= 1;	// CKP and CKE is subject to change ...
    SPI1CON1bits.CKE 		= 0;	// ... based on your communication mode.
	SPI1CON1bits.MSTEN 		= 1; 	// 1 =  Master mode; 0 =  Slave mode
	SPI1CON1bits.SPRE 		= 4; 	// Secondary Prescaler = 4:1
	SPI1CON1bits.PPRE 		= 2; 	// Primary Prescaler = 4:1
    SPI1CON2 				= 0;	// non-framed mode

	SPI_SS_PORT				= 1;	//
	SPI_SS_TRIS				= 0;	// set SS as output
    SPI1STATbits.SPIEN 		= 1; 	// enable SPI port, clear status
}

unsigned short writeSPI1( unsigned short data )
{
    SPI1BUF = data;					// write to buffer for TX
    while(!SPI1STATbits.SPIRBF);	// wait for transfer to complete
    return SPI1BUF;    				// read the received value
}//writeSPI1



void initADC(void)
{
AD1PCFG = 0xFFDF; // AN5(Frob Knob) as analog, all other pins are digital
AD1CON1 = 0x0040; // SSRC bit = 010 implies GP TMR3- FORM1:FORM0=00 integer
AD1CSSL = 0x0020; // set channel-5 for sequence scanning
AD1CON3 = 0x0000; // Sample time is TMR3, Tad = Tcy/2
AD1CON2 = 0x0404; // enable scan inputs for MUX A Input Multiplexer
TMR3 = 0x0000;	  // clear the timer
PR3 = 6250;       // set TMR3 to time out every 100 ms - on the falling edge of the clock a new data will be written to the ADC1BUF0 buffer
T3CON = 0x8030;   // enabled, prescaler 1:256, internal clock
AD1CON1bits.ADON = 1; // turn ADC ON
AD1CON1bits.ASAM = 1; // start auto sampling
}

int ADCValue1;


int main (void)
{

	TRISAbits.TRISA6 = 0;

	SPI1Init();
	initADC();
	initLCD();
	unsigned char a[4];


    while (1) {


        ADCValue1=ADC1BUF0;
        IFS0bits.AD1IF = 0; // clear AD1IF

	    sprintf( a, "%04d",ADCValue1);// padding by 0
		//if the reading is  3 then the a =0003
		//if the reading is 93 then the a =0093
		//if the reading is 135 then the a =0135

		putsLCD(a);

		SPI_SS_PORT	= 0; // enable the chip select ,SS(RB2)=0
		spiBufR[0]	= writeSPI1(a[0]);	 // write	0    MSD
		spiBufR[1]	= writeSPI1(a[1]);   // write	1
		spiBufR[2]	= writeSPI1(a[2]);  // write	3
		spiBufR[3]	= writeSPI1(a[3]);  // write	5   LSD
		SPI_SS_PORT = 1;

	   PORTA=PORTA^0x40;  //Flash LED6
      DELAY();
	  clrLCD();
	}
	return 0;
}
