if(CONFIG_MY_APPS_DHT11)
  nuttx_add_application(
    NAME dht11
    SRCS dht11.c
    STACKSIZE 4096
    PRIORITY 100
  )
endif()
