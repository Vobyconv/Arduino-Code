#include <rdm630.h>
#include <Automaton.h>
#include <LCD.h>
#include <LiquidCrystal_I2C.h>
#include <Wire.h>
#include <Adafruit_NeoPixel.h>

/**
 * LED strip.
 */

const uint16_t LED_NUM = 12;
const uint8_t LED_PIN = 2;

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(
    LED_NUM,
    LED_PIN,
    NEO_GRB + NEO_KHZ800);

/**
 * Buttons.
 */

const uint8_t PIN_BTN_SWITCH = 3;
const uint8_t PIN_BTN_READ = 4;

Atm_button buttonSwitch;
Atm_button buttonRead;

/**
 * RFID readers.
 */

const uint8_t NUM_READERS = 2;

const unsigned long RFID_READ_WAIT_MS = 250;
const unsigned long RFID_MAX_WAIT_MS = 6000;

// RX, TX
RDM6300 rfidOne(5, 6, RFID_READ_WAIT_MS);
RDM6300 rfidTwo(7, 8, RFID_READ_WAIT_MS);

RDM6300 rfidReaders[NUM_READERS] = {rfidOne, rfidTwo};

const int CODE_UNKNOWN = -1;
const int CODE_MISSING = -2;

const int NUM_TAGS_ONE = 4;
const int NUM_TAGS_TWO = 1;

String tagsOne[NUM_TAGS_ONE] = {
    "09008FF3DD00",
    "09008F3CCC00",
    "09008F3CCC10",
    "09008F3CCC20"};

String tagsTwo[NUM_TAGS_TWO] = {
    "09008FF3DD50"};

String tagMessagesOne[NUM_TAGS_ONE] = {
    "Tag 1-1",
    "Tag 1-2",
    "Tag 1-3",
    "Tag 1-4"};

String tagMessagesTwo[NUM_TAGS_TWO] = {
    "Tag 2-1"};

/**
 * Audio FX board.
 */

const uint8_t PIN_AUDIO_RST = 9;
const uint8_t PIN_AUDIO_ACT = 10;

uint8_t pinTracksOne[NUM_TAGS_ONE] = {11, 12, A0, A1};
uint8_t pinTracksTwo[NUM_TAGS_TWO] = {A2};

/**
 * LCD screen.
 */

// SDA - A4
// SCL - A5
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const String STR_START_READ = String("Escaneando");
const String STR_ERROR_NOT_FOUND = String("No encontrado");
const String STR_ERROR_UNKNOWN = String("Elemento desconocido");

/**
 * Program state.
 */

const uint32_t TIMER_STATE_MS = 200;

Atm_timer stateTimer;
uint8_t stateReaderActiveIdx;
unsigned long stateReadStartMs;

void initState()
{
  stateReaderActiveIdx = 0;
  stateReadStartMs = 0;
}

void updateLcd(String theStr)
{
  const int lcdCols = 16;

  int from = theStr.length() - lcdCols;
  from = from < 0 ? 0 : from;

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(theStr.substring(from));
}

void clearLcd()
{
  lcd.clear();
}

void initLcd()
{
  Serial.println(F("Initializing LCD"));

  lcd.begin(16, 2);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.setBacklight(HIGH);

  clearLcd();

  Serial.println(F("LCD initialized"));
}

bool isTrackPlaying()
{
  return digitalRead(PIN_AUDIO_ACT) == LOW;
}

void playTrack(uint8_t trackPin)
{
  if (isTrackPlaying())
  {
    Serial.println(F("Skipping: Audio still playing"));
    return;
  }

  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
  delay(200);
  pinMode(trackPin, INPUT);
}

void initAudioPins()
{
  for (uint8_t i = 0; i < NUM_TAGS_ONE; i++)
  {
    pinMode(pinTracksOne[i], INPUT);
  }

  for (uint8_t i = 0; i < NUM_TAGS_TWO; i++)
  {
    pinMode(pinTracksTwo[i], INPUT);
  }

  pinMode(PIN_AUDIO_ACT, INPUT);
  pinMode(PIN_AUDIO_RST, INPUT);
}

void resetAudio()
{
  Serial.println(F("Audio FX reset"));

  digitalWrite(PIN_AUDIO_RST, LOW);
  pinMode(PIN_AUDIO_RST, OUTPUT);
  delay(10);
  pinMode(PIN_AUDIO_RST, INPUT);

  Serial.println(F("Waiting for Audio FX startup"));

  delay(2000);
}

void initRfidReaders()
{
  for (int i = 0; i < NUM_READERS; i++)
  {
    rfidReaders[i].begin();
  }
}

int readRfid(uint8_t readerIdx)
{
  Serial.print(F("Reading RFID #"));
  Serial.println(readerIdx);

  String tagId = rfidReaders[readerIdx].getTagId();

  if (tagId.length() == 0)
  {
    return CODE_MISSING;
  }

  int numTags = readerIdx == 0 ? NUM_TAGS_ONE : NUM_TAGS_TWO;

  for (int tagIdx = 0; tagIdx < numTags; tagIdx++)
  {
    bool isEqualTag = readerIdx == 0
                          ? tagsOne[tagIdx].compareTo(tagId) == 0
                          : tagsTwo[tagIdx].compareTo(tagId) == 0;

    if (isEqualTag)
    {
      return tagIdx;
    }
  }

  return CODE_UNKNOWN;
}

void updateRfidState()
{
  if (stateReadStartMs == 0 || stateReaderActiveIdx >= 2)
  {
    return;
  }

  int tagIdx = readRfid(stateReaderActiveIdx);

  if (tagIdx == CODE_UNKNOWN)
  {
    Serial.println(F("Unknown tag"));
    updateLcd(STR_ERROR_UNKNOWN);
  }
  else if (tagIdx >= 0)
  {
    Serial.print(F("Found valid tag: "));
    Serial.println(tagIdx);

    String theMessage = stateReaderActiveIdx == 0
                            ? tagMessagesOne[tagIdx]
                            : tagMessagesTwo[tagIdx];

    uint8_t theTrackPin = stateReaderActiveIdx == 0
                              ? pinTracksOne[tagIdx]
                              : pinTracksTwo[tagIdx];

    updateLcd(theMessage);
    playTrack(theTrackPin);
  }

  unsigned long millisLimit = stateReadStartMs + RFID_MAX_WAIT_MS;
  bool isReadTimeout = millis() > millisLimit;

  if (tagIdx != CODE_MISSING || isReadTimeout)
  {
    Serial.println(F("Read flag reset"));
    stateReadStartMs = 0;
  }

  if (tagIdx == CODE_MISSING && isReadTimeout)
  {
    Serial.println(F("Tag read timeout"));
    updateLcd(STR_ERROR_NOT_FOUND);
  }
}

void onTimerState(int idx, int v, int up)
{
  updateRfidState();
}

void initTimerState()
{
  stateTimer
      .begin(TIMER_STATE_MS)
      .repeat(-1)
      .onTimer(onTimerState)
      .start();
}

void onReadPress(int idx, int v, int up)
{
  Serial.println(F("Read press"));

  if (stateReadStartMs > 0)
  {
    return;
  }

  stateReadStartMs = millis();
  updateLcd(STR_START_READ);
}

void onSwitchPress(int idx, int v, int up)
{
  Serial.println(F("Switch press"));
  stateReaderActiveIdx = 0;
}

void onSwitchRelease(int idx, int v, int up)
{
  Serial.println(F("Switch release"));
  stateReaderActiveIdx = 1;
}

void initButtons()
{
  const int debounceDelayMs = 50;

  buttonSwitch
      .begin(PIN_BTN_SWITCH)
      .debounce(debounceDelayMs)
      .onPress(onSwitchPress)
      .onRelease(onSwitchRelease);

  buttonRead
      .begin(PIN_BTN_READ)
      .debounce(debounceDelayMs)
      .onPress(onReadPress);
}

void setup()
{
  Serial.begin(9600);

  initState();
  initAudioPins();
  resetAudio();
  initLcd();
  initRfidReaders();
  initButtons();
  initTimerState();

  Serial.println(F(">> Herencias"));
}

void loop()
{
  automaton.run();
}