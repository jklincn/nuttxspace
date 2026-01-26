#include <nuttx/arch.h>
#include <nuttx/config.h>
#include <nuttx/sensors/dhtxx.h>

#include <debug.h>
#include <errno.h>
#include <stdbool.h>

#include "stm32_gpio.h"
#include "stm32_tim.h"

#define GPIO_DHT11_OUTPUT \
  (GPIO_OUTPUT | GPIO_CNF_OUTOD | GPIO_MODE_50MHz | GPIO_OUTPUT_SET | GPIO_PORTG | GPIO_PIN11)
#define GPIO_DHT11_INPUT (GPIO_INPUT | GPIO_CNF_INFLOAT | GPIO_MODE_INPUT | GPIO_PORTG | GPIO_PIN11)

#define CONFIG_STM32_DHT11_DEVPATH "/dev/dht0"

struct stm32_dht11_config_s {
  struct dhtxx_config_s dev;   /* Standard interface (must be first) */
  struct stm32_tim_dev_s *tim; /* STM32 TIM2 device handle */
};

static struct stm32_dht11_config_s g_dht11_priv;

static void stm32_dht11_config_data_pin(FAR struct dhtxx_config_s *state, bool mode) {
  if (mode) {
    stm32_configgpio(GPIO_DHT11_INPUT);
  } else {
    stm32_configgpio(GPIO_DHT11_OUTPUT);
  }
}

static void stm32_dht11_set_data_pin(FAR struct dhtxx_config_s *state, bool value) {
  stm32_gpiowrite(GPIO_DHT11_OUTPUT, value);
}

static bool stm32_dht11_read_data_pin(FAR struct dhtxx_config_s *state) {
  return stm32_gpioread(GPIO_DHT11_INPUT);
}

static int64_t stm32_dht11_get_clock(FAR struct dhtxx_config_s *state) {
  static uint32_t rollover_count = 0;
  static uint16_t last_cnt = 0;

  FAR struct stm32_dht11_config_s *priv = (FAR struct stm32_dht11_config_s *)state;

  uint16_t current_cnt = (uint16_t)STM32_TIM_GETCOUNTER(priv->tim);

  if (current_cnt < last_cnt) {
    rollover_count++;
  }
  last_cnt = current_cnt;

  return ((int64_t)rollover_count << 16) | current_cnt;
}

int stm32_dht11_init(void) {
  FAR struct stm32_dht11_config_s *priv = &g_dht11_priv;
  int ret;

  // Initialize TIM2 instance
  priv->tim = stm32_tim_init(2);
  if (!priv->tim) {
    snerr("ERROR: Failed to initialize TIM2\n");
    return -ENODEV;
  }

  // Configure TIM2
  STM32_TIM_SETCLOCK(priv->tim, 1000000);
  STM32_TIM_SETPERIOD(priv->tim, 0xFFFF);
  STM32_TIM_SETMODE(priv->tim, STM32_TIM_MODE_UP);
  STM32_TIM_ENABLE(priv->tim);

  // Link interface operations to our static functions
  priv->dev.config_data_pin = stm32_dht11_config_data_pin;
  priv->dev.set_data_pin = stm32_dht11_set_data_pin;
  priv->dev.read_data_pin = stm32_dht11_read_data_pin;
  priv->dev.get_clock = stm32_dht11_get_clock;
  priv->dev.type = DHTXX_DHT11;

  // Register the character driver
  ret = dhtxx_register(CONFIG_STM32_DHT11_DEVPATH, &priv->dev);
  if (ret < 0) {
    snerr("ERROR: Failed to register DHT11: %d\n", ret);
    /* If registration fails, release the timer to save power */
    stm32_tim_deinit(priv->tim);
    return ret;
  }

  return OK;
}