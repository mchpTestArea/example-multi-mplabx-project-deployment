//=======================================================================================================
// Copyright(c) 2018 Microchip Technology Inc. and its subsidiaries.
// Subject to your compliance with these terms, you may use Microchip software and any derivatives
// exclusively with Microchip products. It is your responsibility to comply with third party license
// terms applicable to your use of third-party software (including open source software) that may
// accompany Microchip software.
// THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY,
// APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND
// FITNESS FOR A PARTICULAR PURPOSE.
// IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL
// LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF
// MICROCHIP HAS BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT
// ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT
// EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
//=======================================================================================================

//=======================================================================================================
// @file drv_led.c
//
// @brief contains the functions for accessing the leds
//
// @version v1.0
// @date 2020-07-15
// @author M52409
//
//=======================================================================================================

#include "mcc_generated_files/system.h"
#include "mcc_generated_files/pin_manager.h"
#include "drv_adc.h"
#include "power_controller/drv_pwrctrl_4SWBB.h"
#include"power_controller/drv_pwrctrl_4SWBB_internal.h" //TODO raus damit

#define ADC_CHANNEL_AN06_VAUX		6
#define ADC_CHANNEL_AN10_VDD		10
#define ADC_CHANNEL_AN15_TESTREF	15
#define ADC_CHANNEL_AN17_IOUT		17
#define ADC_CHANNEL_AN19_IOUT		19

PWR_CTRL_DATA_t pwr_ctrl_adc_data;

//-------------------------------------------------------------------------------------------------------
//
//-------------------------------------------------------------------------------------------------------

void __attribute__((__interrupt__, auto_psv)) _ADCAN0Interrupt(void)
{
  static uint16_t ISRCounter = 0;
  
  TP35_TX_SetHigh();
  
	pwr_ctrl_adc_data.drv_adc_val_FB_Iin =  ADCBUF0;
	pwr_ctrl_adc_data.drv_adc_val_FB_Vout = ADCBUF1;

	//TODO make callback function
  if(ISRCounter++ > 0)
  {  
    Drv_PwrCtrl_4SWBB_CtrlLoop();   //Todo: callbackfunction machen!!!!
    ISRCounter = 0;
  }
  
	switch (ADCON3Lbits.CNVCHSEL)
	{
		case ADC_CHANNEL_AN15_TESTREF:		//Read the ADC value from the ADCBUF15
			if(ADSTATLbits.AN15RDY)
			{
				pwr_ctrl_adc_data.drv_adc_val_AN15 = ADCBUF15;
				ADCON3Lbits.CNVCHSEL = ADC_CHANNEL_AN17_IOUT;
			}
			break;
			
		case ADC_CHANNEL_AN17_IOUT:		//Read the ADC value from the ADCBUF19
			if	(ADSTATHbits.AN17RDY)
			{
				pwr_ctrl_adc_data.drv_adc_val_FB_Iout = ADCBUF17;
				ADCON3Lbits.CNVCHSEL = ADC_CHANNEL_AN10_VDD;
			}
			break;
			
		case ADC_CHANNEL_AN10_VDD:		//Read the ADC value from the ADCBUF10
			if	(ADSTATLbits.AN10RDY)
			{
				pwr_ctrl_adc_data.drv_adc_val_FB_Vdd = ADCBUF10;
				ADCON3Lbits.CNVCHSEL = ADC_CHANNEL_AN06_VAUX;
			}
			break;

		case ADC_CHANNEL_AN06_VAUX:	//Read the ADC value from the ADCBUF6
			if	(ADSTATLbits.AN6RDY)
			{
				pwr_ctrl_adc_data.drv_adc_val_FB_Vaux = ADCBUF6;
				ADCON3Lbits.CNVCHSEL = ADC_CHANNEL_AN15_TESTREF;
			}
			break;
			
		default:
			ADCON3Lbits.CNVCHSEL = ADC_CHANNEL_AN15_TESTREF;
			break;
	}

	IFS5bits.ADCAN0IF = 0;			//clear the channel_ANA0 interrupt flag
  
  TP35_TX_SetLow();
}

//-------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------

