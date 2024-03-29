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

Atm_button buttonRead;

/**
 * RFID readers.
 */

const uint8_t NUM_READERS = 2;

const unsigned long RFID_READ_WAIT_MS = 50;
const unsigned long RFID_MAX_WAIT_MS = 6000;

// RX, TX
RDM6300 rfidOne(5, 6, RFID_READ_WAIT_MS);
RDM6300 rfidTwo(7, 8, RFID_READ_WAIT_MS);

RDM6300 rfidReaders[NUM_READERS] = {rfidOne, rfidTwo};

const int CODE_UNKNOWN = -1;
const int CODE_MISSING = -2;

const int NUM_TAGS_ONE = 3;
const int NUM_TAGS_TWO = 1;

String tagsOne[NUM_TAGS_ONE] = {
    "09008FCE2400",
    "0700110E1F00",
    "09008F3A4000"};

String tagsTwo[NUM_TAGS_TWO] = {
    "0700117AA200"};

String tagMessagesOne[NUM_TAGS_ONE] = {
    "Moneda S.XIV",
    "Moneda S.III",
    "Moneda S.IX"};

String tagMessagesTwo[NUM_TAGS_TWO] = {
    "Cabello Rubio"};

/**
 * Audio FX board.
 */

const uint8_t PIN_AUDIO_RST = 9;
const uint8_t PIN_AUDIO_ACT = 10;

uint8_t pinTracksOne[NUM_TAGS_ONE] = {11, 12, A0};
uint8_t pinTracksTwo[NUM_TAGS_TWO] = {A2};

/**
 * LCD screen.
 */

// SDA - A4
// SCL - A5
LiquidCrystal_I2C lcd(0x27, 2, 1, 0, 4, 5, 6, 7, 3, POSITIVE);

const String STR_START_READ = String("Escaneando");
const String STR_ERROR_NOT_FOUND = String("No encontrado");
const String STR_ERROR_UNKNOWN = String("Desconocido");

/**
 * Program state.
 */

const uint32_t TIMER_STATE_MS = 150;

Atm_timer stateTimer;
bool stateReaderSwitch;
unsigned long stateReadStartMs;

void initState()
{
  stateReaderSwitch = false;
  stateReadStartMs = 0;
}

void updateLcd(String theStr)
{
  const int lcdCols = 16;

  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(theStr.substring(0, lcdCols));

  if (theStr.length() > lcdCols)
  {
    lcd.setCursor(0, 1);
    lcd.print(theStr.substring(lcdCols, lcdCols * 2));
  }
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

void clearRfidReaders()
{
  const int numIters = 8;

  Serial.println(F("Clearing RFID readers"));

  for (int i = 0; i < numIters; i++)
  {
    for (int readerIdx = 0; readerIdx < NUM_READERS; readerIdx++)
    {
      rfidReaders[readerIdx].getTagId();
    }
  }

  Serial.println(F("Cleared"));
}

int readRfid(uint8_t readerIdx)
{
  if (readerIdx >= NUM_READERS)
  {
    Serial.println(F("WARNING: Unexpected RFID index"));
    return CODE_UNKNOWN;
  }

  Serial.print(F("Reading RFID #"));
  Serial.println(readerIdx);

  String tagId = rfidReaders[readerIdx].getTagId();

  if (tagId.length() == 0)
  {
    return CODE_MISSING;
  }

  Serial.print(F("Tag ID: "));
  Serial.println(tagId);

  int numTags = readerIdx == 0 ? NUM_TAGS_ONE : NUM_TAGS_TWO;
  String *tagsArr = readerIdx == 0 ? tagsOne : tagsTwo;

  for (int tagIdx = 0; tagIdx < numTags; tagIdx++)
  {
    bool isEqual = tagsArr[tagIdx].compareTo(tagId) == 0;

    if (isEqual)
    {
      return tagIdx;
    }
  }

  return CODE_UNKNOWN;
}

uint8_t getActiveReaderIdx()
{
  return stateReaderSwitch ? 1 : 0;
}

void updateRfidState()
{
  if (stateReadStartMs == 0)
  {
    return;
  }

  uint8_t readerIdx = getActiveReaderIdx();

  if (readerIdx >= NUM_READERS)
  {
    Serial.println(F("WARNING: Unexpected RFID index"));
    return;
  }

  int tagIdx = readRfid(readerIdx);

  if (tagIdx == CODE_UNKNOWN)
  {
    Serial.println(F("Unknown tag"));
    updateLcd(STR_ERROR_UNKNOWN);
  }
  else if (tagIdx >= 0)
  {
    Serial.print(F("Found tag #"));
    Serial.print(tagIdx);
    Serial.print(F(" on RFID #"));
    Serial.println(readerIdx);

    String msg = readerIdx == 0 ? tagMessagesOne[tagIdx] : tagMessagesTwo[tagIdx];
    uint8_t tPin = readerIdx == 0 ? pinTracksOne[tagIdx] : pinTracksTwo[tagIdx];

    updateLcd(msg);
    playTrack(tPin);
  }

  unsigned long millisLimit = stateReadStartMs + RFID_MAX_WAIT_MS;
  bool isReadTimeout = millis() > millisLimit;

  if (tagIdx != CODE_MISSING || isReadTimeout)
  {
    Serial.println(F("Read end"));
    stateReadStartMs = 0;
  }

  if (tagIdx == CODE_MISSING && isReadTimeout)
  {
    Serial.println(F("Tag read timeout"));
    updateLcd(STR_ERROR_NOT_FOUND);
  }
}

void updateReaderSwitch()
{
  stateReaderSwitch = digitalRead(PIN_BTN_SWITCH) ? true : false;
  ledStrip.fill(0, LED_NUM - 2);
  // Descomentar estas lineas para encender los leds indicadores de RFID
  // uint16_t pixelIdx = stateReaderSwitch ? LED_NUM - 1 : LED_NUM - 2;
  // ledStrip.setPixelColor(pixelIdx, Adafruit_NeoPixel::Color(250, 250, 0));
  ledStrip.show();
}

void updateReadLedEffect()
{
  if (stateReadStartMs == 0)
  {
    ledStrip.fill(0, 0, LED_NUM - 2);
    ledStrip.show();
    return;
  }

  unsigned long now = millis();

  if (now <= stateReadStartMs)
  {
    return;
  }

  const float loopsPerRead = 20.0;
  const int minBrightness = 50;
  const int maxBrightness = 250;

  uint16_t msPerLoop = RFID_MAX_WAIT_MS / loopsPerRead;
  unsigned long msDiff = now - stateReadStartMs;
  float currLoopRatio = (float)(msDiff % msPerLoop) / msPerLoop;
  int colorVal = map(currLoopRatio * 100, 0, 100, minBrightness, maxBrightness);

  uint32_t fillColor = Adafruit_NeoPixel::Color(0, 0, colorVal);
  ledStrip.fill(fillColor, 0, LED_NUM - 2);
  ledStrip.show();
}

void onTimerState(int idx, int v, int up)
{
  updateReaderSwitch();
  updateRfidState();
  updateReadLedEffect();
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
  clearRfidReaders();
}

void initButtons()
{
  pinMode(PIN_BTN_SWITCH, INPUT_PULLUP);

  buttonRead
      .begin(PIN_BTN_READ)
      .onPress(onReadPress);
}

void initLed()
{
  const uint8_t defaultBrightness = 180;

  ledStrip.begin();
  ledStrip.setBrightness(defaultBrightness);
  ledStrip.clear();
  ledStrip.show();
}

void startupEffect()
{
  const uint8_t numIters = 3;
  const uint16_t delayMs = 250;
  const uint32_t color = Adafruit_NeoPixel::Color(250, 250, 250);

  for (uint8_t i = 0; i < numIters; i++)
  {
    ledStrip.fill(color);
    ledStrip.show();
    delay(delayMs);
    ledStrip.clear();
    ledStrip.show();
    delay(delayMs);
  }

  ledStrip.clear();
  ledStrip.show();
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
  initLed();
  initTimerState();

  Serial.println(F(">> Herencias"));

  startupEffect();
}

void loop()
{
  automaton.run();
}
