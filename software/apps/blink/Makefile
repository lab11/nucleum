PROJECT_NAME = $(shell basename "$(realpath ./)")

APPLICATION_SRCS = $(notdir $(wildcard ./*.c))
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += led.c

DEVICE = NRF51

ifndef TARGET
	TARGET = NUCLEUM
endif

TARGET_UPPER = $(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LOWER = $(shell echo $(TARGET) | tr A-Z a-z)
BOARD = BOARD_$(TARGET_UPPER)

APPLICATION_SRCS += $(TARGET_LOWER).c

USE_SOFTDEVICE = s110

SDK_VERSION = 7
TEMPLATE_PATH ?= ../../nrf51-pure-gcc-setup/template/
SDK_PATH ?= $(HOME)/code/nrf51822_sdk/sdk_7/
SOFTDEVICE ?= $(HOME)/code/nrf51822_softdevice/s110_nrf51822_7.0.0_softdevice.hex

LIBRARY_PATHS += ../../include
SOURCE_PATHS += ../../source

CFLAGS = -Os
GDB_PORT_NUMBER = 2331

include $(TEMPLATE_PATH)Makefile
