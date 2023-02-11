## Teensy Makefile

# Arduino Library base folder and example structure
SKETCH_BASE ?= $(PWD)
SKETCH ?= sketch

# Arduino CLI executable name and directory location
ARDUINO_CLI = arduino-cli
ARDUINO_CLI_DIR = /bin

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

CORES = arduino:avr teensy:avr:teensy41@1\.57\.2

.PHONY: all compile upload clean

all: compile upload

setup:
	$(ARDUINO_CLI_DIR)/$(ARDUINO_CLI) core update-index
	for core in $(CORES); do \
		$(ARDUINO_CLI_DIR)/$(ARDUINO_CLI) core install $$core; \
	done
	sudo cp $(ARDUINO_PATH)/data/packages/teensy/tools/teensy-tools/1.57.2/00-teensy.rules /etc/udev/rules.d


compile:
	$(ARDUINO_CLI_DIR)/$(ARDUINO_CLI) compile $(VERBOSE) --build-path=$(BUILD_PATH) --build-cache-path=$(BUILD_PATH) -b $(BOARD_TYPE) $(SKETCH_BASE)/$(SKETCH)

upload:
	$(ARDUINO_CLI_DIR)/$(ARDUINO_CLI) upload $(VERBOSE) -t -p $(SERIAL_PORT) -b $(BOARD_TYPE) $(SKETCH_BASE)/$(SKETCH)

clean:
	@rm -rf $(BUILD_PATH)
