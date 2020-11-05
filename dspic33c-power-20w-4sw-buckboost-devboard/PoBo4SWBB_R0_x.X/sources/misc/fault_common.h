#ifndef FAULT_H
#define FAULT_H

//#include <xc.h>

#include <stdint.h>
#include <stdbool.h>

typedef struct
{
   int16_t  val1_Threshold;     // Fault threshold limit: Range of -32768 to 32767
   int16_t  val1_Hysteresis;    // Fault hysteresis limit. If hysteresis is not needed, fltThreshold = fltHysLimit
   int16_t  val2_Threshold;     // Fault threshold limit: Range of -32768 to 32767
   int16_t  val2_Hysteresis;    // Fault hysteresis limit. If hysteresis is not needed, fltThreshold = fltHysLimit
   uint16_t  CounterSet;        // Number of consecutive fault events before fault becomes active.  
   uint32_t  CounterReset;      // Number of consecutive events when input outside hysteresis limit in order to remove fault, set to 0 to disable Hys 
   uint16_t  fltCounter;        // Internal counter for activating/removing fault 
   uint16_t  fltCode;           // Code that can be used to display fault (1st fault occurred) to global variable
   uint8_t   faultActive;       // Set/Cleared inside flt check loop        
}FAULT_OBJ_T;   

typedef void(*FAULT_CALLBACK)(FAULT_OBJ_T* faultInput);

void FAULT_Init(FAULT_OBJ_T *faultInput,
                int16_t  threshold, int16_t  hysLimit,
                uint16_t thresholdCnt, uint32_t hysCnt);

void FAULT_InitRange(FAULT_OBJ_T *faultInput,
                int16_t  thresholdMin, int16_t  hysLimitMin,
                int16_t  thresholdMax, int16_t  hysLimitMax,
                uint16_t thresholdCnt, uint32_t hysCnt);

void FAULT_SetCounters(FAULT_OBJ_T *faultInput, uint16_t fltThresholdCnt, uint32_t fltHysCnt);
void FAULT_SetThresholds(FAULT_OBJ_T *faultInput, int16_t fltThreshold, int16_t fltHysLimit);
bool FAULT_CheckMax(FAULT_OBJ_T *faultInput, int16_t faultSource, FAULT_CALLBACK callback);
bool FAULT_CheckMin(FAULT_OBJ_T *faultInput, int16_t faultSource, FAULT_CALLBACK callback);
bool FAULT_CheckBit(FAULT_OBJ_T *faultInput, bool faultBit, FAULT_CALLBACK callback);
bool FAULT_CheckRange(FAULT_OBJ_T *faultInput, int16_t inputValue, FAULT_CALLBACK callback);

#endif 

