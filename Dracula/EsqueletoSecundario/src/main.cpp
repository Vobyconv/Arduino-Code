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

const uint16_t LED_BOX_NUM = 90;
const uint16_t LED_STRIPS_NUM = 90;

const uint8_t LED_BOX_PIN = 10;
const uint8_t LED_STRIPS_PIN = 11;

Adafruit_NeoPixel ledBox = Adafruit_NeoPixel(
    LED_BOX_NUM, LED_BOX_PIN, NEO_RGB + NEO_KHZ800);

Adafruit_NeoPixel ledStrips = Adafruit_NeoPixel(
    LED_STRIPS_NUM, LED_STRIPS_PIN, NEO_RGB + NEO_KHZ800);

const uint32_t COLOR_DEFAULT = Adafruit_NeoPixel::Color(200, 10, 10);

const uint8_t STRIP_BRIGHTNESS_DEFAULT = 10;
const uint8_t STRIP_BRIGHTNESS = 180;
const uint8_t BOX_BRIGHTNESS_DEFAULT = 10;
const uint8_t BOX_BRIGHTNESS = 180;

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

  const int minDurationMs = 2000;
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
  // Poll the RFID readers if they are still pending

  if (!progState.flagRfidCompletion)
  {
    pollRfidReaders();
    printCurrentTags();
  }

  // Send completion pulse if the RFID readers are pending and the current tags are valid

  if (!progState.flagRfidCompletion && isTagsStateValid())
  {
    Serial.println(F("Sending RFID completion pulse"));
    setOpenDrainOutput(PIN_OUTPUT_COMPLETION, true);
    ledStrips.setBrightness(STRIP_BRIGHTNESS);
    ledStrips.fill(COLOR_DEFAULT);
    ledStrips.show();
    progState.flagRfidCompletion = true;
  }

  // Turn on the box when the RFID stage is OK and the completion pulse from the bats has been received

  if (progState.flagRfidCompletion && progState.isBatsStageComplete && !progState.flagLedBoxUpdate)
  {
    Serial.println(F("Updating box LED"));
    ledBox.setBrightness(BOX_BRIGHTNESS);
    ledBox.fill(COLOR_DEFAULT);
    ledBox.show();
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
}
