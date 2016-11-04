#include <stdint.h>
#include <stdbool.h>
#include "stdlib.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "inc/hw_uart.h"
#include "inc/hw_gpio.h"
#include "inc/hw_pwm.h"
#include "inc/hw_types.h"
#include "driverlib/timer.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/rom.h"
#include "driverlib/rom_map.h"
#include "driverlib/sysctl.h"
#include "driverlib/uart.h"
#include "driverlib/udma.h"
#include "driverlib/pwm.h"
#include "driverlib/ssi.h"
#include "driverlib/systick.h"
#include <string.h>

volatile uint32_t millis=0;

uint32_t time=56666;


void SycTickInt(){
	millis++;
}
void SysTickbegin(){
	SysTickPeriodSet(80000);
	SysTickIntRegister(SycTickInt);
	SysTickIntEnable();
	SysTickEnable();
}

void Wait(uint32_t _time){
	uint32_t temp = millis;
	while( (millis-temp) < _time){
	}
}


uint32_t freq = 100000;

void unlock(void){
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = GPIO_LOCK_KEY;
	HWREG(GPIO_PORTF_BASE + GPIO_O_CR) |= 0x01;
	HWREG(GPIO_PORTF_BASE + GPIO_O_LOCK) = 0;
}

int main()
{

	SysCtlClockSet(SYSCTL_SYSDIV_2_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ); // 80Mhz
	SysTickbegin();

	uint32_t Period, dutyCycle;
	Period = SysCtlClockGet()/freq ;
	dutyCycle = Period-2;

	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
	GPIODirModeSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_DIR_MODE_IN);
	GPIOPadConfigSet(GPIO_PORTF_BASE, GPIO_PIN_4|GPIO_PIN_0, GPIO_STRENGTH_2MA, GPIO_PIN_TYPE_STD_WPU);

	SysCtlDelay(3);

	GPIOPinConfigure(GPIO_PF1_T0CCP1);
	GPIOPinConfigure(GPIO_PF2_T1CCP0);
	GPIOPinConfigure(GPIO_PF3_T1CCP1);
	GPIOPinTypeTimer(GPIO_PORTF_BASE, GPIO_PIN_1|GPIO_PIN_2|GPIO_PIN_3);

	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);
	SysCtlDelay(3);

	TimerConfigure(TIMER0_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_B_PWM);
	TimerLoadSet(TIMER0_BASE, TIMER_B, Period -1);
	TimerMatchSet(TIMER0_BASE, TIMER_B, dutyCycle); // PWM


	SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER1);
	SysCtlDelay(3);

	TimerConfigure(TIMER1_BASE, TIMER_CFG_SPLIT_PAIR|TIMER_CFG_A_PWM|TIMER_CFG_B_PWM);
	TimerLoadSet(TIMER1_BASE, TIMER_A, Period -1);
	TimerLoadSet(TIMER1_BASE, TIMER_B, Period -1);
	TimerMatchSet(TIMER1_BASE, TIMER_A, dutyCycle);
	TimerMatchSet(TIMER1_BASE, TIMER_B, dutyCycle);

	TimerEnable(TIMER0_BASE, TIMER_B);
	TimerEnable(TIMER1_BASE, TIMER_A|TIMER_B);

	int i;

	//Start by rising Red LED
	for(i=Period-2; i >  0;i--){
		TimerMatchSet(TIMER0_BASE, TIMER_B, i);
		SysCtlDelay(time);
	}
	while(1){
		//Green brightness goes up - PF3
		for(i=Period-2; i >  0;i--){
			TimerMatchSet(TIMER1_BASE, TIMER_B, i);
			SysCtlDelay(time);
		}

		//Red brightness goes down - PF1
		for(i=1; i < Period-1; i++){
			TimerMatchSet(TIMER0_BASE, TIMER_B, i);
			SysCtlDelay(time);
		}
		//Blue brightness goes up - PF2
		for(i=Period-2; i > 0;i--){
			TimerMatchSet(TIMER1_BASE, TIMER_A, i);
			SysCtlDelay(time);
		}
		//Green brightness goes down - PF3
		for(i=1; i < Period-1; i++){
			TimerMatchSet(TIMER1_BASE, TIMER_B, i);
			SysCtlDelay(time);
		}
		//Red brightness goes up - PF1
		for(i=Period-2; i > 0;i--){
			TimerMatchSet(TIMER0_BASE, TIMER_B, i);
			SysCtlDelay(time);
		}
		//Blue brightness goes down - PF2
		for(i=1; i < Period-1; i++){
			TimerMatchSet(TIMER1_BASE, TIMER_A, i);
			SysCtlDelay(time);
		}
/*		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_4)==0x00){

			Period = Period * 2;

		}
		if(GPIOPinRead(GPIO_PORTF_BASE,GPIO_PIN_0)==0x00){

			Period = Period / 2;

		}*/


	}

}
