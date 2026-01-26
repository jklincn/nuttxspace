#include <nuttx/config.h>

#include <debug.h>
#include <stdbool.h>

#include <arch/board/board.h>

#include "chip.h"

#define GPIO_LED0                                                              \
  (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET |          \
   GPIO_PORTB | GPIO_PIN5)

#define GPIO_LED1                                                              \
  (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET |          \
   GPIO_PORTE | GPIO_PIN5)
   
static const uint32_t g_ledcfg[BOARD_NLEDS] = {
    GPIO_LED0,
    GPIO_LED1,
};

uint32_t board_userled_initialize(void) {
  int i;
  for (i = 0; i < BOARD_NLEDS; i++) {
    stm32_configgpio(g_ledcfg[i]);
  }

  return BOARD_NLEDS;
}

void board_userled(int led, bool ledon) {
  if ((unsigned)led < BOARD_NLEDS) {
    stm32_gpiowrite(g_ledcfg[led], !ledon);
  }
}

void board_userled_all(uint32_t ledset) {
  int i;
  for (i = 0; i < BOARD_NLEDS; i++) {
    bool on = (ledset & (1 << i)) != 0;
    stm32_gpiowrite(g_ledcfg[i], !on);
  }
}