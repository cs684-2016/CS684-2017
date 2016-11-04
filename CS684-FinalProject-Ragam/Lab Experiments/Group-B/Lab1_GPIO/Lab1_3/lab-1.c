/*

* Author: Texas Instruments 

* Editted by: Saurav Shandilya, Vishwanathan Iyer 
	      ERTS Lab, CSE Department, IIT Bombay

* Description: This code will test file to check software and hardware setup. This code will blink three colors of RGB LED present of Launchpad board in sequence.

* Filename: lab-0.c

* Functions: setup(), led_pin_config(), main()

* Global Variables: none

*/

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_types.h"
#include "inc/hw_memmap.h"
#include "driverlib/sysctl.h"
#include "driverlib/gpio.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

uint8_t count_sw1	= 0;
/*
 *

* Function Name: setup()

* Input: none

* Output: none

* Description: Set crystal frequency and enable GPIO Peripherals  

* Example Call: setup();

*/
void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

/*

* Function Name: led_pin_config()

* Input: none

* Output: none

* Description: Set PORTF Pin 1, Pin 2, Pin 3 as output. On this pin Red, Blue and Green LEDs are connected.

* Example Call: led_pin_config();

*/

void led_pin_config(void)
{
	GPIOPinTypeGPIOOutput(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);
}

void switchPinConfig(void)
{
	// Following two line removes the lock from SW2 interfaced on PORTF Pin0 -- leave this unchanged
	LOCK_F=0x4C4F434BU;
	CR_F=GPIO_PIN_0|GPIO_PIN_4;

	// GPIO PORTF Pin 0 and Pin4
	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_DIR_MODE_IN); // Set Pin-4 of PORT F as Input. Modifiy this to use another switch
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_4);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

	GPIODirModeSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_DIR_MODE_IN); // Set Pin-0 of PORT F as Input. Modifiy this to use another switch 2
	GPIOPinTypeGPIOInput(GPIO_PORTF_BASE, GPIO_PIN_0);
	GPIOPadConfigSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_STRENGTH_12MA,GPIO_PIN_TYPE_STD_WPU);

}

int main(void)
{
	uint8_t ui8LED 		= 2;
	//uint8_t count_sw1	= 0;
	uint8_t delay 		= 1;
	uint8_t count_sw2  	= 0;
	//uin
	bool flag_sw1;
	bool flagprev_sw1 = true;

	bool flag_sw2;
	bool flagprev_sw2 = true;


	setup();
	led_pin_config();
	switchPinConfig();

	while(1)
	{
		// Turn on the LED
		//GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		// Delay for a bit
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
		SysCtlDelay(6700000 * delay); // 500 msec delay
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, 0);
		SysCtlDelay(6700000 * delay); // 500 msec delay
		// Cycle through Red, Green and Blue LEDs
		flag_sw1 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
		if(flag_sw1)
		{
			if(flagprev_sw1){
				count_sw1 = count_sw1+1;
				flagprev_sw1 = false;
			}
		}
		else
		{
			switch (count_sw1){
				case 1:
					ui8LED = 8; //red
					break;
				case 2:
					ui8LED = 4; //gree
					break;
				case 3:
					ui8LED = 2; //blue
					break;
			}
			flagprev_sw1 = true;;
		}

		if (count_sw1 == 4)
			count_sw1 = 1;


		flag_sw2 = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0);
				if(flag_sw2)
				{
					if(flagprev_sw2){
						count_sw2 = count_sw2+1;
						flagprev_sw2 = false;
					}
				}
				else
				{
					switch (count_sw2){
						case 1:
							delay = 1; //red
							break;
						case 2:
							delay = 2; //gree
							break;
						case 3:
							delay = 4; //blue
							break;
					}
					flagprev_sw2 = true;;
				}

				if (count_sw2 == 4)
					count_sw2 = 1;

	}
}
