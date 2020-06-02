
// my libs
#include "display_part.h"
#include "buttons_part.h"
//#include "wifi_part.h"
#include "led_part.h"

#define ADC_PIN         34

int vref = 1100;

int currentMenu = 0;
int menuCount = 3;

void button2PresHandler()
{
    currentMenu = ++currentMenu%menuCount;
    report();
}

uint8_t fpsMultiplier = 1;
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

uint32_t lastUpdated = 0;

void report()
{
    uint16_t v = analogRead(ADC_PIN);
    float battery_voltage = ((float)v / 4095.0) * 2.0 * 3.3 * (vref / 1000.0);

    tft.setTextColor(TFT_GREEN, TFT_BLACK);
    int d = 25;
    clearScreen();

    drawStringWithOffset(currentPatternName() + " " + String(gCurrentPatternNumber+1) + "/" + ARRAY_SIZE(gPatterns), 0, -2*d);
    drawStringWithOffset(parameterNameForCurrentPattern() + " " + String(parameter8), 0, -d);
    drawStringWithOffset("Brghts " + String(brightness+1) + "/10", 0, 0);
    //drawStringWithOffset(, 0, d);
    drawStringWithOffset("Voltage " + String(battery_voltage), 0, 2*d);

    tft.fillCircle(5, tft.height()/2-d*2 + currentMenu*d, 3, TFT_GREEN);
}


void setup() {

    delay(1000);

    Serial.begin(115200);
    Serial.println("Start");

    lastUpdated = millis();

    displaySetup();
    buttonsSetup();

    report();

    led_setup();
    delay(200); 
}

void secondTask()
{
    uint32_t time = millis();
    if(time - lastUpdated > 10000)
    {
        report(); 
        lastUpdated = time;
    }
}
  
void loop()
{
    led_refresh();
  // do some periodic updates
  //  EVERY_N_MILLISECONDS( 20 ) { gHue+=parameter8/20; } // slowly cycle the "base color" through the rainbow
  //  EVERY_N_SECONDS( 10 ) { nextPattern(); } // change patterns periodically
}
