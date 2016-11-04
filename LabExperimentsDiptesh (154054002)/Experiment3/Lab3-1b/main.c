#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/debug.h"
#include "driverlib/pwm.h"
#include "driverlib/pin_map.h"
#include "inc/hw_gpio.h"
#include "driverlib/rom.h"

#define PWM_FREQUENCY 55

#define LONG_COUNT_TIMEOUT 200

uint8_t mode = 0;

int red = 240;
int green = 0;
int blue = 0;

uint32_t angle = 0;
uint32_t incAngle = 1;

void setup(){
	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);

	ROM_GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
	ROM_GPIOPinConfigure(GPIO_PF1_M1PWM5);
	ROM_GPIOPinConfigure(GPIO_PF2_M1PWM6);
	ROM_GPIOPinConfigure(GPIO_PF3_M1PWM7);

	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

}
void changeColor();
unsigned int detectKeyPressSW1();
unsigned int detectKeyPressSW2();
int getSW1Count(int flag);
void processMode0(unsigned int,unsigned int);
void processMode1(unsigned int,unsigned int);
void processMode2(unsigned int,unsigned int);
void processMode3(unsigned int,unsigned int);

void Timer0IntHandler(void) {

	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state

	/*bool longPressx = false;
	unsigned int keyPress = detectKeyPressSW1();
	 */

	bool isModeSet = false;
	// detect key presses
	unsigned int SW1 = detectKeyPressSW1();
	unsigned int SW2 = detectKeyPressSW2();
	if (SW2 == 0x10)
	{
		if(SW1 == 0x10)
		{
			mode = 3;
			isModeSet = true;
			getSW1Count(0);
		}
		else if(SW1 == 0x01)
		{
			int SW1Count = getSW1Count(1);
			if(SW1Count == 1)
			{
				mode = 1;
				isModeSet = true;
			}
			else if(SW1Count == 2)
			{
				mode =2;
				isModeSet = true;
				getSW1Count(0);
			}
		}
	}
	else
	{
		getSW1Count(0);
	}
	if(!isModeSet)
	{
		switch (mode){
		case 0:
			processMode0(SW1,SW2);
			break;
		case 1:
			processMode1(SW1,SW2);
			break;
		case 2:
			processMode2(SW1,SW2);
			break;
		case 3:
			processMode3(SW1,SW2);
			break;
		}
	}

}

volatile uint32_t ui32Load;

int main(void)
{

	volatile uint32_t ui32PWMClock;
	uint32_t ui32Period;

	setup(); // call for setup clock, GPIO, button and peripherel enabling.

	ui32PWMClock = SysCtlClockGet() / 64;
	ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;
	PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_DOWN);
	PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_DOWN);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
	PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red * ui32Load / 240);		// RED
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, green * ui32Load / 240);		// GREEN
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, blue * ui32Load / 240);		// BLUE

	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT | PWM_OUT_6_BIT | PWM_OUT_7_BIT, true);
	PWMGenEnable(PWM1_BASE, PWM_GEN_2);
	PWMGenEnable(PWM1_BASE, PWM_GEN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_PERIODIC);

	ui32Period = (SysCtlClockGet() / 100) ;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period - 1);		// Change here

	IntEnable(INT_TIMER0A);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	IntMasterEnable();
	TimerEnable(TIMER0_BASE, TIMER_A);

	while(1)
	{
		if(mode == 0)
		{
			angle = angle + incAngle;
			changeColor();
		}

		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red * ui32Load / 240);		// RED
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, green * ui32Load / 240);		// GREEN
		PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, blue * ui32Load / 240);		// BLUE

		SysCtlDelay(1000000);
	}
}

void changeColor()
{
	if(angle > 360)
	{
		angle = 0;
	}

	if((int) angle > 0 && (int) angle <= 120)
	{
		red = 240 - 2 * angle;
		blue = 0;
		green = 2 * angle;
	}
	else if(angle > 120 && angle < 240)
	{
		red = 0;
		green = 480 - 2 * angle;
		blue = 2 * angle - 240;
	}

	else
	{
		red = 2 * angle - 480;
		green = 0;
		blue = 720 - 2 * angle;
	}
}

unsigned int detectKeyPressSW2()
{
	static unsigned int longCountSW2 = 0;
	static uint8_t state2 = 0;
	unsigned int retFlag = false;

	uint8_t value = GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0);
	switch(state2)
	{
	case 0:
		if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)&0x01)!=0x01)
		{
			state2 = 1;
			retFlag = 0x00;
			longCountSW2 = 0;
		}
		else
		{
			state2 = 0;
			retFlag = 0x00;
			longCountSW2 = 0;
		}
		break;
	case 1:
		if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)&0x01)!=0x01)
		{
			state2 = 2;
			retFlag = 0x01;
			longCountSW2 = 0;

		}
		else
		{
			state2 = 0;
			retFlag = 0x00;
			longCountSW2 = 0;
		}
		break;
	case 2:
		if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)&0x01)!=0x01)
		{
			state2 = 2;
			retFlag = 0x00;
			longCountSW2++;
			if(longCountSW2 > LONG_COUNT_TIMEOUT)
			{
				retFlag = 0x10;
			}
		}
		else
		{
			state2 = 0;
			retFlag = 0x00;
			longCountSW2 = 0;
		}
		break;
	}
	return retFlag;
}

unsigned int detectKeyPressSW1()
{
	static uint8_t state1 = 0;
	static unsigned int longCountSW1 = 0;
	unsigned int retFlag = 0x00;

	switch(state1)
	{
	case 0:
		if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)&0x10)!=0x10)
		{
			state1 = 1;
			retFlag = 0x00;
			longCountSW1 = 0;
		}
		else
		{
			state1 = 0;
			retFlag = 0x00;
			longCountSW1 = 0;
		}
		break;
	case 1:
		if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)&0x10)!=0x10)
		{
			state1 = 2;
			retFlag = 0x01;
			longCountSW1 = 0;
		}
		else
		{
			state1 = 0;
			retFlag = 0x00;
			longCountSW1 = 0;
		}
		break;
	case 2:
		if((GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)&0x10)!=0x10)
		{
			state1 = 2;
			retFlag = 0x00;
			longCountSW1++;
			if(longCountSW1 > LONG_COUNT_TIMEOUT)
			{
				retFlag = 0x10;
			}
		}
		else
		{
			state1 = 0;
			retFlag = 0x00;
			longCountSW1 = 0;
		}
		break;
	}
	return retFlag;
}

int getSW1Count(int flag )
{
	static int count =0;
	if (flag == 0)
	{
		count = 0;
	}
	else if(flag == 1)
	{
		count++;
	}
	return count;
}

void processMode0(unsigned int SW1,unsigned int SW2)
{
	if(SW1 == 0x01)
	{
		incAngle++;
	}
	else if(SW2 == 0x01)
	{
		incAngle--;
	}

	if(incAngle < 5)
	{
		incAngle =5 ;
	}
	else if(incAngle > 25)
	{
		incAngle = 25;
	}
}

void processMode1(unsigned int SW1,unsigned int SW2)
{

	//blue = 0;
	//green = 0;
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, red * ui32Load / 240);		// RED
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1, 2);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_3, 0);
	blue=0;
	green=0;

	if(SW1 == 0x01)
	{
		red += 10;
	}
	else if(SW2 == 0x01)
	{
		red -= 10;
	}

	if(red < 0)
	{
		red =0 ;
	}
	else if(red > 240)
	{
		red = 240;
	}
}

void processMode2(unsigned int SW1,unsigned int SW2)
{
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2, 4);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_3, 0);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, blue * ui32Load / 240);		// BLUE
	red=0;
	green=0;

	if(SW1 == 0x01)
	{
		blue += 10;
		//red = 0;
		//green = 0;
	}
	else if(SW2 == 0x01)
	{
		blue -= 10;
		//red = 0;
		//green = 0;
	}

	if(blue < 0)
	{
		blue =0 ;
	}
	else if(blue > 240)
	{
		blue = 240;
	}
}

void processMode3(unsigned int SW1,unsigned int SW2)
{
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, green * ui32Load / 240);		// GREEN
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_3, 8);
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1, 0);
	red=0;
	blue=0;

	if(SW1 == 0x01)
	{
		green += 10;
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1, 0);

	}
	else if(SW2 == 0x01)
	{
		green -= 10;
		GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_2|GPIO_PIN_1, 0);

	}

	if(green < 0)
	{
		green =0 ;
	}
	else if(green > 240)
	{
		green = 240;
	}
}
