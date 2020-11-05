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

#include "xc.h"
#include "dev_uart_comm.h"
#include "../../mcc_generated_files/uart1.h"
#include "../../mcc_generated_files/pin_manager.h"
#include "driver/power_controller/drv_pwrctrl_4SWBB.h"
#include "driver/drv_adc.h"
#include "driver/power_controller/drv_pwrctrl_4SWBB_fault.h"
#include "driver/power_controller/Controller_pwrctrl_4SWBB.h"
#include "../app/App_HMI.h"
#include "driver/power_controller/drv_pwrctrl_4SWBB.h"

UART_T UART_RxTx;

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void UARTsend(void)
{
    uint8_t i_;

    if ((UART_RxTx.UartSendCounter >= 0) && (UART_RxTx.UartSendCounter < UART_RxTx.UartSendLength + 5u))
    { 
        //50us @ 16MHz
        UART_RxTx.SendCRC ^= (uint16_t)UART_RxTx.TXBytes[UART_RxTx.UartSendCounter];
        for (i_ = 0; i_ < 8; ++i_)
        {
            if ((UART_RxTx.SendCRC & 1) == 1)
                UART_RxTx.SendCRC = (UART_RxTx.SendCRC >> 1) ^ 0xA001;
            else
                UART_RxTx.SendCRC = (UART_RxTx.SendCRC >> 1);
        }
    }
    if((UART_RxTx.UartSendCounter == UART_RxTx.UartSendLength + 5u))
    {  
        UART_RxTx.TXBytes[UART_RxTx.UartSendLength + 5u] = (uint8_t)(UART_RxTx.SendCRC >> 8);
        UART_RxTx.TXBytes[UART_RxTx.UartSendLength + 6u] = (uint8_t)(UART_RxTx.SendCRC & 0x00FF);
    }

    UART1_Write(UART_RxTx.TXBytes[UART_RxTx.UartSendCounter]);

    UART_RxTx.UartSendCounter++;

    if(UART_RxTx.UartSendCounter > (UART_RxTx.UartSendLength + 7u))
    {
        UART_RxTx.UartSendCounter = 0;
        UART_RxTx.SendCRC = 0;
        UART_RxTx.TXSendDone = 1;
    }
} 

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

uint8_t messageindex = 0;

void Message_AddWord(uint16_t data)
{
    UART_RxTx.TXBytes[messageindex++] = data >> 8;
    UART_RxTx.TXBytes[messageindex++] = data & 0x00FF;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void UART_load_for_send(void)
{
    if (UART_RxTx.TXSendDone)
    {
        messageindex = 5;
        //Message pos 0
        Message_AddWord(pwr_ctrl_adc_data.drv_adc_val_FB_Vdd);

        //Message pos 1
        Message_AddWord(pwr_ctrl_adc_data.drv_adc_val_FB_Vout);

        //Message pos 2
        Message_AddWord(pwr_ctrl_adc_data.drv_adc_val_FB_Iout);

        //Message pos 3
        Message_AddWord(pwr_ctrl_adc_data.drv_adc_val_FB_Iin);

        //Message pos 4
        Message_AddWord(pwr_ctrl_adc_data.drv_adc_val_AN15);

        //Message pos 5
        Message_AddWord(pwr_ctrl_adc_data.drv_adc_val_FB_Vaux);

        //Message pos 6
        Message_AddWord(pwr_ctrl_state);

        //Message pos 7
        Message_AddWord(FourSWBBFaults.CollectiveFaults);

        //Message pos 8
        if(App_HMI_useRefFromPoti) pwr_ctrl_flagbits.CollectivePwrCtrlFlagbits |= 0x4000;
        else pwr_ctrl_flagbits.CollectivePwrCtrlFlagbits &= 0xBFFF;        
        Message_AddWord(pwr_ctrl_flagbits.CollectivePwrCtrlFlagbits);

        //Message pos 9
        Message_AddWord(Controller_4SWBB._DCBuckset);
        //    UART_RxTx.TXBytes[23] = (uint8_t) (PG1DC >> 8);
        //    UART_RxTx.TXBytes[24] = (uint8_t) (PG1DC & 0x00FF);
        //    UART_RxTx.TXBytes[23] = (uint8_t) (Controller_4SWBB._CurrentControllerResult >> 8);
        //    UART_RxTx.TXBytes[24] = (uint8_t) (Controller_4SWBB._CurrentControllerResult & 0x00FF);

        //Message pos 10
        Message_AddWord(Controller_4SWBB._DCBoostset);

        //Message pos 11
        //    UART_RxTx.TXBytes[27] = (uint8_t) (Controller_4SWBB._IACref >> 8);
        //    UART_RxTx.TXBytes[28] = (uint8_t) (Controller_4SWBB._IACref & 0x00FF);
        Message_AddWord(Controller_4SWBB._CurrentControllerResult);

        UART_RxTx.TXSendDone = 0;
    }
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void UARTTxInit(void)
{
    uint8_t i_;

    UART_RxTx.TXBytes[0] = 0x55;
    UART_RxTx.TXBytes[1] = 0x00;
    UART_RxTx.TXBytes[2] = 0x01;

    UART_RxTx.UartSendLength = 24;  //0x40
    UART_RxTx.TXBytes[3] = 0;
    UART_RxTx.TXBytes[4] = UART_RxTx.UartSendLength;

    for (i_ = 5; i_ < (UART_RxTx.UartSendLength + 5u); i_++)
    {
        UART_RxTx.TXBytes[i_] = i_;
    }
    UART_RxTx.SendCRC = 0;

    UART_RxTx.TXBytes[UART_RxTx.UartSendLength + 5u] = 0x00;
    UART_RxTx.TXBytes[UART_RxTx.UartSendLength + 6u] = 0x00;
    UART_RxTx.TXBytes[UART_RxTx.UartSendLength + 7u] = 0x0D;
}

//------------------------------------------------------------------------------
//
//------------------------------------------------------------------------------

void UARTreceive(void)
{
    uint16_t i_, j_, CRC_;

    UART_RxTx.ReceiveCRC = 0;
    for (j_ = 0; j_ < (UART_RxTx.UartRecLength + 5); j_++)
    {
        UART_RxTx.ReceiveCRC ^= UART_RxTx.RXBytes[j_];
        for (i_ = 0; i_ < 8; ++i_)
        {
            if ((UART_RxTx.ReceiveCRC & 1) == 1)
                UART_RxTx.ReceiveCRC = (UART_RxTx.ReceiveCRC >> 1) ^ 0xA001;
            else
                UART_RxTx.ReceiveCRC = (UART_RxTx.ReceiveCRC >> 1);
        }
    }
    CRC_ = (UART_RxTx.RXBytes[UART_RxTx.UartRecLength+5] << 8) | \
            UART_RxTx.RXBytes[UART_RxTx.UartRecLength+6];
  
    if (UART_RxTx.ReceiveCRC == CRC_)
    {
        if ((UART_RxTx.UartRecActionID == 0x40EF) && (UART_RxTx.UartRecLength == 4))
        {
            //pwr_ctrl_adc_data.drv_comm_val_VoutRef = ((UART_RxTx.RXBytes[5] & 0x7F) << 8) + UART_RxTx.RXBytes[6];
            uint16_t Gui_VoutRef = ((UART_RxTx.RXBytes[5] & 0x7F) << 8) + UART_RxTx.RXBytes[6];
            if((UART_RxTx.RXBytes[5] & 0x80) > 0)
            {  
                Drv_PwrCtrl_4SWBB_SetReferenceRaw(Gui_VoutRef);  // raw adc vaule
                App_HMI_useRefFromPoti = false;
            }  
            if((UART_RxTx.RXBytes[5] & 0x80) == 0)
            {  
                App_HMI_useRefFromPoti = true;
            }
        }
        UART_RxTx.UartRecActionID = 0;  
    }
}
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
