/*****************
 * Example: ATEM Monitor
 * Connects to the Atem Switcher and outputs changes to Preview and Program on the Serial monitor (at 9600 baud)
 *
 * - kasper
 */
/*****************
 * TO MAKE THIS EXAMPLE WORK:
 * - You must have an Arduino with Ethernet Shield (or compatible such as "Arduino Ethernet", http://arduino.cc/en/Main/ArduinoBoardEthernet)
 * - You must have an Atem Switcher connected to the same network as the Arduino - and you should have it working with the desktop software
 * - You must make specific set ups in the below lines where the comment "// SETUP" is found!
 */



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <Streaming.h>
#include "usbhid.h"
#include "hiduniversal.h"
#include "Usb.h"
#include <hidxkeysrptparser.h>

//#include <MemoryFree.h>


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 99);              // <= SETUP!  IP address of the Arduino


// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.
#include <ATEM.h>
ATEM AtemSwitcher(IPAddress(192, 168, 0, 240), 56417);  // <= SETUP (the IP address of the ATEM switcher)



// No-cost stream operator as described at 
// http://arduiniana.org/libraries/streaming/
// template<class T>
// inline Print &operator <<(Print &obj, T arg)
// {  
//   obj.print(arg); 
//   return obj; 
// }

// XKeys Stuff

uint8_t counter = 0; 
uint8_t color = BL_BLUE;
uint8_t mode = MODE_ON;

class XKeys : public XkeysReportParser
{

public:
        XKeys();
protected:
  virtual void OnKeyUp(uint8_t keyId); 
  virtual void OnKeyDown(uint8_t keyId);
};

XKeys::XKeys() : XkeysReportParser() {}

XKeys XKs;



void XKeys::OnKeyDown(uint8_t keyId)
{
   
    Serial << F("Key down: ");
    Serial.println(keyId,DEC);
    Serial.println(keyId);
    
   // resetProgramBacklight();

    switch(keyId){
      case 0:    
        AtemSwitcher.changeProgramInput(1);
        break;
      case 8:
        AtemSwitcher.changeProgramInput(2);
        break;
      case 16:
        AtemSwitcher.changeProgramInput(3);
        break;
      case 24:
        AtemSwitcher.changeProgramInput(4);
        break;
      case 32:
        AtemSwitcher.changeProgramInput(5);
        break;
      case 40:
        AtemSwitcher.changeProgramInput(6);
        break;
      case 48:
        AtemSwitcher.changeProgramInput(7);
        break;
      case 56:
        AtemSwitcher.changeProgramInput(8);
        break;
      case 64:
        AtemSwitcher.changeProgramInput(1000);
        break;
      case 72:
        AtemSwitcher.changeProgramInput(0);
        break;
      case 1:
        AtemSwitcher.changePreviewInput(1);
        break;
      case 9:
        AtemSwitcher.changePreviewInput(2);
        break;
      case 17:
        AtemSwitcher.changePreviewInput(3);
        break;
      case 25:
        AtemSwitcher.changePreviewInput(4);
        break;
      case 33:
        AtemSwitcher.changePreviewInput(5);
        break;
      case 41:
        AtemSwitcher.changePreviewInput(6);
        break;
      case 49:
        AtemSwitcher.changePreviewInput(7);
        break;
      case 57:
        AtemSwitcher.changePreviewInput(8);
        break;
      case 65:
        AtemSwitcher.changePreviewInput(1000);
        break;
      case 73:
        AtemSwitcher.changePreviewInput(0);
        break;
    }

      delay(1);    
    indexSetBL(keyId, BL_BLUE, MODE_ON);
        
}

void XKeys::OnKeyUp(uint8_t keyId)
{
    //Serial << F("Key Up: ");
    //Serial << F(keyId, DEC);
}



void setup() { 

      randomSeed(analogRead(5));

  XKs.STICK = 0; // 0 for pad, 1 for stick
  XKs.PADSIZE = 80; // number of keys on your device

  XKs.init();
  for(uint8_t i = 0; i < 8; i++) {
    XKs.setRowBL((i + 1), BL_RED, MODE_OFF);
  }
  for(uint8_t i = 0; i < 8; i++) {
    XKs.setRowBL((i + 1), BL_BLUE, MODE_OFF);
  }
  
  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  // Initialize a connection to the switcher:
  AtemSwitcher.serialOutput(0x80);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();

  // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";
}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher.runLoop();
    XKs.runLoop();
  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();
  } 
     
}
