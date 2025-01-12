# Robotics-SIXpack

A free Robotics hexapod by delta3robotics. 

* https://wonderl.ink/@delta3robotics
* https://cults3d.com/de/modell-3d/verschiedene/sixpack

Chris wrote all the original code and hosts a Discord to help people learn about robotics.

* https://discord.com/invite/fj6HPAsYT8

I am a novice in robotics so these are just my experiments built on top of Chris's work.

My goal is to add a build and upload solution that doesn't use the Arduino IDE. So, a 
simple `make` solution using the arduino-cli to build and upload.

# Install - Mac only.

* brew install arduino-cli
* USB to Serial
    * Macs older than Maverics need these drivers, but new are good to go.
        * Install FTDI drivers: https://ftdichip.com
* make init

# Build

The build step installs the packages and runs the arduino-cli compile to generate the image

* make build

# Upload

You will have to work out the serial port to upload to. Once you have plugged it in, you should see it in this list

* make usb

Then you can upload the built solution:

* SERIAL_PORT=/dev/cu.usbserial-OR-SOMETHING make upload

# Debugging

Trying to create an independent module that can be used to verify the hardware is functioning as expected. A website contains options to testing each component.

* make PROJECT=Firmware/SIXpackDebug build

# References

RS-232 comms. The communication between the computer and the esp32-cam requires a usb to serial connection that is achieved with a device driver and a chip of some kind. A common solution is the FTDI FT232RL chip mounted on a board. You can get a couple from Amazon:

* [Amazon FTDI component](https://www.amazon.com/gp/product/B07WYLK437)

If you are interested in the backround of RS-232 and how it works at the link level this is a good article.

* [MacOS Big Sur & RS-232 Serial Communications](https://just.graphica.com.au/tips/macos-big-sur-rs232/)

Ultimatly you don't need any additional serial drivers since Mac OSX includes its own in versions 11 onwards. I am testing on Serquoia 15 and it works.

Optional alternative USB -> Serial driver:

* [Prolific PL2303GT](https://www.prolific.com.tw/us/ShowProduct.aspx?pcid=41&showlevel=0017-0037-0041)
* [Aideepen USB to TTL RS232 3.3V Serial Download Cable Adapter Original 6PIN 5V FTDI-232 Module 1M Wire AVR ARM(Combined Head)](https://www.amazon.com/gp/product/B07FVMCPMT)
* [HiLetgo 5pcs USB to Serial USB to TTL CH340 Module with STC Microcontroller Download Adapter](amazon.com/gp/product/B00LZV1G6K)
* [PL2303GT USB to RS232 Serial Cable - UART Upgrade Download Module for RS232 Level (Non-TTL)](https://www.amazon.com/dp/B0D3CFX4PC)


## Mac FTDI Story

When I first started this project everything work, the ftdi drivers installed and everything worked. Then I got a new mac i9 chip, running OSX 15, and nothing worted anymore.

In the old days, and based on Chris's instructions, an FDTI chip set is required to connect to the esp32cam board. 

Today, the FTDI board, has build in support in Mac OSX for this board.

