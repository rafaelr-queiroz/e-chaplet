/*****************************************************************************
 * ui.ino - Source code of User Interface. This file contains functions to
 * interpret and process user requests that eventually come from the terminal
 * on the Arduino Nano USB (Serial)
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
 * Private Definitions                                                       *
 *****************************************************************************/

enum RequestState {
    REQ_STATE_IDLE,

    REQ_STATE_RED_OUR_FATHER,
    REQ_STATE_GREEN_OUR_FATHER,
    REQ_STATE_BLUE_OUR_FATHER,

    REQ_STATE_RED_HAIL_MARY,
    REQ_STATE_GREEN_HAIL_MARY,
    REQ_STATE_BLUE_HAIL_MARY,

    REQ_STATE_RED_FORWARD_BUTTON,
    REQ_STATE_GREEN_FORWARD_BUTTON,
    REQ_STATE_BLUE_FORWARD_BUTTON,

    REQ_STATE_RED_BACKWARD_BUTTON,
    REQ_STATE_GREEN_BACKWARD_BUTTON,
    REQ_STATE_BLUE_BACKWARD_BUTTON,

    REQ_STATE_LED_FADE_UP_TIME,
    REQ_STATE_LED_FADE_DW_TIME,

};


/*****************************************************************************
 * Private Function Prototypes                                               *
 *****************************************************************************/

static inline void processRequest(const String &s);


static inline void printMenu();

/*****************************************************************************
 * Private Data                                                              *
 *****************************************************************************/

static const String rgbLabel = "Digite o código RGB para ";
static const String redLabel = "vermelho";
static const String greenLabel = "verde";
static const String blueLabel = "azul";
static const String rangeColorLabel = " (0 a 255)";
static const String upTimeLabel = "Tempo de subida";
static const String dwTimeLabel = "Tempo de descida";
static const String timeUnit = " em milissegundos";

static uint8_t requestState;

/*****************************************************************************
 * Private Functions                                                         *
 *****************************************************************************/

static inline void processReqIdle(const String &s)
{
    if (s.equals("1")) {
        requestState = REQ_STATE_RED_OUR_FATHER;
        Serial.println(rgbLabel + redLabel + rangeColorLabel);
    }
    else if (s.equals("2")) {
        requestState = REQ_STATE_RED_HAIL_MARY;
        Serial.println(rgbLabel + redLabel + rangeColorLabel);
    }
    else if (s.equals("3")) {
        
    }
    else if (s.equals("4")) {
        requestState = REQ_STATE_RED_FORWARD_BUTTON;
        Serial.println(rgbLabel + redLabel + rangeColorLabel);
    }
    else if (s.equals("5")) {
        requestState = REQ_STATE_RED_BACKWARD_BUTTON;
        Serial.println(rgbLabel + redLabel + rangeColorLabel);
    }
    else {
        /* Invalid item */

        Serial.println("Opção inválida");
        Serial.println();
    }
}


static inline void processRequest(const String &s)
{
    switch (requestState) {
        case REQ_STATE_IDLE:
            processReqIdle(s);
            break;
        case REQ_STATE_RED_OUR_FATHER:
            
            break;
        default:
            break;
    }
}


static inline void printMenu()
{
    Serial.println("1 - Configurar RGB para Pai Nosso");
    Serial.println("2 - Configurar RGB para Ave Maria");
    Serial.println("3 - Configurar efeito de fading dos LED");
    Serial.println("4 - Configurar RGB para calibração botão avanço");
    Serial.println("5 - Configurar RGB para calibração botão recuo");
    Serial.println();
}

/*****************************************************************************
 * Public Functions                                                          *
 *****************************************************************************/


void initUi()
{
    Serial.print("Terço eletrônico v1.0.0 - Rafael Rodrigues Queiroz ");
    Serial.println("(rafaelrq@gmail.com)");
    Serial.println();
    Serial.println();

    printMenu();
}


void runUi()
{
    static String s = "";
    char c;

    /* Is there data on Serial Port? */

    if ((c = Serial.read()) < 0) {
        /* No, so just return */

        return;
    }
    
    if (c == '\r' || c == '\n') {
        Serial.println();
        Serial.println();
        
        /* If data is a Carriage Return or Line Feed, it means user hit
         * the Enter button on keyboard, so just process what has been
         * input so far
         */

        processRequest(s);

        /* Clear buffer, print the menu again and return */
        
        s = "";

        printMenu();

        return;
    }
    
    /* User has input some data, store it and print it back */

    s += c;
    Serial.print(c);
}

