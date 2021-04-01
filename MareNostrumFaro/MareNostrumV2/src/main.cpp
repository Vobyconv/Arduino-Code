#include "rdm630.h"
#include <Adafruit_NeoPixel.h>

const int FARO_LED_PIN = 10;
const int NUM_LEDS_FARO = 3;
const int CORTO = 300;
const int LARGO = 500;
const int MUCHO = 1000;

const uint32_t PURPURA_OSCURO = Adafruit_NeoPixel::Color(255, 0, 255);
const uint32_t PURPURA_CLARO = Adafruit_NeoPixel::Color(130, 0, 130);
const uint32_t ROJO = Adafruit_NeoPixel::Color(255, 0, 0);
const uint32_t VERDE = Adafruit_NeoPixel::Color(0, 255, 0);
const uint32_t AZUL = Adafruit_NeoPixel::Color(0, 0, 255);
const uint32_t AMARILLO = Adafruit_NeoPixel::Color(255, 255, 0);
const uint32_t BLANCO_FUERTE = Adafruit_NeoPixel::Color(255, 255, 255);
const uint32_t BLANCO_FLOJO = Adafruit_NeoPixel::Color(50, 50, 50);

Adafruit_NeoPixel ledFaro = Adafruit_NeoPixel(
    NUM_LEDS_FARO,
    FARO_LED_PIN,
    NEO_GRB + NEO_KHZ800);

const byte NUM_READERS = 4;
const unsigned long RFID_READ_WAIT_MS = 100;
const unsigned int EMPTY_TOLERANCE = 3;

// RX, TX
RDM6300 rfid01(2, 3, RFID_READ_WAIT_MS);
RDM6300 rfid02(4, 5, RFID_READ_WAIT_MS);
RDM6300 rfid03(6, 7, RFID_READ_WAIT_MS);
RDM6300 rfid04(8, 9, RFID_READ_WAIT_MS);

RDM6300 rfidReaders[NUM_READERS] = {
    rfid01,
    rfid02,
    rfid03,
    rfid04};

String currentTags[NUM_READERS];

const int NUM_OPTIONS = 2;

String validTags[NUM_READERS][NUM_OPTIONS] = {
    {"2B0045209700", "1D00278F8200"},
    {"100079F15B00", "1D00277EC900"},
    {"100079546300", "1D00278E7600"},
    {"10001BB9DC00", "1D0027AA9B00"}};

typedef struct programState
{
    bool solved;
    unsigned int emptyReadCount[NUM_READERS];
} ProgramState;

ProgramState progState = {
    .solved = false,
    .emptyReadCount = {0, 0, 0, 0}};

void ponerColor(uint32_t elColor, bool apagarDespues)
{
    for (int i = 0; i <= NUM_LEDS_FARO; i++)
    {
        ledFaro.setPixelColor(i, elColor);
        ledFaro.show();
    }

    if (apagarDespues)
    {
        delay(LARGO);
        ledFaro.clear();
        ledFaro.show();
        delay(LARGO);
    }
}

void purpura()
{
    Serial.println(F("Purpura: Antes"));

    for (int i = 0; i <= NUM_LEDS_FARO; i++)
    {
        ledFaro.setPixelColor(i, PURPURA_OSCURO);
        ledFaro.show();
        delay(CORTO);
    }

    for (int i = 0; i <= NUM_LEDS_FARO; i++)
    {
        ledFaro.setPixelColor(i, PURPURA_CLARO);
        ledFaro.show();
        delay(CORTO);
    }

    Serial.println(F("Purpura: Despues"));
    delay(MUCHO);
}

void fogonazo()
{
    Serial.println(F("Fogonazo"));
    ponerColor(BLANCO_FLOJO, false);
    delay(CORTO);
    ponerColor(BLANCO_FUERTE, false);
    delay(CORTO);
    ponerColor(BLANCO_FLOJO, false);
    delay(CORTO);
    ponerColor(BLANCO_FUERTE, false);
    delay(CORTO);
    ponerColor(BLANCO_FLOJO, false);
    delay(CORTO);
    ponerColor(BLANCO_FUERTE, false);
    delay(CORTO);
    delay(MUCHO);
}

void llamada()
{
    Serial.println(F("Llamada"));
    fogonazo();
    ponerColor(ROJO, true);
    ponerColor(AZUL, true);
    ponerColor(ROJO, true);
    ponerColor(VERDE, true);
    ponerColor(ROJO, true);
    ponerColor(AMARILLO, true);
    ponerColor(AZUL, true);
    ponerColor(AZUL, true);
    ponerColor(VERDE, true);
    ponerColor(ROJO, true);
}

void victoria()
{
    Serial.println(F("Victoria"));
    ponerColor(BLANCO_FLOJO, false);
    purpura();
    ponerColor(BLANCO_FUERTE, false);
    purpura();
    ponerColor(BLANCO_FLOJO, false);
    purpura();
    ponerColor(BLANCO_FUERTE, false);
    purpura();
}

void initLed()
{
    ledFaro.begin();
    ledFaro.setBrightness(40);
    ledFaro.show();
    ledFaro.clear();
}

void initRfidReaders()
{
    for (int i = 0; i < NUM_READERS; i++)
    {
        rfidReaders[i].begin();
    }
}

void pollRfidReaders()
{
    String tagId;

    for (int i = 0; i < NUM_READERS; i++)
    {
        tagId = rfidReaders[i].getTagId();

        if (tagId.length())
        {
            progState.emptyReadCount[i] = 0;
        }
        else if (progState.emptyReadCount[i] <= EMPTY_TOLERANCE)
        {
            progState.emptyReadCount[i] += 1;
        }

        if (!tagId.length() &&
            currentTags[i].length() &&
            progState.emptyReadCount[i] <= EMPTY_TOLERANCE)
        {
            Serial.print("Ignoring empty read on reader: ");
            Serial.println(i);
            return;
        }

        currentTags[i] = tagId;
    }
}

bool isTagDefined(int idx)
{
    return currentTags[idx].length() > 0;
}

bool areCurrentTagsValid()
{
    bool isValid;

    for (int i = 0; i < NUM_READERS; i++)
    {
        if (!isTagDefined(i))
        {
            return false;
        }

        isValid = false;

        for (int j = 0; j < NUM_OPTIONS; j++)
        {
            if (validTags[i][j].compareTo(currentTags[i]) == 0)
            {
                isValid = true;
                break;
            }
        }

        if (!isValid)
        {
            return false;
        }
    }

    return true;
}

void printCurrentTags()
{
    Serial.print("## Current tags :: ");
    Serial.println(millis());

    for (int i = 0; i < NUM_READERS; i++)
    {
        Serial.print(i);
        Serial.print(" :: ");
        Serial.println(currentTags[i]);
    }
}

void updateState()
{
    if (areCurrentTagsValid() == true && progState.solved == false)
    {
        Serial.println(F("Solucion: SI"));
        progState.solved = true;
        victoria();
    }
}

void setup()
{
    Serial.begin(9600);

    initRfidReaders();
    initLed();

    Serial.println(">> MareNostrum Faro");
}

void loop()
{
    pollRfidReaders();
    printCurrentTags();
    updateState();

    if (!progState.solved)
    {
        llamada();
    }
}
