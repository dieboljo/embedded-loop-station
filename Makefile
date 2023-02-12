## Teensy Makefile

# Arduino Library base folder and example structure
SKETCH_BASE ?= $(PWD)
SKETCH ?= sketch
SKETCH_FILE = $(SKETCH).ino

# Arduino CLI executable
ARDUINO_CLI = /bin/arduino-cli

# Teensy Loader executable
TEENSY_LOADER = /bin/teensy_loader_cli

# Arduino CLI Board type
BOARD_TYPE ?= teensy:avr:teensy41

# Default port to upload to
SERIAL_PORT ?= /dev/ttyACM0

# Optional verbose compile/upload trigger
V ?= 0
VERBOSE=

# Build path -- used to store built binary and object files
BUILD_DIR= build
BUILD_PATH= $(SKETCH_BASE)/$(SKETCH)/$(BUILD_DIR)

ARDUINO_PATH = $(PWD)/arduino

ifneq ($(V), 0)
    VERBOSE=-v
endif

ARDUINO_CORE = arduino:avr

TEENSY_CORE_VERSION = 1.57.2
TEENSY_MCU = TEENSY41
TEENSY_CORE = teensy:avr@$(TEENSY_CORE_VERSION)

CORES = $(ARDUINO_CORE) $(TEENSY_CORE)

UDEV_URL = https://www.pjrc.com/teensy/00-teensy.rules

.PHONY: all setup compile upload clean

all: compile upload

setup:
	$(ARDUINO_CLI) core update-index
	for core in $(CORES); do \
		$(ARDUINO_CLI) core install $$core; \
	done
	sudo wget -N -P /etc/udev/rules.d $(UDEV_URL)


compile:
	$(ARDUINO_CLI) compile $(VERBOSE) --build-path=$(BUILD_PATH) --build-cache-path=$(BUILD_PATH) -b $(BOARD_TYPE) $(SKETCH_BASE)/$(SKETCH)

upload:
	$(TEENSY_LOADER) --mcu=$(TEENSY_MCU) -s -v $(BUILD_PATH)/$(SKETCH_FILE).hex
	# $(ARDUINO_CLI) upload $(VERBOSE) -t -p $(SERIAL_PORT) -b $(BOARD_TYPE) $(SKETCH_BASE)/$(SKETCH)

clean:
	@rm -rf $(BUILD_PATH)
