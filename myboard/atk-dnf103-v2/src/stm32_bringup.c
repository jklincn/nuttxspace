#include "atk-dnf103-v2.h"
#include "stm32.h"
#include <debug.h>
#include <errno.h>
#include <nuttx/board.h>
#include <nuttx/config.h>
#include <nuttx/fs/fs.h>
#include <nuttx/timers/oneshot.h>
#include <stdbool.h>
#include <stdio.h>
#include <syslog.h>

#ifdef CONFIG_USERLED
#  include <nuttx/leds/userled.h>
#endif

/****************************************************************************
 * Pre-processor Definitions
 ****************************************************************************/

/****************************************************************************
 * Private Data
 ****************************************************************************/

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int stm32_bringup(void) {
  int ret = OK;
#ifdef CONFIG_USERLED
  /* Register the LED driver */

  ret = userled_lower_initialize("/dev/userleds");
  if (ret < 0)
    {
      syslog(LOG_ERR, "ERROR: userled_lower_initialize() failed: %d\n", ret);
    }
#endif
  return ret;
}
