#ifndef __BOARDS_ARM_STM32_ATK_DNF103_V2_SRC_ATK_DNF103_V2_H
#define __BOARDS_ARM_STM32_ATK_DNF103_V2_SRC_ATK_DNF103_V2_H

#include <nuttx/compiler.h>
#include <nuttx/config.h>

#include <arch/chip/chip.h>

#define GPIO_LED0                                                              \
  (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET |          \
   GPIO_PORTB | GPIO_PIN5)

#define GPIO_LED1                                                              \
  (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET |          \
   GPIO_PORTE | GPIO_PIN5)

int stm32_bringup(void);

#endif /* __BOARDS_ARM_STM32_ATK_DNF103_V2_SRC_ATK_DNF103_V2_H */
