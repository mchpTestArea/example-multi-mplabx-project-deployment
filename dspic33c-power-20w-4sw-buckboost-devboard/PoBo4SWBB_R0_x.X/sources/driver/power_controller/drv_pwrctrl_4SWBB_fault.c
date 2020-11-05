/*
 * File:   drv_pwrctrl_4SWBB_fault.c
 * Author: M91406, M52409
 *
 * Created on July 30, 2019, 10:11 PM
 */

#include <xc.h> // include processor files - each processor file is guarded.  
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include "mcc_generated_files/system.h"

#include "misc/fault_common.h"
#include "driver/power_controller/drv_pwrctrl_4SWBB.h"
#include "driver/drv_adc.h"
#include "../mcc_generated_files/cmp3.h"
#include "../mcc_generated_files/cmp1.h"
#include "drv_pwrctrl_4SWBB_fault.h"

PWR_CTRL_FAULTS FourSWBBFaults;

FAULT_OBJ_T fltobj_4SWBB_VinUv;     // Input Under Voltage Protection (UVP) handled by ADC
FAULT_OBJ_T fltobj_4SWBB_VinOv;     // Input Over Voltage Protection (OVP) handled by ADC

FAULT_OBJ_T fltobj_4SWBB_VoutOv;		// Output Over Voltage Protection (OVP) handled by ADC
FAULT_OBJ_T fltobj_4SWBB_VoutOvCMP; // Output Over Voltage Protection (OVP) handled by CMP

FAULT_OBJ_T fltobj_4SWBB_VauxOOR;   // Auxilliary Voltage out of range Protection handled by ADC 

FAULT_OBJ_T fltobj_4SWBB_IoutOC;    // Output Over Current Protection handled by ADC 

FAULT_OBJ_T fltobj_4SWBB_IinOCCMP;  // Input Over Current Protection handled by CMP

		
//-------------------------------------------------------------------------------------------------------
// Defines for thresholds and counters
//-------------------------------------------------------------------------------------------------------

//Vin Under Voltage ADC
#define VinUV_float        (float)((4096/26.4)*7)   //7V
#define VinUVThreshold     (uint16_t) VinUV_float     

#define VinUVHys_float 		(float)((4096/8/3.3)*7.5)  //7.5V
#define VinUVHysLimit      (uint16_t) VinUVHys_float 

#define VinUVThresholdCnt  30
#define VinUVHysCnt        30000
#define VinUVCounter       0
#define VinUVCode          0
#define VinUVActive        0

//Vin Over Voltage ADC
#define VinOV_float        (float)((4096/26.4)*18)   //18V
#define VinOVThreshold     (uint16_t) VinOV_float     

#define VinOVHys_float 		 (float)((4096/26.4)*17)  //17V
#define VinOVHysLimit      (uint16_t) VinOVHys_float 

#define VinOVThresholdCnt  30
#define VinOVHysCnt        30000
#define VinOVCounter       0
#define VinOVCode          0
#define VinOVActive        0

//Vout Over Voltage ADC
#define VoutOV_float        (float)((4096/26.4)*22)   //22V
#define VoutOVThreshold     (uint16_t) VoutOV_float   //  

#define VoutOVHys_float 		(float)((4096/8/3.3)*5)   //5V
#define VoutOVHysLimit      (uint16_t) VoutOVHys_float //

#define VoutOVThresholdCnt  30
#define VoutOVHysCnt        30000
#define VoutOVCounter       0
#define VoutOVCode          0
#define VoutOVActive        0

//Vout Over Voltage CMP
#define VoutOVCMP_float        (float)((4096/26.4)*20)   //25V
#define VoutOVCMPThreshold     (uint16_t) VoutOVCMP_float   // 

#define VoutOVCMPHys_float 		(float)((4096/8/3.3)*5)   //5V
#define VoutOVCMPHysLimit      (uint16_t) VoutOVCMPHys_float //

#define VoutOVCMPThresholdCnt  30
#define VoutOVCMPHysCnt        30000
#define VoutOVCMPCounter       0
#define VoutOVCMPCode          0
#define VoutOVCMPActive        0

//Vaux out of range
#define VauxOOR1_float        (float)((4096/2/3.3)*5)   //5V
#define VauxOORThreshold1     (uint16_t) VauxOOR1_float 
#define VauxOOR2_float        (float)((4096/2/3.3)*6)   //6V
#define VauxOORThreshold2     (uint16_t) VauxOOR2_float  

#define VauxOORHys1_float     (float)((4096/2/3.3)*5)   //5V
#define VauxOORHysLimit1      (uint16_t) VauxOORHys1_float 
#define VauxOORHys2_float     (float)((4096/2/3.3)*6)   //6V
#define VauxOORHysLimit2      (uint16_t) VauxOORHys2_float 

#define VauxOORThresholdCnt  30
#define VauxOORHysCnt        30000
#define VauxOORCounter       0
#define VauxOORCode          0
#define VauxOORActive        0

//Iout Over Current ADC
#define IoutOC_float        (float)((((0.4*2.2)+1.65)*4096)/3.3) //2.2A
#define IoutOCThreshold     (uint16_t) IoutOC_float    

#define IoutOCHys_float 		(float)((((0.4*2)+1.65)*4096)/3.3) //2A
#define IoutOCHysLimit      (uint16_t) IoutOCHys_float 

#define IoutOCThresholdCnt  30
#define IoutOCHysCnt        30000
#define IoutOCCounter       0
#define IoutOCCode          0
#define IoutOCActive        0

//Iin Over Current CMP
#define IinOCCMP_float        (float)((((0.47*2)+0.447)*4096)/3.3) //3A
#define IinOCCMPThreshold     (uint16_t) IinOCCMP_float    

#define IinOCCMPHys_float 		(float)((((0.47*1.8)+0.447)*4096)/3.3) //2.8A
#define IinOCCMPHysLimit      (uint16_t) IinOCCMPHys_float 

#define IinOCCMPThresholdCnt  30
#define IinOCCMPHysCnt        30000
#define IinOCCMPCounter       0
#define IinOCCMPCode          0
#define IinOCCMPActive        0

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void Drv_PwrCtrl_4SWBB_Init_Fault(void)
{
	FAULT_Init( &fltobj_4SWBB_VinUv, VinUVThreshold, VinUVHysLimit, 0, 0 ); //initialize with 0's that fault is immediately activ
	FAULT_Init( &fltobj_4SWBB_VinOv, VinOVThreshold, VinOVHysLimit, 0, 0 ); //initialize with 0's that fault is immediately activ
  
	FAULT_Init( &fltobj_4SWBB_VoutOv, VoutOVThreshold, VoutOVHysLimit, VoutOVThresholdCnt, VoutOVHysCnt );
  FAULT_Init( &fltobj_4SWBB_VoutOvCMP, VoutOVCMPThreshold, VoutOVCMPHysLimit, VoutOVCMPThresholdCnt, VoutOVCMPHysCnt  );  
  CMP3_SetDACDataHighValue(VoutOVCMPThreshold);
  
  FAULT_InitRange(&fltobj_4SWBB_VauxOOR, VauxOORThreshold1, VauxOORHysLimit1,VauxOORThreshold2, VauxOORHysLimit2, VauxOORThresholdCnt, VauxOORHysCnt);

	FAULT_Init(&fltobj_4SWBB_IoutOC, IoutOCThreshold, IoutOCHysLimit, IoutOCThresholdCnt, IoutOCHysCnt );
  
  FAULT_Init(&fltobj_4SWBB_IinOCCMP, IinOCCMPThreshold, IinOCCMPHysLimit, IinOCCMPThresholdCnt, IinOCCMPHysCnt  );  
  CMP1_SetDACDataHighValue(IinOCCMPThreshold);
}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void Drv_PwrCtrl_4SWBB_SetFaultCounters(void)
{
  FAULT_SetCounters(&fltobj_4SWBB_VinUv, VinUVThresholdCnt, VinUVHysCnt);  
  FAULT_SetCounters(&fltobj_4SWBB_VinOv, VinOVThresholdCnt, VinOVHysCnt);
}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void Drv_PwrCtrl_4SWBB_Fault_Check()
{
	//  Vin uvervoltage check
	FourSWBBFaults.inputUnderVoltage = FAULT_CheckMin(&fltobj_4SWBB_VinUv, pwr_ctrl_adc_data.drv_adc_val_FB_Vdd, NULL);
	//  Vin overvoltage check
	FourSWBBFaults.inputOverVoltage = FAULT_CheckMax(&fltobj_4SWBB_VinOv, pwr_ctrl_adc_data.drv_adc_val_FB_Vdd, NULL);

	//  Vout overvoltage check
	FourSWBBFaults.outputOverVoltage = FAULT_CheckMax(&fltobj_4SWBB_VoutOv, pwr_ctrl_adc_data.drv_adc_val_FB_Vout, NULL);
	
  //  Vout Comparator HW overvoltage check
	FourSWBBFaults.outputVoltageCMP   = FAULT_CheckBit(&fltobj_4SWBB_VoutOvCMP, PG1STATbits.FLTEVT, NULL);
  // Iin overcurrent check
  FourSWBBFaults.inputCurrentCMP   = FAULT_CheckBit(&fltobj_4SWBB_IinOCCMP, PG1STATbits.FLTEVT, NULL);
  if ((FourSWBBFaults.outputVoltageCMP || FourSWBBFaults.inputCurrentCMP) && (PG1STATbits.FLTEVT || PG2STATbits.FLTEVT))
  {
    PG1FPCILbits.SWTERM = 1;
    PG1STATbits.FLTEVT = 0;
    PG2FPCILbits.SWTERM = 1;
    PG2STATbits.FLTEVT = 0;
  }    
  // Vaux out of range check
  FourSWBBFaults.auxVoltageOor = FAULT_CheckRange(&fltobj_4SWBB_VauxOOR, pwr_ctrl_adc_data.drv_adc_val_FB_Vaux, NULL);
  
  // Iout overcurrent check
	FourSWBBFaults.outputOverCurrent = FAULT_CheckMax(&fltobj_4SWBB_IoutOC, pwr_ctrl_adc_data.drv_adc_val_FB_Iout, NULL);
  
  // Iin overcurrent check
// 	FourSWBBFaults.inputCurrentCMP   = FAULT_CheckBit(&fltobj_4SWBB_IinOCCMP, DAC1CONLbits.CMPSTAT, NULL);
//  if (FourSWBBFaults.inputCurrentCMP && (PG1STATbits.FLTEVT || PG2STATbits.FLTEVT))
//  {
//    PG1FPCILbits.SWTERM = 1;
//    PG1STATbits.FLTEVT = 0;
//    PG2FPCILbits.SWTERM = 1;
//    PG2STATbits.FLTEVT = 0;
//  }    

}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

PWR_CTRL_FAULTS Drv_PwrCtrl_4SWBB_GetFaults(void)
{
	return FourSWBBFaults;
}

//====================================================================================================================== 
//======================================================================================================================


