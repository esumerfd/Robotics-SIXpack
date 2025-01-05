# Input the serial port using an environment variable.
SELECTED_SERIAL_PORT=$(if $(SERIAL_PORT), $(SERIAL_PORT),notset)

FQBN=esp32:esp32:esp32cam
PROJECT=Firmware/SIXpackAccesPoint

default: build

init: config board dependencies

config:
	@cd $(PROJECT) \
		&& arduino-cli config add board_manager.additional_urls https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json

board:
	@cd $(PROJECT) \
		&& arduino-cli core update-index \
		&& arduino-cli core install esp32:esp32
dependencies:
	@cd $(PROJECT) \
		&& arduino-cli lib install "Adafruit BusIO" \
		&& arduino-cli lib install "Adafruit PWM Servo Driver Library"

build:
	@cd $(PROJECT) \
		&& arduino-cli compile  --fqbn $(FQBN) --libraries libraries .

upload:
ifeq ($(SELECTED_SERIAL_PORT),notset)
	@echo "Pass in serial port: SERIAL_PORT=xxx make upload"
else
	@cd $(PROJECT) \
		&& arduino-cli upload -p $(SELECTED_SERIAL_PORT) --fqbn $(FQBN) .
endif

usb:
	@cd $(PROJECT) \
		&& arduino-cli board list \
		&& ioreg -p IOUSB

