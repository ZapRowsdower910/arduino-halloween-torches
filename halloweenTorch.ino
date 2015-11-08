#include <avr/power.h>
#include <avr/sleep.h>
#include <Adafruit_NeoPixel.h> 

// Sketch configuration:
#define PIXEL_PIN      1    // Pin connected to the NeoPixel strip.
#define PIXEL_COUNT    10
#define PIXEL_TYPE     NEO_GRB + NEO_KHZ800
#define IR_PIN         2    // Pin connected to the IR receiver.

// Adafruit IR Remote Codes:
//   Button       Code         Button  Code
//   -----------  ------       ------  -----
//   VOL-:        0x0000       0/10+:  0x000C
//   Play/Pause:  0x0001       1:      0x0010
//   VOL+:        0x0002       2:      0x0011
//   SETUP:       0x0004       3:      0x0012
//   STOP/MODE:   0x0006       4:      0x0014
//   UP:          0x0005       5:      0x0015
//   DOWN:        0x000D       6:      0x0016
//   LEFT:        0x0008       7:      0x0018
//   RIGHT:       0x000A       8:      0x0019
//   ENTER/SAVE:  0x0009       9:      0x001A
//   Back:        0x000E
//#define COLOR_CHANGE      0x000A   // Button that cycles through color animations.
//#define ANIMATION_CHANGE  0x0008   // Button that cycles through animation types (only two supported).
//#define SPEED_CHANGE      0x0005   // Button that cycles through speed choices.
#define POWER_OFF         0x0000   // Button that turns off/sleeps the pixels.
#define POWER_ON          0x0002   // Button that turns on the pixels.  Must be pressed twice to register!
#define BTN_1             0x0010
#define BTN_2             0x0011
#define BTN_3             0x0012
#define BTN_4             0x0014

#define OFF 0x0000

// Global state used by the sketch:
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
volatile bool receiverFell = false;

//const uint32_t OFF = strip.Color(0,0,0);
const uint32_t color = strip.Color(200,0,250);
const uint32_t RED = strip.Color(50, 0, 0);

short flameState = 0;
short currentStep = 0;

void setup(void) {
  // Setup IR receiver pin as an input.
  pinMode(IR_PIN, INPUT);
  // Initialize and clear the NeoPixel strip.
  strip.begin();
  strip.clear();
  strip.show(); // Initialize all pixels to 'off'
  // Enable an interrupt that's called when the IR receiver pin signal falls
  // from high to low.  This indicates a remote control code being received.
  attachInterrupt(0, receiverFalling, FALLING);
}

void loop(void) {
  // Main loop will first update all the pixels based on the current animation.

  // lower
  // mid
  
  
  switch(flameState){
    case 0: {
      switch(currentStep) {
        case 0:{
          // Bottom
          strip.setPixelColor(9, RED);
          strip.setPixelColor(8, RED);

          strip.setPixelColor(0, RED);
          strip.setPixelColor(7, OFF);

          strip.setPixelColor(6, OFF);
          strip.setPixelColor(1, OFF);
          // mid
          strip.setPixelColor(5, OFF);
          strip.setPixelColor(2, OFF);
          // peak
          strip.setPixelColor(4, OFF);
          strip.setPixelColor(3, OFF);
          break;
        }
      }
      break;
    }
    case 1: {
      switch(currentStep) {
        case 0: {
          // Bottom
          strip.setPixelColor(9, color);
          strip.setPixelColor(8, color);
          strip.setPixelColor(0, color);
          
          strip.setPixelColor(7, OFF);
          strip.setPixelColor(1, OFF);

          strip.setPixelColor(6, OFF);
          strip.setPixelColor(2, OFF);
          // peak
          strip.setPixelColor(4, OFF);
          strip.setPixelColor(3, OFF);
          strip.setPixelColor(5, OFF);

          break;
        }
        case 1:{
          // Bottom
          strip.setPixelColor(9, color);
          strip.setPixelColor(8, color);
          strip.setPixelColor(0, color);
          // lower
          strip.setPixelColor(7, color);
          strip.setPixelColor(1, color);

          strip.setPixelColor(6, OFF);
          strip.setPixelColor(2, OFF);
          // peak
          strip.setPixelColor(4, OFF);
          strip.setPixelColor(3, OFF);
          strip.setPixelColor(5, OFF);
          break;
        }
      }
      break;
    }
    case 2: {
      switch(currentStep) {
        case 0:
          // Bottom
          strip.setPixelColor(9, color);
          strip.setPixelColor(8, color);
          strip.setPixelColor(0, color);
          // lower
          strip.setPixelColor(7, color);
          strip.setPixelColor(1, color);
          // mid
          strip.setPixelColor(6, OFF);
          strip.setPixelColor(2, OFF);

          strip.setPixelColor(4, OFF);
          strip.setPixelColor(3, OFF);
          strip.setPixelColor(5, OFF);
          
          break;
        case 3:
          // Bottom
          strip.setPixelColor(9, color);
          strip.setPixelColor(8, color);
          strip.setPixelColor(0, color);
          // lower
          strip.setPixelColor(7, color);
          strip.setPixelColor(1, color);
          // mid
          strip.setPixelColor(6, color);
          strip.setPixelColor(2, color);

          strip.setPixelColor(4, OFF);
          strip.setPixelColor(3, OFF);
          strip.setPixelColor(5, OFF);
          
          break;
        
        case 2:
        case 4:
          // Bottom
          strip.setPixelColor(9, color);
          strip.setPixelColor(8, color);
          strip.setPixelColor(0, color);
          // lower
          strip.setPixelColor(7, color);
          strip.setPixelColor(1, color);
          // mid
          strip.setPixelColor(6, color);
          strip.setPixelColor(2, color);
          // peak
          strip.setPixelColor(4, color);
          strip.setPixelColor(3, color);

          strip.setPixelColor(5, color);
          break;
        
      }
      
      break;
    }
  }

  currentStep = random(0,4);

  // Next check for any IR remote commands.
  handleRemote();
  // Show the updated pixels.
  strip.show();
  // Again check for IR remote commands.
  handleRemote();
}


void receiverFalling() {
  // Interrupt function that's called when the IR receiver pin falls from high to
  // low and indicates the start of an IR command being received.  Note that
  // interrupts need to be very fast and perform as little processing as possible.
  // This just sets a global variable which the main loop will periodically check
  // and perform appropriate IR command decoding when necessary.
  receiverFell = true;
}

bool readNEC(uint16_t* result) {
  // Check if a NEC IR remote command can be read and decoded from the IR receiver.
  // If the command is decoded then the result is stored in the provided pointer and
  // true is returned.  Otherwise if the command was not decoded then false is returned.
  // First check that a falling signal was detected and start reading pulses.
  if (!receiverFell) {
    return false;
  }
  // Read the first pulse with a large timeout since it's 9ms long, then
  // read subsequent pulses with a shorter 2ms timeout.
  uint32_t durations[33];
  durations[0] = pulseIn(IR_PIN, HIGH, 20000);
  for (uint8_t i = 1; i < 33; ++i) {
    durations[i] = pulseIn(IR_PIN, HIGH, 5000);
  }
  // Reset any state changed by the interrupt.
  receiverFell = false;
  // Check the received pulses are in a NEC format.
  // First verify the initial pulse is around 4.5ms long.
  if ((durations[0] < 4000) || (durations[1] > 5000)) {
    return false;
  }
  // Now read the bits from subsequent pulses.  Stop if any were a timeout (0 value).
  uint8_t data[4] = {0};
  for (uint8_t i=0; i<32; ++i) {
    if (durations[1+i] == 0) {
      return false; // Timeout
    }
    uint8_t b = durations[1+i] < 1000 ? 0 : 1;
    data[i/8] |= b << (i%8);
  }
  // Verify bytes and their inverse match.  Use the same two checks as the NEC IR remote
  // library here: https://github.com/adafruit/Adafruit-NEC-remote-control-library
  if ((data[0] == (~data[1] & 0xFF)) && (data[2] == (~data[3] & 0xFF))) {
    *result = data[0] << 8 | data[2];
    return true;
  }
  else if ((data[0] == 0) && (data[1] == 0xBF) && (data[2] == (~data[3] & 0xFF))) {
    *result = data[2];
    return true;
  }
  else {
    // Something didn't match, fail!
    return false;
  }
}

void handleRemote() {
  // Check if an IR remote code was received and perform the appropriate action.
  // First read a code.
  uint16_t irCode;
  if (!readNEC(&irCode)) {
    return;
  }
  switch(irCode) {
    case BTN_1: 
      flameState = 0;
      break;
    case BTN_2: 
      flameState = 1;
      break;
    case BTN_3: 
      flameState = 2;
      break;
  
    case POWER_OFF:
      // Enter full power down sleep mode.
      // First turn off the NeoPixels.
      strip.clear();
      strip.show();
      while (true) {
        // Next disable the current falling interrupt and enable a low value interrupt.
        // This is required because the ATtiny85 can't wake from a falling interrupt
        // and instead can only wake from a high or low value interrupt.
        detachInterrupt(0);
        attachInterrupt(0, receiverFalling, LOW);
        // Now enter full power down sleep mode.
        power_all_disable();
        set_sleep_mode(SLEEP_MODE_PWR_DOWN);
        sleep_mode();
        // Processor is currently asleep and will wake up when the IR receiver pin
        // is at a low value (i.e. when a IR command is received).
        // Sleep resumes here.  When awake power everything back up.
        power_all_enable();
        // Re-enable the falling interrupt.
        detachInterrupt(0);
        attachInterrupt(0, receiverFalling, FALLING);
        // Now wait up to 1 second for a second power on command to be received.
        // This is necessary because the first power on command will wake up the CPU
        // but happens a little too quickly to be reliably read.
        for (int i=0; i<200; ++i) {
          uint16_t irCode;
          if ((readNEC(&irCode) == 1) && (irCode == POWER_ON)) {
            // Second power on command was received, jump out of the power on loop and
            // return to normal operation.
            return;
          }
          delay(5);
        }
        // If no power on command was received within 1 second of awaking then the
        // code will loop back to the top and go to sleep again.
      }
  }
}


