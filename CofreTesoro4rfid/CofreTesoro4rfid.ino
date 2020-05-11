#include <Adafruit_NeoPixel.h>
#include "rdm630.h"
#include <Wire.h>
#include "Adafruit_TCS34725.h"

/**
 * Program states.
 */

const byte NUM_STAGES = 4;

typedef struct programState
{
    bool isStageCompleted[NUM_STAGES];
    bool isRelayOpen[NUM_STAGES];
} ProgramState;

ProgramState progState = {
    .isStageCompleted = {false, false, false, false},
    .isRelayOpen = {false, false, false, false}};

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
    Adafruit_NeoPixel::Color(255, 255, 0)};

// sum(LED_STAGE_PATCH_SIZES) == NUM_LEDS

const byte NUM_LEDS = 4;

const byte LED_STAGE_PATCH_SIZES[NUM_STAGES] = {
    1, 1, 1, 1};

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
    rfid04};

String validStageTags[NUM_STAGES] = {
    "6100A812AB00",
    "6100A812AB00",
    "6100A812AB00",
    "6100A812AB00"};

const byte NUM_RESET_TAGS = 1;

String resetTags[NUM_RESET_TAGS] = {
    "1D0027AABB00"};

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

void colorRead()
{
  float red, green, blue;
  
  tcs.setInterrupt(false);  // turn on LED

  delay(60);  // takes 50ms to read

  tcs.getRGB(&red, &green, &blue);
  
  tcs.setInterrupt(true);  // turn off LED

  Serial.print("R:\t"); Serial.print(int(red)); 
  Serial.print("\tG:\t"); Serial.print(int(green)); 
  Serial.print("\tB:\t"); Serial.print(int(blue));
  Serial.print("\n");
  
}

void showStageLeds(int idx)
{
    if (idx >= NUM_STAGES)
    {
        return;
    }

    int iniIdx = 0;

    for (int i = 0; i < idx; i++)
    {
        iniIdx += LED_STAGE_PATCH_SIZES[i];
    }

    int endIdx = iniIdx + LED_STAGE_PATCH_SIZES[idx];

    uint32_t color = LED_COLORS[idx];

    for (int i = iniIdx; i < endIdx; i++)
    {
        ledStrip.setPixelColor(i, color);
    }

    ledStrip.show();
}

/**
 * Audio FX board functions.
 */



void lockRelay(byte idx)
{
    digitalWrite(RELAY_PINS, LOW);
    progState.isRelayOpen[idx] = false;
}

void openRelay(byte idx)
{
    digitalWrite(RELAY_PINS, HIGH);
    progState.isRelayOpen[idx] = true;
}

void initRelay(byte idx)
{
    pinMode(RELAY_PINS, OUTPUT);
    lockRelay(idx);
}

void initRelays()
{
    for (int i = 0; i < NUM_STAGES; i++)
    {
        initRelay(i);
    }
}

void updateRelay(byte idx)
{
    if (progState.isStageCompleted[idx] &&
        !progState.isRelayOpen[idx])
    {
        openRelay(idx);
    }
    else if (!progState.isStageCompleted[idx] &&
             progState.isRelayOpen[idx])
    {
        lockRelay(idx);
    }
}

void updateRelays()
{
    for (int i = 0; i < NUM_STAGES; i++)
    {
        updateRelay(i);
    }
}

/**
 * RFID functions.
 */

void initRfidReaders()
{
    for (int i = 0; i < NUM_STAGES; i++)
    {
        rfidReaders[i].begin();
    }
}

void onValidStage(int idx)
{
    const unsigned long OPEN_RELAY_SLEEP_MS = 1000;
    const unsigned long AUDIO_WAIT_SLEEP_MS = 10;
    const unsigned long MAX_AUDIO_WAIT_MS = 10000;

    unsigned long ini;
    unsigned long now;

    progState.isStageCompleted[idx] = true;

    Serial.println(F("Showing LEDs & playing audio"));

    showStageLeds(idx);
    

    Serial.println(F("Waiting for audio"));

    ini = millis();

   

    Serial.print(F("Sleeping for (ms): "));
    Serial.println(OPEN_RELAY_SLEEP_MS);

    delay(OPEN_RELAY_SLEEP_MS);

    Serial.println(F("Opening relay"));

    updateRelay(idx);
}

void pollRfidReaders()
{
    String tagId;

    for (int i = 0; i < NUM_STAGES; i++)
    {
        tagId = rfidReaders[i].getTagId();

        if (!tagId.length())
        {
            continue;
        }

        Serial.print(F("Tag #"));
        Serial.print(i);
        Serial.print(F(": "));
        Serial.println(tagId);

        if (validStageTags[i].compareTo(tagId) == 0 &&
            !progState.isStageCompleted[i])
        {
            Serial.print(F("Valid tag on #"));
            Serial.println(i);

            onValidStage(i);
        }

        for (int j = 0; j < NUM_RESET_TAGS; j++)
        {
            if (resetTags[j].compareTo(tagId) == 0)
            {
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

    for (int i = 0; i < NUM_STAGES; i++)
    {
        lockRelay(i);
        progState.isStageCompleted[i] = false;
    }
}

/**
 * Entrypoint.
 */

void setup()
{
    Serial.begin(9600);

    initRfidReaders();
    initRelays();
    initLedStrip();


    Serial.println(">> Starting crystal lamp program");
    Serial.flush();

    #endif

  // thanks PhilB for this gamma table!
  // it helps convert RGB colors to what humans see
  for (int i=0; i<256; i++) {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;

    if (commonAnode) {
      gammatable[i] = 255 - x;
    } else {
      gammatable[i] = x;
    }
    //Serial.println(gammatable[i]);
  }
}
}

void loop()
{
    colorRead();
    pollRfidReaders();
    updateRelays();
}
