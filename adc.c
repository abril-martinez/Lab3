/*******************************************************************************************/
// ECE 372A Lab3
// Team 08
// Albert Martinez
// Greg Burleson
// Michael Reed
/*********************************************************************************************/


#include "p24fj64ga002.h"
#include "adc.h"

int readADC()
{
    AD1CON1bits.SAMP = 1;      // start sampling, automatic conversion will follow
    while (!AD1CON1bits.DONE); // wait to complete the conversion
    return ADC1BUF0;           // read the conversion result
} 


void ADCInitialize() 
{
	AD1PCFG &= 0xFFDF;       // Disable digital input on AN5 (RB3->IO6)
	AD1CON2 = 0;          	// use MUXA, AVss and AVdd are used as Vref+/-
	AD1CON3 = 0x0101;     	// set TAD to 1, 2*TCY	
  	AD1CON1 = 0x20E4;     	// auto convert after end of sampling
	AD1CHS = 0x0005;        // Cofnigure input channels (AN5 ->I/O pin 6 ->RB3)  
	AD1CSSL = 0;          	// no input is scanned
} 





