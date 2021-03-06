#include "rdm630.h"
#include <Adafruit_NeoPixel.h>

/**
 * Program states.
 */

const byte NUM_STAGES = 3;

typedef struct programState {
    bool isStageCompleted[NUM_STAGES];
    bool isRelayOpen;
} ProgramState;

ProgramState progState = {
    .isStageCompleted = {
        false,
        false,
        false,
    },
    .isRelayOpen = false
};

/**
 * Pins.
 */

const byte RELAY_PINS = 12;

// Arduino RX pin <--> RFID reader TX pin

const byte PIN_RFID_01_RX = 2;
const byte PIN_RFID_01_TX = A0;
const byte PIN_RFID_02_RX = 3;
const byte PIN_RFID_02_TX = A1;
const byte PIN_RFID_03_RX = 4;
const byte PIN_RFID_03_TX = A2;

const uint16_t PIN_STRIP_01 = 6;
const uint16_t PIN_STRIP_02 = 7;
const uint16_t PIN_STRIP_03 = 8;

/**
 * LED strip.
 */

const byte DEFAULT_BRIGHTNESS = 150;

const uint32_t LED_COLORS[NUM_STAGES] = {
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(0, 255, 125),
    Adafruit_NeoPixel::Color(0, 30, 255),
};

const uint16_t SIZE_STRIP_LONG = 44;
const uint16_t SIZE_STRIP_SHORT = 13;

Adafruit_NeoPixel ledStrip1 = Adafruit_NeoPixel(
    SIZE_STRIP_SHORT,
    PIN_STRIP_01,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledStrip2 = Adafruit_NeoPixel(
    SIZE_STRIP_SHORT,
    PIN_STRIP_02,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledStrip3 = Adafruit_NeoPixel(
    SIZE_STRIP_LONG,
    PIN_STRIP_03,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledStrips[NUM_STAGES] = {
    ledStrip1,
    ledStrip2,
    ledStrip3
};

/**
 * RFID readers.
 */

const int NUM_TAG_OPTIONS = 2;

RDM6300 rfid01(PIN_RFID_01_RX, PIN_RFID_01_TX);
RDM6300 rfid02(PIN_RFID_02_RX, PIN_RFID_02_TX);
RDM6300 rfid03(PIN_RFID_03_RX, PIN_RFID_03_TX);

RDM6300 rfidReaders[NUM_STAGES] = {
    rfid01,
    rfid02,
    rfid03
};

String validStageTags[NUM_STAGES][NUM_TAG_OPTIONS] = {
    { "100079521800", "100079521800" },
    { "100079634300", "100079634300" },
    { "100079810300", "100079810300" }
};

const byte NUM_RESET_TAGS = 1;

String resetTags[NUM_RESET_TAGS] = {
    "100079546300"
};

/**
 * LED functions.
 */

void initLedStrip()
{
    ledStrip1.begin();
    ledStrip1.setBrightness(DEFAULT_BRIGHTNESS);
    ledStrip1.clear();
    ledStrip1.show();

    ledStrip2.begin();
    ledStrip2.setBrightness(DEFAULT_BRIGHTNESS);
    ledStrip2.clear();
    ledStrip2.show();

    ledStrip3.begin();
    ledStrip3.setBrightness(DEFAULT_BRIGHTNESS);
    ledStrip3.clear();
    ledStrip3.show();
}

void fillLedStrips(int idx)
{
    if (idx >= NUM_STAGES) {
        return;
    }

    const unsigned long stepDelayMs = 50;

    ledStrips[idx].clear();
    ledStrips[idx].show();

    uint32_t color = LED_COLORS[idx];

    for (int i = 0; i < ledStrips[idx].numPixels(); i++) {
        ledStrips[idx].setPixelColor(i, color);
        ledStrips[idx].show();
        delay(stepDelayMs);
    }
}

/**
 * Relay functions.
 */

void lockRelay()
{
    digitalWrite(RELAY_PINS, LOW);
    progState.isRelayOpen = false;
}

void openRelay()
{
    digitalWrite(RELAY_PINS, HIGH);
    progState.isRelayOpen = true;
}

void initRelay()
{
    pinMode(RELAY_PINS, OUTPUT);
    lockRelay();
}

void openRelayIfAllTags()
{
    const unsigned long delayRelayMs = 1000;

    for (int i = 0; i < NUM_STAGES; i++) {
        if (!progState.isStageCompleted[i]) {
            return;
        }
    }

    if (!progState.isRelayOpen) {
        delay(delayRelayMs);
        openRelay();
    }
}

/**
 * RFID functions.
 */

void initRfidReaders()
{
    for (int i = 0; i < NUM_STAGES; i++) {
        rfidReaders[i].begin();
    }
}

void onValidTag(int idx)
{
    progState.isStageCompleted[idx] = true;
    Serial.println(F("Showing LEDs"));
    fillLedStrips(idx);
}

bool isValidTag(int idxStage, String tagId)
{
    if (progState.isStageCompleted[idxStage]) {
        return false;
    }

    for (int i = 0; i < NUM_TAG_OPTIONS; i++) {
        if (validStageTags[idxStage][i].compareTo(tagId) == 0) {
            return true;
        }
    }

    return false;
}

bool isResetTag(String tagId)
{
    for (int i = 0; i < NUM_RESET_TAGS; i++) {
        if (resetTags[i].compareTo(tagId) == 0) {
            return true;
        }
    }

    return false;
}

void pollRfidReaders()
{
    String tagId;

    for (int i = 0; i < NUM_STAGES; i++) {
        tagId = rfidReaders[i].getTagId();

        if (!tagId.length()) {
            continue;
        }

        Serial.print(F("Tag #"));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(tagId);

        if (isValidTag(i, tagId)) {
            Serial.print(F("Valid tag on #"));
            Serial.println(i);
            onValidTag(i);
        }

        if (isResetTag(tagId)) {
            Serial.println(F("Reset tag"));
            resetGame();
        }
    }
}

/**
 * Utility functions.
 */

void resetGame()
{
    ledStrip1.clear();
    ledStrip1.show();

    ledStrip2.clear();
    ledStrip2.show();

    ledStrip3.clear();
    ledStrip3.show();

    for (int i = 0; i < NUM_STAGES; i++) {
        progState.isStageCompleted[i] = false;
    }

    lockRelay();
}

/**
 * Entrypoint.
 */

void setup()
{
    Serial.begin(9600);

    initRfidReaders();
    initRelay();
    initLedStrip();

    Serial.println(">> Starting program");
    Serial.flush();
}

void loop()
{
    pollRfidReaders();
    openRelayIfAllTags();
}
