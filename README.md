# Robotics-SIXpack

A free Robotics hexapod by delta3robotics. 

* https://wonderl.ink/@delta3robotics
* https://cults3d.com/de/modell-3d/verschiedene/sixpack

Chris wrote all the original code and hosts a Discord to help people learn about robotits.

* https://discord.com/invite/fj6HPAsYT8

I am a novice in robotics so these are just my experiments built on top of Chris's work.

My goal is to add a build and upload solution that doesn't use the Arduino IDE. So, a 
simple `make` solution using the arduino-cli to build and upload.

# Install - Mac only.

* brew install arduino-cli

# Build

The build step uses installs the packages and runs the arduino-cli compile to generate the image

* make build

# Upload

You will have work out the the serial port to upload to. Once you have plugged it in, you should see it in this list

* ioreg -p IOUSB 

Then you can upload the built solution:

* SERIAL_PORT=/dev/cu.usbserial-OR_SOMETHING make upload

 



