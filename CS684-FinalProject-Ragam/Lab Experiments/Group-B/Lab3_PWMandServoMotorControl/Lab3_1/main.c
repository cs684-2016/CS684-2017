#include <stdint.h>
#include <stdbool.h>
#include "inc/tm4c123gh6pm.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
#include "driverlib/gpio.h"
#include "driverlib/timer.h"
#include "driverlib/pwm.h"
#include "inc/hw_gpio.h"
#include "driverlib/pin_map.h"

#define PWM_FREQUENCY 55

// LOCK_F and CR_F - used for unlocking PORTF pin 0
#define LOCK_F (*((volatile unsigned long *)0x40025520))
#define CR_F   (*((volatile unsigned long *)0x40025524))

enum swstate{ idle, press, release};
enum mode{auto_mode,manual_mode};
enum manual_mode{manual_mode1,manual_mode2,manual_mode3};

// Switch
uint8_t sw1_count	= 0;
uint32_t ui32Period;
uint32_t ui32PeriodHold;
uint8_t sw2_count  	= 0;
uint32_t sw1_count_total  	= 0;
uint32_t ui8LED 		= 2;
uint32_t delay_time = 1000000;
bool flag_timerrun = true;
enum swstate state_sw1	= idle;
enum swstate state_sw2	= idle;
enum mode mode_status   = auto_mode;
enum manual_mode manual_mode_status=manual_mode1;

// PWM
volatile uint32_t ui32Load;
volatile uint32_t ui32PWMClock;
volatile uint8_t ui8Adjust = 100;

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

void PWMConfig(void){
	SysCtlPWMClockSet(SYSCTL_PWMDIV_64);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_PWM1);

	//Configure PF1,PF2,PF3 Pins as PWM
	GPIOPinConfigure(GPIO_PF1_M1PWM5);
	GPIOPinConfigure(GPIO_PF2_M1PWM6);
	GPIOPinConfigure(GPIO_PF3_M1PWM7);
	GPIOPinTypePWM(GPIO_PORTF_BASE, GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);


	//Configure PWM Options
    //PWM_GEN_2 Covers M1PWM4 and M1PWM5
    //PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 207 4/11/13 DriverLib doc
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    ui32PWMClock = SysCtlClockGet() / 64;
    ui32Load = (ui32PWMClock / PWM_FREQUENCY) - 1;

    //Configure PWM Options
    //PWM_GEN_2 Covers M1PWM4 and M1PWM5
    //PWM_GEN_3 Covers M1PWM6 and M1PWM7 See page 207 4/11/13 DriverLib doc
    PWMGenConfigure(PWM1_BASE, PWM_GEN_2, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);
    PWMGenConfigure(PWM1_BASE, PWM_GEN_3, PWM_GEN_MODE_UP_DOWN | PWM_GEN_MODE_NO_SYNC);

    //Set the Period (expressed in clock ticks)
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_2, ui32Load);
    PWMGenPeriodSet(PWM1_BASE, PWM_GEN_3, ui32Load);

    //Set PWM duty-(ui8Adjust * 100 / 1000)% (ui8Adjust * ui32Load / 1000)
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5,ui8Adjust * ui32Load / 1000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6,ui8Adjust * ui32Load / 1000);
    PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7,ui8Adjust * ui32Load / 1000);

    // Turn on the Output pins
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , false);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , true);
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , false);

    // Enable the PWM generator
    PWMGenEnable(PWM1_BASE, PWM_GEN_2);
    PWMGenEnable(PWM1_BASE, PWM_GEN_3);

}

void timer0Config(void){
	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	TimerConfigure(TIMER0_BASE, TIMER_CFG_A_PERIODIC | TIMER_CFG_B_PERIODIC);
	ui32Period = (SysCtlClockGet()) / 20;
	TimerLoadSet(TIMER0_BASE, TIMER_A, ui32Period -1);
	IntEnable(INT_TIMER0A);

	ui32PeriodHold = (SysCtlClockGet()) / 20;
	TimerLoadSet(TIMER0_BASE, TIMER_B, ui32PeriodHold -1);
	IntEnable(INT_TIMER0B);
	TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT|TIMER_TIMB_TIMEOUT);

	IntMasterEnable();
	//TimerEnable(TIMER0_BASE, TIMER_A);
	TimerEnable(TIMER0_BASE, TIMER_BOTH);

	//toggle = 0x00000000;
	//TimerEnable(TIMER0_BASE, TIMER_B);
}

void Sw1autoModeFunctionality(void){
	delay_time = delay_time + 100000;
	if (delay_time> 10000000){
		delay_time = 10000000;
	}
}

void Sw2autoModeFunctionality(void){
	delay_time = delay_time - 100000;
	if (delay_time< 100000){
		delay_time = 100000;
	}
}

void Sw1ManualModeFunctionality(void){
	if (ui8Adjust > 249)
		ui8Adjust = 250;
	else
		ui8Adjust= ui8Adjust+10;
}

void Sw2ManualModeFunctionality(void){
	ui8Adjust = ui8Adjust-10;
	if (ui8Adjust < 10)
		ui8Adjust = 10;
}

void manual_mode1_functionality(void){
    PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , true);
    PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , false);
    PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , false);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_5, ui8Adjust * ui32Load / 1000);
}

void manual_mode2_functionality(void){
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , false);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , true);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , false);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_6, ui8Adjust * ui32Load / 1000);
}

void manual_mode3_functionality(void){
	PWMOutputState(PWM1_BASE, PWM_OUT_5_BIT , false);
	PWMOutputState(PWM1_BASE, PWM_OUT_6_BIT , false);
	PWMOutputState(PWM1_BASE, PWM_OUT_7_BIT , true);
	PWMPulseWidthSet(PWM1_BASE, PWM_OUT_7, ui8Adjust * ui32Load / 1000);
}
void auto_mode_functionality(void){
	GPIOPinWrite(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3, ui8LED);
	SysCtlDelay(delay_time);
	if(ui8LED)
		ui8LED = ui8LED+2;
	if (ui8LED == 14)
		ui8LED = 2;
}

void switch1Functionality(void){
	if (state_sw1 == press){
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4)) //Switch 1
		{
			state_sw1 = release;
			sw1_count = 2;

			if (mode_status  == auto_mode){
				Sw1autoModeFunctionality(); //increment
			}
			if (mode_status  == manual_mode){
				if(state_sw2 == release){
					sw1_count_total = sw1_count_total+1;
				}
				else{
					Sw1ManualModeFunctionality();
					sw1_count_total = 0;
				}
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
			//TimerDisable(TIMER0_BASE, TIMER_B);
			flag_timerrun = true;
		}
		else{
			state_sw1 = release;
			if (flag_timerrun){
				//TimerEnable(TIMER0_BASE, TIMER_B);
				flag_timerrun = false;
			}
		}
	}
}


void switch2Functionality(void){
	if (state_sw2 == press){
		if(!GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0)) //Switch 1
		{
			state_sw2 = release;
			sw2_count = 2;
			if (mode_status  == auto_mode)
				Sw2autoModeFunctionality(); //Decrease
			else
				Sw2ManualModeFunctionality();
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

	if(state_sw2==idle)
		sw1_count_total = 0;


	if (state_sw2 == release){
		if((GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_0))) //Switch 1
		{
			state_sw2 = idle;
			sw2_count = 0;
		}
		else{
			state_sw2 = release;
			if (state_sw1 == release)
				mode_status = manual_mode;

			if (sw1_count_total==1)
				manual_mode_status = manual_mode1;

			if (sw1_count_total==2)
				manual_mode_status = manual_mode2;

			if (sw1_count_total==3){
				manual_mode_status = manual_mode3;
				sw1_count_total = 0;
			}
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
		if (mode_status == auto_mode)
			auto_mode_functionality();
		if (mode_status == manual_mode){
			PWMConfig();
			if(manual_mode_status == manual_mode1)
				manual_mode1_functionality();
			if(manual_mode_status == manual_mode2)
				manual_mode2_functionality();
			if(manual_mode_status == manual_mode3)
				manual_mode3_functionality();
		}
	}
}


void Timer0IntHandlerA(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	//bool flag = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
	switch1Functionality();
	switch2Functionality();
}

void Timer0IntHandlerB(void)
{
	// Clear the timer interrupt
	TimerIntClear(TIMER0_BASE, TIMER_TIMB_TIMEOUT);
	// Read the current state of the GPIO pin and
	// write back the opposite state
	//bool flag = GPIOPinRead(GPIO_PORTF_BASE, GPIO_PIN_4);
}
