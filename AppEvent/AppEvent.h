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
#ifndef __ZIGBEEEVENT_H__
#define __ZIGBEEEVENT_H__

#include "app/framework/include/af.h"
/******************************************************************************/
/*                     EXPORTED TYPES and DEFINITIONS                         */
/******************************************************************************/
//#define Debug



typedef enum{
    EventIdNotContain = 0xFF,
}EventDulicateStatus_enum;

typedef struct {
    int16u PanId;
    int8u Channel;
}NwkFormEventData_str;


/******************************************************************************/
/*                              PRIVATE DATA                                  */
/******************************************************************************/

/******************************************************************************/
/*                              EXPORTED DATA                                 */
/******************************************************************************/



extern NwkFormEventData_str NwkFormEventData;

/******************************************************************************/
/*                            PRIVATE FUNCTIONS                               */
/******************************************************************************/

/******************************************************************************/
/*                            EXPORTED FUNCTIONS                              */
/******************************************************************************/

extern EmberEventControl NwkFormEventControl;
extern EmberEventControl StartupFormNwkEventControl;
extern EmberEventControl BlinkLedAfterResetEventControl;
extern EmberEventControl DevResetEventControl;
extern EmberEventControl BlinkLedWhenJoinEventControl;

extern EmberEventControl ToggleLedStatusEventControl;
extern EmberEventControl ToggleLedErrorEventControl;
extern EmberEventControl DeviceChangeToBootModeEventControl;



/******************************************************************************/
/*                            END OF FILE                                     */
/******************************************************************************/









#endif
