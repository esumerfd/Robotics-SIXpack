# Input the serial port using an environment variable.
SELECTED_SERIAL_PORT=$(if $(SERIAL_PORT), $(SERIAL_PORT),notset)
# Optionally input the project PROJECT=Firmware/SIXpackDebug
SELECTED_PROJECT:=$(if $(PROJECT), $(PROJECT), Firmware/SIXpackAccesPoint)

FQBN=esp32:esp32:esp32cam

default: build

init: config board dependencies

config:
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

board:
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli core update-index \
		&& arduino-cli core install esp32:esp32
dependencies:
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli lib install "Adafruit BusIO" \
		&& arduino-cli lib install "Adafruit PWM Servo Driver Library"

# make PROJECT=Firmware/SIXpackDebug debug
debug: build upload

build:
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli compile --build-path gen --fqbn $(FQBN) --libraries libraries .

upload:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make upload"
else
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli upload --build-path gen --port $(SELECTED_SERIAL_PORT) --fqbn $(FQBN) .
endif

monitor:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make monitor"
else
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli monitor --port $(SELECTED_SERIAL_PORT)
endif
 
usb:
	@cd $(SELECTED_PROJECT) \
		&& arduino-cli board list \
		&& ioreg -p IOUSB

