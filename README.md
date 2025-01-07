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
* make init
* Install FTDI drivers: https://ftdichip.com

# Build

The build step installs the packages and runs the arduino-cli compile to generate the image

* make build

# Upload

You will have to work out the serial port to upload to. Once you have plugged it in, you should see it in this list

* make usb

Then you can upload the built solution:

* SERIAL_PORT=/dev/cu.usbserial-OR-SOMETHING make upload

 

# References

RS-232 comms. The communication between the computer and the esp32-cam requires a usb to serial connection that is achieved with a device driver and a chip of some kind. A common solution is the FTDI FT232RL chip mounted on a board. You can get a couple from Amazon:

* https://www.amazon.com/gp/product/B07WYLK437

If you are interested in the backround of RS-232 and how it works at the link level this is a good article.

* [MacOS Big Sur & RS-232 Serial Communications](https://just.graphica.com.au/tips/macos-big-sur-rs232/)

Optional alternative USB -> Serial driver:

* [Prolific PL2303GT](https://www.prolific.com.tw/us/ShowProduct.aspx?pcid=41&showlevel=0017-0037-0041)


## Mac FTDI Story

When I first started this project everything work, the ftdi drivers installed and everything worked. Then I got a new mac i9 chip, running OSX 15, and nothing worted anymore.

TBD...
