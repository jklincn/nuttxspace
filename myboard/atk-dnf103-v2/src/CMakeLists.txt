set(SRCS stm32_boot.c stm32_bringup.c)

if(CONFIG_BOARDCTL)
  list(APPEND SRCS stm32_appinit.c)
endif()

target_sources(board PRIVATE ${SRCS})

set_property(GLOBAL PROPERTY LD_SCRIPT "${NUTTX_BOARD_DIR}/scripts/ld.script")
