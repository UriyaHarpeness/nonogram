#pragma once

/**
 * Defines logging configurations.
 */

/// Defines debug config as on.
#define DEBUG_ON 0x1

/// Defines debug config as off.
#define DEBUG_OFF 0x0

/***********************/
/******DEFINE*HERE******/
/***********************/

#define DEBUG_SUMMARY DEBUG_ON
#define DEBUG_LOGS DEBUG_OFF
#define DEBUG_TRACES DEBUG_OFF

/***********************/
/*******END*DEFINE******/
/***********************/

#ifdef DEBUG_ALL

/// Enable summary logs.
#define DEBUG_SUMMARY DEBUG_ON

/// Enable debug and above level logs.
#define DEBUG_LOGS DEBUG_ON

/// Enable trace level logs.
#define DEBUG_TRACES DEBUG_ON

#endif // DEBUG_ALL
