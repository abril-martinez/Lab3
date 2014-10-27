// ******************************************************************************************* //
// ECE 372A Lab3
// Team 08
// Albert Martinez
// Greg Burleson
// Michael Reed
//********************************************************************************************//
#ifndef __ADCDRV_H__
#define __ADCDRV_H__ 


//pin 1 is ADC channel 4
//pin 2 is ADC channel 7
//pin 7 is ADC channel 12
//pin 8 is ADC channel 11
//pin 9 is ADC channel 10
//pin 10 is ADC channel 9


#define pin1  4			
#define pin2  7
#define pin7  12          
#define pin8  11		
#define pin9  10	
#define pin10 9			

int readADC();   //parameter I/O expansion pin number this function does conversion

void ADCInitialize();	//initialize ADC operation ofr this I/O pin

int translate(int IOpin);
#endif

