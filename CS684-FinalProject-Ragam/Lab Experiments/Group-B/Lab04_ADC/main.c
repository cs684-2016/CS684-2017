#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/gpio.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

volatile uint32_t ui32JoyUD;
volatile uint32_t ui32JoyLR;

void sendcharacter(char* string, unsigned int size){
	unsigned int i;
	for(i = 0; i < size; i++){
		UARTCharPut(UART0_BASE, string[i]);
	}
}

void senddigit(uint32_t data){
	uint8_t digit;

	digit = data / 1000;
	data = data % 1000;
	unsigned char datatotransmit = digit+'0';
	UARTCharPut(UART0_BASE,datatotransmit);

	digit = data / 100;
	data = data % 100;
	datatotransmit = digit+'0';
	UARTCharPut(UART0_BASE,datatotransmit);

	digit = data / 10;
	data = data % 10;
	datatotransmit = digit+'0';
	UARTCharPut(UART0_BASE,datatotransmit);

	datatotransmit = data+'0';
	UARTCharPut(UART0_BASE,datatotransmit);
	//UARTCharPut(UART0_BASE, '\n');
}

int main(void)
{
	/* Sequencer Number of
					Samples 	Depth of FIFO
			SS 3 		1 			1
			SS 2 		4 			4
			SS 1 		4 			4
			SS 0 		8 			8*/
	// SS0 has a FIFO depth of 4
	uint32_t ui32ADC0Value[4];
	int python = 2;

	SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);

	// Joystick is connected to PD0 and PD1
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);

	/*...................................UART..........................................*/
	//Enable the UART0 and GPIOA peripherals (the UART pins are on GPIO Port A)
	SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);
	SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
	// Configure the pins for the receiver and transmitter using GPIOPinConfigure
	GPIOPinConfigure(GPIO_PA0_U0RX);
	GPIOPinConfigure(GPIO_PA1_U0TX);
	GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);
	//Initialize the parameters for the UART: 115200, 8-1-N
	UARTConfigSetExpClk(UART0_BASE, SysCtlClockGet(), 115200,
	 (UART_CONFIG_WLEN_8 | UART_CONFIG_STOP_ONE | UART_CONFIG_PAR_NONE));
	 char sendtext[] = "JOYSTICK VALUE:";
	 UARTCharPut(UART0_BASE, '\n');
	 sendcharacter(sendtext, 15);
	 //UARTCharPut(UART0_BASE, 30);

	/*..................................................................................*/


	//GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0);
    GPIOPinTypeADC(GPIO_PORTD_BASE, GPIO_PIN_0 | GPIO_PIN_1);

	//  enable the ADC0 peripheral
	SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);


 //configure ADC sequencer - use ADC 0, sample sequencer 1, want the processor to trigger sequence, use highest priority
	ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);

    //Configure ADC Sequencer Steps 0 - 2 on sequencer 1 to sample the temperature sensor
	ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH7 );
	ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH7 );
	ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6 );
	//final sequencer step: need to sample temp sensor(ADC_CTL_TS), configure interrupt flag(ADC_CTL_IE) to be set when sample is done
	 //tell ADC logic that this is the last conversion on sequencer (ADC_CTL_END)
	ADCSequenceStepConfigure(ADC0_BASE, 1, 3, ADC_CTL_CH6 |ADC_CTL_IE|ADC_CTL_END);

	ADCSequenceEnable(ADC0_BASE, 1);
	while(1)
	{
		 //clear interrupt flag
		ADCIntClear(ADC0_BASE, 1);
		ADCProcessorTrigger(ADC0_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{

		}
		ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ui32JoyUD = (ui32ADC0Value[0] + ui32ADC0Value[1] + 1)/2;
		ui32JoyLR = (ui32ADC0Value[2] + ui32ADC0Value[3] + 1)/2;
		switch (python){
		case 1:
			sendcharacter("X: ",3);
			senddigit(ui32JoyLR);
			sendcharacter("Y: ",3);
			senddigit(ui32JoyUD);
			UARTCharPut(UART0_BASE, '\n');
			break;
		case 2:
			senddigit(ui32JoyUD);
			UARTCharPut(UART0_BASE, ',');
			senddigit(ui32JoyLR);
			UARTCharPut(UART0_BASE, '\n');
			break;
		}



	}
}
