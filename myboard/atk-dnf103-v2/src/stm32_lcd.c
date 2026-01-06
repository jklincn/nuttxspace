#include <nuttx/config.h>

#include <debug.h>
#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <sys/types.h>

#include <nuttx/arch.h>
#include <nuttx/board.h>
#include <nuttx/lcd/lcd.h>
#include <nuttx/spi/spi.h>

#include <arch/board/board.h>
#include <nuttx/power/pm.h>

#include <nuttx/lcd/ili9341.h>

#define ILI9341_LCD_DEVICE 0

struct ili9341_lcd_s *stm32_ili9341_fsmc_initialize(void);
extern uint16_t stm32_get_lcd_id(void);
extern void stm32_st7789_init(struct ili9341_lcd_s *lcd);

struct lcd_dev_s *g_lcd = NULL;

struct lcd_dev_s *board_lcd_getdev(int lcddev) {
  if (lcddev == ILI9341_LCD_DEVICE) {
    return g_lcd;
  }

  return NULL;
}

int board_lcd_initialize(void) {
  /* check if always initialized */

  if (!g_lcd) {
    /* Initialize the sub driver structure */

    struct ili9341_lcd_s *dev = stm32_ili9341_fsmc_initialize();

    /* Initialize public lcd driver structure */

    if (dev) {
      /* Get a reference to valid lcd driver structure to avoid repeated
       * initialization of the LCD Device. Also enables uninitializing of
       * the LCD Device.
       */

      g_lcd = ili9341_initialize(dev, ILI9341_LCD_DEVICE);
      if (g_lcd) {
        if (stm32_get_lcd_id() == 0x8552) {
          stm32_st7789_init(dev);
        }
        return OK;
      }
    }

    return -ENODEV;
  }

  return OK;
}
