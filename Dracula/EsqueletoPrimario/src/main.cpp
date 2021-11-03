#include <Arduino.h>
#include <rdm630.h>
#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

/**
 * RFID readers.
 */

const uint8_t NUM_READERS = 4;
const unsigned long RFID_READ_WAIT_MS = 500;

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

const uint16_t LED_BOX_NUM = 90;
const uint16_t LED_STRIPS_NUM = 90;

const uint8_t LED_BOX_PIN = 10;
const uint8_t LED_STRIPS_PIN = 11;

Adafruit_NeoPixel ledBox = Adafruit_NeoPixel(
    LED_BOX_NUM, LED_BOX_PIN, NEO_RGB + NEO_KHZ800);

Adafruit_NeoPixel ledStrips = Adafruit_NeoPixel(
    LED_STRIPS_NUM, LED_STRIPS_PIN, NEO_RGB + NEO_KHZ800);

const uint32_t COLOR_DEFAULT = Adafruit_NeoPixel::Color(180, 180, 220);

const uint8_t STRIP_BRIGHTNESS_DEFAULT = 10;
const uint8_t STRIP_BRIGHTNESS = 180;
const uint8_t BOX_BRIGHTNESS_DEFAULT = 10;
const uint8_t BOX_BRIGHTNESS = 180;

/**
 * Control pins and relays.
 */

const uint8_t PIN_OUTPUT_RELAY_PRIMARY = 12;
const uint8_t PIN_OUTPUT_RELAY_SECONDARY = A0;
const uint8_t PIN_INPUT_COMPLETION_SECONDARY = A1;
const uint8_t PIN_OUTPUT_ACTIVATION_BATS = A2;
const uint8_t PIN_INPUT_COMPLETION_BATS = A3;

Atm_digital digitalInputCompletionSecondary;
Atm_digital digitalInputCompletionBats;

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

/**
 * https://www.avdweb.nl/arduino/misc/handy-functions
 * When the output is high, the impedance must be high; this is done by making the pinmode INPUT. 
 * When the output is low, it must be 0V; this is done by making the pinmode OUTPUT and the output level LOW.
 * The receiver must use INPUT_PULLUP. 
 * The signal will be LOW if the transmitter is powered down.
 */
void setOpenDrainOutput(uint8_t pin, bool value)
{
  if (value)
  {
    pinMode(pin, INPUT);
  }
  else
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

void onSecondaryCompletion(int idx, int v, int up)
{
  Serial.println(F("Pulse from secondary RFID"));
  progState.isSecondaryComplete = true;
}

void onBatsCompletion(int idx, int v, int up)
{
  Serial.println(F("Pulse from bats"));
  progState.isBatsStageComplete = true;
}

void initControlPins()
{
  initRelay(PIN_OUTPUT_RELAY_PRIMARY);
  initRelay(PIN_OUTPUT_RELAY_SECONDARY);
  setOpenDrainOutput(PIN_OUTPUT_ACTIVATION_BATS, false);

  const int minDurationMs = 2000;
  const bool activeLow = false;
  const bool pullUp = true;

  digitalInputCompletionSecondary
      .begin(PIN_INPUT_COMPLETION_SECONDARY, minDurationMs, activeLow, pullUp)
      .onChange(HIGH, onSecondaryCompletion);

  digitalInputCompletionBats
      .begin(PIN_INPUT_COMPLETION_BATS, minDurationMs, activeLow, pullUp)
      .onChange(HIGH, onBatsCompletion);
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
  ledBox.begin();
  ledBox.setBrightness(BOX_BRIGHTNESS_DEFAULT);
  ledBox.fill(COLOR_DEFAULT);
  ledBox.show();
}

void initLedStrips()
{
  ledStrips.begin();
  ledStrips.setBrightness(STRIP_BRIGHTNESS_DEFAULT);
  ledStrips.fill(COLOR_DEFAULT);
  ledStrips.show();
}

void onTimerState(int idx, int v, int up)
{
  bool rfidComplete = progState.isPrimaryComplete || progState.isSecondaryComplete;

  // If the RFID stage is complete, we need to activate the bats controller

  if (rfidComplete)
  {
    setOpenDrainOutput(PIN_OUTPUT_ACTIVATION_BATS, true);

    if (!progState.flagBatsActivation)
    {
      Serial.println(F("Sent activation pulse to bats controller"));
      ledStrips.setBrightness(STRIP_BRIGHTNESS);
      ledStrips.fill(COLOR_DEFAULT);
      ledStrips.show();
      progState.flagBatsActivation = true;
    }
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
    if (progState.isSecondaryComplete)
    {
      lockRelay(PIN_OUTPUT_RELAY_PRIMARY);
      openRelay(PIN_OUTPUT_RELAY_SECONDARY);
    }
    else
    {
      openRelay(PIN_OUTPUT_RELAY_PRIMARY);
      lockRelay(PIN_OUTPUT_RELAY_SECONDARY);
    }

    if (!progState.flagRelayUpdate)
    {
      Serial.println(F("Updated relay state"));
      ledBox.setBrightness(BOX_BRIGHTNESS);
      ledBox.fill(COLOR_DEFAULT);
      ledBox.show();
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
