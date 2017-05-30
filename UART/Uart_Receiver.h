
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



#ifndef __UART_RECEIVER_H__
#define __UART_RECEIVER_H__




//-----------------------------------------------------------------------//



typedef enum {
    HC_UartConectionError = 0x80,
    HC_FormNetwork = 0x81,
    HC_GetNetworkInfo = 0x82,
    HC_NetworkPjoin = 0x83,
    HC_PjoinStatus = 0x85,

    HC_ZclClustercmdResponse = 0x06,
    HC_ZclClusterCmdRequest = 0x86,
    HC_ZclGlobalCmdRequest = 0x87,
    HC_ZdoCmdRequest = 0x88,
    
    HC_ResetDevice = 0x8E,
    HC_GetDeviceMCVersion = 0x8F,
    HC_GotoUpdateFirmware = 0x90,

} UartRxCommandID_enum;



//---------------------Protocol------------------//

void UartProcess(unsigned char *UartPacketCommand);

void UartGetCommand(void);








//----------------------END-----------------------//






#endif




