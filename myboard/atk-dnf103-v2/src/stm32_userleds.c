#include <nuttx/config.h>

#include <stdint.h>
#include <stdbool.h>
#include <debug.h>

#include <arch/board/board.h>

#include "chip.h"
#include "stm32.h"
#include "atk-dnf103-v2.h"

#ifndef CONFIG_ARCH_LEDS

/****************************************************************************
 * Private Data
 ****************************************************************************/

/* This array maps an LED number to GPIO pin configuration */

static const uint32_t g_ledcfg[BOARD_NLEDS] =
{
  GPIO_LED0,
  GPIO_LED1,
};

/****************************************************************************
 * Public Functions
 ****************************************************************************/

uint32_t board_userled_initialize(void)
{
  int i;

  /* Configure LED GPIOs for output */

  for (i = 0; i < BOARD_NLEDS; i++)
    {
      stm32_configgpio(g_ledcfg[i]);
    }

  return BOARD_NLEDS;
}

void board_userled(int led, bool ledon)
{
  if ((unsigned)led < BOARD_NLEDS)
    {
      stm32_gpiowrite(g_ledcfg[led], !ledon);
    }
}

void board_userled_all(uint32_t ledset)
{
  int i;

  for (i = 0; i < BOARD_NLEDS; i++)
    {
      bool on = (ledset & (1 << i)) != 0;
      stm32_gpiowrite(g_ledcfg[i], !on);
    }
}

#endif /* !CONFIG_ARCH_LEDS */
