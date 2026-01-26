#include <nuttx/config.h>

#include <errno.h>
#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <nuttx/leds/userled.h>

#define LED_DEVPATH "/dev/userleds"

int main(int argc, FAR char *argv[]) {
  int fd;
  int ret;
  userled_set_t supported;
  userled_set_t current_state;
  userled_set_t new_state;

  /* Open the LED driver */

  fd = open(LED_DEVPATH, O_RDWR);
  if (fd < 0) {
    int errcode = errno;
    printf("ERROR: Failed to open %s: %d\n", LED_DEVPATH, errcode);
    return EXIT_FAILURE;
  }

  /* Get the set of LEDs supported */

  ret = ioctl(fd, ULEDIOC_SUPPORTED, (unsigned long)((uintptr_t)&supported));
  if (ret < 0) {
    int errcode = errno;
    printf("ERROR: ioctl(ULEDIOC_SUPPORTED) failed: %d\n", errcode);
    close(fd);
    return EXIT_FAILURE;
  }

  /* Get the current state of the LEDs */

  ret = ioctl(fd, ULEDIOC_GETALL, (unsigned long)((uintptr_t)&current_state));
  if (ret < 0) {
    printf("Could not read current state.\n");
    close(fd);
    return EXIT_FAILURE;
  }

  /* Toggle logic:
   * If any supported LED is ON, turn ALL supported LEDs OFF.
   * If ALL supported LEDs are OFF, turn ALL supported LEDs ON.
   */

  if ((current_state & supported) != 0) {
    /* At least one is on, turn all off */
    new_state = 0;
    printf("Turning LEDs OFF\n");
  } else {
    /* All are off, turn all supported on */
    new_state = supported;
    printf("Turning LEDs ON\n");
  }

  /* Apply the new state */

  ret = ioctl(fd, ULEDIOC_SETALL, new_state);
  if (ret < 0) {
    int errcode = errno;
    printf("ERROR: ioctl(ULEDIOC_SETALL) failed: %d\n", errcode);
    close(fd);
    return EXIT_FAILURE;
  }

  close(fd);
  return EXIT_SUCCESS;
}
