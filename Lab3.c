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


#define PWM_FREQ                100
#define PR_VALUE                (57600/PWM_FREQ)-1 //for prescaler of 1:256


#define TRIS_MOTOR_F            TRISAbits.TRISA0
#define TRIS_MOTOR_B            TRISAbits.TRISA1
#define LAT_MOTOR_F             LATAbits.LATA0
#define LAT_MOTOR_B             LATAbits.LATA1  

volatile int ADC_value;
volatile double AD_value;
volatile char count;
volatile char done;


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

      char value[8];

   // ADC_value = readADC();        
   // sprintf(value, "%6d", ADC_value);
   // LCDMoveCursor(0,0);
   // LCDPrintString(value);

	AD_value = (ADC_value * 3.3)/1024;
    sprintf(value, "%6.2f",AD_value);
    LCDMoveCursor(0,0);
	LCDPrintString(value);
	}	

void PWMInitialize() {
    //Set up Timer 2 for PWM
    TMR1 = 0;
    PR1 = PR_VALUE; // (1/100) / (1/(Fcy)   * 256)
    T2CON = 0x0030; // prescale 256
   
    //Set up pin RP8 for PWM, using OC1 -- LEFT PWM
    RPOR4bits.RP8R =  18; //OC1 using RP8
    OC1R = 0; // PR1 * Duty cycle, which is initialized to 0%
    OC1RS = 0;
    OC1CON = 0x0006;

    //Set up pin RP9 for PWM, using OC2 -- RIGHT PWM
    RPOR4bits.RP9R =  19; //OC2 using RP9
    OC2R = 0; // PR2 * Duty cycle, which is initialized to 0%
    OC2RS = 0;
    OC2CON = 0x0006;

	T1CONbits.TON = 1; // turn timer1 on
}


int main(void)
{
	ADC_value = 0;
	AD_value = 0;
	char value[8];


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
    PWMInitialize();

 	// Set the directional pins
    TRIS_MOTOR_F = 0;  // RA0
    TRIS_MOTOR_B = 0;  // RA1
    LAT_MOTOR_F = 0;   // idle mode
    LAT_MOTOR_B = 0;   // idle mode

	// Motor related variables	
	double duty_cycle1;
    double duty_cycle2;

	// SW1 momentary switch as input
    TRISBbits.TRISB5 = 1;	

	while(1) 
      { 					
	  if(done)
		{
			if(AD_value >= 1.65) 
			{
				duty_cycle1 = 1; //motor 1 is on full from 1.65-3.3
				duty_cycle2 = (AD_value/(-1.65))+2; //motor 2 decreases
			}
			else
			{
				duty_cycle1 = (AD_value/1.65); //motor 1 decreases
				duty_cycle2 = 1; //motor 2 on full from 0-1.65
			}
			OC1RS = (int)(PR2*duty_cycle1); //modifying PWM duty cycle
			OC2RS = (int)(PR2*duty_cycle2); ////modifying PWM duty cycle
			sprintf(value, "%.2f", duty_cycle1);
			LCDMoveCursor(1,0);LCDPrintString(value);
			sprintf(value, "%.2f", duty_cycle2);
			LCDMoveCursor(1,4);LCDPrintString(value);
			
			done = 0;
		} 
	  }

	return 0;
	}



void __attribute__((interrupt,auto_psv)) _CNInterrupt(void)
{	
	IFS1bits.CNIF = 0;
	
    //switch from idle, forward, idle, backward, idle modes on button release
	if(PORTBbits.RB5 == 1)
	{
		if(count == 0)//move forwards
		{
			LATAbits.LATA0 = 0;	
			LATAbits.LATA1 = 1;
		}
		else if(count == 1) //idle1
		{
			LATAbits.LATA0 = 0;	
			LATAbits.LATA1 = 0;	
		}
		else if(count == 2) //move backwards
		{
			LATAbits.LATA0 = 1;	
			LATAbits.LATA1 = 0;	
		}
		else if(count == 3) //idle2
		{
			LATAbits.LATA0 = 0;	
			LATAbits.LATA1 = 0;
			count=-1;	//reset count variable
		}
		
		count++;
	}
	
}

// ******************************************************************************************* //
void  _ISR _ADC1Interrupt(void) 
{	
	IFS0bits.AD1IF = 0;
	AD_Conversion();	
	done = 1;
}
// ******************************************************************************************* //




