PROJECT_NAME = $(shell basename "$(realpath ./)")

APPLICATION_SRCS = $(notdir $(wildcard ./*.c))
APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += led.c
APPLICATION_SRCS += fm25l04b.c
APPLICATION_SRCS += spi_master_nucleum.c
APPLICATION_SRCS += ble_advdata.c                                               
APPLICATION_SRCS += ble_debug_assert_handler.c                                  
APPLICATION_SRCS += ble_srv_common.c                                            

DEVICE = NRF51

ifndef TARGET
	TARGET = NUCLEUM
endif

TARGET_UPPER = $(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LOWER = $(shell echo $(TARGET) | tr A-Z a-z)
BOARD = BOARD_$(TARGET_UPPER)

APPLICATION_SRCS += $(TARGET_LOWER).c

USE_SOFTDEVICE = s110

TEMPLATE_PATH ?= ../../nrf51-pure-gcc-setup/template/

#Define these variables to match your install locations and settings
SDK_VERSION = 7
SDK_PATH ?= $(HOME)/Programs/Nordic/sdk/
SOFTDEVICE ?= $(HOME)/Programs/Nordic/softdevices/s110/s110_nrf51822_7.0.0_softdevice.hex

ifndef SDK_VERSION
$(error An SDK_VERSION must be defined. Edit Makefile to define a valid SDK_VERSION)
endif

ifeq ($(SDK_VERSION),7)
else ifeq ($(SDK_VERSION),6)
else
$(error A valid SDK_VERSION must be defined. Edit Makefile to define a valid SDK_VERSION)
endif

ifndef SDK_PATH
$(error An SDK_PATH must be defined. Edit Makefile to define a valid SDK_PATH)
endif

ifndef SOFTDEVICE 
$(error A SOFTDEVICE path must be defined. Edit Makefile to define a valid SOFTDEVICE path)
endif


LIBRARY_PATHS += ../../include
SOURCE_PATHS += ../../source

CFLAGS = -Os
GDB_PORT_NUMBER = 2331

include $(TEMPLATE_PATH)Makefile
