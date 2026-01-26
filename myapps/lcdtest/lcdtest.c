#include <fcntl.h>
#include <nuttx/config.h>
#include <nuttx/lcd/lcd_dev.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd;
  struct fb_videoinfo_s vinfo;
  struct lcddev_area_s area;
  uint16_t *buffer;
  int ret;
  int i;

  printf("Opening /dev/lcd0...\n");
  fd = open("/dev/lcd0", O_RDWR);
  if (fd < 0) {
    perror("Error opening /dev/lcd0");
    return 1;
  }

  printf("Getting video info...\n");
  ret = ioctl(fd, LCDDEVIO_GETVIDEOINFO, (unsigned long)((uintptr_t)&vinfo));
  if (ret < 0) {
    perror("Error getting video info");
    close(fd);
    return 1;
  }

  printf("Resolution: %d x %d, Format: %d\n", vinfo.xres, vinfo.yres, vinfo.fmt);

  /* Allocate buffer for a chunk of lines (e.g., 10 lines) to save SRAM */
  int chunk_lines = 10;
  size_t bufsize = vinfo.xres * chunk_lines * 2;
  buffer = (uint16_t *)malloc(bufsize);
  if (!buffer) {
    perror("Error allocating buffer");
    close(fd);
    return 1;
  }

  uint16_t colors[] = {0xF800, 0x07E0, 0x001F};
  const char *color_names[] = {"Red", "Green", "Blue"};

  for (int c = 0; c < 3; c++) {
    printf("Filling %s...\n", color_names[c]);

    /* Fill buffer with current color */
    for (i = 0; i < vinfo.xres * chunk_lines; i++)
      buffer[i] = colors[c];

    area.col_start = 0;
    area.col_end = vinfo.xres - 1;
    area.stride = vinfo.xres * 2;
    area.data = (uint8_t *)buffer;

    for (int y = 0; y < vinfo.yres; y += chunk_lines) {
      int height = chunk_lines;
      if (y + height > vinfo.yres)
        height = vinfo.yres - y;

      area.row_start = y;
      area.row_end = y + height - 1;
      ioctl(fd, LCDDEVIO_PUTAREA, (unsigned long)((uintptr_t)&area));
    }

    sleep(1);
  }

  free(buffer);
  close(fd);
  return 0;
}
