
#include "fault_common.h"

#include <stdbool.h>
#include <stddef.h>

/*******************************************************************************
 * Function:       FAULT_Init
 * Inputs:         Pointer to faultStructure, 
 * 
 *  
 * Description:	   
 * 
 * 
 *******************************************************************************/

void FAULT_Init(FAULT_OBJ_T *faultInput,
                int16_t  threshold, int16_t  hysLimit,
                uint16_t thresholdCnt, uint32_t hysCnt)
{
  faultInput->val1_Threshold = threshold;
  faultInput->val1_Hysteresis = hysLimit;
  faultInput->CounterSet = thresholdCnt;
  faultInput->CounterReset = hysCnt;
  faultInput->fltCode = 0;
  faultInput->faultActive = 0;
}

void FAULT_InitRange(FAULT_OBJ_T *faultInput,
                int16_t  thresholdMin, int16_t  hysLimitMin,
                int16_t  thresholdMax, int16_t  hysLimitMax,
                uint16_t thresholdCnt, uint32_t hysCnt)
{
  faultInput->val1_Threshold = thresholdMin;
  faultInput->val1_Hysteresis = hysLimitMin;
  faultInput->val2_Threshold = thresholdMax;
  faultInput->val2_Hysteresis = hysLimitMax;
  faultInput->CounterSet = thresholdCnt;
  faultInput->CounterReset = hysCnt;
  faultInput->fltCode = 0;
  faultInput->faultActive = 0;
}


/*******************************************************************************
 * Function:       FAULT_SetCounters
 * Inputs:         Pointer to faultStructure, 
 * 
 *  
 * Description:	   
 * 
 * 
 *******************************************************************************/
void FAULT_SetCounters(FAULT_OBJ_T *faultInput, uint16_t fltThresholdCnt, uint32_t fltHysCnt)
{
   faultInput->CounterSet = fltThresholdCnt;
   faultInput->CounterReset = fltHysCnt;
}

/*******************************************************************************
 * Function:       FAULT_SetThresholds
 * Inputs:         Pointer to faultStructure, 
 * 
 *  
 * Description:	   
 * 
 * 
 *******************************************************************************/
void FAULT_SetThresholds(FAULT_OBJ_T *faultInput, int16_t fltThreshold, int16_t fltHysLimit)
{
   faultInput->val1_Threshold = fltThreshold;
   faultInput->val1_Hysteresis = fltHysLimit;
}

/*******************************************************************************
 * Function:       FAULT_CheckMax
 * Inputs:         Pointer to faultStructure, input fault source, pointer to 
 *                 variable that captures fault code, and callback function pointer 
 * Output:         Returns true if fault active else false  
 * Description:	   Checks if input fault source is greater than set max threshold,
 *                 sets fltState which captures event and will remove fault once
 *                 input source goes below Hysteresis limit (if set). Function does 
 *                 not clear fltState. If callback pointer non null, then after
 *                 fault is set, routine will call the callback function allowing
 *                 quick shutdown response if needed.
 *******************************************************************************/

bool FAULT_CheckMax(FAULT_OBJ_T *faultInput, int16_t faultSource, FAULT_CALLBACK callback)
{
  if ((faultSource >= faultInput->val1_Threshold) && (faultInput->faultActive == false))
  {
    faultInput->fltCounter++;

    if (faultInput->fltCounter > faultInput->CounterSet)
    {
      if (callback != NULL)
      {
        callback(faultInput); // Call Back function used to quickly shutdown system if needed when fault detected
      }

      faultInput->faultActive = true;
      faultInput->fltCounter = 0;
    }
  } 
  else if ((faultSource < (faultInput->val1_Hysteresis)) && (faultInput->faultActive == true))
  {
    faultInput->fltCounter++;
    if (faultInput->fltCounter > faultInput->CounterReset)
    {
      faultInput->fltCounter = 0;
      faultInput->faultActive = false;
    }
  }
  else
  {
    faultInput->fltCounter = 0;
  }

  return (faultInput->faultActive);
}

/*******************************************************************************
 * Function:       FAULT_CheckMin
 * Inputs:         Pointer to faultStructure, input fault source, pointer to 
 *                 variable that captures fault code, and callback function pointer  
 * Output:         Returns true if fault active else false  
 * Description:    Checks if input fault source is less than set threshold,
 *                 sets fltState which captures event and will remove fault once
 *                 input source goes above Hysteresis limit (if set). Function does 
 *                 not clear fltState. If callback pointer non null, then after
 *                 fault is set, routine will call the callback function allowing
 *                 quick shutdown response if needed. 
 *******************************************************************************/

bool FAULT_CheckMin(FAULT_OBJ_T *faultInput, int16_t faultSource, FAULT_CALLBACK callback)
{
  if ((faultSource <= faultInput->val1_Threshold) && (faultInput->faultActive == false))
  {
    faultInput->fltCounter++;
    if (faultInput->fltCounter > faultInput->CounterSet)
    {
      if (callback != NULL)
      {
        callback(faultInput); // Call Back function used to quickly shutdown system if needed when fault detected
      }

      faultInput->faultActive = true;
      faultInput->fltCounter = 0;
    }
  }
  else if ((faultSource > (faultInput->val1_Hysteresis)) && (faultInput->faultActive == true))
  {
    faultInput->fltCounter++;
    if (faultInput->fltCounter > faultInput->CounterReset)
    {
      faultInput->fltCounter = 0;
      faultInput->faultActive = false;
    }
  }
  else
  {
    faultInput->fltCounter = 0;
  }

  return (faultInput->faultActive);
}


/*******************************************************************************
 * Function:       FAULT_CheckBit
 * Inputs:         Pointer to faultStructure, input fault source, pointer to 
 *                 variable that captures fault code, and callback function pointer 
 * Output:         Returns true if fault active else false  
 * Description:	   Checks if input fault bit is 1,
 *                 sets fltState which captures event and will remove fault once
 *                 input fault bit gets 0. Function does 
 *                 not clear fltState. If callback pointer non null, then after
 *                 fault is set, routine will call the callback function allowing
 *                 quick shutdown response if needed.
 *******************************************************************************/

bool FAULT_CheckBit(FAULT_OBJ_T *faultInput, bool faultBit, FAULT_CALLBACK callback)
{
  if ((faultBit) && (faultInput->faultActive == false))
  {
    faultInput->fltCounter++;

    if (faultInput->fltCounter > faultInput->CounterSet)
    {
      if (callback != NULL)
      {
        callback(faultInput); // Call Back function used to quickly shutdown system if needed when fault detected
      }

      faultInput->faultActive = true;
      faultInput->fltCounter = 0;
    }
  }
  else if ((!faultBit) && (faultInput->faultActive == true))
  {
    faultInput->fltCounter++;
    if (faultInput->fltCounter > faultInput->CounterReset)
    {
      faultInput->fltCounter = 0;
      faultInput->faultActive = false;
    }
  } else
  {
    faultInput->fltCounter = 0;
  }

  return (faultInput->faultActive);
}

/*******************************************************************************
 * Function:       FAULT_CheckRange
 * Inputs:         Pointer to faultStructure, input fault source, pointer to 
 *                 variable that captures fault code, and callback function pointer  
 * Output:         Returns true if fault active else false  
 * Description:    Checks if input fault source is less than set threshold,
 *                 sets fltState which captures event and will remove fault once
 *                 input source goes above Hysteresis limit (if set). Function does 
 *                 not clear fltState. If callback pointer non null, then after
 *                 fault is set, routine will call the callback function allowing
 *                 quick shutdown response if needed. 
 *******************************************************************************/

bool FAULT_CheckRange(FAULT_OBJ_T *faultInput, int16_t inputValue, FAULT_CALLBACK callback)
{
    if (faultInput->faultActive == false)
    {
        if ((inputValue <= faultInput->val1_Threshold) ||
            (inputValue >= faultInput->val2_Threshold))
        {
            if (++faultInput->fltCounter >= faultInput->CounterSet)
            {
                if (callback != NULL)
                {
                    callback(faultInput); // Call Back function used to quickly shutdown system if needed when fault detected
                }
                faultInput->faultActive = true;
                faultInput->fltCounter = 0;
            }
        }
        else
            faultInput->fltCounter = 0;
    }
    else
    {
        if ((inputValue > faultInput->val1_Hysteresis) &&
            (inputValue < faultInput->val2_Hysteresis))
        {
            if (++faultInput->fltCounter > faultInput->CounterReset)
            {
                faultInput->fltCounter = 0;
                faultInput->faultActive = false;
            }
        }
        else
            faultInput->fltCounter = 0;
    }
    return (faultInput->faultActive);
}
