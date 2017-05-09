/*******************************************************************************
 *
 * Copyright (c) 2016
 * Lumi, JSC.
 * All Rights Reserved
 *
 *
 * Description:
 *
 * Author: Thonv
 *
 * Last Changed By:  $Author: Thonv $
 * Revision:         $Revision: 1.0.0.1 $
 * Last Changed:     $Date: 2016-06-08 10:45:00 (Wen, 08 Jun 2016) $
 *
 ******************************************************************************/

/******************************************************************************/
/*                              INCLUDE FILES                                 */
/******************************************************************************/

#include BOARD_HEADER
#include <AppEvent.h>
#include "macro.h"
#include "typedefs.h"
#include "halDelay.h"
#include "app/framework/include/af.h"
#include "app/util/common/form-and-join.h"
#include "Uart_Receiver.h"
#include "Uart_Transmitter.h"
#include "Uart_CmdExec.h"
#include "app/util/zigbee-framework/zigbee-device-common.h"
#include "app/framework/plugin/concentrator/concentrator-support.h"
#include "hal/micro/bootloader-interface-standalone.h"

/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
typedef enum {
	Startup = 0, Idle = 1,
} RunningStep_enum;
#define BlinkLedAfterResetTime 20

/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/
EmberEventControl NwkFormEventControl; // Event control struct declaration
EmberEventControl UartGetEventControl;
EmberEventControl StartupFormNwkEventControl;
EmberEventControl BlinkLedAfterResetEventControl;
EmberEventControl DevResetEventControl;
EmberEventControl BlinkLedWhenJoinEventControl;
EmberEventControl ToggleLedStatusEventControl;
EmberEventControl ToggleLedErrorEventControl;
EmberEventControl DeviceChangeToBootModeEventControl;

int16u BlinkLedAfterResetEventData_BlinkCnt = 0;
int8u ToggleLedStatusEventData_BlinkCnt = 0;
int8u ToggleLedErrorEventData_BlinkCnt = 0;

NwkFormEventData_str NwkFormEventData;  // Event Data Struct

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/
void NwkFormEventFunction(void); // Event function forward declaration
void UartGetEventFunction(void);
void StartupFormNwkEventFunction(void);
void BlinkLedAfterResetEventFunction(void);
void DevResetEventFunction(void);
void BlinkLedWhenJoinEventFunction(void);
void ToggleLedStatusEventFunction(void);
void ToggleLedErrorEventFunction(void);
void DeviceChangeToBootModeEventFunction(void);

//-------------------------------------Fuction---------------------------------//
/** @brief Pre ZDO Message Received
 *
 * This function passes the application an incoming ZDO message and gives the
 * appictation the opportunity to handle it. By default, this callback returns
 * FALSE indicating that the incoming ZDO message has not been handled and
 * should be handled by the Application Framework.
 *
 * @param emberNodeId   Ver.: always
 * @param apsFrame   Ver.: always
 * @param message   Ver.: always
 * @param Length   Ver.: always
 */
boolean emberAfPreZDOMessageReceivedCallback(EmberNodeId emberNodeId,
		EmberApsFrame* apsFrame, int8u* message, int16u Length) {
	if (apsFrame->clusterId == ACTIVE_ENDPOINTS_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, ACTIVE_ENDPOINTS_RESPONSE, Length,
				message);
		return TRUE;
	} else if (apsFrame->clusterId == SIMPLE_DESCRIPTOR_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, SIMPLE_DESCRIPTOR_RESPONSE, Length,
				message);
		return TRUE;
	} else if (apsFrame->clusterId == NETWORK_ADDRESS_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, NETWORK_ADDRESS_RESPONSE, Length,
				message);
		return TRUE;
	} else if (apsFrame->clusterId == IEEE_ADDRESS_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, IEEE_ADDRESS_RESPONSE, Length,
				message);
		return TRUE;
	} else if (apsFrame->clusterId == END_DEVICE_ANNOUNCE) {
		UartSendZdoCmdResponse(emberNodeId, END_DEVICE_ANNOUNCE, Length,
				message);
		if (PjoinTimes == 0) {
			(void) emberAfPermitJoin(180, 1);
		} else {
			(void) emberAfPermitJoin(PjoinTimes, 1);
		}
		return TRUE;
	} else if (apsFrame->clusterId == END_DEVICE_BIND_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, END_DEVICE_BIND_RESPONSE, Length,
				message);
		return TRUE;
	} else if (apsFrame->clusterId == BIND_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, BIND_RESPONSE, Length, message);

		return TRUE;
	} else if (apsFrame->clusterId == UNBIND_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, UNBIND_RESPONSE, Length, message);

		return TRUE;
	} else if (apsFrame->clusterId == LEAVE_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, LEAVE_RESPONSE, Length, message);
		return TRUE;
	} else if (apsFrame->clusterId == NODE_DESCRIPTOR_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, NODE_DESCRIPTOR_RESPONSE, Length,
				message);
		return TRUE;
	} else if (apsFrame->clusterId == LQI_TABLE_RESPONSE) {
		UartSendZdoCmdResponse(emberNodeId, LQI_TABLE_RESPONSE, Length,
				message);
		return TRUE;
	}

	return false;
}

/** @brief Pre Command Received
 *
 * This callback is the second in the Application Framework's message processing
 * chain. At this point in the processing of incoming over-the-air messages, the
 * application has determined that the incoming message is a ZCL command. It
 * parses enough of the message to populate an EmberAfClusterCommand struct. The
 * Application Framework defines this struct value in a local scope to the
 * command processing but also makes it available through a global pointer
 * called emberAfCurrentCommand, in app/framework/util/util.c. When command
 * processing is complete, this pointer is cleared.
 *
 * @param cmd   Ver.: always
 */
bool emberAfPreCommandReceivedCallback(EmberAfClusterCommand* cmd) {
	int16u NwkAddr = cmd->source;
	int8u Endpoint = cmd->apsFrame->sourceEndpoint;
	int16u ClusterId = cmd->apsFrame->clusterId;
	if (cmd->clusterSpecific == ZCL_CLUSTER_SPECIFIC_COMMAND) {
		int8u CmdPayloadLength = cmd->bufLen - 2;
		int8u *CmdPayload = (int8u*) &(cmd->buffer[2]);
		UartSendZclClusterCmdResponse(NwkAddr, Endpoint, ClusterId,
				CmdPayloadLength, CmdPayload);
	} else if (cmd->clusterSpecific == ZCL_GLOBAL_COMMAND) {
		int8u CmdPayloadLength = cmd->bufLen - 3;
		int8u GeneralCmd = cmd->buffer[2];
		int8u *CmdPayload = (int8u*) &(cmd->buffer[3]);
		UartSendZclGlobalCmdResponse(NwkAddr, Endpoint, ClusterId, GeneralCmd,
				CmdPayloadLength, CmdPayload);
	}
	// neu sau 6 phut khong nhan duoc bat ki ban tin nao, du doan loi Coor, cho reset lai coor
	emberEventControlSetInactive(DevResetEventControl);
	emberEventControlSetDelayMinutes(DevResetEventControl, 6);

	return false;
}

/**
 * @func   NwkFormEventFunction
 *
 * @brief  Event Tao mang, bat dau qua trinh tao mang
 *
 * @param  None
 *
 * @retval None
 */
void NwkFormEventFunction(void) {
	EmberStatus byStatus;
	EmberNodeType byCurrentNodeType;
	EmberNetworkStatus byNetworkStatus;
	//Tao mang
	byNetworkStatus = emberAfNetworkState();

	emberAfGetNodeType(&byCurrentNodeType);

	switch (byNetworkStatus) {
	case EMBER_NO_NETWORK:
		emberEventControlSetActive(NwkFormEventControl);
		if (emberFormAndJoinIsScanning() == 0) {
			if (NwkFormEventData.PanId != 0xFFFF) {
				EmberNetworkParameters networkParams;
				(void) emberLeaveNetwork();
				MEMSET(&networkParams, 0, sizeof(EmberNetworkParameters));
				emberAfGetFormAndJoinExtendedPanIdCallback(
						networkParams.extendedPanId);
				networkParams.radioChannel = NwkFormEventData.Channel;
				networkParams.radioTxPower = 10;
				networkParams.panId = NwkFormEventData.PanId;
				emberSetTxPowerMode(10);
				byStatus = emberAfFormNetwork(&networkParams);
			} else {
				emberSetTxPowerMode(10);
				emberAfFindUnusedPanIdAndForm();
			}
			emberEventControlSetInactive(NwkFormEventControl);
			emberEventControlSetDelayMS(NwkFormEventControl, 4000);
		}
		break;
	case EMBER_JOINED_NETWORK:
		if (byCurrentNodeType != EMBER_COORDINATOR) {
			byStatus = emberLeaveNetwork();
			emberEventControlSetDelayMS(NwkFormEventControl, 500);
		} else {
			EmberNetworkParameters networkParams;
			EmberNodeType byNodeType;
			byStatus = emberAfGetNetworkParameters(&byNodeType, &networkParams);
			SwapEndiannessEUI64(networkParams.extendedPanId);
			UartSendNwkInfo(networkParams.extendedPanId, networkParams.panId,
					networkParams.radioChannel);
			emberEventControlSetInactive(NwkFormEventControl);

			// khoi tao mang xong
#ifdef Debug
			emberSerialPrintf(APP_SERIAL,"      Forming Done");
			emberSerialPrintf(APP_SERIAL,"      Pan ID: 0x%2x \n\r",networkParams.panId);
			emberSerialPrintf(APP_SERIAL,"      Channel: %u \n\r",networkParams.radioChannel);

			emberSerialPrintf(APP_SERIAL,"\n\r");
#endif //Debug
		}
		break;
	default:
		emberEventControlSetInactive(NwkFormEventControl);
		break;
	}
}

//---------------------------------------------------------------------------//

void UartGetEventFunction(void) {
	emberEventControlSetActive(UartGetEventControl);
	UartGetCommand();
}

void StartupFormNwkEventFunction(void) {

	EmberNodeType CurrentNodeType;
	EmberNetworkStatus NetworkStatus;
	//Tao mang
	NetworkStatus = emberAfNetworkState();
	emberAfGetNodeType(&CurrentNodeType);
	switch (NetworkStatus) {
	case EMBER_NO_NETWORK:
		if (emberFormAndJoinIsScanning() == 0) {
			emberSetTxPowerMode(10);
			emberAfFindUnusedPanIdAndForm();
		}
		emberEventControlSetActive(StartupFormNwkEventControl);

		break;
	case EMBER_JOINED_NETWORK:
		if (CurrentNodeType != EMBER_COORDINATOR) {
			emberLeaveNetwork();
			emberEventControlSetActive(StartupFormNwkEventControl);
		} else {
			emberEventControlSetInactive(StartupFormNwkEventControl);
			// tao mang thanh cong, cho phep nhan uart
			emberEventControlSetActive(UartGetEventControl);
			// tat nhay led
			emberEventControlSetInactive(BlinkLedAfterResetEventControl);
			halSetLed(StatusNormalLed);
			// quet cac thiet bi xung quanh
			emAfConcentratorStartDiscovery();
		}
		break;
	default:
		break;
	}
}

void BlinkLedAfterResetEventFunction(void) {
	halToggleLed(StatusNormalLed);
	emberEventControlSetInactive(BlinkLedAfterResetEventControl);
	emberEventControlSetDelayMS(BlinkLedAfterResetEventControl, 500);
}

void DevResetEventFunction(void) {
	halReboot();
}

void BlinkLedWhenJoinEventFunction(void) {
	if (emberGetPermitJoining()) {
		halToggleLed(StatusErrorLed);
		emberEventControlSetInactive(BlinkLedWhenJoinEventControl);
		emberEventControlSetDelayMS(BlinkLedWhenJoinEventControl, 400);
	} else {
		halClearLed(StatusErrorLed);
		UartSendPjoinStatus(NotPermitJoin);
		emberEventControlSetInactive(BlinkLedWhenJoinEventControl);
	}
}

void ToggleLedStatusEventFunction(void) {
	ToggleLedStatusEventData_BlinkCnt++;
	halToggleLed(StatusNormalLed);
	if (ToggleLedStatusEventData_BlinkCnt < 3) {
		emberEventControlSetInactive(ToggleLedStatusEventControl);
		emberEventControlSetDelayMS(ToggleLedStatusEventControl, 50);
	} else {
		emberEventControlSetInactive(ToggleLedStatusEventControl);
		ToggleLedStatusEventData_BlinkCnt = 0;
		halClearLed(StatusErrorLed);
		halSetLed(StatusNormalLed);
	}

}
void ToggleLedErrorEventFunction(void) {
	ToggleLedErrorEventData_BlinkCnt++;
	halToggleLed(StatusErrorLed);
	if (ToggleLedErrorEventData_BlinkCnt < 3) {
		emberEventControlSetInactive(ToggleLedErrorEventControl);
		emberEventControlSetDelayMS(ToggleLedErrorEventControl, 50);
	} else {
		emberEventControlSetInactive(ToggleLedErrorEventControl);
		ToggleLedErrorEventData_BlinkCnt = 0;
		halClearLed(StatusErrorLed);
		halSetLed(StatusNormalLed);
	}

}
void DeviceChangeToBootModeEventFunction(void) {
	(void) halLaunchStandaloneBootloader(STANDALONE_BOOTLOADER_NORMAL_MODE);
}

