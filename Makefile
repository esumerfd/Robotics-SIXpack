# Input the serial port using an environment variable.
SELECTED_SERIAL_PORT=$(if $(SERIAL_PORT), $(SERIAL_PORT), "foo")

PROJECT=Firmware/SIXpackAccesPoint

default: build

build:
	@cd $(PROJECT) && arduino-cli compile  --fqbn arduino:avr:uno --libraries libraries .

upload:
	@cd $(PROJECT) && arduino-cli upload -p $(SELECTED_SERIAL_PORT) --fqbn arduino:avr:uno .

usb:
	ioreg -p IOUSB

