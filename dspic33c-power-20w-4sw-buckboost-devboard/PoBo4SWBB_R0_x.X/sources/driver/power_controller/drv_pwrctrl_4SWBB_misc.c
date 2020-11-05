/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products. You agree
    that you are solely responsible for testing the software and determining its
    suitability. Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY I
 * 025MPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
 */

#include <p33CK256MP506.h>
#include <stdint.h>

#include "drv_pwrctrl_4SWBB.h"
#include "drv_pwrctrl_4SWBB_internal.h"
#include "../drv_adc.h"
#include "../mcc_generated_files/pwm.h"
#include "../mcc_generated_files/pin_manager.h"

PWR_CTRL_REFDATA_t pwr_ctrl_ref_data;
//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Start(void)
{
  while (pwr_ctrl_flagbits.run == 0) //it needs to be a while function to avoid interrupt access conflicts
  {
    pwr_ctrl_flagbits.run = 1;
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Stop(void)
{
  while (pwr_ctrl_flagbits.run == 1) //it needs to be a while function to avoid interrupt access conflicts
  {
    pwr_ctrl_flagbits.run = 0;
  }
}
//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetMode_OpenLoop(void)
{
  //it needs to be a while function to avoid interrupt access conflicts
  while (pwr_ctrl_flagbits.inopenloop == 0 || pwr_ctrl_flagbits.inclosedloop == 1)
  {
    pwr_ctrl_flagbits.inopenloop = 1;
    pwr_ctrl_flagbits.inclosedloop = 0;
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetMode_ClosedLoop(void)
{
  //it needs to be a while function to avoid interrupt access conflicts
  while (pwr_ctrl_flagbits.inopenloop == 1 || pwr_ctrl_flagbits.inclosedloop == 0)
  {
    pwr_ctrl_flagbits.inopenloop = 0;
    pwr_ctrl_flagbits.inclosedloop = 1;
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetReferenceRaw(uint16_t ref)
{
  pwr_ctrl_ref_data.drv_val_VoutRef = ref;
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SetReference_mV(uint16_t ref_mV)
{
  //Todo
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void StartUpTestMaxCurrentMethodCurrent(void)
{
  static uint16_t _CounterStartupCurrent = 0;

  //IIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIIII
  //  if (_CounterStartupCurrent++ > 0)
  {
    if (pwr_ctrl_ref_data.val_IinRef_internal < pwr_ctrl_ref_data.drv_val_IinRef)
    {
      if (pwr_ctrl_ref_data.val_IinRef_internal + SOFT_START_RAMP_SPEED_I < pwr_ctrl_ref_data.drv_val_IinRef)
      {
        pwr_ctrl_ref_data.val_IinRef_internal += SOFT_START_RAMP_SPEED_I;
      }
      else
      {
        pwr_ctrl_ref_data.val_IinRef_internal = pwr_ctrl_ref_data.drv_val_IinRef;
        pwr_ctrl_state = PCS_SOFT_START;
      }
    }
    _CounterStartupCurrent = 0;

  }

}

//==============================================================================
// @brief this function contains 
//==============================================================================

void StartUpTestMaxCurrentMethodVoltage(void)
{
  //UUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUUU 
  {
    if (pwr_ctrl_ref_data.val_VoutRef_internal < pwr_ctrl_ref_data.drv_val_VoutRef)
    {
      if (pwr_ctrl_ref_data.val_VoutRef_internal + SOFT_START_RAMP_SPEED < pwr_ctrl_ref_data.drv_val_VoutRef)
        pwr_ctrl_ref_data.val_VoutRef_internal += SOFT_START_RAMP_SPEED;
      else
      {
        pwr_ctrl_ref_data.val_VoutRef_internal = pwr_ctrl_ref_data.drv_val_VoutRef;
        pwr_ctrl_state = PCS_UP_AND_RUNNING;
      }
    }
    else
    {
      if (pwr_ctrl_ref_data.val_VoutRef_internal + SOFT_START_RAMP_SPEED > pwr_ctrl_ref_data.drv_val_VoutRef)
        pwr_ctrl_ref_data.val_VoutRef_internal -= SOFT_START_RAMP_SPEED;
      else
      {
        pwr_ctrl_ref_data.val_VoutRef_internal = pwr_ctrl_ref_data.drv_val_VoutRef;
        pwr_ctrl_state = PCS_UP_AND_RUNNING;
      }

    }
  }
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void PreBias10ms(uint16_t taskCounterPreBias_)
{
  if (taskCounterPreBias_ > 99)
  {
    if (pwr_ctrl_flagbits.inclosedloop)
    {
//      PG1IOCONHbits.PENL = 0;
//      PWM_OverrideLowEnable(BuckPWM);   //disable PWM output on Buck-L 
      
//      PWM_OverrideLowEnable(BoostPWM);  //disable PWM output on Boost-L 
      PWM_OverrideHighEnable(BoostPWM); //disable PWM output on Boost-H 
    }
//    pwr_ctrl_state = PCS_SOFT_START_OL;
    pwr_ctrl_state = PCS_SOFT_START;

    //@ftx set start values for Iacref ramp and Voutref ramp
    //     Iacref ramp should be higher that expected voltage controller output
    //     that prefents saturating of voltage controller output
    pwr_ctrl_ref_data.drv_val_IinRef = 2600;
    if (pwr_ctrl_flagbits.inclosedloop)
      pwr_ctrl_ref_data.val_VoutRef_internal = 500; //750;
    else
      pwr_ctrl_ref_data.val_VoutRef_internal = 0;

      pwr_ctrl_ref_data.val_IinRef_internal = (9600 - (pwr_ctrl_adc_data.drv_adc_val_FB_Vdd<<1))/3;  //power feedforward for current limit
  }
}

//==============================================================================
//==============================================================================
