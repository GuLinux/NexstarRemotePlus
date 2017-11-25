USB_PORT=ttyACM0
BUILD_DIR=$(PWD)/.build/target
CACHE_DIR=$(PWD)/.build/cache

all: build
	

build_dir:
	mkdir -p $(BUILD_DIR) $(CACHE_DIR)

build: build_dir *.cpp *.h *.ino icons/*.h
	arduino-builder -compile -logger=human -hardware /usr/share/arduino/hardware -hardware $(HOME)/.arduino15/packages -hardware $(HOME)/Arduino/hardware -tools /usr/share/arduino/tools-builder -tools $(HOME)/.arduino15/packages -libraries $(HOME)/Arduino/libraries -fqbn=Arduino_STM32-master:STM32F1:mapleMini:bootloader_version=original,cpu_speed=speed_72mhz,opt=osstd -vid-pid=0X1EAF_0X0004 -ide-version=10805 -build-path $(BUILD_DIR) -warnings=default -build-cache $(CACHE_DIR) -prefs=build.warn_data_percentage=75 NexstarHelper.ino

upload: build
	$(HOME)/Arduino/hardware/Arduino_STM32-master/tools/linux/maple_upload $(USB_PORT) 1 1EAF:0003 $(BUILD_DIR)/NexstarHelper.ino.bin 

clean:
	rm -rf $(BUILD_DIR) $(CACHE_DIR)


