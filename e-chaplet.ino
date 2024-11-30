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

#define RF_CONFIG_TIMEOUT_MS                        5000UL

/* Address to store RF OP codes in EEPROM */

#define RF_OP_CODES_ADDR                            128

/* Pin used to drive the LED strip is PIN D6 */

#define LED_PIN                                     6

/* The chaplet's LED strip is composed of 61 LEDs */

#define NUM_LEDS                                    61

/* Initial brightness scale */

#define BRIGHTNESS                                  64

/* Used controller is WS2811 */

#define LED_TYPE                                    WS2811

/* The order of the RGB color code used by the controller */

#define COLOR_ORDER                                 GRB


enum AppState {
    APP_STATE_CHAPLET,
    APP_STATE_RF_CALIBRATION,
};


struct RfOpCodes
{
    unsigned long forward;
    unsigned long backward;
};


/*****************************************************************************
 * Private Function Prototypes                                               *
 *****************************************************************************/
static void saveRfCodes();
static bool checkForRfCalibration();

static void runChaplet();
static void runRfCalibration();

/*****************************************************************************
 * Private Data                                                              *
 *****************************************************************************/

static RCSwitch mSwitch = RCSwitch();
static bool rfCtrlState;
static unsigned long pressInstant; 
static uint8_t appState;
static struct RfOpCodes rfOpCodes;
static CRGB leds[NUM_LEDS];
static uint8_t currentBrightness;
unsigned long dwTime;
unsigned long upTime;

/*****************************************************************************
 * Private Functions                                                         *
 *****************************************************************************/

static void fadeLeds()
{
    uint8_t scale = currentBrightness;
    while (scale) {
        FastLED.setBrightness(--scale);
        FastLED.show();
        delay(dwTime);
    }

    scale = 0;
    while (scale < currentBrightness) {
        FastLED.setBrightness(++scale);
        FastLED.show();
        delay(upTime);
    }
}

static void initLeds()
{
    FastLED.addLeds<LED_TYPE, LED_PIN, COLOR_ORDER>(leds, NUM_LEDS);
    FastLED.setBrightness(BRIGHTNESS);
    
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i] = CRGB::White;
    }

    FastLED.show();
}


static void saveRfCodes()
{
    size_t size = sizeof(rfOpCodes);
    int addr = RF_OP_CODES_ADDR;
    const uint8_t *p = (const uint8_t *)&rfOpCodes;

    while (size--) {
        EEPROM.update(addr++, *p++);
    }
}


static void retrieveRfCodes()
{
    size_t size = sizeof(rfOpCodes);
    int addr = RF_OP_CODES_ADDR;
    uint8_t *p = (uint8_t *)&rfOpCodes; 

    while (size--) {
        *p++ = EEPROM.read(addr++);
    }
}


static bool checkForRfCalibration()
{
    if (rfCtrlState) {
        if (digitalRead(RF_CTRL_CONFIG) == HIGH) {
            rfCtrlState = false;
        }
        else if (millis() - pressInstant >= RF_CONFIG_TIMEOUT_MS) {
            return true;
        }
    }
    else if (digitalRead(RF_CTRL_CONFIG) == LOW) {
        rfCtrlState  = true;
        pressInstant = millis();
    }

    return false;
}


static void runChaplet()
{
    if (checkForRfCalibration()) {
        /* We must put the system in RF calibration mode */

        appState = APP_STATE_RF_CALIBRATION;

        return;
    }

    /* Do we need to update the chaplet LEDs? */

    if (mSwitch.available()) {
        /* Yes, we do */

        mSwitch.resetAvailable();
    }

}


static void runRfCalibration()
{

}

/*****************************************************************************
 * Public Functions                                                          *
 *****************************************************************************/

void setup() {    
    /* initialize serial port for debugging and logging purposes */
    
    Serial.begin(9600);

    /* User LED to signal some activity and/or debugging */

    pinMode(LED_BUILTIN, OUTPUT);
    
    /* Initialize pin to enable calibration of RF controller */

    pinMode(RF_CTRL_CONFIG, INPUT);

    /* Retrieve the saved RF codes from NVS */

    retrieveRfCodes();

    /* Initialize LEDs */

    initLeds();

    /* Enable Receiving signals of RF controller on INT line 0 (PIN D2) */

    mSwitch.enableReceive(0);
}

void loop() {
    
    if (Serial.available() > 0) {
        String s = Serial.readStringUntil('\n');
        Serial.println("Received: " + s);
    }

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




