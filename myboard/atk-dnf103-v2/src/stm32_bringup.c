#include "atk-dnf103-v2.h"
#include <debug.h>
#include <nuttx/board.h>
#include <nuttx/config.h>
#include <nuttx/fs/fs.h>
#include <nuttx/timers/oneshot.h>
#include <stdbool.h>
#include <syslog.h>

#ifdef CONFIG_USERLED
#include <nuttx/leds/userled.h>
#endif

#ifdef CONFIG_LCD_DEV
#include <nuttx/lcd/lcd_dev.h>
#endif

int stm32_bringup(void) {
  int ret = OK;
#ifdef CONFIG_USERLED
  /* Register the LED driver */

  ret = userled_lower_initialize("/dev/userleds");
  if (ret < 0) {
    syslog(LOG_ERR, "ERROR: userled_lower_initialize() failed: %d\n", ret);
  }
#endif
#ifdef CONFIG_LCD_DEV
  /* Initialize the LCD and get the LCD device instance */

  ret = board_lcd_initialize();
  if (ret < 0) {
    syslog(LOG_ERR, "ERROR: board_lcd_initialize() failed: %d\n", ret);
  }
  ret = lcddev_register(0);
  if (ret < 0) {
    syslog(LOG_ERR, "ERROR: lcddev_register() failed: %d\n", ret);
  }
#endif
  return ret;
}
