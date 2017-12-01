USB_PORT ?= ttyACM0
OS := $(shell uname)
BUILD_DIR := $(PWD)/.build/target
CACHE_DIR := $(PWD)/.build/cache
ARDUINO_PATH ?= /usr/share/arduino
SHELL := /bin/bash
ARDUINO_HOME := $(HOME)/Arduino
ifeq ($(OS),Darwin)
	ARDUINO_PATH := /Applications/Arduino.app/Contents/Java
	ARDUINO_HOME := $(HOME)/Documents/Arduino
endif
ARDUINO_BUILDER := $(ARDUINO_PATH)/arduino-builder
HARDWARE_FLAGS := -hardware $(ARDUINO_PATH)/hardware -hardware $(ARDUINO_HOME)/.arduino15/packages -hardware $(ARDUINO_HOME)/hardware
LIBRARIES_FLAGS := -libraries $(ARDUINO_HOME)/libraries
TOOLS_FLAGS := -tools $(ARDUINO_PATH)/tools-builder -tools $(HOME)/.arduino15/packages 

STM32_OS_DIR=linux64
ifeq ($(OS),Darwin)
	HARDWARE_FLAGS += -hardware $(HOME)/Library/Arduino15/packages
	TOOLS_FLAGS += -tools $(HOME)/Library/Arduino15/packages  
	STM32_OS_DIR=macosx
endif


all: build upload usb
	

menu.h: menu_creator.py
	python3 menu_creator.py

build_dir:
	mkdir -p $(BUILD_DIR) $(CACHE_DIR)

build: build_dir *.cpp *.h *.ino icons/*.h menu.h
	$(ARDUINO_BUILDER) -compile -logger=human $(HARDWARE_FLAGS) $(TOOLS_FLAGS) $(LIBRARIES_FLAGS) -fqbn=Arduino_STM32:STM32F1:mapleMini:bootloader_version=original,cpu_speed=speed_72mhz,opt=osstd -vid-pid=0X1EAF_0X0004 -ide-version=10805 -build-path $(BUILD_DIR) -warnings=default -build-cache $(CACHE_DIR) -prefs=build.warn_data_percentage=75 *.ino

upload: build
	$(ARDUINO_HOME)/hardware/Arduino_STM32/tools/$(STM32_OS_DIR)/maple_upload $(USB_PORT) 1 1EAF:0003 $(BUILD_DIR)/*.ino.bin 

clean:
	rm -rf $(BUILD_DIR) $(CACHE_DIR)

usb:
	picocom -c /dev/$(USB_PORT) --omap crcrlf --imap lfcrlf

# vim: noexpandtab:
