# arduino-ATEM-xkeys

This arduino sketch allows control of a Black Magic ATEM video switch using an XKeys keyboard connected to an arduino.

Libraries required:

 * usbhid.h / hiduniversal.h / Usb.h :- https://github.com/felis/USB_Host_Shield_2.0
 * hidxkeysrptparser.h :- https://github.com/JasonRivers/XKeys-Library-for-Arduino (USBHID-fixes branch)
 * ATEM.h :- https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering
 * Timer.h :- https://github.com/JChristensen/Timer
 * Steaming.h :- http://arduiniana.org/libraries/streaming/

Libraries are included in the Libraries directory through git submodules other than ATEM.h which is directly included as the Skaarhoj repository doesn't easily allow for including a single library. 

# Building:

Note: This will currently build for the Arduino Mega ADK and assumes that the arduino toolchain is installed to /opt/arduino-1.8.2. If you wish to build for a different device you can use the arduino IDE or adjust the build.options.json file accordingly.

```sh
# Checkout from github:
git checkout https://github.com/JasonRivers/arduino-ATEM-xkeys.git

cd arduino-ATEM-xkeys

# Update Git Submodules

git submodule update --init

# run arduino builder

arduino-builder -build-options-file ./build.options.json -verbose -compile ./arduino-ATEM-xkeys.ino
```


# TODO:

 * Replace ATEM.h with ATEMstd.h
 * READ Settings from SD Card
 * Web Interface for configuration
 * Make it work on the Arduino Yún or Arduino Tian for an all-in-one device.

# Notes

I have tested this with an X-Keys XK60 with a Black Magic ATEM Production Studio 4K running on an Arduino Mega ADK
with Ethernet shield.
