#include <nuttx/config.h>

#include <stdbool.h>
#include <syslog.h>

#include <nuttx/board.h>
#include <nuttx/timers/oneshot.h>

#include "atk-dnf103-v2.h"

int board_app_initialize(uintptr_t arg) { return stm32_bringup(); }
