#include <fcntl.h>
#include <nuttx/config.h>
#include <nuttx/sensors/dhtxx.h>
#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  int fd;
  struct dhtxx_sensor_data_s data;
  ssize_t ret;

  fd = open("/dev/dht0", O_RDONLY);
  if (fd < 0) {
    printf("Failed to open /dev/dht0\n");
    return -1;
  }

  printf("Reading DHT11...\n");

  // Read data from DHT11 sensor
  ret = read(fd, &data, sizeof(data));

  if (ret < 0) {
    printf("Read failed. ret=%d\n", ret);
  } else {
    if (data.status == DHTXX_SUCCESS) {
      printf("Humidity:    %.1f %%\n", data.hum);
      printf("Temperature: %.1f C\n", data.temp);
    } else {
      printf("Error: %d\n", data.status);
    }
  }

  close(fd);
  return 0;
}