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

const byte LONG_STRIP = 44;
const byte SHORT_STRIP = 13;

const byte RELAY_PINS = 12;

// Arduino RX pin <--> RFID reader TX pin

const byte PIN_RFID_01_RX = 2;
const byte PIN_RFID_01_TX = A0;
const byte PIN_RFID_02_RX = 3;
const byte PIN_RFID_02_TX = A1;
const byte PIN_RFID_03_RX = 4;
const byte PIN_RFID_03_TX = A2;

/**
 * LED strip.
 */

const byte DEFAULT_BRIGHTNESS = 150;

const uint32_t LED_COLORS[NUM_STAGES] = {
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(0, 255, 125),
    Adafruit_NeoPixel::Color(0, 30, 255),
};

Adafruit_NeoPixel ledStrip1 = Adafruit_NeoPixel(SHORT_STRIP, 6, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledStrip2 = Adafruit_NeoPixel(SHORT_STRIP, 7, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel ledStrip3 = Adafruit_NeoPixel(LONG_STRIP, 8, NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel LED_Strips[NUM_STAGES] = {
    ledStrip1,
    ledStrip2,
    ledStrip3
};

/**
 * RFID readers.
 */

RDM6300 rfid01(PIN_RFID_01_RX, PIN_RFID_01_TX);
RDM6300 rfid02(PIN_RFID_02_RX, PIN_RFID_02_TX);
RDM6300 rfid03(PIN_RFID_03_RX, PIN_RFID_03_TX);

RDM6300 rfidReaders[NUM_STAGES] = {
    rfid01,
    rfid02,
    rfid03
};

String validStageTags[NUM_STAGES] = {
    "100079521800",
    "100079634300",
    "100079810300",
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
    LED_Strips[idx].clear();
    LED_Strips[idx].show();

    if (idx >= NUM_STAGES) {
        return;
    }

    uint32_t color = LED_COLORS[idx];

    for (int i = 0; i < LED_Strips[idx].numPixels(); i++) {
        LED_Strips[idx].setPixelColor(i, color);
        LED_Strips[idx].show();
        delay(50);
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
    //showStageLeds(idx);
    fillLedStrips(idx);
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
