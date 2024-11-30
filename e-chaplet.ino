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


/*****************************************************************************
 * Private Function Prototypes                                               *
 *****************************************************************************/


/*****************************************************************************
 * Private Data                                                              *
 *****************************************************************************/

static RCSwitch mSwitch = RCSwitch();


/*****************************************************************************
 * Private Functions                                                         *
 *****************************************************************************/


/*****************************************************************************
 * Public Functions                                                          *
 *****************************************************************************/

void setup() {
    
    /* User LED to signal some activity and/or debugging */

    pinMode(LED_BUILTIN, OUTPUT);
    
    /* initialize serial port for debugging purposes */
    
    Serial.begin(9600);
    
    /*  */

    mSwitch.enableReceive(0);
}

void loop() {

    if (mSwitch.available()) {
        mSwitch.resetAvailable();
    }

}




