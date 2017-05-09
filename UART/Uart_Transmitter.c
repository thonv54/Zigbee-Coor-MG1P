

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


//-----------------------------Khai bao thu vien---------------------//

#include BOARD_HEADER
#include <AppEvent.h>
#include "app/framework/include/af.h"
#include "Uart_CmdEnum.h"
#include "halDelay.h"
#include "macro.h"
#include "typedefs.h"
#include "macro.h"
#include "Uart_Transmitter.h"


//-----------------------------Khai bao bien-------------------------//


int8u UART_PACKET_KEY[2]	=	{0x4C,0x4D};
int8u TxSeqNumber           =   0;




//---------------------------------UART Send Command--------------------------//

/**
 * @func   NwkFormEventFunction
 *
 * @brief  
 *
 * @param  None
 *
 * @retval None
 */

void UartSendUartError(int8u ErrorCode, int8u seqNumber){
    Zbs_UartErrorDataStr UartErrorData;

    UartErrorData.Length =  5   ;
    UartErrorData.TxSeqNumber = TxSeqNumber;
    TxSeqNumber ++;
    UartErrorData.CmdId = Zbs_UartError;
    UartErrorData.ErrorCode = ErrorCode;
    UartErrorData.seqNumber = seqNumber;
    UartErrorData.CheckXor = xorStr((int8u *) &UartErrorData.CmdId,UartErrorData.Length - 2);
		
    emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
		emberSerialWriteData(HC_SERIAL,(int8u*)&UartErrorData,UartErrorData.Length);
    emberEventControlSetActive(ToggleLedErrorEventControl);
    
}
/**
 * @func   NwkFormEventFunction
 *
 * @brief  
 *
 * @param  None
 *
 * @retval None
 */
void UartSendNwkInfo(int8u ExtendedPanId[8], int16u PanId, int8u Channel){
    Zbs_NwkInfoDataStr NwkInfoData;
    int16u val;
    NwkInfoData.Length = 14;
    NwkInfoData.TxSeqNumber = TxSeqNumber;  

    TxSeqNumber ++;
    NwkInfoData.CmdId = Zbs_NwkInfo;
    memcpy(NwkInfoData.ExtendedPanId, ExtendedPanId, 8);
    val = SwapEndiannessInt16u(PanId);
    memcpy(NwkInfoData.PanId, (int8u*)&val, 2);
    
    NwkInfoData.Channel = Channel;
    NwkInfoData.CheckXor = xorStr((int8u *) &NwkInfoData.CmdId,(NwkInfoData.Length -2));
    emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
		emberSerialWriteData(HC_SERIAL,(int8u*)&NwkInfoData,NwkInfoData.Length+1);
    emberEventControlSetActive(ToggleLedErrorEventControl);
    
}
/**
 * @func   NwkFormEventFunction
 *
 * @brief  
 *
 * @param  None
 *
 * @retval None
 */
void UartSendPjoinStatus(int8u State){
    
    Zbs_PjoinStatusDataStr PjoinStatusData;
    PjoinStatusData.Length = 4;
    PjoinStatusData.TxSeqNumber = TxSeqNumber;
    TxSeqNumber ++;
    PjoinStatusData.CmdId = Zbs_PjoinStatus;
    PjoinStatusData.State = State;
    PjoinStatusData.CheckXor = xorStr((int8u *) &PjoinStatusData.CmdId,(PjoinStatusData.Length -2));
    
    emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
		emberSerialWriteData(HC_SERIAL,(int8u*)&PjoinStatusData,PjoinStatusData.Length+1); 

    emberEventControlSetActive(ToggleLedErrorEventControl);
}




/**
 * @func   NwkFormEventFunction
 *
 * @brief  
 *
 * @param  None
 *
 * @retval None
 */
void UartSendZclClusterCmdResponse(int16u NwkAddr,
                                   int8u Endpoint,
                                   int16u ClusterId,
                                   int8u CmdPayloadLength,
                                   int8u *CmdPayload){
                                       
    Zbs_ZclClusterCmdResponseStr ZclClusterCmdResponseData;
    int16u val;
    ZclClusterCmdResponseData.Length = 9 + CmdPayloadLength;
    ZclClusterCmdResponseData.TxSeqNumber = TxSeqNumber;
    TxSeqNumber ++;
    ZclClusterCmdResponseData.CmdId = Zbs_ZclClusterCmdResponse;
    val = SwapEndiannessInt16u(NwkAddr);
    memcpy(ZclClusterCmdResponseData.NwkAddr, (int8u*)&val, 2);
    ZclClusterCmdResponseData.Endpoint = Endpoint;
    val = SwapEndiannessInt16u(ClusterId);
    memcpy(ZclClusterCmdResponseData.ClusterId, (int8u*)&val, 2);
    ZclClusterCmdResponseData.CmdPayloadLength = CmdPayloadLength;
    ZclClusterCmdResponseData.CmdPayload = CmdPayload;
    ZclClusterCmdResponseData.CheckXor = 
        xorStr((int8u *) &ZclClusterCmdResponseData.CmdId,7) ^
        xorStr(ZclClusterCmdResponseData.CmdPayload,ZclClusterCmdResponseData.CmdPayloadLength);

    emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
		emberSerialWriteData(HC_SERIAL,(int8u*)&ZclClusterCmdResponseData,9); 
		emberSerialWriteData(HC_SERIAL,
                         ZclClusterCmdResponseData.CmdPayload,
                         ZclClusterCmdResponseData.CmdPayloadLength); 
		emberSerialWriteData(HC_SERIAL,(int8u*)&ZclClusterCmdResponseData.CheckXor,1); 
    
    emberEventControlSetActive(ToggleLedErrorEventControl);
}
/**
 * @func   NwkFormEventFunction
 *
 * @brief  
 *
 * @param  None
 *
 * @retval None
 */
void UartSendZclGlobalCmdResponse(int16u NwkAddr,
                                   int8u Endpoint,
                                   int16u ClusterId,
                                   int8u GeneralCmd,
                                   int8u CmdPayloadLength,
                                   int8u *CmdPayload){
    Zbs_ZclGlobalCmdResponseStr ZclGlobalCmdResponseData;
    int16u val;

    ZclGlobalCmdResponseData.Length = 10 + CmdPayloadLength;
    ZclGlobalCmdResponseData.TxSeqNumber = TxSeqNumber;
    TxSeqNumber ++;
    ZclGlobalCmdResponseData.CmdId = Zbs_ZclGlobalCmdResponse;    
    val = SwapEndiannessInt16u(NwkAddr);
    memcpy(ZclGlobalCmdResponseData.NwkAddr, (int8u*)&val, 2);  
    ZclGlobalCmdResponseData.Endpoint = Endpoint;
    val = SwapEndiannessInt16u(ClusterId);
    memcpy(ZclGlobalCmdResponseData.ClusterId, (int8u*)&val, 2);
    ZclGlobalCmdResponseData.GeneralCmd = GeneralCmd;
    ZclGlobalCmdResponseData.CmdPayloadLength = CmdPayloadLength;
    ZclGlobalCmdResponseData.CmdPayload = CmdPayload;
    ZclGlobalCmdResponseData.CheckXor = 
        xorStr((int8u *) &ZclGlobalCmdResponseData.CmdId,8) ^
        xorStr(ZclGlobalCmdResponseData.CmdPayload, ZclGlobalCmdResponseData.CmdPayloadLength);

    emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
		emberSerialWriteData(HC_SERIAL,(int8u*)&ZclGlobalCmdResponseData,10); 
		emberSerialWriteData(HC_SERIAL,
                         ZclGlobalCmdResponseData.CmdPayload,
                         ZclGlobalCmdResponseData.CmdPayloadLength); 
		emberSerialWriteData(HC_SERIAL,(int8u*)&ZclGlobalCmdResponseData.CheckXor,1); 
    
    emberEventControlSetActive(ToggleLedErrorEventControl);
}

/**
 * @func   UartSendZdoCmdResponse
 *
 * @brief  
 *
 * @param  None
 *
 * @retval None
 */
void UartSendZdoCmdResponse(int16u NwkAddr,
                            int16u ZdoCmd,
                            int8u CmdPayloadLength,
                            int8u *CmdPayload){
    Zbs_ZdoCmdResponseStr ZdoCmdResponseData;
    int16u val;

    ZdoCmdResponseData.Length = 8 + CmdPayloadLength;
    ZdoCmdResponseData.TxSeqNumber = TxSeqNumber;
    TxSeqNumber ++;
    ZdoCmdResponseData.CmdId = Zbs_ZdoCmdResponse;
    val = SwapEndiannessInt16u(NwkAddr);
    memcpy(ZdoCmdResponseData.NwkAddr, (int8u*)&val, 2);
    val = SwapEndiannessInt16u(ZdoCmd);
    memcpy(ZdoCmdResponseData.ZdoCmd, (int8u*)&val, 2);
    ZdoCmdResponseData.CmdPayloadLength = CmdPayloadLength;
    ZdoCmdResponseData.CmdPayload = CmdPayload;
    ZdoCmdResponseData.CheckXor = 
        xorStr((int8u *) &ZdoCmdResponseData.CmdId,6) ^
        xorStr(ZdoCmdResponseData.CmdPayload,ZdoCmdResponseData.CmdPayloadLength);
        emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
        
		emberSerialWriteData(HC_SERIAL,(int8u*)&ZdoCmdResponseData,8); 
		emberSerialWriteData(HC_SERIAL,
                         ZdoCmdResponseData.CmdPayload,
                         ZdoCmdResponseData.CmdPayloadLength); 
		emberSerialWriteData(HC_SERIAL,(int8u*)&ZdoCmdResponseData.CheckXor,1); 
    emberEventControlSetActive(ToggleLedErrorEventControl);
}

void UartSendConfirmUpdateFirmware(void){
    Zbs_ConfirmUpdateFirmwareStr ConfirmUpdateFirmwareData;

    ConfirmUpdateFirmwareData.Length =  4;
    ConfirmUpdateFirmwareData.TxSeqNumber = TxSeqNumber;
    TxSeqNumber ++;
    ConfirmUpdateFirmwareData.CmdId = Zbs_ConfirmUpdateFirmware;
    ConfirmUpdateFirmwareData.Status = 0x00;
    ConfirmUpdateFirmwareData.CheckXor = xorStr((int8u *) &ConfirmUpdateFirmwareData.CmdId,(ConfirmUpdateFirmwareData.Length -2));
		
    emberSerialWriteData(HC_SERIAL,UART_PACKET_KEY,2);
		emberSerialWriteData(HC_SERIAL,(int8u*)&ConfirmUpdateFirmwareData,(ConfirmUpdateFirmwareData.Length + 1));
    emberEventControlSetActive(ToggleLedErrorEventControl);
}

 


//---------------------------------Check Uart Status And Send Quere Data Buffer-------------------------------//
//----------------------------------------------END--------------------------------------//






