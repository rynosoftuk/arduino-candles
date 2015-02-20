// Christmas candle program for 7x NeoPixel strip
// By rynosoftuk, November 2014
// Portions of code taken from Tim Bartlett, December 2013

#include <Adafruit_NeoPixel.h>

#define MODE1_PIN 10
#define MODE2_PIN 11
#define NEOPIXEL_PIN 12

int mode = -1;

// color variables: mix RGB (0-255) for desired yellow
int redPx = 255;
int grnHigh = 100; //110-120 for 5v, 135 for 3.3v
int bluePx = 10; //10 for 5v, 15 for 3.3v

// animation time variables, with recommendations
int burnDepth = 10; //10 for 5v, 14 for 3.3v -- how much green dips below grnHigh for normal burn - 
int flutterDepth = 25; //25 for 5v, 30 for 3.3v -- maximum dip for flutter
int cycleTime = 120; //120 -- duration of one dip in milliseconds

// pay no attention to that man behind the curtain
int fDelay;
int fRep;
int flickerDepth;
int burnDelay;
int burnLow;
int flickDelay;
int flickLow;
int flutDelay;
int flutLow;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(7, NEOPIXEL_PIN, NEO_GRB + NEO_KHZ800);

void setup() {
  pinMode(MODE1_PIN,INPUT_PULLUP);
  pinMode(MODE2_PIN,INPUT_PULLUP);
  
  flickerDepth = (burnDepth + flutterDepth) / 2.4;
  burnLow = grnHigh - burnDepth;
  burnDelay = (cycleTime / 2) / burnDepth;
  flickLow = grnHigh - flickerDepth;
  flickDelay = (cycleTime / 2) / flickerDepth;
  flutLow = grnHigh - flutterDepth;
  flutDelay = ((cycleTime / 2) / flutterDepth);
  
  strip.begin();
  strip.show();
}

// In loop, call CANDLE STATES, with duration in seconds
// 1. on() = solid yellow
// 2. burn() = candle is burning normally, flickering slightly
// 3. flicker() = candle flickers noticably
// 4. flutter() = the candle needs air!

void loop() {
  mode = readMode();
  
  switch(mode) {
    default:
    case 1:
      burn(10);
      flicker(5);
      burn(8);
      flutter(6);
      burn(3);
      on(10);
      burn(10);
      flicker(10);
      break;
      
    case 2:
      colorWipe(strip.Color(255, 0, 0), 50); // Red
      colorWipe(strip.Color(0, 255, 0), 50); // Green
      colorWipe(strip.Color(0, 0, 255), 50); // Blue
      rainbow(20);
      rainbowCycle(20);
      break;
  }
}

void readMode() {
    return !digitalRead(MODE2_PIN) ? 2 : 1;
}

void delayWithModeCheck() {
  uint16_t start = (uint16_t)micros();

  while (ms > 0) {
    // If the mode has changed
    if(readMode() != mode) {
      asm volatile ("jmp 0");
    }
    
    if (((uint16_t)micros() - start) >= 1000) {
      ms--;
      start += 1000;
    }
  }
}

/***************************************************************
* Candle Functions
***************************************************************/

// basic fire funciton - not called in main loop
void fire(int grnLow) {
  for (int grnPx = grnHigh; grnPx > grnLow; grnPx--) {
    for(int i = 0; i< 7; i++)
      strip.setPixelColor(i, redPx, grnPx, bluePx);
    strip.show();
    delayWithModeCheck(fDelay);
  }  
  for (int grnPx = grnLow; grnPx < grnHigh; grnPx++) {
    for(int i = 0; i< 7; i++)
      strip.setPixelColor(i, redPx, grnPx, bluePx);
    strip.show();
    delayWithModeCheck(fDelay);
  }
}

// fire animation
void on(int f) {
  fRep = f * 1000;
  int grnPx = grnHigh - 5;
  for(int i = 0; i< 7; i++)
    strip.setPixelColor(i, redPx, grnPx, bluePx);
  strip.show();
  delayWithModeCheck(fRep);
}

void burn(int f) {
  fRep = f * 8;
  fDelay = burnDelay;
  for (int var = 0; var < fRep; var++) {
    fire(burnLow);
  }  
}

void flicker(int f) {
  fRep = f * 8;
  fDelay = burnDelay;
  fire(burnLow);
  fDelay = flickDelay;
  for (int var = 0; var < fRep; var++) {
    fire(flickLow);
  }
  fDelay = burnDelay;
  fire(burnLow);
  fire(burnLow);
  fire(burnLow);
}

void flutter(int f) {
  fRep = f * 8;  
  fDelay = burnDelay;
  fire(burnLow);
  fDelay = flickDelay;
  fire(flickLow);
  fDelay = flutDelay;
  for (int var = 0; var < fRep; var++) {
    fire(flutLow);
  }
  fDelay = flickDelay;
  fire(flickLow);
  fire(flickLow);
  fDelay = burnDelay;
  fire(burnLow);
  fire(burnLow);
}

/********************************************************
* Rainbow Functions
********************************************************/

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delayWithModeCheck(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delayWithModeCheck(wait);
  }
}

// Slightly different, this makes the rainbow equally distributed throughout
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
    }
    strip.show();
    delayWithModeCheck(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
   WheelPos -= 170;
   return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
}
