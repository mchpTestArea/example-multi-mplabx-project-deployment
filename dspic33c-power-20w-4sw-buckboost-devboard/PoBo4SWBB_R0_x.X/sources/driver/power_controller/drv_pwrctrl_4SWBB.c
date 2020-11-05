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

#include "../mcc_generated_files/system.h"
#include "../mcc_generated_files/pin_manager.h"
#include "../mcc_generated_files/pwm.h"
#include "mcc_generated_files/cmp2.h"

#include "drv_pwrctrl_4SWBB.h"
#include "drv_pwrctrl_4SWBB_fault.h"
#include "drv_pwrctrl_4SWBB_internal.h"
#include "device/dev_button.h"

#include "Controller_pwrctrl_4SWBB.h"
#include "driver/drv_adc.h"
#include "./dcdt/avgcm/dcdt_generated_code/avgcm_dcdt.h"

//------------------------------------------------------------------------------

PWR_CTRL_STATE_e pwr_ctrl_state = 1; //PCS_INIT;
PWR_CTRL_FLAGBITS_t pwr_ctrl_flagbits;

//------------------------------------------------------------------------------
static volatile uint16_t DCmaxClampBuck;
static volatile uint16_t DCmaxClampBoost;
static volatile uint16_t DCminClampBuck;
static volatile uint16_t DCminClampBoost;

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Init(void)
{
  uint32_t l_zw_;

  l_zw_ = __builtin_muluu(uQ15val(0.9), PG1PER);
  DCmaxClampBuck = (uint16_t) (l_zw_ >> 16);
  l_zw_ = __builtin_muluu(uQ15val(0.07), PG1PER);
  DCminClampBuck = (uint16_t) (l_zw_ >> 16) + PG1DTH;

  l_zw_ = __builtin_muluu(uQ15val(0.9), PG1PER);
  DCmaxClampBoost = (uint16_t) (l_zw_ >> 16);
  l_zw_ = __builtin_muluu(uQ15val(0.07), PG1PER);
  DCminClampBoost = (uint16_t) (l_zw_ >> 16) + PG2DTH;

  pwr_ctrl_flagbits.CollectivePwrCtrlFlagbits = 0;
  pwr_ctrl_flagbits.inopenloop = 1;
  pwr_ctrl_flagbits.enabled = 1;
#ifdef AVGCURRENT
  pwr_ctrl_flagbits.avgcurrentmode = 1;
#else
  pwr_ctrl_flagbits.avgcurrentmode = 0;
#endif
  
  //============================================================================
  // @brief initialize Controllers
  CurrentController_PwrCtrl_4SWBB_Init();
  VoltageController_PwrCtrl_4SWBB_Init();
  VoltageControllerOnly_PwrCtrl_4SWBB_Init();
  //============================================================================
  //ONLY THAT PROCEDURE WORKS---------------------------------------------------
  //PENH/L must be enabled in MCC that PWM pins show up in Pin Module
  //The following procedure prevents glitch at both PWM startup
  // After that init the PWMs are in override mode
  //----------------------------------------------------------------------------
  PG2IOCONHbits.PENH = 0; // PWMxH Output Port Disable: PWM generator NOT controls the PWMxH output pin
  PG2IOCONHbits.PENL = 0; // PWMxL Output Port Disable: PWM generator NOT controls the PWMxL output pin
  PWM_DutyCycleSet(BoostPWM, DCminClampBoost);
  PWM_GeneratorEnable(BoostPWM);
  PG2IOCONHbits.PENH = 1; // PWMxH Output Port Enable: PWM generator controls the PWMxH output pin
  PG2IOCONHbits.PENL = 1; // PWMxL Output Port Enable: PWM generator controls the PWMxL output pin

  PG1IOCONHbits.PENH = 0; // PWMxH Output Port Disable: PWM generator NOT controls the PWMxH output pin
  PG1IOCONHbits.PENL = 0; // PWMxL Output Port Disable: PWM generator NOT controls the PWMxL output pin
  PWM_DutyCycleSet(BuckPWM, DCminClampBuck);
  PWM_GeneratorEnable(BuckPWM);
  PG1IOCONHbits.PENH = 1; // PWMxH Output Port Enable: PWM generator controls the PWMxH output pin
  PG1IOCONHbits.PENL = 1; //@ftx // PWMxL Output Port Enable: PWM generator controls the PWMxL output pin
  //----------------------------------------------------------------------------

  Drv_PwrCtrl_4SWBB_Init_Fault();

}

//==============================================================================
// @brief this function contains 
// @ftx document PWM startup signals; difference OL-CL
//==============================================================================

void Drv_PwrCtrl_4SWBB_SwitchOnPWM(void)
{
  PWM_GeneratorDisable(BuckPWM);
  PWM_GeneratorDisable(BoostPWM);

  SMPS_Controller2P2ZInitialize(&VMC2p2z); // Clear histories Voltage Controller
  SMPS_Controller2P2ZInitialize(&AVG_CM2p2z); // Clear histories Voltage Controller

  PWM_TriggerACompareValueSet(BuckPWM, 0); //PG1TRIGA = 0;
  PWM_DutyCycleSet(BuckPWM, 0);
  PWM_DutyCycleSet(BoostPWM, 0);

  if(pwr_ctrl_flagbits.inopenloop)
    PWM_OverrideHighDisable(BoostPWM); //enable PWM output on Boost-H
  else if(pwr_ctrl_flagbits.inclosedloop) 
    PWM_OverrideHighEnable(BoostPWM); //disable PWM output on Boost-H

  PWM_GeneratorEnable(BoostPWM);
  PWM_OverrideLowDisable(BoostPWM); //enable PWM output on Boost-L

  PWM_OverrideLowDisable(BuckPWM); //enable PWM output on Buck-L   
  PWM_GeneratorEnable(BuckPWM);
  PWM_OverrideHighDisable(BuckPWM); //enable PWM output on Buck-H

  pwr_ctrl_flagbits.pwm_running = 1;
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_SwitchOffPWM(void)
{
  pwr_ctrl_ref_data.val_VoutRef_internal = 0; //start from zero volt	
  pwr_ctrl_ref_data.val_IinRef_internal = 0; //start from zero current

  PWM_OverrideHighEnable(BuckPWM); //disable PWM output on Buck-H
  PG2IOCONLbits.OVRENH = 1;
  PWM_OverrideLowEnable(BuckPWM); //disable PWM output on Buck-L
  PG2IOCONLbits.OVRENL = 1;
  pwr_ctrl_flagbits.pwm_running = 0;
}

//==============================================================================
// @brief this function contains 
//==============================================================================

void Drv_PwrCtrl_4SWBB_Task_100us(void)
{
  static uint16_t taskCounterPreBias = 0;

  //Check for Fault
  Drv_PwrCtrl_4SWBB_Fault_Check(); 

  switch (pwr_ctrl_state)
  {
      //------------------------------------------------------------------------
    case PCS_INIT: //1
      //peripherals should be already initialized by MCC, so goto next state
      //            if (FourSWBBFaults.CollectiveFaults == 0)
        {
          Drv_PwrCtrl_4SWBB_SetFaultCounters();
          pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
        }
        break;

      //------------------------------------------------------------------------
    case PCS_WAIT_IF_FAULT_ACTIVE: //2
        if ((FourSWBBFaults.CollectiveFaults == 0))
        {
            pwr_ctrl_ref_data.val_VoutRef_internal = 0; //start from zero volt
            pwr_ctrl_state = PCS_STANDBY;
        }
        break;

      //------------------------------------------------------------------------
    case PCS_STANDBY: //3
        if (FourSWBBFaults.CollectiveFaults)
        {
            pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
        }
        else if (pwr_ctrl_ref_data.drv_val_VoutRef > 0 && pwr_ctrl_flagbits.run == 1)
        {
            Drv_PwrCtrl_4SWBB_SwitchOnPWM();
            pwr_ctrl_state = PCS_POWER_ON_DELAY;
            taskCounterPreBias = 0;
        }
        break;

      //------------------------------------------------------------------------
    case PCS_POWER_ON_DELAY: //4

      //@ftx startup 10ms with all switches run at min DC, precharge bootstrap and check after the 10ms
      //     check if Vout > 0.5V, otherwise fatal error (shortcut...)    
      PreBias10ms(taskCounterPreBias++);

      break;

      //------------------------------------------------------------------------
    case PCS_SOFT_START_OL: //5 
      break;

      //------------------------------------------------------------------------
    case PCS_SOFT_START: //6
        if (FourSWBBFaults.CollectiveFaults != 0)
        {
            Drv_PwrCtrl_4SWBB_SwitchOffPWM();
            pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
        }
        else
            StartUpTestMaxCurrentMethodVoltage();
        break;

      //------------------------------------------------------------------------
    case PCS_UP_AND_RUNNING: //7
        if (FourSWBBFaults.CollectiveFaults != 0)
        {
            Drv_PwrCtrl_4SWBB_SwitchOffPWM();
            pwr_ctrl_state = PCS_WAIT_IF_FAULT_ACTIVE;
        }
        else
        {
            if (pwr_ctrl_ref_data.drv_val_VoutRef == 0 || pwr_ctrl_flagbits.run == 0)
            {
                pwr_ctrl_state = PCS_STANDBY;
                Drv_PwrCtrl_4SWBB_SwitchOffPWM();
                break;
            }
            if (pwr_ctrl_ref_data.val_VoutRef_internal != pwr_ctrl_ref_data.drv_val_VoutRef)
            {
                pwr_ctrl_state = PCS_SOFT_START;
                break;
            }
        }
        break;

      //------------------------------------------------------------------------
    case PCS_SHUTDOWN: //8
      break;

      //------------------------------------------------------------------------

  }

}

//==============================================================================
// @brief|| this function contains the AVG Current Controller (every 2nd PWM cycle) 
// @brief|| and the Voltage Controller every x-PWM cycles
//==============================================================================

void Drv_PwrCtrl_4SWBB_CtrlLoop(void)
{
  static uint16_t loopCounterV = 0;

  //Debug DACout------------------------------------
  CMP2_SetDACDataHighValue(Controller_4SWBB._IACref);
  //Debug DACout------------------------------------

  //DCM DCM -------------------------------------------------------------------------------------------------------
  if ((pwr_ctrl_flagbits.inclosedloop) && (pwr_ctrl_state > 4)) //power controller state >= startup //closed loop
  {
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Iout > 2260) && (pwr_ctrl_state == PCS_UP_AND_RUNNING))
    {
      PWM_OverrideHighDisable(BoostPWM); //enable PWM output on Boost-H 

      pwr_ctrl_flagbits.CCM_mode = 1;
    }
    if ((pwr_ctrl_adc_data.drv_adc_val_FB_Iout < 2160) && (pwr_ctrl_state == PCS_UP_AND_RUNNING))
    {
      PWM_OverrideHighEnable(BoostPWM); //disable PWM output on Boost-H 

      pwr_ctrl_flagbits.CCM_mode = 0;
    }
  }

  //VLOOP  ---------------------------------------------------------------------------------------------------------
  if (loopCounterV++ > 3)
  {
    Controller_4SWBB._VDCref = pwr_ctrl_ref_data.val_VoutRef_internal >> 0; //xxV  

#ifdef AVGCURRENT
    SMPS_Controller2P2ZUpdate(&VMC2p2z, &pwr_ctrl_adc_data.drv_adc_val_FB_Vout, Controller_4SWBB._VDCref, &Controller_4SWBB._IACref);
#else      
    SMPS_Controller3P3ZUpdate(&VMC3p3z, &pwr_ctrl_adc_data.drv_adc_val_FB_Vout, Controller_4SWBB._VDCref, &Controller_4SWBB._CurrentControllerResult);
#endif

    //Vin<->Iin feedforward, numbers optimized
    pwr_ctrl_ref_data.val_IinRef_internal = (9600 - (pwr_ctrl_adc_data.drv_adc_val_FB_Vdd << 1)) / 3; //power feedforward for current limit

    if ((Controller_4SWBB._IACref > pwr_ctrl_ref_data.val_IinRef_internal)) //&& (pwr_ctrl_state < PCS_SOFT_START))
    {
      Controller_4SWBB._IACref = pwr_ctrl_ref_data.val_IinRef_internal; //2600=3.5A
    }
    loopCounterV = 0;
  }

  //ILOOP  ----------------------------------------------------------------------------------------------------------
#ifdef AVGCURRENT
  SMPS_Controller2P2ZUpdate(&AVG_CM2p2z, &pwr_ctrl_adc_data.drv_adc_val_FB_Iin, Controller_4SWBB._IACref, &Controller_4SWBB._CurrentControllerResult);
#endif
  //----------------------------------------------------------------------
  // check result and write value to DC for Buck leg
  //----------------------------------------------------------------------
  if (Controller_4SWBB._CurrentControllerResult > DCmaxClampBuck)
  {
    Controller_4SWBB._DCBuckset = DCmaxClampBuck;
  }
  else if (Controller_4SWBB._CurrentControllerResult < DCminClampBuck)
  {
    if (pwr_ctrl_state > PCS_POWER_ON_DELAY)
      Controller_4SWBB._DCBuckset = 0;
    else
      Controller_4SWBB._DCBuckset = DCminClampBuck;
  }
  else
  {
    Controller_4SWBB._DCBuckset = Controller_4SWBB._CurrentControllerResult;
  }
  if (pwr_ctrl_flagbits.inclosedloop) //closed loop
  {
    PWM_TriggerCompareValueSet(BUCK_LEG_PWM, (Controller_4SWBB._DCBuckset >> 1));
    PWM_DutyCycleSet(BUCK_LEG_PWM, Controller_4SWBB._DCBuckset);
  }
  //----------------------------------------------------------------------
  // check result if boost mode is required and write value to DC for Boost leg
  //----------------------------------------------------------------------
  if (Controller_4SWBB._CurrentControllerResult > DCmaxClampBuck /*PG1PER*/)
  {
    Controller_4SWBB._DCBoostset = Controller_4SWBB._CurrentControllerResult - DCmaxClampBuck /*PG1PER*/;
    if (Controller_4SWBB._DCBoostset > DCmaxClampBoost)
    {
      Controller_4SWBB._DCBoostset = DCmaxClampBoost;
    }
    else if (Controller_4SWBB._DCBoostset < DCminClampBoost)
    {
      Controller_4SWBB._DCBoostset = DCminClampBoost; //0
    }
    else
    {
      Controller_4SWBB._DCBoostset = Controller_4SWBB._DCBoostset;
    }
    if (pwr_ctrl_flagbits.inclosedloop) //closed loop
    {
      PWM_DutyCycleSet(BOOST_LEG_PWM, Controller_4SWBB._DCBoostset);
    }
  }
  else
  {
    if (pwr_ctrl_flagbits.inclosedloop) //closed loop
    {
      Controller_4SWBB._DCBoostset = DCminClampBoost; //0
      PWM_DutyCycleSet(BOOST_LEG_PWM, Controller_4SWBB._DCBoostset);
    }
  }
  //-------------------------------------------------------------------------------------------------------

  //openloop ------------------------------------------------------------------------------------------------
  if (pwr_ctrl_flagbits.inopenloop)
  {
    Drv_PwrCtrl_4SWBB_DriveBuckBoost(pwr_ctrl_ref_data.val_VoutRef_internal);
  }
  
}

//==============================================================================
// @brief this function contains 
//-----------------------------------------------------------------------------
// AN15:      0...........2048.........4096
// fract:     0...........32767........65535
// Value      0...........0.5..........1
// 
// Mode       Buck........Boost........
// ValueDC    0...........1/0..........1         
// DC         0       100%/0.........100% 
// over the poti range the DC must go twice from 0% to 100%
// AN15 value is shifted by 5 instead of 4 for 12 Bit to 16 bit
//==============================================================================

inline void Drv_PwrCtrl_4SWBB_DriveBuckBoost(uint16_t drive_val)
{
  uint32_t l_BoostDC, l_BuckDC;
  uint16_t BoostDC, BuckDC;

  //----------------------------------------------------------------------------
  //Boost
  if (drive_val > (0x0800))
  {
    l_BoostDC = __builtin_muluu(drive_val << 5, PG2PER); //
    BoostDC = (uint16_t) (l_BoostDC >> 16);
  }
  else
  {
    BoostDC = DCminClampBoost;
  }

  if (BoostDC > DCmaxClampBoost)
  {
    BoostDC = DCmaxClampBoost;
  }
  else if (BoostDC < DCminClampBoost)
  {
    BoostDC = DCminClampBoost;
  }

  //----------------------------------------------------------------------------
  //Buck
  if (drive_val < 0x0800)
  {
    l_BuckDC = __builtin_muluu(drive_val << 5, PG1PER); //
    BuckDC = (uint16_t) (l_BuckDC >> 16);
  }
  else
  {
    BuckDC = DCmaxClampBuck;
  }

  if (BuckDC > DCmaxClampBuck)
  {
    BuckDC = DCmaxClampBuck;
  }
  else if (BuckDC < DCminClampBuck)
  {
    BuckDC = DCminClampBuck;
  }

  PG1TRIGA = BuckDC >> 1;
  PWM_DutyCycleSet(1, BuckDC);
  PWM_DutyCycleSet(2, BoostDC);

}

//==============================================================================
//==============================================================================
