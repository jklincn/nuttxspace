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
  return ret;
}
