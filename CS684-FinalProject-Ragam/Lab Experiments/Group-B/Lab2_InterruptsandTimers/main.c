#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

enum swstate{ idle, press, release};

uint8_t sw1_count	= 0;
uint32_t ui32Period;
uint8_t sw2_count  	= 0;
uint32_t sw2_count_total  	= 0;
uint8_t ui8LED 		= 2;
enum swstate state_sw1	= idle;
enum swstate state_sw2	= idle;

void setup(void)
{
	SysCtlClockSet(SYSCTL_SYSDIV_4|SYSCTL_USE_PLL|SYSCTL_XTAL_16MHZ|SYSCTL_OSC_MAIN);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
}

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

void timer0Config(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);
	ui32Period = (SysCtlClockGet()) / 20;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);
}

void switch1Functionality(void){
	if (state_sw1 == press){
				if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) //Switch 1
				{
					state_sw1 = release;
					sw1_count = 2;
					if (ui8LED == 2)
					{
						ui8LED = 8;
					}
					else
					{
						ui8LED = ui8LED/2;
					}
				}
				else{
					state_sw1	= idle;
					sw1_count 	= 0;
				}
			}

		if(sw1_count == 0){
			if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) //Switch 1
			{
				state_sw1 = press;
				sw1_count = 1;
			}
			else{
				state_sw1 = idle;
				sw1_count = 0;
			}
		}
		if (state_sw1 == release){
			if((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4))) //Switch 1
			{
				state_sw1 = idle;
				sw1_count = 0;
			}
			else{
				state_sw1 = release;
			}
		}
}

void switch2Functionality(void){
	if (state_sw2 == press){
				if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) //Switch 1
				{
					state_sw2 = release;
					sw2_count = 2;
					sw2_count_total = sw2_count_total+1;
				}
				else{
					state_sw2 		= idle;
					sw2_count 	= 0;
				}
			}

		if(sw2_count == 0){
			if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) //Switch 1
			{
				state_sw2 = press;
				sw2_count = 1;
			}
			else{
				state_sw2 = idle;
				sw2_count = 0;
			}
		}
		if (state_sw2 == release){
			if((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))) //Switch 1
			{
				state_sw2 = idle;
				sw2_count = 0;
			}
			else{
				state_sw2 = release;
			}
		}
}


int main(void)
{
	setup();
	led_pin_config();
	switchPinConfig();
	timer0Config();
	while(1)
	{
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
	}
}

void Timer0IntHandler(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	//bool flag = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	switch1Functionality();
	switch2Functionality();
	}
