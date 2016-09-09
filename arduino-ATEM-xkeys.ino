/*****************
 * Copyright Jason Rivers 2016
 *****************/



// Including libraries: 
#include <SPI.h>         // needed for Arduino versions later than 0018
#include <Ethernet.h>
#include <SD.h>
#include "Streaming.h"
#include "usbhid.h"
#include "hiduniversal.h"
#include "Usb.h"
#include "hidxkeysrptparser.h"
#include "Timer.h"
#include "ATEM.h"


// MAC address and IP address for this *particular* Arduino / Ethernet Shield!
// The MAC address is printed on a label on the shield or on the back of your device
// The IP address should be an available address you choose on your subnet where the switcher is also present
byte mac[] = { 
  0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
IPAddress ip(192, 168, 0, 99);              // <= SETUP!  IP address of the Arduino

// Include ATEM library and make an instance:
// Connect to an ATEM switcher on this address and using this local port:
// The port number is chosen randomly among high numbers.

ATEM AtemSwitcher(IPAddress(192, 168, 0, 240), 56417);  // <= SETUP (the IP address of the ATEM switcher)

// SD Card

const int chipSelect = 4;

// XKeys Stuff

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


// Track ATEM Features 
bool FTBLastState;
int TransitionTypeLastState = 6; //Out of range to force an update on load
uint16_t AuxState;
uint16_t ProgramInputLastState;
uint16_t PreviewInputLastState;

// Keys

// Misc Keys
uint8_t FTBKEY = 7;
uint8_t MIXKEY = 6;
uint8_t DIPKEY = 5;
uint8_t WIPEKEY = 4;
uint8_t TRANSKEY = 0;
uint8_t CUTKEY = 72;

// Program Control
uint8_t PROGRAM1 = 64;
uint8_t PROGRAM2 = 56;
uint8_t PROGRAM3 = 48;
uint8_t PROGRAM4 = 40;
uint8_t PROGRAM5 = 32;
uint8_t PROGRAM6 = 24;
uint8_t PROGRAM7 = 16;
uint8_t PROGRAM8 = 8;

uint8_t PROGRAMCBAR = 39;
uint8_t PROGRAMBLK = 31;

// Preview Control
uint8_t PREVIEW1 = 65;
uint8_t PREVIEW2 = 57;
uint8_t PREVIEW3 = 49;
uint8_t PREVIEW4 = 41;
uint8_t PREVIEW5 = 33;
uint8_t PREVIEW6 = 25;
uint8_t PREVIEW7 = 17;
uint8_t PREVIEW8 = 9;

uint8_t PREVIEWCBAR = 55;
uint8_t PREVIEWBLK = 47;

// Aux Control
uint8_t AUXDEBUG = 51;
uint8_t AUXPROGRAM = 27;
uint8_t AUXPEVIEW = 35;
uint8_t AUXINPUT1 = 53;
uint8_t AUXINPUT2 = 45;
uint8_t AUXINPUT3 = 37;
uint8_t AUXINPUT4 = 29;
uint8_t AUXINPUT5 = 52;
uint8_t AUXINPUT6 = 44;
uint8_t AUXINPUT7 = 36;
uint8_t AUXINPUT8 = 28;


// LED Groups

// only show backlights on the keys I'm using.
  uint8_t BlueLEDGroup[] = {7,6,5,4,64,56,48,40,32,24,16,8,55,47,65,57,49,41,33,25,17,9,39,31};
  uint8_t RedLEDGroup[] = {0,1,72,73};
  
Timer t;

void XKeys::OnKeyDown(uint8_t keyId)
{
    //Serial << F("Key down: ");
    //Serial.println(keyId,DEC);
    //Serial.println(keyId);
    
  //Key Switching
  // Program
  if (keyId == PROGRAM1) {AtemSwitcher.changeProgramInput(1);}
  else if (keyId == PROGRAM2) {AtemSwitcher.changeProgramInput(2);}
  else if (keyId == PROGRAM3) {AtemSwitcher.changeProgramInput(3);}
  else if (keyId == PROGRAM4) {AtemSwitcher.changeProgramInput(4);}
  else if (keyId == PROGRAM5) {AtemSwitcher.changeProgramInput(5);}
  else if (keyId == PROGRAM6) {AtemSwitcher.changeProgramInput(6);}
  else if (keyId == PROGRAM7) {AtemSwitcher.changeProgramInput(7);}
  else if (keyId == PROGRAM8) {AtemSwitcher.changeProgramInput(8);}
  else if (keyId == PROGRAMCBAR) {AtemSwitcher.changeProgramInput(1000);}
  else if (keyId == PROGRAMBLK) {AtemSwitcher.changeProgramInput(0);}
  
  // Preview
  else if (keyId == PREVIEW1) {AtemSwitcher.changePreviewInput(1);}
  else if (keyId == PREVIEW2) {AtemSwitcher.changePreviewInput(2);}
  else if (keyId == PREVIEW3) {AtemSwitcher.changePreviewInput(3);}
  else if (keyId == PREVIEW4) {AtemSwitcher.changePreviewInput(4);}
  else if (keyId == PREVIEW5) {AtemSwitcher.changePreviewInput(5);}
  else if (keyId == PREVIEW6) {AtemSwitcher.changePreviewInput(6);}
  else if (keyId == PREVIEW7) {AtemSwitcher.changePreviewInput(7);}
  else if (keyId == PREVIEW8) {AtemSwitcher.changePreviewInput(8);}
  else if (keyId == PREVIEWCBAR) {AtemSwitcher.changePreviewInput(1000);}
  else if (keyId == PREVIEWBLK) {AtemSwitcher.changePreviewInput(0);}
  
  // Aux - (Doesn't appear to work on a Production Studio 4K)
  else if (keyId == AUXDEBUG) {AtemSwitcher.getAuxState(1);
  AtemSwitcher.getAuxState(0);}
  else if (keyId == AUXINPUT1) {AtemSwitcher.changeAuxState(1,1);}
  else if (keyId == AUXINPUT2) {AtemSwitcher.changeAuxState(1,2);}
  else if (keyId == AUXINPUT3) {AtemSwitcher.changeAuxState(1,3);}
  else if (keyId == AUXINPUT4) {AtemSwitcher.changeAuxState(1,4);}
  else if (keyId == AUXINPUT5) {AtemSwitcher.changeAuxState(1,5);}
  else if (keyId == AUXINPUT6) {AtemSwitcher.changeAuxState(1,6);}
  else if (keyId == AUXINPUT7) {AtemSwitcher.changeAuxState(1,7);}
  else if (keyId == AUXINPUT8) {AtemSwitcher.changeAuxState(1,8);}
 
  // Fade To Black
  else if (keyId == FTBKEY) {AtemSwitcher.fadeToBlackActivate();}
  // Transition Styles
  else if (keyId == MIXKEY) {AtemSwitcher.changeTransitionType(0);}
  else if (keyId == DIPKEY) {AtemSwitcher.changeTransitionType(1);}
  else if (keyId == WIPEKEY) {AtemSwitcher.changeTransitionType(2);}
  // Transitions
  else if (keyId == TRANSKEY) {AtemSwitcher.doAuto();}
  else if (keyId == CUTKEY) {AtemSwitcher.doCut();}
}

void XKeys::OnKeyUp(uint8_t keyId)
{
    //Serial << F("Key Up: ");
    //Serial << F(keyId, DEC);
}


void resetProgramBacklight() {
  
   for(uint8_t i = 0; i < 8; i++) {
    XKs.setRowBL((i + 1), BL_RED, MODE_OFF);
  }
  for(uint8_t i = 0; i < 8; i++) {
    XKs.setRowBL((i + 1), BL_BLUE, MODE_OFF);
  }
  for (uint8_t x = 0; x < sizeof(BlueLEDGroup); x++){
          XKs.indexSetBL(BlueLEDGroup[x], BL_BLUE, MODE_ON);
          XKs.indexSetBL(BlueLEDGroup[x], BL_RED, MODE_OFF);
  }
  for (uint8_t i = 0; i < sizeof(RedLEDGroup); i++){
          XKs.indexSetBL(RedLEDGroup[i], BL_BLUE, MODE_OFF);
          XKs.indexSetBL(RedLEDGroup[i], BL_RED, MODE_ON);
  }

}

void getBacklightFromAtem(){
 
 // Transition Type
 int TransitionType = AtemSwitcher.getTransitionType();
 if (TransitionType != TransitionTypeLastState) {
      XKs.indexSetBL(MIXKEY, BL_RED, MODE_OFF);
      XKs.indexSetBL(DIPKEY, BL_RED, MODE_OFF);
      XKs.indexSetBL(WIPEKEY, BL_RED, MODE_OFF);
      XKs.indexSetBL(MIXKEY, BL_BLUE, MODE_ON);
      XKs.indexSetBL(DIPKEY, BL_BLUE, MODE_ON);
      XKs.indexSetBL(WIPEKEY, BL_BLUE, MODE_ON);
  switch (TransitionType) {
    case 0:
      XKs.indexSetBL(MIXKEY, BL_RED, MODE_ON);
      XKs.indexSetBL(MIXKEY, BL_BLUE, MODE_OFF);
      break;
    case 1:
      XKs.indexSetBL(DIPKEY, BL_RED, MODE_ON);
      XKs.indexSetBL(DIPKEY, BL_BLUE, MODE_OFF);
      break;
    case 2:
      XKs.indexSetBL(WIPEKEY, BL_RED, MODE_ON);
      XKs.indexSetBL(WIPEKEY, BL_BLUE, MODE_OFF);
      break;
   }
   TransitionTypeLastState = TransitionType;
 }

 //Fade To Black
 bool FTBState = AtemSwitcher.getFadeToBlackState();

 if (FTBState != FTBLastState) {
  if (FTBState == 0) {
    XKs.indexSetBL(FTBKEY, BL_RED, MODE_OFF);
    XKs.indexSetBL(FTBKEY, BL_BLUE, MODE_ON);
  } else {
    XKs.setFlashFreq(50);
    XKs.indexSetBL(FTBKEY, BL_BLUE, MODE_OFF);
    XKs.indexSetBL(FTBKEY, BL_RED, MODE_FLASH);
    
  }
    FTBLastState = FTBState;
 }

 // Program Inputs
 uint16_t ProgramInput = AtemSwitcher.getProgramInput();
 if (ProgramInput != ProgramInputLastState) {
  XKs.indexSetBL(PROGRAM1, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM2, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM3, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM4, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM5, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM6, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM7, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM8, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAMCBAR, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAMBLK, BL_RED, MODE_OFF);
  XKs.indexSetBL(PROGRAM1, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM2, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM3, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM4, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM5, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM6, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM7, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAM8, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAMCBAR, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PROGRAMBLK, BL_BLUE, MODE_ON);
  switch (ProgramInput) {
    case 1:
      XKs.indexSetBL(PROGRAM1, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM1, BL_BLUE, MODE_OFF);
      break;
    case 2:
      XKs.indexSetBL(PROGRAM2, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM2, BL_BLUE, MODE_OFF);
      break;
    case 3:
      XKs.indexSetBL(PROGRAM3, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM3, BL_BLUE, MODE_OFF);
      break;
    case 4:
      XKs.indexSetBL(PROGRAM4, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM4, BL_BLUE, MODE_OFF);
      break;
    case 5:
      XKs.indexSetBL(PROGRAM5, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM5, BL_BLUE, MODE_OFF);
      break;
    case 6:
      XKs.indexSetBL(PROGRAM6, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM6, BL_BLUE, MODE_OFF);
      break;
    case 7:
      XKs.indexSetBL(PROGRAM7, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM7, BL_BLUE, MODE_OFF);
      break;
    case 8:
      XKs.indexSetBL(PROGRAM8, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAM8, BL_BLUE, MODE_OFF);
      break;
    case 1000:
      XKs.indexSetBL(PROGRAMCBAR, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAMCBAR, BL_BLUE, MODE_OFF);
      break;
    case 0:
      XKs.indexSetBL(PROGRAMBLK, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAMBLK, BL_BLUE, MODE_OFF);
      break;
  }
  ProgramInputLastState = ProgramInput;
 }
 
 // Preview Inputs
 uint16_t PreviewInput = AtemSwitcher.getPreviewInput();
 if (PreviewInput != PreviewInputLastState) {
  XKs.indexSetBL(PREVIEW1, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW2, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW3, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW4, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW5, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW6, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW7, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW8, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEWCBAR, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEWBLK, BL_RED, MODE_OFF);
  XKs.indexSetBL(PREVIEW1, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW2, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW3, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW4, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW5, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW6, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW7, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEW8, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEWCBAR, BL_BLUE, MODE_ON);
  XKs.indexSetBL(PREVIEWBLK, BL_BLUE, MODE_ON);
  
  switch (PreviewInput) {
    case 1:
      XKs.indexSetBL(PREVIEW1, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW1, BL_BLUE, MODE_OFF);
      break;
    case 2:
      XKs.indexSetBL(PREVIEW2, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW2, BL_BLUE, MODE_OFF);
      break;
    case 3:
      XKs.indexSetBL(PREVIEW3, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW3, BL_BLUE, MODE_OFF);
      break;
    case 4:
      XKs.indexSetBL(PREVIEW4, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW4, BL_BLUE, MODE_OFF);
      break;
    case 5:
      XKs.indexSetBL(PREVIEW5, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW5, BL_BLUE, MODE_OFF);
      break;
    case 6:
      XKs.indexSetBL(PREVIEW6, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW6, BL_BLUE, MODE_OFF);
      break;
    case 7:
      XKs.indexSetBL(PREVIEW7, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW7, BL_BLUE, MODE_OFF);
      break;
    case 8:
      XKs.indexSetBL(PREVIEW8, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEW8, BL_BLUE, MODE_OFF);
      break;
    case 1000:
      XKs.indexSetBL(PREVIEWCBAR, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEWCBAR, BL_BLUE, MODE_OFF);
      break;
    case 0:
      XKs.indexSetBL(PREVIEWBLK, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEWBLK, BL_BLUE, MODE_OFF);
      break;
  }
  PreviewInputLastState = PreviewInput;
 }
 
}

void setup() { 

      randomSeed(analogRead(5));

  XKs.STICK = 0; // 0 for pad, 1 for stick
  XKs.PADSIZE = 80; // number of keys on your device

  XKs.init();
  resetProgramBacklight();
  
  // Start the Ethernet, Serial (debugging) and UDP:
  Ethernet.begin(mac,ip);
  Serial.begin(115200);
  Serial << F("\n- - - - - - - -\nSerial Started\n");  

  //Initialize SD Card
  Serial.print("Initializing SD card...");

  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    Serial.println("Card failed, or not present");
    
  } else {
    Serial.println("SD Card initialized");
    Serial.println("Using default configuration");

  }
   
  // Initialize a connection to the switcher:
  //AtemSwitcher.serialOutput(0x80);  // Remove or comment out this line for production code. Serial output may decrease performance!
  AtemSwitcher.connect();

  // Shows free memory:  
//  Serial << F("freeMemory()=") << freeMemory() << "\n";

// Update the LEDs every 1 second
  int tickEvent = t.every(100, getBacklightFromAtem);

}

void loop() {
  // Check for packets, respond to them etc. Keeping the connection alive!
  // VERY important that this function is called all the time - otherwise connection might be lost because packets from the switcher is
  // overlooked and not responded to.
    AtemSwitcher.runLoop();
    XKs.runLoop();

    t.update();
    
  // If connection is gone anyway, try to reconnect:
  if (AtemSwitcher.isConnectionTimedOut())  {
    //Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    AtemSwitcher.connect();
  } 
     
}
