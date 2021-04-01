#include "rdm630.h"
#include <Adafruit_NeoPixel.h>
#include <neotimer.h>

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

Neotimer tempoCorto = Neotimer(200);
Neotimer tempoLargo = Neotimer(500);
Neotimer tempoMuyLargo = Neotimer(1000);

const uint8_t ESTADO_COLOR = 10;
const uint8_t ESTADO_LIMPIAR = 20;
const uint8_t ESTADO_TEMPO_CORTO = 30;
const uint8_t ESTADO_TEMPO_LARGO = 40;
const uint8_t ESTADO_TEMPO_MUY_LARGO = 40;

const uint8_t NUM_COLORES_FLUJO = 16;

// Este array representa el orden en el que se ponen los
// colores independientemente de los delays que haya por medio
const uint32_t COLORES_FLUJO[NUM_COLORES_FLUJO] = {
    BLANCO_FLOJO,
    BLANCO_FUERTE,
    BLANCO_FLOJO,
    BLANCO_FUERTE,
    BLANCO_FLOJO,
    BLANCO_FUERTE,
    ROJO,
    AZUL,
    ROJO,
    VERDE,
    ROJO,
    AMARILLO,
    AZUL,
    AZUL,
    VERDE,
    ROJO};

const uint8_t NUM_ESTADOS_FLUJO = 53;

// El numero de valores ESTADO_COLOR en este array tiene
// que coincidir con el valor de NUM_COLORES_FLUJO
const uint8_t FLUJO_EFECTO[NUM_ESTADOS_FLUJO] = {
    ESTADO_COLOR,
    ESTADO_TEMPO_CORTO,
    ESTADO_COLOR,
    ESTADO_TEMPO_CORTO,
    ESTADO_COLOR,
    ESTADO_TEMPO_CORTO,
    ESTADO_COLOR,
    ESTADO_TEMPO_CORTO,
    ESTADO_COLOR,
    ESTADO_TEMPO_CORTO,
    ESTADO_COLOR,
    ESTADO_TEMPO_CORTO,
    ESTADO_TEMPO_MUY_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO,
    ESTADO_COLOR,
    ESTADO_TEMPO_LARGO,
    ESTADO_LIMPIAR,
    ESTADO_TEMPO_LARGO};

const byte NUM_READERS = 4;
const unsigned long RFID_READ_WAIT_MS = 100;
const unsigned int EMPTY_TOLERANCE = 5;

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
    uint8_t indiceFlujo;
    uint8_t indiceColor;
} ProgramState;

ProgramState progState = {
    .solved = false,
    .emptyReadCount = {0, 0, 0, 0},
    .indiceFlujo = 0,
    .indiceColor = 0};

void avanzarEfecto()
{
    Serial.println(F("Avanzando efecto LED"));

    progState.indiceFlujo++;

    if (progState.indiceFlujo >= NUM_ESTADOS_FLUJO)
    {
        progState.indiceFlujo = 0;
    }

    tempoCorto.reset();
    tempoLargo.reset();
    tempoMuyLargo.reset();
}

void avanzarColor()
{
    Serial.println(F("Avanzando color"));

    progState.indiceColor++;

    if (progState.indiceColor >= NUM_COLORES_FLUJO)
    {
        progState.indiceColor = 0;
    }
}

void actualizarTempo(Neotimer &elTempo)
{
    if (!elTempo.started())
    {
        elTempo.start();
    }

    if (elTempo.done())
    {
        avanzarEfecto();
    }
}

void actualizarEfectoLed()
{
    const uint8_t estadoActual = FLUJO_EFECTO[progState.indiceFlujo];

    if (estadoActual == ESTADO_COLOR)
    {
        const uint32_t elColor = COLORES_FLUJO[progState.indiceColor];
        ledFaro.fill(elColor);
        ledFaro.show();
        avanzarEfecto();
        avanzarColor();
    }
    else if (estadoActual == ESTADO_LIMPIAR)
    {
        ledFaro.clear();
        ledFaro.show();
        avanzarEfecto();
    }
    else if (estadoActual == ESTADO_TEMPO_CORTO)
    {
        actualizarTempo(tempoCorto);
    }
    else if (estadoActual == ESTADO_TEMPO_LARGO)
    {
        actualizarTempo(tempoLargo);
    }
    else if (estadoActual == ESTADO_TEMPO_MUY_LARGO)
    {
        actualizarTempo(tempoMuyLargo);
    }
}

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
            Serial.print("Ignorando lectura vacia en lector: ");
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
    Serial.print("## Tags actuales :: ");
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

    Serial.println(">> MareNostrum Faro V2");
}

void loop()
{
    pollRfidReaders();
    printCurrentTags();
    updateState();

    if (!progState.solved)
    {
        actualizarEfectoLed();
    }
}
