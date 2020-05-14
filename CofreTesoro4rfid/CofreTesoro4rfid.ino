#include "rdm630.h"
#include <Adafruit_NeoPixel.h>

/**
 * Program states.
 */

const byte NUM_STAGES = 4;

typedef struct programState {
    bool isStageCompleted[NUM_STAGES];
    bool isRelayOpen;
} ProgramState;

ProgramState progState = {
    .isStageCompleted = { false, false, false, false },
    .isRelayOpen = false
};

/**
 * Pins.
 */

const byte PIN_LEDS = 11;

//const byte RELAY_PINS[NUM_STAGES] = {10, 9, 8};

const byte RELAY_PINS = 12;

// Arduino RX pin <--> RFID reader TX pin

const byte PIN_RFID_01_RX = 2;
const byte PIN_RFID_01_TX = A0;
const byte PIN_RFID_02_RX = 4;
const byte PIN_RFID_02_TX = A1;
const byte PIN_RFID_03_RX = 6;
const byte PIN_RFID_03_TX = A2;
const byte PIN_RFID_04_RX = 8;
const byte PIN_RFID_04_TX = A3;

/**
 * LED strip.
 */

const byte DEFAULT_BRIGHTNESS = 150;

const uint32_t LED_COLORS[NUM_STAGES] = {
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(0, 255, 0),
    Adafruit_NeoPixel::Color(0, 0, 255),
    Adafruit_NeoPixel::Color(255, 255, 0)
};

// sum(LED_STAGE_PATCH_SIZES) == NUM_LEDS

const byte NUM_LEDS = 4;

const byte LED_STAGE_PATCH_SIZES[NUM_STAGES] = {
    1, 1, 1, 1
};

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(NUM_LEDS, PIN_LEDS, NEO_GRB + NEO_KHZ800);

/**
 * RFID readers.
 */

RDM6300 rfid01(PIN_RFID_01_RX, PIN_RFID_01_TX);
RDM6300 rfid02(PIN_RFID_02_RX, PIN_RFID_02_TX);
RDM6300 rfid03(PIN_RFID_03_RX, PIN_RFID_03_TX);
RDM6300 rfid04(PIN_RFID_04_RX, PIN_RFID_04_TX);

RDM6300 rfidReaders[NUM_STAGES] = {
    rfid01,
    rfid02,
    rfid03,
    rfid04
};

String validStageTags[NUM_STAGES] = {
    "100079541400",
    "100078F4A500",
    "100079816C00",
    "1000795A9B00"
};

const byte NUM_RESET_TAGS = 1;

String resetTags[NUM_RESET_TAGS] = {
    "1D0027AABB00"
};

/**
 * LED functions.
 */

void initLedStrip()
{
    ledStrip.begin();
    ledStrip.setBrightness(DEFAULT_BRIGHTNESS);
    ledStrip.clear();
    ledStrip.show();
}

void showStageLeds(int idx)
{
    if (idx >= NUM_STAGES) {
        return;
    }

    int iniIdx = 0;

    for (int i = 0; i < idx; i++) {
        iniIdx += LED_STAGE_PATCH_SIZES[i];
    }

    int endIdx = iniIdx + LED_STAGE_PATCH_SIZES[idx];

    uint32_t color = LED_COLORS[idx];

    for (int i = iniIdx; i < endIdx; i++) {
        ledStrip.setPixelColor(i, color);
    }

    ledStrip.show();
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
    showStageLeds(idx);
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

        if (validStageTags[i].compareTo(tagId) == 0 && !progState.isStageCompleted[i]) {
            Serial.print(F("Valid tag on #"));
            Serial.println(i);

            onValidTag(i);
        }

        for (int j = 0; j < NUM_RESET_TAGS; j++) {
            if (resetTags[j].compareTo(tagId) == 0) {
                Serial.println(F("Reset tag detected"));
                resetGame();
                break;
            }
        }
    }
}

/**
 * Utility functions.
 */

void resetGame()
{
    ledStrip.clear();
    ledStrip.show();

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
