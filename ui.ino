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

                                 
#define MAX_RGB_CODE                              255

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
};


/*****************************************************************************
 * Private Function Prototypes                                               *
 *****************************************************************************/

static bool retrieveCodeFromUi(const String &s, uint8_t &code,
                               unsigned long lim);
static inline void processReqIdle(const String &s);
static inline void processRequest(const String &s);


static void printMenu();

/*****************************************************************************
 * Private Data                                                              *
 *****************************************************************************/

static const char * const rgbLabel = "Digite o código RGB para ";
static const char * const redLabel = "vermelho";
static const char * const greenLabel = "verde";
static const char * const blueLabel = "azul";
static const char * const rangeColorLabel = " (0 a 255)";
static const char * const invalidValueLabel = "Valor inválido";
static const char * const alteredValueLabel = "Valor alterado com sucesso!";

static uint8_t requestState;
static int currentNVSAddr;

static CRGB rgbBuff;


/*****************************************************************************
 * Private Functions                                                         *
 *****************************************************************************/

static bool retrieveCodeFromUi(const String &s, uint8_t &code,
                               unsigned long lim)
{
    unsigned long val;
    char *eptr;
    val = strtoul(s.begin(), &eptr, 10);
    if (*eptr != '\0' || val > lim) {
        /* User has input invalid data */

        Serial.println(invalidValueLabel);
        printMenu();

        return false;
    }

    code = val;

    return true;
}

static inline void processReqIdle(const String &s)
{
    if (s.equals("1")) {
        requestState = REQ_STATE_RED_OUR_FATHER;
        currentNVSAddr = RGB_OUR_FATHER_ADDR;
        Serial.print(rgbLabel);
        Serial.print(redLabel);
        Serial.println(rangeColorLabel);
    }
    else if (s.equals("2")) {
        requestState = REQ_STATE_RED_HAIL_MARY;
        currentNVSAddr = RGB_HAIL_MARY_ADDR;
        Serial.print(rgbLabel);
        Serial.print(redLabel);
        Serial.println(rangeColorLabel);
    }
    else if (s.equals("3")) {
        requestState = REQ_STATE_RED_FORWARD_BUTTON;
        currentNVSAddr = RGB_FORWARD_BUTTON_ADDR;
        Serial.print(rgbLabel);
        Serial.print(redLabel);
        Serial.println(rangeColorLabel);
    }
    else if (s.equals("4")) {
        requestState = REQ_STATE_RED_BACKWARD_BUTTON;
        currentNVSAddr = RGB_BACKWARD_BUTTON_ADDR;
        Serial.print(rgbLabel);
        Serial.print(redLabel);
        Serial.println(rangeColorLabel);
    }
    else {
        /* Invalid item */

        Serial.println("Opção inválida");
        printMenu();
    }
}


static inline void processRequest(const String &s)
{
    switch (requestState) {
        case REQ_STATE_IDLE:
            processReqIdle(s);
            break;
        
        /* Cases regarding Our Father RGB */

        case REQ_STATE_RED_OUR_FATHER:
        case REQ_STATE_RED_HAIL_MARY:
        case REQ_STATE_RED_FORWARD_BUTTON:
        case REQ_STATE_RED_BACKWARD_BUTTON:
            if (retrieveCodeFromUi(s, rgbBuff.red, MAX_RGB_CODE)) {
                requestState++;
                Serial.print(rgbLabel);
                Serial.print(greenLabel);
                Serial.println(rangeColorLabel);
            }
            break;
        case REQ_STATE_GREEN_OUR_FATHER:
        case REQ_STATE_GREEN_HAIL_MARY:
        case REQ_STATE_GREEN_FORWARD_BUTTON:
        case REQ_STATE_GREEN_BACKWARD_BUTTON:
            if (retrieveCodeFromUi(s, rgbBuff.green, MAX_RGB_CODE)) {
                requestState++;
                Serial.print(rgbLabel);
                Serial.print(blueLabel);
                Serial.println(rangeColorLabel);
            }
            break;
        case REQ_STATE_BLUE_OUR_FATHER:
        case REQ_STATE_BLUE_HAIL_MARY:
        case REQ_STATE_BLUE_FORWARD_BUTTON:
        case REQ_STATE_BLUE_BACKWARD_BUTTON:
            if (retrieveCodeFromUi(s, rgbBuff.blue, MAX_RGB_CODE)) {
                saveIntoNVS(currentNVSAddr, &rgbBuff, sizeof(rgbBuff));
                retrieveDataFromNVS();
                Serial.println(alteredValueLabel);
                printMenu();
            }
            break;



        default:
            break;
    }
}


static void printMenu()
{
    requestState = REQ_STATE_IDLE;
    Serial.println();
    Serial.println("1 - Configurar RGB para Pai Nosso");
    Serial.println("2 - Configurar RGB para Ave Maria");
    Serial.println("3 - Configurar RGB para calibração botão avanço");
    Serial.println("4 - Configurar RGB para calibração botão recuo");
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

        /* Clear buffer, and return */
        
        s = "";

        return;
    }
    
    /* User has input some data, store it and print it back */

    s += c;
    Serial.print(c);
}

