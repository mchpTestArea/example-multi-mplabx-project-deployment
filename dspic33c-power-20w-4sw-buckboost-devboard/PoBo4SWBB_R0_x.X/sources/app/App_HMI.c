//======================================================================================================================
// Copyright(c) 2018 Microchip Technology Inc. and its subsidiaries.
// Subject to your compliance with these terms, you may use Microchip software and any derivatives exclusively with
// Microchip products. It is your responsibility to comply with third party license terms applicable to your use of
// third-party software (including open source software) that may accompany Microchip software.
// THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER EXPRESS, IMPLIED OR STATUTORY, APPLY TO
// THIS SOFTWARE, INCLUDING ANY IMPLIED WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A PARTICULAR
// PURPOSE.
// IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE, INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE,
// COST OR EXPENSE OF ANY KIND WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS BEEN ADVISED
// OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY
// ON ALL CLAIMS IN ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY, THAT YOU HAVE
// PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.
//======================================================================================================================

//======================================================================================================================
// @file App_HMI.c
//
// @brief contains the HMI application
//
// @note this application handles the inputs from the buttons and controls the 4SWBB and LEDs
//
//======================================================================================================================

#include <xc.h>
#include <stdbool.h>
#include "device/dev_button.h"
#include "driver/power_controller/drv_pwrctrl_4SWBB.h"
#include "driver/power_controller/drv_pwrctrl_4SWBB_fault.h"
#include "../mcc_generated_files/pin_manager.h"
#include "driver/drv_led.h"
#include"driver/drv_adc.h"

//local functions
void CheckButtons(void);
void SetLEDs_100ms(void);

extern uint16_t os_resetCause;  //@sw??

bool App_HMI_useRefFromPoti = true;
//======================================================================================================================
// @brief initializes the HMI application
//======================================================================================================================

void App_HMI_Init(void)
{
    Drv_LED_Blink(LED_PIM_RED);
}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void CheckButtons(void)
{
    uint8_t buttonresponse = Dev_Button_GetEvent();

    if (buttonresponse == DEV_BUTTON_EVENT_PRESSED_SHORT)
    {
        if (pwr_ctrl_flagbits.run)
        {
            Drv_PwrCtrl_4SWBB_Stop();
            Drv_LED_Off(LED_BOARD_GREEN);
        }
        else
        {
            Drv_PwrCtrl_4SWBB_Start();
            Drv_LED_On(LED_BOARD_GREEN);
        }
    } 
    else if (buttonresponse == DEV_BUTTON_EVENT_PRESSED_LONG)
    {
        if (pwr_ctrl_flagbits.inopenloop)
        {  
            Drv_PwrCtrl_4SWBB_SetMode_ClosedLoop();
        }  
        else
        {  
            Drv_PwrCtrl_4SWBB_SetMode_OpenLoop();
        }
    }
}

//======================================================================================================================
// @brief this function contains 
//======================================================================================================================

void SetLEDs_100ms()
{
    //state < running --> toggle grenn LED on power board
    if (!pwr_ctrl_flagbits.run)
    {
        Drv_LED_Blink(LED_BOARD_GREEN);
    }

    //fault fheck
    PWR_CTRL_FAULTS faults = Drv_PwrCtrl_4SWBB_GetFaults(); //@ftx

    //any fault --> fast toggle red LED on power board
    if (os_resetCause & 0x01 || faults.CollectiveFaults != 0)
    {
        Drv_LED_On(LED_BOARD_RED);
    } 
    else if (pwr_ctrl_flagbits.inopenloop)
    {
        Drv_LED_Blink(LED_BOARD_RED);
    }
    else
    {
        //LED_Red_SetLow();
        Drv_LED_Off(LED_BOARD_RED);
    }
}

//======================================================================================================================
// @brief this function contains the HMI application that handles the LED outputs
//======================================================================================================================

void App_HMI_Task_100ms(void)
{
    CheckButtons(); 
    	//Open loop test:
    if (App_HMI_useRefFromPoti)
        Drv_PwrCtrl_4SWBB_SetReferenceRaw(pwr_ctrl_adc_data.drv_adc_val_AN15);  //@ftx mention in users guide that ref input option

    SetLEDs_100ms();
}

//======================================================================================================================
//======================================================================================================================
