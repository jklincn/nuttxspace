if(CONFIG_MY_APPS_LVGL_DEMO)
  nuttx_add_application(
    NAME lvgl_demo
    SRCS lvgl_demo.c
    STACKSIZE 4096
    PRIORITY 100
  )
endif()
