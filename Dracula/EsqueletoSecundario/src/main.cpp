#include <Arduino.h>
#include <rdm630.h>
#include <WS2812FX.h>
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
    {"1D00278F8200"},
    {"1D00277EC900"},
    {"1D00278E7600"},
    {"1D0027AA9B00"}};

/**
 * LEDs.
 */

const uint16_t LED_BOX_NUM = 100;
const uint16_t LED_STRIPS_NUM = 100;

const uint8_t LED_BOX_PIN = 10;
const uint8_t LED_STRIPS_PIN = 11;

WS2812FX ledBox = WS2812FX(LED_BOX_NUM, LED_BOX_PIN, NEO_GRB + NEO_KHZ800);
WS2812FX ledStrips = WS2812FX(LED_STRIPS_NUM, LED_STRIPS_PIN, NEO_GRB + NEO_KHZ800);

const uint16_t STRIP_DEFAULT_SPEED = 300;
const uint8_t STRIP_DEFAULT_BRIGHTNESS = 20;
const uint8_t STRIP_BRIGHTNESS = 160;
const uint16_t BOX_DEFAULT_SPEED = 2000;
const uint8_t BOX_DEFAULT_BRIGHTNESS = 20;
const uint16_t BOX_SPEED = 600;
const uint8_t BOX_BRIGHTNESS = 160;

/**
 * Control pins and relays.
 */

const uint8_t PIN_OUTPUT_COMPLETION = 12;
const uint8_t PIN_INPUT_COMPLETION_BATS = A0;

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
  bool isBatsStageComplete;
  bool flagRfidCompletion;
  bool flagLedBoxUpdate;
} ProgramState;

ProgramState progState = {
    .emptyReadCount = {0, 0, 0, 0},
    .isBatsStageComplete = false,
    .flagRfidCompletion = false,
    .flagLedBoxUpdate = false};

void initState()
{
  for (int i = 0; i < NUM_READERS; i++)
  {
    progState.emptyReadCount[i] = 0;
  }

  progState.isBatsStageComplete = false;
  progState.flagRfidCompletion = false;
  progState.flagLedBoxUpdate = false;
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

void onBatsCompletion(int idx, int v, int up)
{
  Serial.println(F("Pulse from bats"));
  progState.isBatsStageComplete = true;
}

void initControlPins()
{
  setOpenDrainOutput(PIN_OUTPUT_COMPLETION, false);

  const int minDurationMs = 500;
  const bool activeLow = false;
  const bool pullUp = true;

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
  ledBox.init();
  ledBox.setBrightness(BOX_DEFAULT_BRIGHTNESS);
  ledBox.setSpeed(BOX_DEFAULT_SPEED);
  ledBox.setMode(FX_MODE_FADE);
  ledBox.setColor(WHITE);
  ledBox.start();
}

void initLedStrips()
{
  ledStrips.init();
  ledStrips.setBrightness(STRIP_DEFAULT_BRIGHTNESS);
  ledStrips.setSpeed(STRIP_DEFAULT_SPEED);
  ledStrips.setMode(FX_MODE_STATIC);
  ledStrips.setColor(WHITE);
  ledStrips.start();
}

void onTimerState(int idx, int v, int up)
{
  if (!progState.flagRfidCompletion)
  {
    pollRfidReaders();
    printCurrentTags();

    if (isTagsStateValid())
    {
      Serial.println(F("Setting completion flag"));
      setOpenDrainOutput(PIN_OUTPUT_COMPLETION, true);
      ledStrips.setMode(FX_MODE_COLOR_WIPE);
      ledStrips.setBrightness(STRIP_BRIGHTNESS);
      progState.flagRfidCompletion = true;
    }
  }

  if (progState.flagRfidCompletion && progState.isBatsStageComplete && !progState.flagLedBoxUpdate)
  {
    Serial.println(F("Updating box LED"));
    ledBox.setSpeed(BOX_SPEED);
    ledBox.setBrightness(BOX_BRIGHTNESS);
    progState.flagLedBoxUpdate = true;
  }
  else if (!progState.flagRfidCompletion && progState.isBatsStageComplete)
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
  Serial.println(F("Primary Skeleton program"));
}

void loop()
{
  automaton.run();
  ledBox.service();
  ledStrips.service();
}
