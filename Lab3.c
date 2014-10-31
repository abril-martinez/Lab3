// ******************************************************************************************* //
// ECE 372A Lab3
// Team 08
// Albert Martinez
// Greg Burleson
// Michael Reed
//********************************************************************************************//
#include "p24fj64ga002.h"
#include <stdio.h>
#include "lcd.h"

#define PWM_FREQ                100
#define PR_VALUE                (57600/PWM_FREQ)-1 //for prescaler of 1:256

#define TRIS_MOTOR_D            TRISBbits.TRISB2 // back left wheel
#define TRIS_MOTOR_C            TRISBbits.TRISB3 // back right wheel
#define LAT_MOTOR_D             LATBbits.LATB2
#define LAT_MOTOR_C             LATBbits.LATB3  

volatile int ADC_value;
volatile int Forward_Backward_State;
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


void PWMInitialize() {
    
    /**********************Timer2****************************/
    TMR2 = 0;
    PR2 = PR_VALUE; // 575
    T2CON = 0x0030; // prescale 256
   
    //Set up pin RP8 for PWM, using OC1 -- LEFT PWM
    RPOR4bits.RP8R =  18; // OC1 using RP8
    OC1R = 0; // PR2 * Duty cycle (initialized to 0%)
    OC1RS = 0;
    OC1CON = 0x0006; // Timer2

    //set pin RP9 for PWM, using OC2 -- RIGHT PWM
    RPOR4bits.RP9R = 19; // OC2 using RP9
    OC2R = 0; // PR2 * Duty cycle, (initialized to 0%)
    OC2RS = 0;
    OC2CON = 0x0006;

	T2CONbits.TON = 1; // turn timer2 on	

    /**********************Timer3****************************/
	
    TMR3 = 0;
    PR3 = PR_VALUE; // 575
    T3CON = 0x0030; // prescale 256

    //Set up pin RP10 for PWM, using OC1 -- LEFT PWM
    RPOR5bits.RP10R =  18; //OC1 using RP8
    OC1R = 0; // PR3 * Duty cycle, which is initialized to 0%
    OC1RS = 0;
    OC1CON = 0x000E; //Timer3

    //Set up pin RP11 for PWM, using OC2 -- RIGHT PWM
    RPOR5bits.RP11R = 19; //OC2 using RP10 (RB10 -> I09)
    OC2R = 0; // PR3 * Duty cycle, which is initialized to 0%
    OC2RS = 0;
    OC2CON = 0x000E; // Timer3

	T3CONbits.TON = 1; // turn timer1 on
}

void ADCInitialize(){
	AD1PCFG &= 0xFFFE;      // AN0 input pin is analog
	AD1CON2 = 0;          	// use MUXA, AVss and AVdd are used as Vref+/-
	AD1CON3 = 0x0101;     	// set TAD to 1, 2*TCY	
  	AD1CON1 = 0x20E4;     	// auto convert after end of sampling
	AD1CHS = 0;        		// Cofnigure input channels (AN5 ->I/O pin 6 ->RB3)  
	AD1CSSL = 0;          	// no input is scanned

	// Configure A/D interrupt
	IFS0bits.AD1IF = 0; 	// Clear A/D interrupt flag
	IEC0bits.AD1IE = 1; 	// Enable A/D conversion interrupt
	AD1CON1bits.ADON = 1; 	// Turn on A/D	
	done = 0;				// Flag to tell us if we have read new ADC values
}

int main(void)
{
	char value[8];
	Forward_Backward_State;
	
	// Set the directional pins
    TRIS_MOTOR_C = 0;  // RB2
    TRIS_MOTOR_D = 0;  // RB3
    LAT_MOTOR_C = 1;   // idle mode
    LAT_MOTOR_D = 1;   // idle mode

	// Motor related variables	
	int duty_cycle1 = 0;
    int duty_cycle2 = 0;

    // Configure Change notification of RB5 (SW1 momentary switch)
	TRISBbits.TRISB5 = 1;
	CNPU2bits.CN27PUE=1;	
    CNEN2bits.CN27IE=1;
    IFS1bits.CNIF=0;
    IEC1bits.CNIE=1;

	// Initialize LCD
    LCDInitialize();

	// Configure PWM
    PWMInitialize();

	// Configure A/D unit
    ADCInitialize(); 	

	while(1) 
      {		
	  if(done)
		{
		 sprintf(value, "%6d", ADC_value);
    	 LCDMoveCursor(0,0);
         LCDPrintString(value);
		

		 if((ADC_value > 462 || ADC_value == 462) && (ADC_value <562 || ADC_value == 562)) 
		  {
		   duty_cycle1 = 100;
		   duty_cycle2 = 100;
		  }
			
		 else if((ADC_value > 563 || ADC_value == 563) && (ADC_value <690 || ADC_value == 690)) 
		  {
		   duty_cycle1 = 75;
		   duty_cycle2 = 100;
		  }

		 else if((ADC_value > 691 || ADC_value == 691) && (ADC_value <818 || ADC_value == 818)) 	
		 {
 		   duty_cycle1 = 50;
		   duty_cycle2 = 100;
		 }

		else if((ADC_value > 819 || ADC_value == 819) && (ADC_value <975 || ADC_value == 975)) 	
		 {
 		   duty_cycle1 = 25;
		   duty_cycle2 = 100;
		 }

		else if (ADC_value > 975) 	
		 {
 		   duty_cycle1 = 0;
		   duty_cycle2 = 100;
		 }	

	 	else if((ADC_value < 461 || ADC_value == 461) && (ADC_value >334 || ADC_value == 334)) 
		  {
		   duty_cycle1 = 100;
		   duty_cycle2 = 75;
		  }
			
		else if((ADC_value < 333 || ADC_value == 333) && (ADC_value >206 || ADC_value == 206)) 
		  {
		   duty_cycle1 = 100;
		   duty_cycle2 = 50;
		  }

		else if((ADC_value < 205 || ADC_value == 205) && (ADC_value >40 || ADC_value == 40)) 
		  {
		   duty_cycle1 = 100;
		   duty_cycle2 = 25;
		  }
		
	   else if (ADC_value < 40) 
		  {
		   duty_cycle1 = 100;
		   duty_cycle2 = 0;
		  }

		
			OC1RS = (int)(PR_VALUE*duty_cycle1); //modifying PWM duty cycle
			OC1RS = OC1RS / 100;
			OC2RS = (int)(PR_VALUE*duty_cycle2); //modifying PWM duty cycle
			OC2RS = OC2RS / 100;
			
			sprintf(value, "%3d", duty_cycle1);
			LCDMoveCursor(1,0);
			LCDPrintString(value);
			sprintf(value, "%3d", duty_cycle2);
			LCDMoveCursor(1,5);
			LCDPrintString(value);  
			
			done = 0;
		} 
	  }
	return 0;
	}

void __attribute__((interrupt,auto_psv)) _CNInterrupt(void)
{	
	IFS1bits.CNIF = 0;
	
   // transition to next Forward_Backward_State when SW1 is pressed
    if(PORTBbits.RB5 == 0)
      {
       Forward_Backward_State++;
         if(Forward_Backward_State  > 4)
           {
            Forward_Backward_State = 1;
           }
      } 

	switch(Forward_Backward_State){
        
		//idle1: both motors stopped
        case 1:
            LAT_MOTOR_D = 1;
            LAT_MOTOR_C = 1;
            break;
        
		//forward: both motors moves in a forward direction
        case 2:
            LAT_MOTOR_D = 0;
            LAT_MOTOR_C = 1;
            break;
        
		//idle2: both motors stopped
        case 3:
            LAT_MOTOR_D = 1;
            LAT_MOTOR_C = 1;
            break;
        
		//backward:both motors move in a backward direction
        case 4:
            LAT_MOTOR_D = 1;
            LAT_MOTOR_C = 0;
            break;

    }
}	

// ******************************************************************************************* //
void  __attribute__((interrupt,auto_psv)) _ADC1Interrupt(void)
{
 IFS0bits.AD1IF = 0;
 ADC_value = ADC1BUF0;
 done = 1;
}


