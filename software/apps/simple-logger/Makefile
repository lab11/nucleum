PROJECT_NAME = $(shell basename "$(realpath ./)")

APPLICATION_SRCS = $(notdir $(wildcard ./*.c))
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += ble_advdata.c
APPLICATION_SRCS += ble_conn_params.c
APPLICATION_SRCS += app_timer.c

APPLICATION_SRCS += simple_ble.c
APPLICATION_SRCS += eddystone.c
APPLICATION_SRCS += simple_adv.c
APPLICATION_SRCS += simple_timer.c
APPLICATION_SRCS += multi_adv.c
APPLICATION_SRCS += mem-ffs.c
APPLICATION_SRCS += nucleum.c
APPLICATION_SRCS += led.c
APPLICATION_SRCS += mem-mmcsd.c
APPLICATION_SRCS += simple_logger.c

LIBRARY_PATHS += ../../include
LIBRARY_PATHS += ../../lib/simple_logger
LIBRARY_PATHS += .
SOURCE_PATHS += ../../src
SOURCE_PATHS += ../../lib/simple_logger
SOURCE_PATHS += ../../source

SOFTDEVICE_MODEL = s110

ENABLE_WIRELESS_DFU = 1

NRF_BASE_PATH ?= ../../nrf5x-base
include $(NRF_BASE_PATH)/make/Makefile
