#include <nuttx/config.h>

#include <debug.h>
#include <stdint.h>
#include <syslog.h>

#include "arm_internal.h"
#include "hardware/stm32f103z_pinmap.h"
#include "stm32.h"
#include <nuttx/lcd/ili9341.h>

struct ili9341_lcd_s g_lcddev;
uint16_t g_lcd_id = 0;

#define LCD_BASE 0x6C000000

/* RS (Register Select) connected to A10.
 * In 16-bit mode, internal address HADDR[25:1] maps to FSMC_A[24:0].
 * So HADDR[11] maps to FSMC_A10.
 * Offset = 1 << 11 = 0x800.
 */
#define LCD_CMD (*(volatile uint16_t *)(LCD_BASE))
#define LCD_DATA (*(volatile uint16_t *)(LCD_BASE + 0x800))

#define GPIO_LCD_BACKLIGHT                                                     \
  (GPIO_OUTPUT | GPIO_CNF_OUTPP | GPIO_MODE_50MHz | GPIO_OUTPUT_SET |          \
   GPIO_PORTB | GPIO_PIN0)

static void stm32_ili9341_select(FAR struct ili9341_lcd_s *lcd) {}

static void stm32_ili9341_deselect(FAR struct ili9341_lcd_s *lcd) {}

static int stm32_ili9341_sendcmd(FAR struct ili9341_lcd_s *lcd,
                                 const uint8_t cmd) {
  LCD_CMD = cmd;
  return OK;
}

static int stm32_ili9341_sendparam(FAR struct ili9341_lcd_s *lcd,
                                   const uint8_t param) {
  LCD_DATA = param;
  return OK;
}

static int stm32_ili9341_recvparam(FAR struct ili9341_lcd_s *lcd,
                                   uint8_t *param) {
  *param = LCD_DATA;
  return OK;
}

static int stm32_ili9341_sendgram(FAR struct ili9341_lcd_s *lcd,
                                  const uint16_t *wd, uint32_t nwords) {
  while (nwords--) {
    LCD_DATA = *wd++;
  }
  return OK;
}

static int stm32_ili9341_recvgram(FAR struct ili9341_lcd_s *lcd, uint16_t *wd,
                                  uint32_t nwords) {
  while (nwords--) {
    *wd++ = LCD_DATA;
  }
  return OK;
}

static int stm32_ili9341_backlight(FAR struct ili9341_lcd_s *lcd, int level) {
  stm32_gpiowrite(GPIO_LCD_BACKLIGHT, level > 0);
  return OK;
}

static void stm32_read_id(void) {
  uint16_t id;
  uint16_t param[4];

  /* Try ILI9341 ID (0xD3) */
  LCD_CMD = 0xD3;
  param[0] = LCD_DATA; /* Dummy */
  param[1] = LCD_DATA; /* 0x00 */
  param[2] = LCD_DATA; /* 0x93 */
  param[3] = LCD_DATA; /* 0x41 */

  id = ((param[2] << 8) | param[3]);

  if (id == 0x9341) {
    g_lcd_id = 0x9341;
    syslog(LOG_INFO, "LCD: Found ILI9341\n");
    return;
  }

  /* Try ST7789 ID (0x04) */
  LCD_CMD = 0x04;
  param[0] = LCD_DATA; /* Dummy */
  param[1] = LCD_DATA; /* ID1 */
  param[2] = LCD_DATA; /* ID2 */
  param[3] = LCD_DATA; /* ID3 */

  if (param[2] == 0x85 && param[3] == 0x52) {
    g_lcd_id = 0x8552;
    syslog(LOG_INFO, "LCD: Found ST7789\n");
  }
}

void stm32_st7789_init(struct ili9341_lcd_s *lcd) {
  lcd->select(lcd);

  /* Memory Data Access Control */
  lcd->sendcmd(lcd, 0x36);
  lcd->sendparam(lcd, 0xA0); // Landscape (MV=1, MY=1, MX=0), RGB

  /* RGB 5-6-5-bit */
  lcd->sendcmd(lcd, 0x3A);
  lcd->sendparam(lcd, 0x55);

  /* Porch Setting */
  lcd->sendcmd(lcd, 0xB2);
  lcd->sendparam(lcd, 0x0C);
  lcd->sendparam(lcd, 0x0C);
  lcd->sendparam(lcd, 0x00);
  lcd->sendparam(lcd, 0x33);
  lcd->sendparam(lcd, 0x33);

  /* Gate Control */
  lcd->sendcmd(lcd, 0xB7);
  lcd->sendparam(lcd, 0x35);

  /* VCOM Setting */
  lcd->sendcmd(lcd, 0xBB);
  lcd->sendparam(lcd, 0x19);

  /* LCM Control */
  lcd->sendcmd(lcd, 0xC0);
  lcd->sendparam(lcd, 0x2C);

  /* VDV and VRH Command Enable */
  lcd->sendcmd(lcd, 0xC2);
  lcd->sendparam(lcd, 0x01);

  /* VRH Set */
  lcd->sendcmd(lcd, 0xC3);
  lcd->sendparam(lcd, 0x12);

  /* VDV Set */
  lcd->sendcmd(lcd, 0xC4);
  lcd->sendparam(lcd, 0x20);

  /* Frame Rate Control */
  lcd->sendcmd(lcd, 0xC6);
  lcd->sendparam(lcd, 0x0F);

  /* Power Control 1 */
  lcd->sendcmd(lcd, 0xD0);
  lcd->sendparam(lcd, 0xA4);
  lcd->sendparam(lcd, 0xA1);

  /* Positive Voltage Gamma Control */
  lcd->sendcmd(lcd, 0xE0);
  lcd->sendparam(lcd, 0xD0);
  lcd->sendparam(lcd, 0x04);
  lcd->sendparam(lcd, 0x0D);
  lcd->sendparam(lcd, 0x11);
  lcd->sendparam(lcd, 0x13);
  lcd->sendparam(lcd, 0x2B);
  lcd->sendparam(lcd, 0x3F);
  lcd->sendparam(lcd, 0x54);
  lcd->sendparam(lcd, 0x4C);
  lcd->sendparam(lcd, 0x18);
  lcd->sendparam(lcd, 0x0D);
  lcd->sendparam(lcd, 0x0B);
  lcd->sendparam(lcd, 0x1F);
  lcd->sendparam(lcd, 0x23);

  /* Negative Voltage Gamma Control */
  lcd->sendcmd(lcd, 0xE1);
  lcd->sendparam(lcd, 0xD0);
  lcd->sendparam(lcd, 0x04);
  lcd->sendparam(lcd, 0x0C);
  lcd->sendparam(lcd, 0x11);
  lcd->sendparam(lcd, 0x13);
  lcd->sendparam(lcd, 0x2C);
  lcd->sendparam(lcd, 0x3F);
  lcd->sendparam(lcd, 0x44);
  lcd->sendparam(lcd, 0x51);
  lcd->sendparam(lcd, 0x2F);
  lcd->sendparam(lcd, 0x1F);
  lcd->sendparam(lcd, 0x1F);
  lcd->sendparam(lcd, 0x20);
  lcd->sendparam(lcd, 0x23);

  /* Display Inversion Off */
  lcd->sendcmd(lcd, 0x20);

  /* Sleep Out */
  lcd->sendcmd(lcd, 0x11);
  up_mdelay(100);

  /* Display On */
  lcd->sendcmd(lcd, 0x29);

  /* Turn on backlight */
  lcd->backlight(lcd, 1);

  lcd->deselect(lcd);
}

uint16_t stm32_get_lcd_id(void) { return g_lcd_id; }

void stm32_fsmc_init(void) {

  // Configure FSMC GPIO

  static const uint32_t g_lcd_pins[] = {
      GPIO_NPS_NE4_0, GPIO_NPS_A10_0, GPIO_NPS_NOE_0, GPIO_NPS_NWE_0,
      GPIO_NPS_D0_0,  GPIO_NPS_D1_0,  GPIO_NPS_D2_0,  GPIO_NPS_D3_0,
      GPIO_NPS_D4_0,  GPIO_NPS_D5_0,  GPIO_NPS_D6_0,  GPIO_NPS_D7_0,
      GPIO_NPS_D8_0,  GPIO_NPS_D9_0,  GPIO_NPS_D10_0, GPIO_NPS_D11_0,
      GPIO_NPS_D12_0, GPIO_NPS_D13_0, GPIO_NPS_D14_0, GPIO_NPS_D15_0};

  // Configure GPIOs
  for (int i = 0; i < sizeof(g_lcd_pins) / sizeof(uint32_t); i++) {
    stm32_configgpio(GPIO_ADJUST_MODE(g_lcd_pins[i], GPIO_MODE_50MHz));
  }

  // Configure Backlight GPIO
  stm32_configgpio(GPIO_LCD_BACKLIGHT);

  // Enable FSMC clock
  stm32_fsmc_enable();

  // Configure BCR4 (DO NOT enable bank yet)
  putreg32(FSMC_BCR_SRAM | FSMC_BCR_MWID16 | FSMC_BCR_WREN | FSMC_BCR_EXTMOD,
           STM32_FSMC_BCR4);

  // Configure READ timing (BTR4)
  putreg32(FSMC_BTR_ADDSET(1) | FSMC_BTR_DATAST(16) | FSMC_BTR_ACCMODA,
           STM32_FSMC_BTR4);

  // Configure WRITE timing (BWTR4)
  putreg32(FSMC_BTR_ADDSET(1) | FSMC_BTR_DATAST(2) | FSMC_BTR_ACCMODA,
           STM32_FSMC_BWTR4);

  // Enable the bank by setting the MBKEN bit
  putreg32(FSMC_BCR_MBKEN | FSMC_BCR_SRAM | FSMC_BCR_MWID16 | FSMC_BCR_WREN |
               FSMC_BCR_EXTMOD,
           STM32_FSMC_BCR4);
}

struct ili9341_lcd_s *stm32_ili9341_fsmc_initialize(void) {
  struct ili9341_lcd_s *priv = &g_lcddev;

  stm32_fsmc_init();

  stm32_read_id();

  /* Initialize structure */

  priv->select = stm32_ili9341_select;
  priv->deselect = stm32_ili9341_deselect;
  priv->sendcmd = stm32_ili9341_sendcmd;
  priv->sendparam = stm32_ili9341_sendparam;
  priv->recvparam = stm32_ili9341_recvparam;
  priv->sendgram = stm32_ili9341_sendgram;
  priv->recvgram = stm32_ili9341_recvgram;
  priv->backlight = stm32_ili9341_backlight;

  return priv;
}