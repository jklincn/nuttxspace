#include <debug.h>
#include <nuttx/config.h>
#include <nuttx/spi/spi.h>

#include <arch/board/board.h>
#include <nuttx/board.h>

#include "arm_internal.h"
#include "atk-dnf103-v2.h"

/****************************************************************************
 * Public Functions
 ****************************************************************************/

void stm32_boardinitialize(void) {}

#ifdef CONFIG_BOARD_LATE_INITIALIZE
void board_late_initialize(void) {
#ifndef CONFIG_BOARDCTL
  stm32_bringup();
#endif
}
#endif
