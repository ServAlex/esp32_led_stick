
// my libs
#include "display_part.h"
#include "buttons_part.h"
//#include "wifi_part.h"
//

#include <FastLED.h>

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
#define NUM_LEDS    30
CRGB leds[NUM_LEDS];

#define BRIGHTNESS          26
#define FRAMES_PER_SECOND  (120*2*2)

#define ADC_PIN         34

int vref = 1100;

///////


uint8_t gHue = 0; // rotating "base color" used by many of the patterns
uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

// List of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
//SimplePatternList gPatterns = { solidRed, solidGreen, solidBlue};

void blank();
void runner();
void rainbow();
void fixedRainbow();
void rainbowWithGlitter();
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
SimplePatternList gPatterns = { redBlue, blank, runner, solidWhite, solidHue, solidRed, solidGreen, solidBlue, rainbow, fixedRainbow, /*confetti,*/ sinelon, juggle, bpm};

int maxBrightness = 10;
int brightness = 0;
int brightnessMultiplier = 28;

int currentMenu = 0;
int menuCount = 3;


void button2PresHandler()
{
  //Serial.println("b 1");
  currentMenu = ++currentMenu%menuCount;
  report();
}

uint8_t fpsMultiplier = 1;
uint8_t parameter8 = 0;
uint8_t continuousUpdate = 0;

void cycleOption(int dir)
{
  switch (currentMenu)
  {
    case 0:
      if(dir==-1)
        prevPattern();
      else
        nextPattern();
      break;
    case 1:
      fpsMultiplier = (fpsMultiplier + 10 - 1 + dir) % 10 + 1;
      parameter8 = (parameter8+dir+256)%256;
      break;
    case 2:
      brightness = (brightness + maxBrightness + dir)%maxBrightness;
      Serial.println(brightness*brightnessMultiplier);
      FastLED.setBrightness(brightness*brightnessMultiplier + 3);
      break;
    
    default:
      break;
  }
}

void button1PresHandler()
{
  cycleOption(1);
//  Serial.println(fpsMultiplier);
  report();
}

void buttonUpPresHandler()
{
  currentMenu = (--currentMenu+menuCount)%menuCount;
  report();
}
void buttonDownPresHandler()
{
  currentMenu = ++currentMenu%menuCount;
  report();
}
void buttonLeftPresHandler()
{
  //Serial.println("button");
  cycleOption(-1);
  report();
}
void buttonRightPresHandler()
{
  //Serial.println("button");
  cycleOption(1);
  report();
}

//void bothButtonsPressHandler() {}

void report()
{
  uint16_t v = analogRead(ADC_PIN);
  float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);

  clearScreen();
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.drawString(currentPatternName() + " " + String(gCurrentPatternNumber+1) + "/" + ARRAY_SIZE(gPatterns), tft.width() / 2, tft.height() / 2 - 30-10);
  //tft.drawString("Speed " + String(fpsMultiplier) + "/10", tft.width() / 2, tft.height() / 2-10);
  tft.drawString(parameterNameForCurrentPattern() + " " + String(parameter8), tft.width() / 2, tft.height() / 2-10);
  tft.drawString("Brghts " + String(brightness+1) + "/10", tft.width() / 2, tft.height() / 2 + 30-10);
  tft.drawString("Voltage " + String(battery_voltage), tft.width() / 2, tft.height() / 2 + 30-10 + 30);
  tft.fillCircle(5, tft.height()/2-30 + currentMenu*30-10, 3, TFT_GREEN);
  //tft.drawCircle(35, tft.height()/2-30 + currentMenu*30, 3, TFT_GREEN);
}

void setup() {

  delay(1000);

  Serial.begin(115200);
  Serial.println("Start");

  displaySetup();
  buttonsSetup();

  report();

  //delay(1000); // 3 second delay for recovery
  
  FastLED.addLeds<LED_TYPE,DATA_PIN,COLOR_ORDER>(leds, NUM_LEDS).setCorrection(TypicalLEDStrip);

  // set master brightness control
  FastLED.setBrightness(10);
 
  delay(200); 
}

uint32_t lastUpdated = 0;

void secondTask()
{
  /*
  uint32_t time = millis();
  if(lastUpdated - time > 100)
  {
    //report(); 
    //secondTask();
    lastUpdated = time;
  }
  */
}
  
void loop()
{
  gPatterns[gCurrentPatternNumber]();

  FastLED.show();  
  FastLED.delay(1000/(60)); 
  //FastLED.delay(1000/(30*fpsMultiplier)); 
  //FastLED.delay(1000/(FRAMES_PER_SECOND)); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue+=parameter8/20; } // slowly cycle the "base color" through the rainbow
//  EVERY_N_MILLISECONDS( 100 ) { report(); }
//  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
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
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void fixedRainbow() 
{
  fill_rainbow( leds, NUM_LEDS, parameter8, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
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
    leds[i] = CHSV(0, 0, 0);
}
void solidGreen()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGB(0, 255, 0);
  }
}
void solidRed()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGB(255, 0, 0);
  }
}
void solidBlue()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGB(0, 0, 255);
  }
}
void solidWhite()
{
  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
  for( int i = 0; i < NUM_LEDS; i++) { //9948
    leds[i] = CRGB(255, 255, 255);
  }
}
void solidHue()
{
  for( int i = 0; i < NUM_LEDS; i++) 
    leds[i] = CHSV(parameter8, 255, 255);
}

void redBlue()
{
  uint8_t halfNum = NUM_LEDS/2;
  uint8_t reversedParameter = 255 - parameter8;

  uint8_t lit = (reversedParameter*halfNum)/255;
  uint8_t partialLighting = (reversedParameter*halfNum)%255;

  for(int i = 0; i < halfNum; i++)
  {
    leds[i] = CRGB(255*(i<lit), 0, 0);
    leds[NUM_LEDS - i - 1] = CRGB(0, 0, 255*(i<lit));
  }

  if(partialLighting != 0)
  {
    leds[lit] = CRGB(correctIntensity(partialLighting), 0, 0);
    leds[NUM_LEDS - lit - 1] = CRGB(0, 0, correctIntensity(partialLighting));
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
  fadeToBlackBy( leds, NUM_LEDS, 60);
  leds[(pos++)%NUM_LEDS] = CHSV(0, 0, 255);
  
  //int pos = bitsaw8(189, 0, NUM_LEDS-1 );
  //leds[pos] = CHSV(0, 0, 255);
}

void juggle() {
  // eight colored dots, weaving in and out of sync with each other
  fadeToBlackBy( leds, NUM_LEDS, 20);
  byte dothue = 0;
  for( int i = 0; i < 8; i++) {
    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
    dothue += 32;
  }
}

