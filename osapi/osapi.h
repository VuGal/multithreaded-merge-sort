#ifndef OSAPI_H
#define OSAPI_H

#include <csignal>

// check if any operating system was selected
#if (!defined _WIN32) && (!defined OSAPI_USE_FREERTOS) && (!defined OSAPI_USE_RTX)
#error "Unable to select operating system for OSAPI. Provide a compiler definition: OSAPI_USE_FREERTOS or OSAPI_USE_RTX"
#endif


#ifdef _WIN32
#include "Windows.h"
#endif

#ifdef OSAPI_USE_FREERTOS
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"
#endif

#ifdef OSAPI_USE_RTX
#include "cmsis_os2.h"
#endif


namespace osapi {

#include "osapi_mutex_interface.h"
#include "osapi_thread_interface.h"

#ifdef _WIN32
// include windows implementation
#include "windows/osapi_mutex_windows.h"
#include "windows/osapi_recursive_mutex_windows.h"
#include "windows/osapi_thread_windows.h"
#endif

#ifdef OSAPI_USE_FREERTOS
// include FreeRTOS implementation
#include "freertos/osapi_mutex_freertos.h"
#include "freertos/osapi_recursive_mutex_freertos.h"
#include "freertos/osapi_thread_freertos.h"
#endif

#ifdef OSAPI_USE_RTX
// include RTX implementation
#include "rtx/osapi_mutex_rtx.h"
#include "rtx/osapi_recursive_mutex_rtx.h"
#include "rtx/osapi_thread_rtx.h"
#endif

#include "osapi_mortal_thread.h"

/**
 * This system-related function returns the number of system ticks
 * elapsed since the system was started.
 *
 * @return current value of the system tick counter
 */
unsigned int getSystemTime();

} // namespace osapi

#endif // OSAPI_H
