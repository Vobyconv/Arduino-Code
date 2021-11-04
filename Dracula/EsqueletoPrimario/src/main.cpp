#include <Arduino.h>
#include <rdm630.h>
#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

/**
 * RFID readers.
 */

const uint8_t NUM_READERS = 4;
const unsigned long RFID_READ_WAIT_MS = 250;

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

const unsigned int EMPTY_TOLERANCE = 1;

const int NUM_OPTIONS = 1;

String validTags[NUM_READERS][NUM_OPTIONS] = {
    {"09008FF3DD00"},
    {"09008F3CCC00"},
    {"09008F528000"},
    {"09008EE32200"}};

/**
 * LEDs.
 */

const uint16_t LED_BOX_NUM = 35;
const uint16_t LED_STRIPS_NUM = 60;

const uint8_t LED_BOX_PRI_PIN = 11;
const uint8_t LED_BOX_SEC_PIN = A4;
const uint8_t LED_STRIPS_PRI_PIN = 10;
const uint8_t LED_STRIPS_SEC_PIN = A5;

Adafruit_NeoPixel ledBoxPri = Adafruit_NeoPixel(
    LED_BOX_NUM,
    LED_BOX_PRI_PIN,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledBoxSec = Adafruit_NeoPixel(
    LED_BOX_NUM,
    LED_BOX_SEC_PIN,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledStripsPri = Adafruit_NeoPixel(
    LED_STRIPS_NUM,
    LED_STRIPS_PRI_PIN,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledStripsSec = Adafruit_NeoPixel(
    LED_STRIPS_NUM,
    LED_STRIPS_SEC_PIN,
    NEO_GRB + NEO_KHZ800);

const uint32_t COLOR_PRI = Adafruit_NeoPixel::Color(100, 100, 240);
const uint32_t COLOR_SEC = Adafruit_NeoPixel::Color(240, 0, 0);

const uint8_t STRIP_BRIGHTNESS_DEFAULT = 1;
const uint8_t STRIP_BRIGHTNESS = 200;
const uint8_t BOX_BRIGHTNESS_DEFAULT = 1;
const uint8_t BOX_BRIGHTNESS = 200;

/**
 * Control pins and relays.
 */

const uint8_t PIN_OUTPUT_RELAY_PRIMARY = 12;
const uint8_t PIN_OUTPUT_RELAY_SECONDARY = A0;
const uint8_t PIN_INPUT_COMPLETION_SECONDARY = A1;
const uint8_t PIN_OUTPUT_RELAY_ACTIVATION_BATS = A2;
const uint8_t PIN_INPUT_RELAY_COMPLETION_BATS = A3;

Atm_digital digitalInputCompletionSecondary;
Atm_button buttonCompletionBats;

/**
 * Program state.
 */

const uint32_t TIMER_STATE_MS = 200;

Atm_timer timerState;

String currentTags[NUM_READERS];

typedef struct programState
{
  unsigned int emptyReadCount[NUM_READERS];
  bool isPrimaryComplete;
  bool isSecondaryComplete;
  bool isBatsStageComplete;
  bool flagBatsActivation;
  bool flagRelayUpdate;
} ProgramState;

ProgramState progState = {
    .emptyReadCount = {0, 0, 0, 0},
    .isPrimaryComplete = false,
    .isSecondaryComplete = false,
    .isBatsStageComplete = false,
    .flagBatsActivation = false,
    .flagRelayUpdate = false};

void initState()
{
  for (int i = 0; i < NUM_READERS; i++)
  {
    progState.emptyReadCount[i] = 0;
  }

  progState.isPrimaryComplete = false;
  progState.isSecondaryComplete = false;
  progState.isBatsStageComplete = false;
  progState.flagBatsActivation = false;
  progState.flagRelayUpdate = false;
}

void lockRelay(uint8_t pin)
{
  digitalWrite(pin, LOW);
}

void openRelay(uint8_t pin)
{
  digitalWrite(pin, HIGH);
}

void initRelay(uint8_t pin)
{
  pinMode(pin, OUTPUT);
  lockRelay(pin);
}

void onSecondaryHigh(int idx, int v, int up)
{
  Serial.println(F("isSecondaryComplete = true"));
  progState.isSecondaryComplete = true;
}

void onSecondaryLow(int idx, int v, int up)
{
  Serial.println(F("isSecondaryComplete = false"));
  progState.isSecondaryComplete = false;
}

void onBatsPress(int idx, int v, int up)
{
  Serial.println(F("isBatsStageComplete = true"));
  progState.isBatsStageComplete = true;
}

void onBatsRelease(int idx, int v, int up)
{
  Serial.println(F("isBatsStageComplete = false"));
  progState.isBatsStageComplete = false;
}

void initControlPins()
{
  initRelay(PIN_OUTPUT_RELAY_PRIMARY);
  initRelay(PIN_OUTPUT_RELAY_SECONDARY);
  initRelay(PIN_OUTPUT_RELAY_ACTIVATION_BATS);

  const int minDurationMs = 1000;
  const bool activeLow = false;
  const bool pullUp = true;

  digitalInputCompletionSecondary
      .begin(PIN_INPUT_COMPLETION_SECONDARY, minDurationMs, activeLow, pullUp)
      .onChange(HIGH, onSecondaryHigh)
      .onChange(LOW, onSecondaryLow);

  const int debounceDelayMs = 500;

  buttonCompletionBats
      .begin(PIN_INPUT_RELAY_COMPLETION_BATS)
      .debounce(debounceDelayMs)
      .onPress(onBatsPress)
      .onRelease(onBatsRelease);
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
      Serial.print(F("Ignoring empty read on: "));
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

bool isTagsStateValid()
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
  Serial.print(F("## Current tags :: "));
  Serial.println(millis());

  for (int i = 0; i < NUM_READERS; i++)
  {
    Serial.print(i);
    Serial.print(F(" :: "));
    Serial.println(currentTags[i]);
  }
}

void initLedBox()
{
  ledBoxPri.begin();
  ledBoxPri.setBrightness(BOX_BRIGHTNESS_DEFAULT);
  ledBoxPri.fill(COLOR_PRI);
  ledBoxPri.show();

  ledBoxSec.begin();
  ledBoxSec.setBrightness(BOX_BRIGHTNESS_DEFAULT);
  ledBoxSec.fill(COLOR_SEC);
  ledBoxSec.show();
}

void initLedStrips()
{
  ledStripsPri.begin();
  ledStripsPri.setBrightness(STRIP_BRIGHTNESS_DEFAULT);
  ledStripsPri.fill(COLOR_PRI);
  ledStripsPri.show();

  ledStripsSec.begin();
  ledStripsSec.setBrightness(STRIP_BRIGHTNESS_DEFAULT);
  ledStripsSec.fill(COLOR_SEC);
  ledStripsSec.show();
}

void onTimerState(int idx, int v, int up)
{
  bool rfidComplete = progState.isPrimaryComplete || progState.isSecondaryComplete;

  // If the RFID stage is complete and we haven't activated the bats yet, we need to update the LED

  if (progState.isPrimaryComplete && !progState.flagBatsActivation)
  {
    ledStripsPri.setBrightness(STRIP_BRIGHTNESS);
    ledStripsPri.fill(COLOR_PRI);
    ledStripsPri.show();

    ledStripsSec.clear();
    ledStripsSec.show();

    ledBoxSec.clear();
    ledBoxSec.show();
  }
  else if (progState.isSecondaryComplete && !progState.flagBatsActivation)
  {
    ledStripsSec.setBrightness(STRIP_BRIGHTNESS);
    ledStripsSec.fill(COLOR_SEC);
    ledStripsSec.show();

    ledStripsPri.clear();
    ledStripsPri.show();

    ledBoxPri.clear();
    ledBoxPri.show();
  }

  // If the RFID stage is complete, we need to activate the bats controller

  if (rfidComplete && !progState.flagBatsActivation)
  {
    Serial.println(F("Opening bats activation relay"));
    openRelay(PIN_OUTPUT_RELAY_ACTIVATION_BATS);
    progState.flagBatsActivation = true;
  }

  // If the RFID stage is still pending, we have to poll the RFID readers

  if (!rfidComplete)
  {
    pollRfidReaders();
    printCurrentTags();

    if (isTagsStateValid())
    {
      Serial.println(F("Primary RFID OK"));
      progState.isPrimaryComplete = true;
    }
  }

  // If both the RFID and bats stages are complete, we need to update the relays

  if (rfidComplete && progState.isBatsStageComplete)
  {
    if (progState.isPrimaryComplete)
    {
      openRelay(PIN_OUTPUT_RELAY_PRIMARY);
      lockRelay(PIN_OUTPUT_RELAY_SECONDARY);

      ledBoxPri.setBrightness(BOX_BRIGHTNESS);
      ledBoxPri.fill(COLOR_PRI);
      ledBoxPri.show();
    }
    else
    {
      lockRelay(PIN_OUTPUT_RELAY_PRIMARY);
      openRelay(PIN_OUTPUT_RELAY_SECONDARY);

      ledBoxSec.setBrightness(BOX_BRIGHTNESS);
      ledBoxSec.fill(COLOR_SEC);
      ledBoxSec.show();
    }

    if (!progState.flagRelayUpdate)
    {
      Serial.println(F("Updated relay state"));
      progState.flagRelayUpdate = true;
    }
  }
  else if (!rfidComplete && progState.isBatsStageComplete)
  {
    Serial.println(F("Warn: Bats HIGH but RFID pending"));
  }
}

void initTimerState()
{
  timerState
      .begin(TIMER_STATE_MS)
      .repeat(-1)
      .onTimer(onTimerState)
      .start();
}

void setup()
{
  Serial.begin(9600);
  initState();
  initControlPins();
  initRfidReaders();
  initLedBox();
  initLedStrips();
  initTimerState();
  Serial.println(F("Esqueleto Primario"));
}

void loop()
{
  automaton.run();
}
