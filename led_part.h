#ifndef led_part_h
#define led_part_h

#include <FastLED.h>
#include "FastLED_RGBW.h"

FASTLED_USING_NAMESPACE

// FastLED "100-lines-of-code" demo reel, showing just a few 
// of the kinds of animation patterns you can quickly and easily 
// compose using FastLED.  
//
// This example also shows one easy way to define multiple 
// animations patterns and have them automatically rotate.
//
// -Mark Kriegsman, December 2014

#if defined(FASTLED_VERSION) && (FASTLED_VERSION < 3001000)
#warning "Requires FastLED 3.1 or later; check github for latest code."
#endif

#define DATA_PIN    27
//#define CLK_PIN   4
//#define LED_TYPE    WS2801
#define LED_TYPE    WS2811

#define COLOR_ORDER GRB
#define NUM_LEDS    72
//CRGB leds[NUM_LEDS];
CRGBW leds[NUM_LEDS];
CRGB *ledsRGB = (CRGB *) &leds[0];

#define BRIGHTNESS          26
#define FRAMES_PER_SECOND  (120*2*2)


uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { solidRed, solidGreen, solidBlue};

uint8_t parameter8 = 0;

void blank();
void runner();
void rainbow();
void fixedRainbow();
void rainbowWithGlitter();
void addGlitter(fract8 chanceOfGlitter);
void confetti();
void sinelon();
void bpm();
void solidGreen();
void solidRed();
void solidBlue();
void solidWhite();
void solidHue();
void juggle();
void redBlue();
uint8_t correctIntensity(uint8_t val);

SimplePatternList gPatterns = { 
    redBlue, 
    blank, 
//    runner, 
    solidWhite, 
    solidHue, 
    solidRed, 
    solidGreen, 
    solidBlue, 
    rainbow, 
    fixedRainbow, 
    /*confetti,*/ 
//    sinelon, 
//    juggle, 
    bpm
};

int maxBrightness = 10;
int brightness = 0;
int brightnessMultiplier = 28;

void led_setup()
{
/*
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);
  // set master brightness control
  FastLED.setBrightness(10);
*/
	FastLED.addLeds<WS2812B, DATA_PIN, RGB>(ledsRGB, getRGBWsize(NUM_LEDS));
	FastLED.setBrightness(brightness);
	FastLED.show();
}

void led_refresh()
{
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/(60)); 
  //FastLED.delay(1000/(30*fpsMultiplier)); 
  //FastLED.delay(1000/(FRAMES_PER_SECOND)); 
}

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void prevPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber - 1 + ARRAY_SIZE( gPatterns)) % ARRAY_SIZE( gPatterns);
}

/*
blank, runner, solidWhite, solidHue, solidRed, solidGreen, solidBlue, rainbow, sinelon, juggle, bpm
blank, solidWhite, solidRed, solidGreen, solidBlue
runner, solidHue, rainbow, confetti, sinelon, juggle, bpm
*/
String parameterNameForCurrentPattern()
{
  if(gPatterns[gCurrentPatternNumber] == runner)
    return "Speed";
  if(gPatterns[gCurrentPatternNumber] == rainbow)
    return "Speed";
  if(gPatterns[gCurrentPatternNumber] == fixedRainbow)
    return "Hue";
  //if(gPatterns[gCurrentPatternNumber] == rainbow)
  //  return "Hue delta";
  if(gPatterns[gCurrentPatternNumber] == solidHue)
    return "Hue";
  if(gPatterns[gCurrentPatternNumber] == sinelon)
    return "Hue delta";
  if(gPatterns[gCurrentPatternNumber] == bpm)
    return "Hue delta";
  if(gPatterns[gCurrentPatternNumber] == redBlue)
    return "Gap";
/*
  if(gPatterns[gCurrentPatternNumber] == )
    return "";
*/
  return "No param";
}

String currentPatternName()
{
  if(gPatterns[gCurrentPatternNumber] == solidHue)
    return "Hue";
  if(gPatterns[gCurrentPatternNumber] == solidRed)
    return "Red";
  if(gPatterns[gCurrentPatternNumber] == solidGreen)
    return "Green";
  if(gPatterns[gCurrentPatternNumber] == solidBlue)
    return "Blue";
  if(gPatterns[gCurrentPatternNumber] == rainbow)
    return "Rainbow";
  if(gPatterns[gCurrentPatternNumber] == fixedRainbow)
    return "Fix Rainbow";
  if(gPatterns[gCurrentPatternNumber] == sinelon)
    return "Sinelon";
  if(gPatterns[gCurrentPatternNumber] == juggle)
    return "Juggle";
  if(gPatterns[gCurrentPatternNumber] == bpm)
    return "BPM";
  if(gPatterns[gCurrentPatternNumber] == solidWhite)
    return "White";
  if(gPatterns[gCurrentPatternNumber] == runner)
    return "Runner";
  if(gPatterns[gCurrentPatternNumber] == blank)
    return "None";
  if(gPatterns[gCurrentPatternNumber] == redBlue)
    return "Red/Blue";
  return "";
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  //fill_rainbow( leds, NUM_LEDS, gHue, 7);

	for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CHSV((i * 256 / NUM_LEDS) + gHue, 255, 255);
	}
	FastLED.show();
	gHue++;
}

void fixedRainbow() 
{
//  fill_rainbow( leds, NUM_LEDS, parameter8, 7);
	for(int i = 0; i < NUM_LEDS; i++){
		leds[i] = CHSV((i * 256 / NUM_LEDS) + gHue, 255, 255);
	}
	FastLED.show();
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}
void addGlitter( fract8 chanceOfGlitter) 
{
/*
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGBW(255, 255, 255, 255);
  }
*/
}
void confetti() 
{
/*
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
*/
}
void sinelon()
{
/*
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
*/
}

void bpm()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  uint8_t BeatsPerMinute = 62;
  CRGBPalette16 palette = PartyColors_p;
  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
  }
}

void blank()
{
  for(int i=0; i<NUM_LEDS; i++)
    leds[i] = CRGBW(0, 0, 0);
    //leds[i] = CHSV(0, 0, 0);
}
void solidGreen()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGBW(0, 255, 0);
  }
}
void solidRed()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGBW(255, 0, 0);
  }
}
void solidBlue()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGBW(0, 0, 255);
  }
}
void solidWhite()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGBW(255, 255, 255, 255);
  }
}
void solidHue()
{
  for( int i = 0; i < NUM_LEDS; i++) 
  {
    leds[i] = CHSV(parameter8, 255, 255);
  }
}

void redBlue()
{
  uint8_t halfNum = NUM_LEDS/2;
  uint8_t reversedParameter = 255 - parameter8;

  uint8_t lit = (reversedParameter*halfNum)/255;
  uint8_t partialLighting = (reversedParameter*halfNum)%255;

  for(int i = 0; i < halfNum; i++)
  {
    leds[i] = CRGBW(255*(i<lit), 0, 0);
    leds[NUM_LEDS - i - 1] = CRGBW(0, 0, 255*(i<lit));
  }

  if(partialLighting != 0)
  {
    leds[lit] = CRGB(correctIntensity(partialLighting), 0, 0);
    leds[NUM_LEDS - lit - 1] = CRGBW(0, 0, correctIntensity(partialLighting));
  }
}

uint8_t correctIntensity(uint8_t val)
{
  uint8_t min = 0;
  uint8_t max = 255-100;
  
  return (val*(max-min))/255 + min;
}

/// beatsin8 generates an 8-bit sine wave at a given BPM,
///           that oscillates within a given range.
static inline uint8_t bitsaw8( accum88 beats_per_minute, uint8_t lowest = 0, uint8_t highest = 255,
                            uint32_t timebase = 0, uint8_t phase_offset = 0)
{
    uint8_t beat = beat8( beats_per_minute, timebase);
    uint8_t rangewidth = highest - lowest;
    uint8_t scaledbeat = scale8( beat, rangewidth);
    uint8_t result = lowest + scaledbeat;
    return result;
}

int pos = 0;
void runner()
{
/*
  fadeToBlackBy( leds, NUM_LEDS, 60);
  leds[(pos++)%NUM_LEDS] = CHSV(0, 0, 255);
  
  //int pos = bitsaw8(189, 0, NUM_LEDS-1 );
  //leds[pos] = CHSV(0, 0, 255);
*/
}
void juggle() {
/*
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
*/
}

#endif
