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


// DEBUG
bool SERIALDEBUG = true;
bool DEBUGONLY = true;
bool SDCARD = false;


// Defaults
static uint8_t default_ip[] = {     // Default IP of Arduino
  192, 168, 0, 99 };
uint8_t ip[4];        // IP of the Arduino
uint8_t atem_ip[4];  // IP of the ATEM
uint8_t mac[6];    // MAC Address of the Arduino

bool BacklightActive = true;
bool BacklightInactive = false;



// ATEM

ATEM AtemSwitcher;

// Track ATEM Features 

int TransitionTypeLastState = 6; //Out of range to force an update on load

bool FTBLastState;
bool ATEMOnline=false;
bool TransPrev=false;
bool TransPrevLED=false;

uint16_t AuxState;
uint16_t ProgramInputLastState;
uint16_t PreviewInputLastState;






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




// Keys

// Misc Keys
uint8_t FTBKEY = 31;
uint8_t MIXKEY = 0;
uint8_t DIPKEY = 8;
uint8_t WIPEKEY = 16;
uint8_t TRANSKEY = 55;
uint8_t CUTKEY = 47;
uint8_t TRANSPREV = 39;

// Program Control
uint8_t PROGRAM1 = 67;
uint8_t PROGRAM2 = 75;
uint8_t PROGRAM3 = 68;
uint8_t PROGRAM4 = 76;
uint8_t PROGRAM5 = 69;
uint8_t PROGRAM6 = 77;
uint8_t PROGRAM7 = 70;
uint8_t PROGRAM8 = 78;

uint8_t PROGRAMCBAR = 79;
uint8_t PROGRAMBLK = 71;

uint8_t PROGRAMMP1 = 35;
uint8_t PROGRAMMP2 = 36;
uint8_t PROGRAMCOL1 = 37;
uint8_t PROGRAMCOL2 = 38;

// Preview Control
uint8_t PREVIEW1 = 3;
uint8_t PREVIEW2 = 11;
uint8_t PREVIEW3 = 4;
uint8_t PREVIEW4 = 12;
uint8_t PREVIEW5 = 5;
uint8_t PREVIEW6 = 13;
uint8_t PREVIEW7 = 6;
uint8_t PREVIEW8 = 14;

uint8_t PREVIEWCBAR = 15;
uint8_t PREVIEWBLK = 7;

uint8_t PREVIEWMP1 = 27;
uint8_t PREVIEWMP2 = 28;
uint8_t PREVIEWCOL1 = 29;
uint8_t PREVIEWCOL2 = 30;

// Aux Control
//uint8_t AUXDEBUG = 51;
//uint8_t AUXPROGRAM = 27;
//uint8_t AUXPEVIEW = 35;
//uint8_t AUXINPUT1 = 53;
//uint8_t AUXINPUT2 = 45;
//uint8_t AUXINPUT3 = 37;
//uint8_t AUXINPUT4 = 29;
//uint8_t AUXINPUT5 = 52;
//uint8_t AUXINPUT6 = 44;
//uint8_t AUXINPUT7 = 36;
//uint8_t AUXINPUT8 = 28;


// LED Groups

// only show backlights on the keys I'm using.
  //uint8_t BlueLEDGroup[] = {7,6,5,4,64,56,48,40,32,24,16,8,55,47,65,57,49,41,33,25,17,9,39,31};
  uint8_t BlueLEDGroup[] = {46,47,54,55};
  //uint8_t RedLEDGroup[] = {0,1,72,73};
  
Timer t;

void XKeys::OnKeyDown(uint8_t keyId)
{
  if (SERIALDEBUG == true) {
    Serial << F("Key down: ");
    Serial.println(keyId,DEC);
    Serial.println(keyId);
  }
    
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
  else if (keyId == PROGRAMMP1) {AtemSwitcher.changeProgramInput(3010);}
  else if (keyId == PROGRAMMP2) {AtemSwitcher.changeProgramInput(3020);}
  else if (keyId == PROGRAMCOL1) {AtemSwitcher.changeProgramInput(2001);}
  else if (keyId == PROGRAMCOL2) {AtemSwitcher.changeProgramInput(2002);}
  
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
  else if (keyId == PREVIEWMP1) {AtemSwitcher.changePreviewInput(3010);}
  else if (keyId == PREVIEWMP2) {AtemSwitcher.changePreviewInput(3020);}
  else if (keyId == PREVIEWCOL1) {AtemSwitcher.changePreviewInput(2001);}
  else if (keyId == PREVIEWCOL2) {AtemSwitcher.changePreviewInput(2002);}
  
  // Aux - (Doesn't appear to work on a Production Studio 4K)
  //else if (keyId == AUXDEBUG) {AtemSwitcher.getAuxState(1);
  //AtemSwitcher.getAuxState(0);}
  //else if (keyId == AUXINPUT1) {AtemSwitcher.changeAuxState(1,1);}
  //else if (keyId == AUXINPUT2) {AtemSwitcher.changeAuxState(1,2);}
  //else if (keyId == AUXINPUT3) {AtemSwitcher.changeAuxState(1,3);}
  //else if (keyId == AUXINPUT4) {AtemSwitcher.changeAuxState(1,4);}
  //else if (keyId == AUXINPUT5) {AtemSwitcher.changeAuxState(1,5);}
  //else if (keyId == AUXINPUT6) {AtemSwitcher.changeAuxState(1,6);}
  //else if (keyId == AUXINPUT7) {AtemSwitcher.changeAuxState(1,7);}
  //else if (keyId == AUXINPUT8) {AtemSwitcher.changeAuxState(1,8);}
 
  // Fade To Black
  else if (keyId == FTBKEY) {AtemSwitcher.fadeToBlackActivate();}
  // Transition Styles
  else if (keyId == MIXKEY) {AtemSwitcher.changeTransitionType(0);}
  else if (keyId == DIPKEY) {AtemSwitcher.changeTransitionType(1);}
  else if (keyId == WIPEKEY) {AtemSwitcher.changeTransitionType(2);}
  // Transitions
  else if (keyId == TRANSKEY) {AtemSwitcher.doAuto();}
  else if (keyId == CUTKEY) {AtemSwitcher.doCut();}
  else if (keyId == TRANSPREV) {
    if (TransPrev == true) {
      AtemSwitcher.changeTransitionPreview(false);
      TransPrev = false;
    } else {
      
      AtemSwitcher.changeTransitionPreview(true);
      TransPrev = true;
    }
  }
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
  //if (BacklightInactive == true) {
    for (uint8_t x = 0; x < sizeof(BlueLEDGroup); x++){
            XKs.indexSetBL(BlueLEDGroup[x], BL_BLUE, MODE_ON);
            XKs.indexSetBL(BlueLEDGroup[x], BL_RED, MODE_OFF);
    }
   // for (uint8_t i = 0; i < sizeof(RedLEDGroup); i++){
   //         XKs.indexSetBL(RedLEDGroup[i], BL_BLUE, MODE_OFF);
   //         XKs.indexSetBL(RedLEDGroup[i], BL_RED, MODE_ON);
   // }
  //}

}

void getBacklightFromAtem(){

 //Get Data
 
 int TransitionType = AtemSwitcher.getTransitionType();
 
 bool FTBState = AtemSwitcher.getFadeToBlackState();
 bool TransitionPreview = AtemSwitcher.getTransitionPreview();
 uint16_t ProgramInput = AtemSwitcher.getProgramInput();
 uint16_t PreviewInput = AtemSwitcher.getPreviewInput();
 
 
 // Transition Type
 if (TransitionType != TransitionTypeLastState) {
      XKs.indexSetBL(MIXKEY, BL_RED, MODE_OFF);
      XKs.indexSetBL(DIPKEY, BL_RED, MODE_OFF);
      XKs.indexSetBL(WIPEKEY, BL_RED, MODE_OFF);
      if (BacklightInactive == true) {
        XKs.indexSetBL(MIXKEY, BL_BLUE, MODE_ON);
        XKs.indexSetBL(DIPKEY, BL_BLUE, MODE_ON);
        XKs.indexSetBL(WIPEKEY, BL_BLUE, MODE_ON);
      }
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

 if (FTBState != FTBLastState) {
  if (FTBState == 0) {
    XKs.indexSetBL(FTBKEY, BL_RED, MODE_OFF);
    if (BacklightInactive == true) {
      XKs.indexSetBL(FTBKEY, BL_BLUE, MODE_ON);
    }
  } else {
    XKs.setFlashFreq(50);
    XKs.indexSetBL(FTBKEY, BL_BLUE, MODE_OFF);
    XKs.indexSetBL(FTBKEY, BL_RED, MODE_FLASH);
    
  }
    FTBLastState = FTBState;
 }
 if (TransitionPreview != TransPrevLED) {
   if (TransitionPreview == true) {
     XKs.indexSetBL(TRANSPREV, BL_BLUE, MODE_OFF);
     XKs.indexSetBL(TRANSPREV, BL_RED, MODE_ON);
     TransPrev = true;
     TransPrevLED = true;
   } else {
     XKs.indexSetBL(TRANSPREV, BL_RED, MODE_OFF);
     TransPrev = false;
     TransPrevLED = false;
    
   }
 }

 // Program Inputs
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
  XKs.indexSetBL(PROGRAMMP1, BL_RED, MODE_OFF);;
  XKs.indexSetBL(PROGRAMMP2, BL_RED, MODE_OFF);;
  XKs.indexSetBL(PROGRAMCOL1, BL_RED, MODE_OFF);;
  XKs.indexSetBL(PROGRAMCOL2, BL_RED, MODE_OFF);
  if (BacklightInactive == true) {
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
    XKs.indexSetBL(PROGRAMMP1, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PROGRAMMP2, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PROGRAMCOL1, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PROGRAMCOL2, BL_BLUE, MODE_ON);
  }
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
    case 3010:
      XKs.indexSetBL(PROGRAMMP1, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAMMP1, BL_BLUE, MODE_OFF);
      break;
    case 3020:
      XKs.indexSetBL(PROGRAMMP2, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAMMP2, BL_BLUE, MODE_OFF);
      break;
    case 2001:
      XKs.indexSetBL(PROGRAMCOL1, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAMCOL1, BL_BLUE, MODE_OFF);
      break;
    case 2002:
      XKs.indexSetBL(PROGRAMCOL2, BL_RED, MODE_ON);
      XKs.indexSetBL(PROGRAMCOL2, BL_BLUE, MODE_OFF);
      break;
  }
  ProgramInputLastState = ProgramInput;
 }
 
 // Preview Inputs
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
  XKs.indexSetBL(PREVIEWMP1, BL_RED, MODE_OFF);;
  XKs.indexSetBL(PREVIEWMP2, BL_RED, MODE_OFF);;
  XKs.indexSetBL(PREVIEWCOL1, BL_RED, MODE_OFF);;
  XKs.indexSetBL(PREVIEWCOL2, BL_RED, MODE_OFF);
  if (BacklightInactive == true) {
    XKs.indexSetBL(PREVIEW1, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW2, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW3, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW4, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW5, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW6, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW7, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEW8, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEWCBAR, BL_BLUE, MODE_ON);
    XKs.indexSetBL(PREVIEWBLK, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PREVIEWMP1, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PREVIEWMP2, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PREVIEWCOL1, BL_BLUE, MODE_ON);;
    XKs.indexSetBL(PREVIEWCOL2, BL_BLUE, MODE_ON);
  }
  
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
    case 3010:
      XKs.indexSetBL(PREVIEWMP1, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEWMP1, BL_BLUE, MODE_OFF);
      break;
    case 3020:
      XKs.indexSetBL(PREVIEWMP2, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEWMP2, BL_BLUE, MODE_OFF);
      break;
    case 2001:
      XKs.indexSetBL(PREVIEWCOL1, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEWCOL1, BL_BLUE, MODE_OFF);
      break;
    case 2002:
      XKs.indexSetBL(PREVIEWCOL2, BL_RED, MODE_ON);
      XKs.indexSetBL(PREVIEWCOL2, BL_BLUE, MODE_OFF);
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
  
  if (SERIALDEBUG == true){
    // Start Serial (debugging)

    Serial.begin(115200);
    Serial << F("\n- - - - - - - -\nSerial Started\n");  
  }
  
  //Initialize SD Card
  if (SERIALDEBUG == true){
    Serial.println("Initializing SD card...");
  }
  // see if the card is present and can be initialized:
  if (!SD.begin(chipSelect)) {
    if (SERIALDEBUG == true){
      Serial.println("Card failed, or not present");
    }
    SDCARD = false;
    
    
  } else {
    if (SERIALDEBUG == true){
      Serial.println("SD Card found");
      Serial.println("Checking for atemconfig.txt");
      
    }
    if (SD.exists("atemconfig.txt")) {
      if (SERIALDEBUG == true){
        Serial.println("found atemconfig.txt");
      }
      SDCARD = true;
    } else {
      if (SERIALDEBUG == true){
        Serial.println("atemconfig.txt not found");
      }
      SDCARD = false;
    }
  }



  if (SDCARD == false) {
    if (SERIALDEBUG == true){
      Serial.println("Using default configuration");
    }
    
    atem_ip[0] = 192;
    atem_ip[1] = 168;
    atem_ip[2] = 0;
    atem_ip[3] = 240;
    
    ip[0] = default_ip[0];
    ip[1] = default_ip[1];
    ip[2] = default_ip[2];
    ip[3] = default_ip[3];
  }

  
  // MAC address and IP address for this *particular* Arduino / Ethernet Shield!
  // The MAC address is printed on a label on the shield or on the back of your device
  // The IP address should be an available address you choose on your subnet where the switcher is also present
  byte mac[] = { 
    0x90, 0xA2, 0xDA, 0x0D, 0x6B, 0xB9 };      // <= SETUP!  MAC address of the Arduino
  IPAddress ip(192, 168, 0, 99);              // <= SETUP!  IP address of the Arduino

  // Include ATEM library and make an instance:
  // Connect to an ATEM switcher on this address and using this local port:
  // The port number is chosen randomly among high numbers.
  
  AtemSwitcher.begin(IPAddress(atem_ip[0], atem_ip[1], atem_ip[2], atem_ip[3]), 56417);  // <= SETUP (the IP address of the ATEM switcher)
  
  //Start Ethernet
  Ethernet.begin(mac,ip);

   
  // Initialize a connection to the switcher:
  if (SERIALDEBUG == true){
    AtemSwitcher.serialOutput(0x80);  // Remove or comment out this line for production code. Serial output may decrease performance!
  }
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
    if (SERIALDEBUG == true){
      Serial << F("Connection to ATEM Switcher has timed out - reconnecting!\n");
    }
    AtemSwitcher.connect();
  } 
     
}
