#include <stdint.h>
#include <stdbool.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/debug.h"
#include "driverlib/sysctl.h"
#include "driverlib/adc.h"
#include "driverlib/systick.h"
#include "driverlib/rom.h"
#include "inc/hw_ints.h"
#include "driverlib/gpio.h"
#include "driverlib/interrupt.h"
#include "driverlib/pin_map.h"
#include "driverlib/uart.h"

uint32_t ui32ADC0Value[4];
uint32_t ui32ADC0Value2[4];

volatile uint32_t ui32TempAvg0;
volatile uint32_t ui32TempAvg1;

int main(void)
{

	ROM_SysCtlClockSet(SYSCTL_SYSDIV_5|SYSCTL_USE_PLL|SYSCTL_OSC_MAIN|SYSCTL_XTAL_16MHZ);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC0);
	ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_ADC1);

	//ROM_ADCHardwareOversampleConfigure(ADC0_BASE, 64);

	ROM_ADCSequenceConfigure(ADC0_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 0, ADC_CTL_CH6);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 1, ADC_CTL_CH6);
	ROM_ADCSequenceStepConfigure(ADC0_BASE, 1, 2, ADC_CTL_CH6);
	ROM_ADCSequenceStepConfigure(ADC0_BASE,1,3,ADC_CTL_CH6|ADC_CTL_IE|ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC0_BASE, 1);

	ROM_ADCSequenceConfigure(ADC1_BASE, 1, ADC_TRIGGER_PROCESSOR, 0);
	ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 0, ADC_CTL_CH7);
	ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 1, ADC_CTL_CH7);
	ROM_ADCSequenceStepConfigure(ADC1_BASE, 1, 2, ADC_CTL_CH7);
	ROM_ADCSequenceStepConfigure(ADC1_BASE,1,3,ADC_CTL_CH7|ADC_CTL_IE|ADC_CTL_END);
	ROM_ADCSequenceEnable(ADC1_BASE, 1);

	while(1)
	{
		ROM_ADCIntClear(ADC0_BASE, 1);
		ROM_ADCProcessorTrigger(ADC0_BASE, 1);
		ROM_ADCIntClear(ADC1_BASE, 1);
		ROM_ADCProcessorTrigger(ADC1_BASE, 1);
		while(!ADCIntStatus(ADC0_BASE, 1, false))
		{
		}
		while(!ADCIntStatus(ADC1_BASE, 1, false))
		{
		}
		ROM_ADCSequenceDataGet(ADC0_BASE, 1, ui32ADC0Value);
		ROM_ADCSequenceDataGet(ADC1_BASE, 1, ui32ADC0Value2);

		ui32TempAvg0 = (ui32ADC0Value[0] + ui32ADC0Value[1] + ui32ADC0Value[2] + ui32ADC0Value[3] + 2)/4;
		ui32TempAvg1 = (ui32ADC0Value2[0] + ui32ADC0Value2[1] + ui32ADC0Value2[2] + ui32ADC0Value2[3] + 2)/4;

	}
}





