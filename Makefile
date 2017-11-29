USB_PORT ?= ttyACM0
BUILD_DIR := $(PWD)/.build/target
CACHE_DIR := $(PWD)/.build/cache
ARDUINO_PATH ?= /usr/share/arduino
SHELL := /bin/bash
ARDUINO_BUILDER := $(ARDUINO_PATH)/arduino-builder


all: build upload usb
	

menu.h: menu_creator.py
	python3 menu_creator.py

build_dir:
	mkdir -p $(BUILD_DIR) $(CACHE_DIR)

build: build_dir *.cpp *.h *.ino icons/*.h menu.h
	$(ARDUINO_BUILDER) -compile -logger=human -hardware $(ARDUINO_PATH)/hardware -hardware $(HOME)/.arduino15/packages -hardware $(HOME)/Arduino/hardware -tools $(ARDUINO_PATH)/tools-builder -tools $(HOME)/.arduino15/packages -libraries $(HOME)/Arduino/libraries -fqbn=Arduino_STM32:STM32F1:mapleMini:bootloader_version=original,cpu_speed=speed_72mhz,opt=osstd -vid-pid=0X1EAF_0X0004 -ide-version=10805 -build-path $(BUILD_DIR) -warnings=default -build-cache $(CACHE_DIR) -prefs=build.warn_data_percentage=75 *.ino

upload: build
	$(HOME)/Arduino/hardware/Arduino_STM32/tools/linux/maple_upload $(USB_PORT) 1 1EAF:0003 $(BUILD_DIR)/*.ino.bin 

clean:
	rm -rf $(BUILD_DIR) $(CACHE_DIR)

usb:
	picocom -c /dev/$(USB_PORT)

# vim: noexpandtab:
