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
	
    AD1CHS  = 0xFFFA;           // analog input channel based on I/O pin 6 (RB3)
    AD1CON1bits.SAMP = 1;      // start sampling, automatic conversion will follow
    while (!AD1CON1bits.DONE); // wait to complete the conversion
    return ADC1BUF0;           // read the conversion result
} 


void ADCInitialize() 
{
	AD1PCFGbits.PCFG5 = 0;  // Disable digital input on AN5 (RB3->IO6)
  	AD1CON1 = 0x20E4;     	// auto convert after end of sampling
	AD1CSSL = 0;          	// no input is scanned
    AD1CON3 = 0x0101;     	//set value used in lecture slides
    AD1CON2 = 0;          	// use MUXA, AVss and AVdd are used as Vref+/-
    AD1CON1bits.ADON = 1; 	// turn on the ADC
} 

///////////////////////////////////////////////////////needs work!!!!!!!!!!!!!!!!!!!!!!
int translate(int IOpin) {

 int tempmask = 0xFF7F; // defalult pin I/O pin 7

	if (IOpin==1)
		tempmask= 0xFFFE;

	if (IOpin==4) 
		tempmask= 0xFFEF;
	
 	if (IOpin==6) 
		tempmask= 0xFF7F;
	
	if (IOpin==12) 
		tempmask= 0xEFFF;
	
	if (IOpin==11) 
		tempmask= 0xF7FF;
		
	if (IOpin==10) 
		tempmask= 0xFBFF;
	
	if (IOpin==9) 
		tempmask= 0xFDFF;
		
return tempmask;
	
} 




