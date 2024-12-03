/*****************************************************************************
 * E-chaplet: an electronic chaplet to help a group of pepople to pray
 * together without the need of each person be holding his/her own chaplet.
 * The e-chaplet is operated by a remote 433 MHz RF controller. Basically, the
 * controller has 2 buttons, one to advance the state of the pray and other to
 * go back to the previous state. Each state is represented by an RGB LED.
 *
 *
 *
 * Copyright (C) 2024  Rafael R. Queiroz   rafaelrq@gmail.com
 *
 * This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program.  If not, see <https://www.gnu.org/licenses/>.
 *
 *****************************************************************************/

/*****************************************************************************
 * Included Files                                                            *
 *****************************************************************************/

#include <RCSwitch.h>
#include <FastLED.h>
#include <EEPROM.h>

/*****************************************************************************
 * Private Definitions                                                       *
 *****************************************************************************/

/* Pin used to calibrate RF controller is PIN D12 */

#define RF_CTRL_CONFIG                              12

/* Needed amount of time in ms holding the config button to trigger RF
 * controller calibration
 */

#define RF_CONFIG_TIMEOUT_MS                        3000UL

/* Address to store RF OP codes in EEPROM */

#define RF_OP_CODES_ADDR                            128


#define RGB_OUR_FATHER_ADDR                         \
(RF_OP_CODES_ADDR + NUM_OP_CODES * sizeof(unsigned long))

#define RGB_HAIL_MARY_ADDR                          \
(RGB_OUR_FATHER_ADDR + sizeof(CRGB))

#define RGB_FORWARD_BUTTON_ADDR                     \
(RGB_HAIL_MARY_ADDR + sizeof(CRGB))

#define RGB_BACKWARD_BUTTON_ADDR                    \
(RGB_FORWARD_BUTTON_ADDR + sizeof(CRGB))


/* Pin used to drive the LED strip is PIN D6 */

#define LED_PIN                                     6

/* The chaplet's LED strip is composed of 61 LEDs */

#define NUM_LEDS                                    61

#define NUM_OP_CODES                                2

/* Brightness LED on */

#define BRIGHTNESS                                  64

/* Brightness LED off */

#define BRIGHTNESS_IDLE                             5

/* Used LED controller is WS2811 */

#define LED_TYPE                                    WS2811

/* The order of the RGB color code used by the controller */

#define COLOR_ORDER                                 GRB


enum AppState {
    APP_STATE_CHAPLET,
    APP_STATE_RF_CALIBRATION,
};



/*****************************************************************************
 * Private Function Prototypes                                               *
 *****************************************************************************/

static void setAllLEDs(const CRGB &rgb);
static void fadeUp(uint8_t minBrightness, uint8_t maxBrightness);
static void fadeDown(uint8_t minBrightness, uint8_t maxBrightness);

static inline void initLeds();
static inline bool checkForRfCalibration();
static inline void runChaplet();
static inline void runRfCalibration();
static inline void initRcSwitch();

/*****************************************************************************
 * Private Data                                                              *
 *****************************************************************************/


static RCSwitch mSwitch;
static bool rfCtrlState;
static unsigned long pressInstant; 
static uint8_t appState;
static unsigned long rfOpCodes[NUM_OP_CODES];
static uint8_t ledIdx;
static uint8_t opCodeIdx;
static CRGB leds[NUM_LEDS];
static CRGB ourFatherRGB;
static CRGB hailMaryRGB;
static CRGB forwardButtonRGB;
static CRGB backwardButtonRGB;

/*****************************************************************************
 * Private Functions                                                         *
 *****************************************************************************/

static void setAllLEDs(const CRGB &rgb)
{
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = rgb;
    }

    FastLED.show();
}

static void fadeUp(uint8_t minBrightness, uint8_t maxBrightness)
{
    uint8_t scale = minBrightness;
    FastLED.setBrightness(scale);
    FastLED.show();

    while (scale < maxBrightness) {
        FastLED.setBrightness(++scale);
        FastLED.show();
        delay(5);
    }
}


static void fadeDown(uint8_t minBrightness, uint8_t maxBrightness)
{
    uint8_t scale = maxBrightness;
    FastLED.setBrightness(scale);
    FastLED.show();

    while (scale > minBrightness) {
        FastLED.setBrightness(--scale);
        FastLED.show();
        delay(10);
    }
}


static inline void initLeds()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    FastLED.clearData();
    
    setAllLEDs(CRGB::White);

    for (int i = 2; i >= 0; i--) {
        fadeDown(0, BRIGHTNESS);
        fadeUp(0, BRIGHTNESS);
    }

    fadeDown(BRIGHTNESS_IDLE, BRIGHTNESS);
}


static inline bool checkForRfCalibration()
{
    if (rfCtrlState) {
        if (digitalRead(RF_CTRL_CONFIG) == HIGH) {
            rfCtrlState = false;
        }
        else if (millis() - pressInstant >= RF_CONFIG_TIMEOUT_MS) {
            rfCtrlState = false;
            return true;
        }
    }
    else if (digitalRead(RF_CTRL_CONFIG) == LOW) {
        rfCtrlState  = true;
        pressInstant = millis();
    }

    return false;
}


static inline void forwardLED()
{
    
}


static inline void backwardLED()
{

}


static inline void runChaplet()
{
    if (checkForRfCalibration()) {
        /* We must put the system in RF calibration mode */

        appState = APP_STATE_RF_CALIBRATION;
        FastLED.setBrightness(BRIGHTNESS);
        setAllLEDs(forwardButtonRGB);

        return;
    }

    /* Do we need to update the chaplet LEDs? */

    if (mSwitch.available()) {
        /* Yes, we do */

        if (mSwitch.getReceivedValue() == rfOpCodes[0]) {
            forwardLED();
        }
        else if (mSwitch.getReceivedValue() == rfOpCodes[1]) {
            backwardLED();
        }

        mSwitch.resetAvailable();
    }

}


static inline void runRfCalibration()
{
    if (mSwitch.available()) {
        rfOpCodes[opCodeIdx++] = mSwitch.getReceivedValue();

        mSwitch.resetAvailable();

        for (int i = 1; i >= 0; i--) {
            fadeDown(0, BRIGHTNESS);
            fadeUp(0, BRIGHTNESS);
        }
        

        if (opCodeIdx >= NUM_OP_CODES) {
            FastLED.setBrightness(BRIGHTNESS_IDLE);
            setAllLEDs(CRGB::White);
            
            opCodeIdx = 0;
            saveIntoNVS(RF_OP_CODES_ADDR, rfOpCodes, sizeof(rfOpCodes));
            appState = APP_STATE_CHAPLET;
            return;
        }
        
        setAllLEDs(backwardButtonRGB);
    }
}


static inline void initRcSwitch()
{
    /* Initialize pin to enable calibration of RF controller */

    pinMode(RF_CTRL_CONFIG, INPUT);

    /* Instantiate RCSwitch class */

    mSwitch = RCSwitch();

    /* Enable Receiving signals of RF controller on INT line 0 (PIN D2) */

    mSwitch.enableReceive(INT0);
}

/*****************************************************************************
 * Public Functions                                                          *
 *****************************************************************************/


void retrieveDataFromNVS()
{
    getFromNVS(RGB_OUR_FATHER_ADDR, &ourFatherRGB, sizeof(CRGB));
    getFromNVS(RGB_HAIL_MARY_ADDR, &hailMaryRGB, sizeof(CRGB));
    getFromNVS(RGB_FORWARD_BUTTON_ADDR, &forwardButtonRGB, sizeof(CRGB));
    getFromNVS(RGB_BACKWARD_BUTTON_ADDR, &backwardButtonRGB, sizeof(CRGB));
}


void setup()
{    
    /* initialize serial port for debugging and logging purposes */
    
    Serial.begin(57600);

    /* User LED to signal some activity and/or debugging */

    pinMode(LED_BUILTIN, OUTPUT);
    
    /* Retrieve the saved RF codes from NVS */

    getFromNVS(RF_OP_CODES_ADDR, rfOpCodes, sizeof(rfOpCodes));

    /* Retrieve data from NVS that is changeable from UI */

    retrieveDataFromNVS();

    /* Initialize LEDs */

    initLeds();

    /* Initialize RF controller interface */
    
    initRcSwitch();

    /* Initialize the User Interface (Terminal on USB) */

    initUi();
}

void loop()
{
    runUi();

    switch (appState) {
        case APP_STATE_CHAPLET:
            runChaplet();
            break;
        case APP_STATE_RF_CALIBRATION:
            runRfCalibration();
            break;
        default:
            /* We should not get here */
            
            Serial.println("Illegal app state. Going back to chaplet state");
            appState = APP_STATE_CHAPLET;
            break;
    }
}




