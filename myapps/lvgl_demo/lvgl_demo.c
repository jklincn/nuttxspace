#include <nuttx/config.h>
#include <stdio.h>
#include <unistd.h>

#include "lvgl/src/drivers/nuttx/lv_nuttx_entry.h"
#include <lvgl/lvgl.h>

/****************************************************************************
 * Private Functions
 ****************************************************************************/

static void create_ui(void) {
  lv_obj_t *label = lv_label_create(lv_screen_active());
  lv_label_set_text(label, "Hello LVGL!");
  lv_obj_center(label);
}

/****************************************************************************
 * Public Functions
 ****************************************************************************/

int main(int argc, char *argv[]) {
  lv_nuttx_dsc_t info;
  lv_nuttx_result_t result;

  /* 1. Initialize LVGL */
  if (lv_is_initialized()) {
    printf("LVGL already initialized! aborting.\n");
    return -1;
  }

  lv_init();

  /* 2. Initialize NuttX driver descriptor */
  lv_nuttx_dsc_init(&info);

  /* Use hardware LCD driver at /dev/lcd0 */
  info.fb_path = "/dev/lcd0";

  /* 3. Initialize NuttX backend (creates display, input, etc.) */
  lv_nuttx_init(&info, &result);

  if (result.disp == NULL) {
    printf("NuttX LVGL driver initialization failure!\n");
    return 1;
  }

  /* 4. Create UI */
  create_ui();

  /* 5. Enter main loop */
  /* The built-in runner handles timer handler and sleeping */
  /* lv_nuttx_run will loop forever unless using libuv with specific exit cond
   */
  printf("Starting LVGL loop...\n");
  lv_nuttx_run(&result);

  return 0;
}
