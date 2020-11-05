/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    This software and any accompanying information is for suggestion only. It
    does not modify Microchip's standard warranty for its products. You agree
    that you are solely responsible for testing the software and determining its
    suitability. Microchip has no obligation to modify, test, certify, or
    support the software.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
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

// This is a guard condition so that contents of this file are not included
// more than once.  
#ifndef _DRV_PWRCTRL_4SWBB_INTERNAL_H_
#define	_DRV_PWRCTRL_4SWBB_INTERNAL_H_

#include <xc.h> // include processor files - each processor file is guarded.  

//------------------------------------------------------------------------------

#define sQ15val(val_) ((val_ < 0.0) ? (32768 * (val_) - 0.5) : (32767 * (val_) + 0.5))
#define uQ15val(val_) (uint16_t)((float)val_ * 0xFFFF)

#define Prozent90value (uint16_t) uQ15val(90) //90% clamp
#define Prozent7value uQ15val(7) //90% clamp     //200ns = 800*250ps


#define SOFT_START_RAMP_SPEED   2
#define SOFT_START_RAMP_SPEED_I 2

#define BuckPWM  1
#define BoostPWM 2

//------------------------------------------------------------------------------

typedef struct
{
    volatile uint16_t val_VoutRef_internal;
    volatile uint16_t drv_val_VoutRef;
    volatile uint16_t val_IinRef_internal;
    volatile uint16_t drv_val_IinRef;
    volatile uint16_t drv_comm_val_VoutRef;
}PWR_CTRL_REFDATA_t;                       // power control soft-start settings and variables
extern PWR_CTRL_REFDATA_t pwr_ctrl_ref_data;

//------------------------------------------------------------------------------

void Drv_PwrCtrl_4SWBB_SwitchOnPWM(void);
void Drv_PwrCtrl_4SWBB_SwitchOffPWM(void);
void Drv_PwrCtrl_4SWBB_CtrlLoop(void);
void Drv_PwrCtrl_4SWBB_DriveBuckBoost(uint16_t drive_val);
void StartUpTestMaxCurrentMethodCurrent(void);
void StartUpTestMaxCurrentMethodVoltage(void);
void PreBias10ms(uint16_t taskCounterPreBias_);

//------------------------------------------------------------------------------

#endif	// _DRV_PWRCTRL_4SWBB_INTERNAL_H_

