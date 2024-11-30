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

static void checkForRfCalibration();

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

/*****************************************************************************
 * Private Functions                                                         *
 *****************************************************************************/

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

/*****************************************************************************
 * Public Functions                                                          *
 *****************************************************************************/

void setup() {
    
    /* User LED to signal some activity and/or debugging */

    pinMode(LED_BUILTIN, OUTPUT);
    
    /* Initialize pin to enable calibration of RF controller */

    pinMode(RF_CTRL_CONFIG, INPUT);

    /* initialize serial port for debugging and logging purposes */
    
    Serial.begin(9600);
    
    /* Enable Receiving signals of RF controller on INT line 0 (PIN D2) */

    mSwitch.enableReceive(0);
}

void loop() {
    
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




