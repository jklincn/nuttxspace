#ifndef __BOARDS_ARM_STM32_ATK_DNF103_V2_SRC_ATK_DNF103_V2_H
#define __BOARDS_ARM_STM32_ATK_DNF103_V2_SRC_ATK_DNF103_V2_H

#include <nuttx/compiler.h>
#include <nuttx/config.h>

#include <arch/chip/chip.h>

int stm32_bringup(void);

#ifdef CONFIG_SENSORS_DHTXX
int stm32_dht11_init(void);
#endif

#endif /* __BOARDS_ARM_STM32_ATK_DNF103_V2_SRC_ATK_DNF103_V2_H */
