// Slave Configuration
#include <p24FJ128GA010.h>
#include "LCD.h"
#include <stdio.h>

// RB2 as SS for SPI slave MCU
#define SPI_SS_TRIS      TRISBbits.TRISB2
#define SPI_SS_PORT      PORTBbits.RB2

unsigned short spiBufT;		// SPI buffer for transmission
unsigned short My_Slave_Array[4];	// SPI buffer for Receiving

unsigned char spiCount=0;		// variable for how many data has been received from SPI bus
void DELAY_2(int x)
{
	TMR1=0;
    while(TMR1<(5*(x+27))){}
}

void SPI1Init(void)
{
    //config SPI1
    SPI1STATbits.SPIEN 		= 0;	// disable SPI port
    SPI1STATbits.SPISIDL 	= 0; 	// Continue module operation in Idle mode

    SPI1BUF 				= 0;   	// clear SPI buffer

    IFS0bits.SPI1IF 		= 0;	// clear interrupt flag
    IEC0bits.SPI1IE 		= 1;	// enable interrupt

    SPI1CON1bits.DISSDO		= 0;	// SDOx pin is controlled by the module
    SPI1CON1bits.MODE16 	= 1;	// set in 16-bit mode, clear in 8-bit mode
    SPI1CON1bits.SMP		= 0;	// SMP must be cleared when SPIx is used in Slave mode
    SPI1CON1bits.CKP 		= 1;	// CKP and CKE is subject to change ...
    SPI1CON1bits.CKE 		= 0;	// ... based on your communication mode.
    SPI1CON1bits.SSEN		= 1;	// SSx pin is used for Slave mode
	SPI1CON1bits.MSTEN 		= 0; 	// 1 =  Master mode; 0 =  Slave mode

    SPI1CON2 				= 0;	// non-framed mode

    SPI_SS_PORT				= 1;	//
	SPI_SS_TRIS				= 1;	// set SS as input

    SPI1STATbits.SPIEN 		= 1; 	// enable SPI port, clear status
}

int main(void)
{
	// Disable Watch Dog Timer
	RCONbits.SWDTEN = 0;

	TRISA = 0x0000; 	//bit 7 input on PORTA- S5, Data direction register -input=1
    T1CON =0x8030; // Timer 1 is ON, Prescalar 256, Tclk/2

    int Rvalue;
    char line[16];
	SPI1Init();
	initLCD();
    while(1){
	 //homeLCD();
  	if (spiCount>=3) {
			__builtin_btg((unsigned int *)&LATB, 12);
             homeLCD();

		snprintf(line, sizeof(line), "%c %c %c %c", spiBufR[0], spiBufR[1],spiBufR[2], spiBufR[3]);


        putsLCD(line);
        Rvalue=(line[0]-'0')*1000+(line[2]-'0')*100+(line[4]-'0')*10+(line[6]-'0');
       setLCDC(0x40);
        putdLCD(Rvalue);


		PORTA=0x00FF;
   	    DELAY_2(Rvalue);
		PORTA=0x0000;
   	    DELAY_2(Rvalue);
        clrLCD() ;
       spiCount = 0;

		}

	}

 }


void __attribute__((interrupt, no_auto_psv))  _SPI1Interrupt(void) // this is the reserved inerrupt handler of the SPI
{
	IFS0bits.SPI1IF = 0;  // clear this bit. This bit will be set when a new reading is recieved

	if (!SPI1STATbits.SPIROV)// no overflow, you may more than one byte,it will take last byte
	{
		if (spiCount==0) {
			My_Slave_Array[0]	= SPI1BUF;		// first byte is received from the master and stored in the slave(SPI1BUF) in the stored in the spiBufR
			spiCount++;
			spiBufT = My_Slave_Array[0] + 1;		// loop with data + 1
		}
		else
		{
			My_Slave_Array[spiCount] = SPI1BUF;
			My_Slave_Array = My_Slave_Array[spiCount++] + 1;	// loop with data + 1
		}

		while (SPI1STATbits.SPITBF);
		SPI1BUF = spiBufT;				// send back
	}
}
