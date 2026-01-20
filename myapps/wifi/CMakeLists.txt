if(CONFIG_MY_APPS_WIFI)
  nuttx_add_application(
    NAME wifi
    SRCS wifi.c
    STACKSIZE 4096
    PRIORITY 100
  )
endif()
