/*******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description: Include file for application
 *
 * Author: Thonv
 *
 * Last Changed By:  $Author: thonv $
 * Revision:         $Revision: 1.0.0.0 $
 * Last Changed:     $Date: 2016-05-20 10:20:00 (Fri, 20 May 2016) $
 *

 ******************************************************************************/

#include <AppEvent.h>
#include "app/framework/include/af.h"
#include "ZigbeeCoorMG1P.h"
#include "hal/micro/bootloader-interface-standalone.h"

#include "typedefs.h"
#include "macro.h"
#include "Uart_CmdExec.h"
#include "Uart_Transmitter.h"
#include "Uart_Receiver.h"
#include "halDelay.h"


u8_t UartRxDataStep = 0;
u8_t UartRxPacketLength;
u8_t UartRxPacketCheckXor = 0;
u8_t UartRxCommandData[128] = { 0 };
u8_t UartRxCurrentLength = 0;


void UartProcess(u8_t *UartPacketCommand) {
    u8_t seqNumber = UartPacketCommand[0];
    u8_t UartPacketID = UartPacketCommand[1];

    u8_t ErrorCode = UartCmdNormal;
    switch (UartPacketID) {
    case HC_UartConectionError:
        ErrorCode = HC_UartConectionError_Task(UartPacketCommand);
        break;
    case HC_FormNetwork:
        ErrorCode = HC_FormNetwork_Task(UartPacketCommand);
        break;
    case HC_GetNetworkInfo:
        ErrorCode = HC_GetNetworkInfo_Task();
        break;
    case HC_NetworkPjoin:
        ErrorCode = HC_NetworkPjoin_Task(UartPacketCommand);
        break;
    case HC_PjoinStatus:
        ErrorCode = HC_PjoinStatus_Task();
        break;
    case HC_ZclClusterCmdRequest:
        ErrorCode = HC_ZclClusterCmdRequest_Task(UartPacketCommand);
        break;
    case HC_ZclGlobalCmdRequest:
        ErrorCode = HC_ZclGlobalCmdRequest_Task(UartPacketCommand);
        break;
    case HC_ZdoCmdRequest:
        ErrorCode = HC_ZdoCmdRequest_Task(UartPacketCommand);
        break;
    case HC_ResetDevice:
        ErrorCode = HC_ResetDevice_Task(UartPacketCommand);
        break;
    case HC_GetDeviceMCVersion:
        ErrorCode = HC_GetDeviceMCVersion_Task(UartPacketCommand);
        break;
    case HC_GotoUpdateFirmware:
        ErrorCode = HC_GotoUpdateFirmware_Task();
        break;
    default:
        ErrorCode = CmdIdNotAvaiable;
        break;
    }
    if (ErrorCode != UartCmdNormal) {
#ifdef Debug
        emberSerialPrintf(APP_SERIAL,"		Uart Error Code: %u \n\r", ErrorCode);
        emberSerialPrintf(APP_SERIAL,"		Uart Error seqNumber: %u \n\r", seqNumber);
#endif //Debug
//        halToggleLed(StatusErrorLed);
//        __delay_ms(1);
//        halToggleLed(StatusErrorLed);
    }
    else{
//        halToggleLed(StatusNormalLed);
//        __delay_ms(1);
//        halToggleLed(StatusNormalLed);
    }
}

void UartGetCommand(void) {
    int16u NumberOfByteReCeiver;
    int8u ReadSerialData[512];
    int8u ReadStatus;

    NumberOfByteReCeiver = emberSerialReadAvailable(HC_SERIAL);
    if (NumberOfByteReCeiver > 0) {
        ReadStatus = emberSerialReadData(HC_SERIAL,
                        ReadSerialData,
                        NumberOfByteReCeiver,
                        NULL);

//        emberSerialWriteData(APP_SERIAL,(int8u*)&ReadSerialData,NumberOfByteReCeiver) ;
//          COM_Printf(1,"hello %d",10);
//        for ( int8u i=0;i<NumberOfByteReCeiver; i++){
//          emberSerialPrintf(APP_SERIAL,"%X ",ReadSerialData[i]); 
//        }
           
        if (ReadStatus == EMBER_SUCCESS) {
            for(int8u i = 0; i<NumberOfByteReCeiver; i++){
                switch (UartRxDataStep) {
                case 0:
                    if (ReadSerialData[i] == 0x4C) {
                        UartRxDataStep = 1;
                    }
                    else {
                        UartRxDataStep = 0;
                    }
                    break;
                case 1:
                    if (ReadSerialData[i] == 0x4D) {
                        UartRxDataStep = 2;
                    }
                    else {
                        UartRxDataStep = 0;
                    }
                    break;
                case 2:
                    UartRxPacketLength = ReadSerialData[i];
                    if (UartRxPacketLength > 40) {
                        UartRxDataStep = 0;
                    }
                    else {
                        UartRxDataStep = 3;
                    }
                    break;
                case 3:
                    if (UartRxCurrentLength < UartRxPacketLength - 1) {
                        UartRxCommandData[UartRxCurrentLength] = ReadSerialData[i];
                        UartRxCurrentLength++;
                    }
                    else {
                        u8_t PacketCheckXor;
                        u8_t CheckXor = 0;
                        PacketCheckXor = ReadSerialData[i];
                        {
                            for (u8_t j = 1; j < UartRxPacketLength - 1; j++) {
                                CheckXor = CheckXor ^ UartRxCommandData[j];
                            }
                        }
                        if (PacketCheckXor == CheckXor) {
                            UartProcess(UartRxCommandData);
                            emberEventControlSetActive(ToggleLedStatusEventControl);
                        }
                        else {
                            u8_t ErrorCode = XorError;
                            u8_t seqNumber = UartRxCommandData[0];
#ifdef Debug
                            emberSerialPrintf(APP_SERIAL,"    Length ofPacket data :  %u \n\r",UartRxPacketLength);
                            emberSerialPrintf(APP_SERIAL,"		Packet data :"); 
                            for(u8_t i=0; i< UartRxCurrentLength - 1; i++){
                               emberSerialPrintf(APP_SERIAL,"%X ",UartRxCommandData[i]); 
                            }
                            emberSerialPrintf(APP_SERIAL,"		\n\r"); 
                            emberSerialPrintf(APP_SERIAL,"		Uart Error Code: %u \n\r", ErrorCode);
                            emberSerialPrintf(APP_SERIAL,"		Uart Error seqNumber: %u \n\r", seqNumber);
#endif // Debug
                        }

                        UartRxCurrentLength = 0;
                        UartRxDataStep = 0;
                    }
                    break;
                default:
                    break;
                }
            }
        }
    }
}
