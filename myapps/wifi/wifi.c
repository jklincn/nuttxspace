#include <arpa/inet.h>
#include <netdb.h>
#include <nuttx/config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "netutils/esp8266.h"

static void print_ip(const char *label, in_addr_t ip) {
  printf("[wifi] %s: %d.%d.%d.%d\n", label, (int)(ip & 0xff),
         (int)((ip >> 8) & 0xff), (int)((ip >> 16) & 0xff), (int)(ip >> 24));
}

/* Scan callback */
static void wifi_scan_callback(lesp_ap_t *ap) {
  if (ap) {
    printf("   SSID: %-25s | RSSI: %d | Sec: %s\n", ap->ssid, ap->rssi,
           lesp_security_to_str(ap->security));
  }
}

/* Subcommand: scan */
static int cmd_scan(void) {
  printf("[wifi] Performing Soft Reset before scan...\n");
  lesp_soft_reset();

  printf("[wifi] Starting Scan...\n");
  int count = lesp_list_access_points(wifi_scan_callback);

  if (count < 0) {
    printf("[wifi] Scan failed.\n");
    return -1;
  }
  printf("[wifi] Scan complete. Found %d APs.\n", count);
  return 0;
}

/* Subcommand: connect */
static int cmd_connect(const char *ssid, const char *pwd) {
  printf("[wifi] Resetting module...\n");

  // Reset before connecting is a good practice to ensure a clean state
  lesp_soft_reset();

  printf("[wifi] Connecting to SSID: %s...\n", ssid);

  if (lesp_ap_connect(ssid, pwd, 10) < 0) {
    printf("[wifi] Join failed (Auth error or Timeout).\n");
    return -1;
  }

  printf("[wifi] Joined. Waiting for DHCP (Max 15s)...\n");

  in_addr_t ip = 0, mask, gw;
  int retries = 10; // Wait up to 10 seconds for an IP address

  while (retries-- > 0) {
    if (lesp_get_net(LESP_MODE_STATION, &ip, &mask, &gw) == 0) {
      if (ip != 0) {
        printf("[wifi] DHCP Success!\n");
        print_ip("IP Addr", ip);
        print_ip("Gateway", gw);
        return 0;
      }
    }
    usleep(1000 * 1000); // Wait 1 second
  }

  printf("\n[wifi] Timeout waiting for IP address.\n");
  return -1;
}

/* Subcommand: test (access example.com) */
static int cmd_test(void) {

  // 1. Check network status
  in_addr_t ip = 0, mask, gw;
  lesp_get_net(LESP_MODE_STATION, &ip, &mask, &gw);
  if (ip == 0) {
    printf("[wifi] Error: No IP address. Did you run 'wifi connect' first?\n");
    return -1;
  }
  print_ip("Current IP", ip);

  const char *hostname = "example.com";

  // 2. DNS resolution
  printf("[wifi] Resolving DNS for %s ...\n", hostname);
  struct hostent *he = lesp_gethostbyname(hostname);
  if (he == NULL) {
    printf("[wifi] DNS Lookup failed.\n");
    return -1;
  }

  struct in_addr **addr_list = (struct in_addr **)he->h_addr_list;
  in_addr_t target_ip = addr_list[0]->s_addr;

  if (target_ip == 0) {
    printf("[wifi] DNS returned 0.0.0.0 (treat as failure). Check "
           "DNS/CIPDOMAIN.\n");
    return -1;
  }

  print_ip("Target IP", target_ip);

  // 3. Create socket (SSL)
  printf("[wifi] Creating SSL Socket...\n");
  int sockfd = lesp_socket(AF_INET, -1, 0);
  if (sockfd < 0) {
    printf("[wifi] Socket creation failed.\n");
    return -1;
  }

  printf("[wifi] Connecting to Server...\n");

  /* For HTTPS servers requiring SNI, set SNI before connect(). */
  if (lesp_setsni(sockfd, hostname) < 0) {
    printf("[wifi] Failed to set SNI.\n");
    lesp_closesocket(sockfd);
    return -1;
  }

  struct sockaddr_in server_addr;
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(443);
  server_addr.sin_addr.s_addr = target_ip;

  if (lesp_connect(sockfd, (struct sockaddr *)&server_addr,
                   sizeof(server_addr)) < 0) {
    printf("[wifi] Connect failed.\n");
    lesp_closesocket(sockfd);
    return -1;
  }

  // 4. Send HTTP request
  // To avoid large responses causing FIFO overflow,
  // use HEAD to fetch only the response headers.
  char packet[256];
  snprintf(packet, sizeof(packet),
           "GET / HTTP/1.1\r\n"
           "Host: %s\r\n"
           "User-Agent: NuttX-ESP8266\r\n"
           "Accept: */*\r\n"
           "Connection: close\r\n"
           "\r\n",
           hostname);

  printf("[wifi] Sending Request:\n%s", packet);
  lesp_send(sockfd, (uint8_t *)packet, strlen(packet), 0);

  // 5. Receive response
  printf("[wifi] Waiting for response...\n");
  uint8_t rx_buf[256]; /* Keep buffer small due to limited memory */
  int len;

  /* Drain loop: keep reading to avoid ESP FIFO overflow.
   * Print all received bytes (HEAD should be small).
   */
  size_t received = 0;

  printf("\n----- REMOTE RESPONSE -----\n");

  while ((len = lesp_recv(sockfd, rx_buf, sizeof(rx_buf), 0)) > 0) {
    received += (size_t)len;

    /* Don't assume NUL-terminated text; print raw bytes. */
    fwrite(rx_buf, 1, (size_t)len, stdout);
  }

  printf("\n--------------------------------------------\n");
  if (received == 0) {
    printf("[wifi] No response received or connection closed.\n");
  } else {
    printf("[wifi] Total received: %u bytes\n", (unsigned)received);
  }

  lesp_closesocket(sockfd);
  return 0;
}

int main(int argc, char *argv[]) {
  int ret = 0;

  if (argc < 2) {
    goto usage;
  }

  printf("[wifi] Initializing Driver...\n");
  if (lesp_initialize() < 0) {
    printf("[wifi] Init failed. Check /dev/ttyS1.\n");
    return EXIT_FAILURE;
  }

  /* Command dispatch */
  if (strcmp(argv[1], "scan") == 0) {
    ret = cmd_scan();
  } else if (strcmp(argv[1], "connect") == 0 || strcmp(argv[1], "join") == 0) {
    if (argc < 4)
      goto usage;
    ret = cmd_connect(argv[2], argv[3]);
  } else if (strcmp(argv[1], "test") == 0) {
    ret = cmd_test();
  } else {
    goto usage;
  }

  /* Cleanup resources */
  lesp_finalize();
  return (ret == 0) ? EXIT_SUCCESS : EXIT_FAILURE;

usage:
  printf("Usage:\n");
  printf("  wifi scan\n");
  printf("  wifi connect <ssid> <password>\n");
  printf("  wifi test\n");
  /* Do not call finalize if initialization failed */
  return EXIT_FAILURE;
}
