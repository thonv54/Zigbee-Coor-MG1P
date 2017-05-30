//

// This callback file is created for your convenience. You may add application
// code to this file. If you regenerate this file over a previous version, the
// previous version will be overwritten and any code you have added will be
// lost.

#include <AppEvent.h>
#include "app/framework/include/af.h"
#include "stack/include/ember-types.h"
#include "app/util/common/form-and-join.h"
#include "app/framework/util/af-main.h"
#include "Uart_Transmitter.h"
#include "Uart_Receiver.h"
#include "Uart_CmdExec.h"






/** @brief Main Init
 *
 * This function is called from the application's main function. It gives the
 * application a chance to do any initialization required at system startup. Any
 * code that you would normally put into the top of the application's main()
 * routine should be put into this function. This is called before the clusters,
 * plugins, and the network are initialized so some functionality is not yet
 * available.
        Note: No callback in the Application Framework is
 * associated with resource cleanup. If you are implementing your application on
 * a Unix host where resource cleanup is a consideration, we expect that you
 * will use the standard Posix system calls, including the use of atexit() and
 * handlers for signals such as SIGTERM, SIGINT, SIGCHLD, SIGPIPE and so on. If
 * you use the signal() function to register your signal handler, please mind
 * the returned value which may be an Application Framework function. If the
 * return value is non-null, please make sure that you call the returned
 * function from your handler to avoid negating the resource cleanup of the
 * Application Framework itself.
 *
 */
void emberAfMainInitCallback(void)
{

	emberSerialInit(HC_SERIAL, 19200, PARITY_NONE, 1);
	NwkFormEventData.PanId = 0xFFFF;
	NwkFormEventData.Channel = 0xFF;
	emberEventControlSetInactive(StartupFormNwkEventControl);
	emberEventControlSetDelayMS(StartupFormNwkEventControl,10000);
	emberEventControlSetInactive(BlinkLedAfterResetEventControl);
	emberEventControlSetDelayMS(BlinkLedAfterResetEventControl,100);
}


//

/** @brief Main Tick
 *
 * Whenever main application tick is called, this callback will be called at the
 * end of the main tick execution.
 *
 */
void emberAfMainTickCallback(void){

}

 /** @brief IAS Zone Cluster Zone Status Change Notification
  *
  * 
  *
  * @param zoneStatus   Ver.: always
  * @param extendedStatus   Ver.: always
  * @param zoneId   Ver.: since ha-1.2-05-3520-29
  * @param delay   Ver.: since ha-1.2-05-3520-29
  */
 boolean emberAfIasZoneClusterZoneStatusChangeNotificationCallback(int16u zoneStatus,
                                                                   int8u extendedStatus,
                                                                   int8u zoneId,
                                                                   int16u delay)
 {
     
   return TRUE;
 }
 
/** @brief IAS Zone Cluster Initiate Normal Operation Mode Response
 *
 * 
 *
 */
boolean emberAfIasZoneClusterInitiateNormalOperationModeResponseCallback(void)
{
  return FALSE;
}

/** @brief IAS Zone Cluster Initiate Test Mode Response
 *
 * 
 *
 */
boolean emberAfIasZoneClusterInitiateTestModeResponseCallback(void)
{
  return FALSE;
}

/** @brief IAS Zone Cluster Zone Enroll Request
*
* 
*
* @param zoneType   Ver.: always
* @param manufacturerCode   Ver.: always
*/
boolean emberAfIasZoneClusterZoneEnrollRequestCallback(int16u zoneType,
                                                    int16u manufacturerCode)
{

    return TRUE;
}






