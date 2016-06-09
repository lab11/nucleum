PROJECT_NAME = $(shell basename "$(realpath ./)")

APPLICATION_SRCS = $(notdir $(wildcard ./*.c))
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += led.c
APPLICATION_SRCS += rv3049.c
APPLICATION_SRCS += spi_master_nucleum.c
APPLICATION_SRCS += ble_advdata.c                                               
APPLICATION_SRCS += ble_conn_params.c                                               
APPLICATION_SRCS += ble_debug_assert_handler.c                                  
APPLICATION_SRCS += ble_srv_common.c                                            

APPLICATION_SRCS += simple_ble.c
APPLICATION_SRCS += simple_adv.c                                            
APPLICATION_SRCS += simple_timer.c                                            

DEVICE = NRF51

ifndef TARGET
	TARGET = NUCLEUM
endif

TARGET_UPPER = $(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LOWER = $(shell echo $(TARGET) | tr A-Z a-z)
BOARD = BOARD_$(TARGET_UPPER)

APPLICATION_SRCS += $(TARGET_LOWER).c

SOFTDEVICE_MODEL = s110


LIBRARY_PATHS += ../../include
SOURCE_PATHS += ../../source

include ../../include/Makefile.RTC

NRF_BASE_PATH ?= ../../nrf5x-base
include $(NRF_BASE_PATH)/make/Makefile
