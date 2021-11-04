#include <Arduino.h>
#include <rdm630.h>
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
 * Control pins.
 */

const uint8_t PIN_OUTPUT_COMPLETION = 12;

/**
 * Program state.
 */

const uint32_t TIMER_STATE_MS = 200;

Atm_timer timerState;

String currentTags[NUM_READERS];

typedef struct programState
{
  unsigned int emptyReadCount[NUM_READERS];
} ProgramState;

ProgramState progState = {
    .emptyReadCount = {0, 0, 0, 0}};

void initState()
{
  for (int i = 0; i < NUM_READERS; i++)
  {
    progState.emptyReadCount[i] = 0;
  }
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

void initControlPins()
{
  setOpenDrainOutput(PIN_OUTPUT_COMPLETION, false);
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

void onTimerState(int idx, int v, int up)
{
  pollRfidReaders();
  printCurrentTags();

  if (isTagsStateValid())
  {
    Serial.println(F("Output pin: ON"));
    setOpenDrainOutput(PIN_OUTPUT_COMPLETION, true);
  }
  else
  {
    Serial.println(F("Output pin: OFF"));
    setOpenDrainOutput(PIN_OUTPUT_COMPLETION, false);
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
  initTimerState();
  Serial.println(F("Esqueleto Secundario"));
}

void loop()
{
  automaton.run();
}
