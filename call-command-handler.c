// This file is generated by Simplicity Studio.  Please do not edit manually.
//
//

// This is a set of generated functions that parse the
// the incomming message, and call appropriate command handler.



#include PLATFORM_HEADER
#ifdef EZSP_HOST
// Includes needed for ember related functions for the EZSP host
#include "stack/include/error.h"
#include "stack/include/ember-types.h"
#include "app/util/ezsp/ezsp-protocol.h"
#include "app/util/ezsp/ezsp.h"
#include "app/util/ezsp/ezsp-utils.h"
#include "app/util/ezsp/serial-interface.h"
#else
// Includes needed for ember related functions for the EM250
#include "stack/include/ember.h"
#endif // EZSP_HOST

#include "app/framework/util/util.h"
#include "af-structs.h"
#include "call-command-handler.h"
#include "command-id.h"
#include "callback.h"

static EmberAfStatus status(bool wasHandled, bool clusterExists, bool mfgSpecific)
{
  if (wasHandled) {
    return EMBER_ZCL_STATUS_SUCCESS;
  } else if (mfgSpecific) {
    return EMBER_ZCL_STATUS_UNSUP_MANUF_CLUSTER_COMMAND;
  } else if (clusterExists) {
    return EMBER_ZCL_STATUS_UNSUP_CLUSTER_COMMAND;
  } else {
    return EMBER_ZCL_STATUS_UNSUPPORTED_CLUSTER;
  }
}

// Main command parsing controller.
EmberAfStatus emberAfClusterSpecificCommandParse(EmberAfClusterCommand *cmd)
{
  if (cmd->direction == ZCL_DIRECTION_SERVER_TO_CLIENT
      && emberAfContainsClient(cmd->apsFrame->destinationEndpoint,
                               cmd->apsFrame->clusterId)) {
    switch (cmd->apsFrame->clusterId) {
    case ZCL_IAS_ZONE_CLUSTER_ID:
      return emberAfIasZoneClusterClientCommandParse(cmd);
    case ZCL_SIMPLE_METERING_CLUSTER_ID:
      return emberAfSimpleMeteringClusterClientCommandParse(cmd);
    }
  } else if (cmd->direction == ZCL_DIRECTION_CLIENT_TO_SERVER
             && emberAfContainsServer(cmd->apsFrame->destinationEndpoint,
                                      cmd->apsFrame->clusterId)) {
    switch (cmd->apsFrame->clusterId) {
    case ZCL_IDENTIFY_CLUSTER_ID:
      return emberAfIdentifyClusterServerCommandParse(cmd);
    }
  }
  return status(false, false, cmd->mfgSpecific);
}

// Cluster: Identify, server
EmberAfStatus emberAfIdentifyClusterServerCommandParse(EmberAfClusterCommand *cmd)
{
  bool wasHandled = false;
  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
    case ZCL_IDENTIFY_COMMAND_ID:
      {
        uint16_t payloadOffset = cmd->payloadStartIndex;
        uint16_t identifyTime;  // Ver.: always
        // Command is fixed length: 2
        if (cmd->bufLen < payloadOffset + 2) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        identifyTime = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);
        wasHandled = emberAfIdentifyClusterIdentifyCallback(identifyTime);
        break;
      }
    case ZCL_IDENTIFY_QUERY_COMMAND_ID:
      {
        // Command is fixed length: 0
        wasHandled = emberAfIdentifyClusterIdentifyQueryCallback();
        break;
      }
    }
  }
  return status(wasHandled, true, cmd->mfgSpecific);
}

// Cluster: IAS Zone, client
EmberAfStatus emberAfIasZoneClusterClientCommandParse(EmberAfClusterCommand *cmd)
{
  bool wasHandled = false;
  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
    case ZCL_ZONE_STATUS_CHANGE_NOTIFICATION_COMMAND_ID:
      {
        uint16_t payloadOffset = cmd->payloadStartIndex;
        uint16_t zoneStatus;  // Ver.: always
        uint8_t extendedStatus;  // Ver.: always
        uint8_t zoneId;  // Ver.: since ha-1.2-05-3520-29
        uint16_t delay;  // Ver.: since ha-1.2-05-3520-29
        // Command is not a fixed length
        if (cmd->bufLen < payloadOffset + 2) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        zoneStatus = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 2;
        if (cmd->bufLen < payloadOffset + 1) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        extendedStatus = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 1;
        if ( ( cmd->bufLen < payloadOffset + 1)) {
          // Argument is not always present:
          // - it is present only in versions higher than: ha-1.2-05-3520-29
          zoneId = 0xFF;
        } else {
          zoneId = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
          payloadOffset += 1;
        }
        if ( ( cmd->bufLen < payloadOffset + 2)) {
          // Argument is not always present:
          // - it is present only in versions higher than: ha-1.2-05-3520-29
          delay = 0xFFFF;
        } else {
          delay = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);
        }
        wasHandled = emberAfIasZoneClusterZoneStatusChangeNotificationCallback(zoneStatus,
                                                                               extendedStatus,
                                                                               zoneId,
                                                                               delay);
        break;
      }
    case ZCL_ZONE_ENROLL_REQUEST_COMMAND_ID:
      {
        uint16_t payloadOffset = cmd->payloadStartIndex;
        uint16_t zoneType;  // Ver.: always
        uint16_t manufacturerCode;  // Ver.: always
        // Command is fixed length: 4
        if (cmd->bufLen < payloadOffset + 4) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        zoneType = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 2;
        manufacturerCode = emberAfGetInt16u(cmd->buffer, payloadOffset, cmd->bufLen);
        wasHandled = emberAfIasZoneClusterZoneEnrollRequestCallback(zoneType,
                                                                    manufacturerCode);
        emberAfFillExternalBuffer((ZCL_CLUSTER_SPECIFIC_COMMAND \
                     | ZCL_FRAME_CONTROL_CLIENT_TO_SERVER\
                     | ZCL_DISABLE_DEFAULT_RESPONSE_MASK),
                    ZCL_IAS_ZONE_CLUSTER_ID,
                    ZCL_ZONE_ENROLL_RESPONSE_COMMAND_ID,
                    "u",
                    EMBER_SUCCESS);
        emberAfSetCommandEndpoints(1, cmd->apsFrame->sourceEndpoint);
        emberAfSendCommandUnicast(EMBER_OUTGOING_DIRECT, cmd->source); 
      
        break;
      }
    }
  }
  return status(wasHandled, true, cmd->mfgSpecific);
}

// Cluster: Simple Metering, client
EmberAfStatus emberAfSimpleMeteringClusterClientCommandParse(EmberAfClusterCommand *cmd)
{
  bool wasHandled = false;
  if (!cmd->mfgSpecific) {
    switch (cmd->commandId) {
    case ZCL_GET_PROFILE_RESPONSE_COMMAND_ID:
      {
        uint16_t payloadOffset = cmd->payloadStartIndex;
        uint32_t endTime;  // Ver.: always
        uint8_t status;  // Ver.: always
        uint8_t profileIntervalPeriod;  // Ver.: always
        uint8_t numberOfPeriodsDelivered;  // Ver.: always
        uint8_t* intervals;  // Ver.: always
        // Command is fixed length: 7
        if (cmd->bufLen < payloadOffset + 7) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        endTime = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 4;
        status = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 1;
        profileIntervalPeriod = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 1;
        numberOfPeriodsDelivered = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 1;
        intervals = cmd->buffer + payloadOffset;
        wasHandled = emberAfSimpleMeteringClusterGetProfileResponseCallback(endTime,
                                                                            status,
                                                                            profileIntervalPeriod,
                                                                            numberOfPeriodsDelivered,
                                                                            intervals);
        break;
      }
    case ZCL_REQUEST_MIRROR_COMMAND_ID:
      {
        // Command is fixed length: 0
        wasHandled = emberAfSimpleMeteringClusterRequestMirrorCallback();
        break;
      }
    case ZCL_REMOVE_MIRROR_COMMAND_ID:
      {
        // Command is fixed length: 0
        wasHandled = emberAfSimpleMeteringClusterRemoveMirrorCallback();
        break;
      }
    case ZCL_REQUEST_FAST_POLL_MODE_RESPONSE_COMMAND_ID:
      {
        uint16_t payloadOffset = cmd->payloadStartIndex;
        uint8_t appliedUpdatePeriod;  // Ver.: always
        uint32_t fastPollModeEndtime;  // Ver.: always
        // Command is fixed length: 5
        if (cmd->bufLen < payloadOffset + 5) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        appliedUpdatePeriod = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 1;
        fastPollModeEndtime = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);
        wasHandled = emberAfSimpleMeteringClusterRequestFastPollModeResponseCallback(appliedUpdatePeriod,
                                                                                     fastPollModeEndtime);
        break;
      }
    case ZCL_SUPPLY_STATUS_RESPONSE_COMMAND_ID:
      {
        uint16_t payloadOffset = cmd->payloadStartIndex;
        uint32_t providerId;  // Ver.: always
        uint32_t issuerEventId;  // Ver.: always
        uint32_t implementationDateTime;  // Ver.: always
        uint8_t supplyStatus;  // Ver.: always
        // Command is fixed length: 13
        if (cmd->bufLen < payloadOffset + 13) return EMBER_ZCL_STATUS_MALFORMED_COMMAND;
        providerId = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 4;
        issuerEventId = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 4;
        implementationDateTime = emberAfGetInt32u(cmd->buffer, payloadOffset, cmd->bufLen);
        payloadOffset += 4;
        supplyStatus = emberAfGetInt8u(cmd->buffer, payloadOffset, cmd->bufLen);
        wasHandled = emberAfSimpleMeteringClusterSupplyStatusResponseCallback(providerId,
                                                                              issuerEventId,
                                                                              implementationDateTime,
                                                                              supplyStatus);
        break;
      }
    }
  }
  return status(wasHandled, true, cmd->mfgSpecific);
}
