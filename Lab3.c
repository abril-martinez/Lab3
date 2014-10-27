// ******************************************************************************************* //
// ECE 372A Lab2
// Team 08
// Albert Martinez
// Greg Burleson
// Michael Reed
//********************************************************************************************//
#include "p24fj64ga002.h"
#include <stdio.h>
#include "lcd.h"
#include "adc.h"
//#include "delay.h"

//define variables
#define PWM_FREQ                100
#define PR_VALUE                (57600/PWM_FREQ)-1 //for prescaler of 1:256


#define TRIS_MOTOR_D            TRISBbits.TRISB1
#define TRIS_MOTOR_C            TRISBbits.TRISB2
#define LAT_MOTOR_D             LATBbits.LATB1
#define LAT_MOTOR_C             LATBbits.LATB2


// ******************************************************************************************* //
// Configuration bits for CONFIG1 settings. 
//
// Make sure "Configuration Bits set in code." option is checked in MPLAB.
//
// These settings are appropriate for debugging the PIC microcontroller. If you need to 
// program the PIC for standalone operation, change the COE_ON option to COE_OFF.

_CONFIG1( JTAGEN_OFF & GCP_OFF & GWRP_OFF & 
		 BKBUG_ON & COE_ON & ICS_PGx1 & 
		 FWDTEN_OFF & WINDIS_OFF & FWPSA_PR128 & WDTPS_PS32768 )

// ********************************************************************************************/
// Configuration bits for CONFIG2 settings.
// Make sure "Configuration Bits set in code." option is checked in MPLAB.

_CONFIG2( IESO_OFF & SOSCSEL_SOSC & WUTSEL_LEG & FNOSC_PRIPLL & FCKSM_CSDCMD & OSCIOFNC_OFF &
		 IOL1WAY_OFF & I2C1SEL_PRI & POSCMOD_XT )


/************************************A/D Conversion********************************************/

void AD_Conversion() {
	int ADC_value;
    char value[8];
	double AD_value;
	
    ADC_value = readADC();         //select input channel 
    sprintf(value, "%6d", ADC_value);
    LCDMoveCursor(0,0);
    LCDPrintString(value);

	AD_value = (ADC_value * 3.3)/1024;
    sprintf(value, "%6.2f",AD_value);
    LCDMoveCursor(1,0);
	LCDPrintString(value);
	}	


int main(void)
{
 
 	// Configure Timer
    PR1 = 5760/20;
    TMR1 = 0;
    IFS0bits.T1IF = 0;
    IEC0bits.T1IE = 1;
    T1CONbits.TCKPS = 3;
    T1CONbits.TON = 1;

	 // Configure Change notification of RB5
    CNEN2bits.CN27IE=1;
    CNPU2bits.CN27PUE=1;
    IFS1bits.CNIF=0;
    IEC1bits.CNIE=1;

	// Initialize LCD
    LCDInitialize();

    // Configure A/D unit
    ADCInitialize();    

    // Configure PWM
   // PWMInitialize();

 	// Set the directional pins
    TRIS_MOTOR_D = 0;
    TRIS_MOTOR_C = 0;
    LAT_MOTOR_D = 1;
    LAT_MOTOR_C = 1;

 	// Set momentary switch to RB5
	TRISBbits.TRISB5 = 1;

	// Motor related variables	
//	int leftDutyCycle;
//    int rightDutyCycle;

	while(1) { 						
	AD_Conversion();				
	//	delay(); //soft delay for 100 msec or so.
	}

	return 0;
	}








