/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/

#include "project.h"

/* Macros */
#define MAX_ATTEMPTS        (500u)  /* Maximum number of attempts for RTC operation */ 
#define INIT_DELAY          (5u)    /* delay 5 milliseconds before trying again */

#define SECONDS_PER_MIN     (60u)   /* used to keep values in range */
#define MINUTES_PER_HOUR    (60u)

#define TICK_INTERVAL       (10u)   /* seconds or minutes interval. The range should be 1-59 */
#define USE_SECONDS         (0u)    /* set to one to use, set to zero to not use */
#define USE_MINUTES         (1u)    /* use seconds OR minutes, not both  */

#define AFE_TIME            (10u)   // AFE processing time

/*******************************************************************************/
/*                      Function Prototypes                                    */
/*******************************************************************************/
extern cy_en_rtc_status_t RtcInit(void);
extern cy_en_rtc_status_t RtcAlarmConfig(void);
extern void RtcInterruptHandler(void);
extern void RtcStepAlarm(void);

/* [] END OF FILE */
