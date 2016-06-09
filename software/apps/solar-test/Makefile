PROJECT_NAME = $(shell basename "$(realpath ./)")

APPLICATION_SRCS = $(notdir $(wildcard ./*.c))
#APPLICATION_SRCS += softdevice_handler.c
APPLICATION_SRCS += app_error.c
APPLICATION_SRCS += app_timer.c
APPLICATION_SRCS += led.c
APPLICATION_SRCS += spi_master_nucleum.c
APPLICATION_SRCS += adc.c
APPLICATION_SRCS += rv3049.c

DEVICE = NRF51

ifndef TARGET
	TARGET = NUCLEUM
endif

TARGET_UPPER = $(shell echo $(TARGET) | tr a-z A-Z)
TARGET_LOWER = $(shell echo $(TARGET) | tr A-Z a-z)
BOARD = BOARD_$(TARGET_UPPER)

APPLICATION_SRCS += $(TARGET_LOWER).c

##SOFTDEVICE_MODEL = s110


LIBRARY_PATHS += ../../include
SOURCE_PATHS += ../../source

include ../../include/Makefile.RTC

all: all                                                                        
                                                                                
collect: collect.jlink                                                          
	JLinkExe collect.jlink -device nrf51822 -if swd -speed 1000 -AutoConnect 1 > collect.out
                                                                                
collect.jlink:                                                                  
	printf "r\nmem32 0 0x4000\nmem32 0x00010000 0x4000\nmem32 0x00020000 0x4000\nmem32 0x00030000 0x4000\nq" > collect.jlink


NRF_BASE_PATH ?= ../../nrf5x-base
include $(NRF_BASE_PATH)/make/Makefile
