if(CONFIG_MY_APPS_LCDTEST)
  nuttx_add_application(
    NAME lcdtest
    SRCS lcdtest.c
    STACKSIZE 4096
    PRIORITY 100
  )
endif()
