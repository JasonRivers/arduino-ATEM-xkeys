# arduino-ATEM-xkeys

This arduino sketch allows control of a Black Magic ATEM video switch using an XKeys keyboard connected to an arduino.

Libraries required:

 * usbhid.h / hiduniversal.h / Usb.h :- https://github.com/felis/USB_Host_Shield_2.0
 * hidxkeysrptparser.h :- https://github.com/JasonRivers/XKeys-Library-for-Arduino (USBHID-fixes branch)
 * ATEM.h :- https://github.com/kasperskaarhoj/SKAARHOJ-Open-Engineering
 * Timer.h :- https://github.com/JChristensen/Timer
 * Steaming.h :- http://arduiniana.org/libraries/streaming/

# TODO:

 * READ Settings from SD Card
 * Web Interface for configuration

# Notes

I have tested this with an X-Keys XK60 with a Black Magic ATEM Production Studio 4K running on an Arduino Mega ADK
with Ethernet shield.
