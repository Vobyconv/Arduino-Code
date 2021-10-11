#include <Arduino.h>
#include <SlowSoftWire.h>
#include "Adafruit_TCS34725.h"
#include <Automaton.h>
#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>
#include <SoftwareSerial.h>
#include <SerialRFID.h>
#include <KickSort.h>
#include <CircularBuffer.h>

/**
 * RGB sensors.
 */

const uint8_t NUM_RGB_SENSORS = 4;

SlowSoftWire softWires[NUM_RGB_SENSORS] = {
    SlowSoftWire(22, 23),
    SlowSoftWire(24, 25),
    SlowSoftWire(26, 27),
    SlowSoftWire(28, 29)};

Adafruit_TCS34725 rgbSensors[NUM_RGB_SENSORS] = {
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X),
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X),
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X),
    Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_4X)};

/**
 * LED strips for the RGB sensors.
 */

const uint16_t LED_RGB_SENSOR_NUMS[NUM_RGB_SENSORS] = {
    90, 90, 90, 90};

const uint8_t LED_RGB_SENSOR_PINS[NUM_RGB_SENSORS] = {
    30, 31, 32, 33};

WS2812FX ledRgbSensors[NUM_RGB_SENSORS] = {
    WS2812FX(LED_RGB_SENSOR_NUMS[0], LED_RGB_SENSOR_PINS[0], NEO_GRB + NEO_KHZ800),
    WS2812FX(LED_RGB_SENSOR_NUMS[1], LED_RGB_SENSOR_PINS[1], NEO_GRB + NEO_KHZ800),
    WS2812FX(LED_RGB_SENSOR_NUMS[2], LED_RGB_SENSOR_PINS[2], NEO_GRB + NEO_KHZ800),
    WS2812FX(LED_RGB_SENSOR_NUMS[3], LED_RGB_SENSOR_PINS[3], NEO_GRB + NEO_KHZ800)};

const uint16_t LED_COALS_NUM = 150;
const uint8_t LED_COALS_PIN = 34;

WS2812FX ledCoals = WS2812FX(LED_COALS_NUM, LED_COALS_PIN, NEO_GRB + NEO_KHZ800);

/**
 * Colors recognized by the RGB sensor.
 */

const uint16_t CLEAR_CHANNEL_THRESHOLD = 450;

typedef struct colorDef
{
  float redRatio;
  float greenRatio;
  float blueRatio;
  uint32_t ledColor;
} ColorDef;

const uint8_t NUM_RECOGNIZED_COLORS = 4;

const ColorDef RECOGNIZED_COLORS[NUM_RECOGNIZED_COLORS] = {
    {.redRatio = 0.5,
     .greenRatio = 0,
     .blueRatio = 0,
     .ledColor = Adafruit_NeoPixel::Color(255, 0, 0)},
    {.redRatio = 0,
     .greenRatio = 0.5,
     .blueRatio = 0,
     .ledColor = Adafruit_NeoPixel::Color(0, 255, 0)},
    {.redRatio = 0,
     .greenRatio = 0,
     .blueRatio = 0.5,
     .ledColor = Adafruit_NeoPixel::Color(0, 0, 255)},
    {.redRatio = 0.35,
     .greenRatio = 0.35,
     .blueRatio = 0,
     .ledColor = Adafruit_NeoPixel::Color(255, 255, 0)}};

const uint8_t RGB_SENSORS_COLOR_KEY[NUM_RGB_SENSORS] = {0, 1, 2, 3};

/**
 * RGB sensor timer.
 */

const uint32_t TIMER_RGB_MS = 1000;

Atm_timer timerRgbSensor;

/**
 * RFID readers.
 */

const uint8_t NUM_RFID = 5;
const uint8_t NUM_RFID_SOFT_SERIALS = 2;

SoftwareSerial sSerial4 = SoftwareSerial(3, 2);
SoftwareSerial sSerial5 = SoftwareSerial(4, 5);

typedef struct softSerialDef
{
  uint8_t rfidIdx;
  SoftwareSerial sSerial;
} SoftSerialDef;

SoftSerialDef softSerials[NUM_RFID_SOFT_SERIALS] = {
    {.rfidIdx = 3, .sSerial = sSerial4},
    {.rfidIdx = 4, .sSerial = sSerial5}};

SerialRFID rfids[NUM_RFID] = {
    SerialRFID(Serial1),
    SerialRFID(Serial2),
    SerialRFID(Serial3),
    SerialRFID(sSerial4),
    SerialRFID(sSerial5)};

Atm_digital tagInRangeDigitals[NUM_RFID];

const uint8_t TAG_IN_RANGE_PINS[NUM_RFID] = {
    38, 39, 40, 41, 42};

const uint32_t TIMER_RFID_MS = 300;

Atm_timer timerRfid;

/**
 * RFID materials.
 */

enum Materials
{
  unknown,
  gold,
  silver,
  bronze,
  mithril,
  orichalcum
};

const uint8_t NUM_RECIPES = 3;

Materials recipes[NUM_RECIPES][NUM_RFID] = {
    {gold, gold, gold, silver, silver},
    {gold, gold, gold, bronze, silver},
    {gold, gold, gold, bronze, bronze}};

const uint8_t NUM_TAGS_MATERIAL = 4;

char tagsGold[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "1D00277FBDF8",
    "1D00277FBDF8",
    "1D00277FBDF8",
    "1D00277FBDF8"};

char tagsSilver[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "1D00277FBDF9",
    "1D00277FBDF9",
    "1D00277FBDF9",
    "1D00277FBDF9"};

char tagsBronze[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "1D00277FBDF0",
    "1D00277FBDF0",
    "1D00277FBDF0",
    "1D00277FBDF0"};

char tagsMithril[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "1D00277FBDF1",
    "1D00277FBDF1",
    "1D00277FBDF1",
    "1D00277FBDF1"};

char tagsOrichalcum[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "1D00277FBDF2",
    "1D00277FBDF2",
    "1D00277FBDF2",
    "1D00277FBDF2"};

/**
 * Knock sensors.
 */

const uint8_t NUM_KNOCK_SENSORS = 2;
const uint8_t KNOCK_PINS[NUM_KNOCK_SENSORS] = {A2, A3};
const int KNOCK_SAMPLERATE = 50;
const int KNOCK_RANGE_MIN = 0;
const int KNOCK_RANGE_MAX = 100;
const int KNOCK_THRESHOLD = 20;

const uint8_t SIZE_KNOCK_PATTERN = 5;

const uint8_t KNOCK_PATTERNS[NUM_RECIPES][SIZE_KNOCK_PATTERN] = {
    {0, 0, 0, 0, 0},
    {1, 1, 1, 1, 1},
    {0, 1, 0, 1, 0}};

Atm_analog knockAnalogs[NUM_KNOCK_SENSORS];
Atm_controller knockControllers[NUM_KNOCK_SENSORS];

/**
 * LED strips for the knock sensors (anvils).
 */

const uint16_t LED_KNOCK_NUMS[NUM_KNOCK_SENSORS] = {12, 12};
const uint16_t LED_KNOCK_PINS[NUM_KNOCK_SENSORS] = {4, 5};

Adafruit_NeoPixel ledKnockSensors[NUM_KNOCK_SENSORS] = {
    Adafruit_NeoPixel(LED_KNOCK_NUMS[0], LED_KNOCK_PINS[0], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_KNOCK_NUMS[1], LED_KNOCK_PINS[1], NEO_GRB + NEO_KHZ800)};

const uint32_t LED_KNOCK_COLORS[NUM_KNOCK_SENSORS] = {
    Adafruit_NeoPixel::Color(0, 0, 255),
    Adafruit_NeoPixel::Color(255, 255, 0)};

const unsigned long LED_KNOCK_DELAY_MS = 50;

/**
 * LED strip to signal progress.
 */

const uint16_t LED_PROGRESS_NUM = 60;
const uint8_t LED_PROGRESS_PIN = 6;

WS2812FX ledProgress = WS2812FX(LED_PROGRESS_NUM, LED_PROGRESS_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t LED_PROGRESS_COLOR = Adafruit_NeoPixel::Color(255, 255, 0);

/**
 * LED strip for the eye of Odin.
 */

const uint16_t LED_EYE_NUM = 20;
const uint16_t LED_EYE_PIN = 7;

Adafruit_NeoPixel ledEye = Adafruit_NeoPixel(LED_EYE_NUM, LED_EYE_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t LED_EYE_COLORS[NUM_KNOCK_SENSORS] = {
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(0, 0, 255)};

const unsigned long LED_EYE_DELAY_MS = 200;

/**
 * Knock (anvil) audio timings.
 */

const unsigned long EYE_AUDIO_TIMINGS[NUM_RECIPES][SIZE_KNOCK_PATTERN] = {
    {0, 1000, 2000, 2500, 3500},
    {2000, 4000, 6000, 8000, 10000},
    {0, 3000, 6000, 9000, 12000}};

const unsigned long EYE_AUDIO_DELAY_LOOPS_MS = 8000;

/**
 * Audio FX.
 */

const uint8_t PIN_AUDIO_ACT = 8;
const uint8_t PIN_AUDIO_RST = 9;
const uint8_t AUDIO_TRACK_PINS[NUM_KNOCK_SENSORS] = {10, 11};
const unsigned long AUDIO_PLAY_DELAY_MS = 200;

/**
 * General-purpose timer.
 */

const uint32_t TIMER_GENERAL_MS = 50;

Atm_timer timerGeneral;

/**
 * Program state.
 */

int16_t rgbSensorsColorIndex[NUM_RGB_SENSORS];

char stateTags[NUM_RFID][SIZE_TAG_ID];
unsigned long stateTagsMillis[NUM_RFID];

const uint16_t KNOCK_BUF_SIZE = 30;
CircularBuffer<int, KNOCK_BUF_SIZE> knockHistory;

unsigned long ledKnockSensorsFillMillis[NUM_KNOCK_SENSORS];

typedef struct programState
{
  int16_t *rgbSensorsColorIndex;
  bool isRgbSensorsPhaseCompleted;
  uint8_t currentRecipe;
  bool isAnvilStepActive;
  unsigned long *ledKnockSensorsFillMillis;
  unsigned long audioPlayMillis;
  bool isEyeAudioPlaying;
  unsigned long lastEyeAudioMillis;
  uint8_t currentEyeAudioIdx;
  bool ledEyeClearFlag;
} ProgramState;

ProgramState progState;

void validateSoftSerialDefs()
{
  for (uint8_t i = 0; i < NUM_RFID_SOFT_SERIALS; i++)
  {
    if (softSerials[i].rfidIdx >= NUM_RFID)
    {
      Serial.println(F("Invalid RFID reader index"));

      while (true)
      {
        delay(1000);
      }
    }
  }
}

void validateKnockPatterns()
{
  for (uint8_t idxRecipe = 0; idxRecipe < NUM_RECIPES; idxRecipe++)
  {
    for (uint8_t i = 0; i < SIZE_KNOCK_PATTERN; i++)
    {
      if (KNOCK_PATTERNS[idxRecipe][i] >= NUM_KNOCK_SENSORS)
      {
        Serial.println(F("Invalid knock pattern value"));

        while (true)
        {
          delay(1000);
        }
      }
    }
  }
}

void initState()
{
  validateSoftSerialDefs();
  validateKnockPatterns();

  progState.rgbSensorsColorIndex = rgbSensorsColorIndex;

  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    progState.rgbSensorsColorIndex[i] = -1;
  }

  for (uint8_t i = 0; i < NUM_RFID; i++)
  {
    memset(stateTags[i], 0, sizeof(stateTags[i]));
    stateTagsMillis[i] = 0;
  }

  for (uint8_t i = 0; i < NUM_KNOCK_SENSORS; i++)
  {
    progState.ledKnockSensorsFillMillis[i] = 0;
  }

  progState.isRgbSensorsPhaseCompleted = false;
  progState.currentRecipe = 0;
  progState.isAnvilStepActive = false;
  progState.audioPlayMillis = 0;
  progState.isEyeAudioPlaying = false;
  progState.lastEyeAudioMillis = 0;
  progState.currentEyeAudioIdx = 0;
  progState.ledEyeClearFlag = false;
}

bool isTrackPlaying()
{
  return progState.audioPlayMillis > 0 || digitalRead(PIN_AUDIO_ACT) == LOW;
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

  progState.audioPlayMillis = millis();
}

void initAudioPins()
{
  for (uint8_t i = 0; i < NUM_KNOCK_SENSORS; i++)
  {
    pinMode(AUDIO_TRACK_PINS[i], INPUT);
  }

  pinMode(PIN_AUDIO_ACT, INPUT);
  pinMode(PIN_AUDIO_RST, INPUT);
}

void resetAudio()
{
  const unsigned long resetDelayMs = 100;
  const unsigned long waitDelayMs = 2000;

  Serial.println(F("Audio FX reset"));

  digitalWrite(PIN_AUDIO_RST, LOW);
  pinMode(PIN_AUDIO_RST, OUTPUT);
  delay(resetDelayMs);
  pinMode(PIN_AUDIO_RST, INPUT);

  Serial.println(F("Waiting for Audio FX startup"));

  delay(waitDelayMs);
}

void initAudio()
{
  initAudioPins();
  resetAudio();
}

void updateLedProgress()
{
  if (ledProgress.isRunning())
  {
    ledProgress.stop();
  }

  uint16_t numPerPhase = floor((float)LED_PROGRESS_NUM / NUM_RECIPES);
  uint16_t count = min(numPerPhase * progState.currentRecipe, ledProgress.numPixels());

  const uint8_t idxSegment = 0;
  const uint16_t start = 0;
  const uint16_t speed = 1500;
  const bool reverse = false;

  ledProgress.clear();
  ledProgress.resetSegments();

  ledProgress.setSegment(
      idxSegment,
      start,
      count,
      FX_MODE_COLOR_WIPE_REV,
      LED_PROGRESS_COLOR,
      speed,
      reverse);

  if (!ledProgress.isRunning())
  {
    ledProgress.start();
  }
}

void initLedProgress()
{
  const uint8_t defaultBrightness = 150;

  ledProgress.init();
  ledProgress.setBrightness(defaultBrightness);
  ledProgress.setMode(FX_MODE_COLOR_WIPE_REV);
  ledProgress.setColor(LED_PROGRESS_COLOR);
  ledProgress.stop();
}

void initLedEye()
{
  const uint8_t defaultBrightness = 200;

  ledEye.begin();
  ledEye.setBrightness(defaultBrightness);
  ledEye.clear();
  ledEye.show();
}

void stopEyeAudioPattern()
{
  progState.isEyeAudioPlaying = false;
  progState.lastEyeAudioMillis = 0;
  progState.currentEyeAudioIdx = 0;
  progState.ledEyeClearFlag = false;
}

void startEyeAudioPattern()
{
  progState.isEyeAudioPlaying = true;
}

void advanceToAnvilStep()
{
  Serial.print(F("Advancing to anvil step: recipe #"));
  Serial.println(progState.currentRecipe);

  progState.isAnvilStepActive = true;
  stopEyeAudioPattern();
  startEyeAudioPattern();
}

void advanceToNextMaterialsPhase()
{
  progState.currentRecipe++;

  Serial.print(F("Advancing to next materials phase: recipe #"));
  Serial.println(progState.currentRecipe);

  progState.isAnvilStepActive = false;
  stopEyeAudioPattern();
  updateLedProgress();
}

void clearLedEye()
{
  if (!progState.ledEyeClearFlag)
  {
    return;
  }

  unsigned long now = millis();
  unsigned long millisClear = progState.lastEyeAudioMillis + LED_EYE_DELAY_MS;

  if (progState.lastEyeAudioMillis == 0 || now >= millisClear)
  {
    ledEye.clear();
    ledEye.show();
    progState.ledEyeClearFlag = false;
  }
}

void runEyeAudioPattern()
{
  clearLedEye();

  uint8_t idxRecipe = progState.currentRecipe;
  uint8_t idxPattern = progState.currentEyeAudioIdx;

  if (!progState.isEyeAudioPlaying || idxRecipe >= NUM_RECIPES)
  {
    return;
  }

  if (idxPattern > SIZE_KNOCK_PATTERN)
  {
    Serial.println(F("Unexpected value for knock pattern item index"));
    return;
  }

  unsigned long now = millis();

  if (progState.lastEyeAudioMillis == 0)
  {
    progState.lastEyeAudioMillis = now;
  }

  unsigned long baseMilllis = progState.lastEyeAudioMillis;

  if (idxPattern == SIZE_KNOCK_PATTERN)
  {
    unsigned long millisWait = baseMilllis + EYE_AUDIO_DELAY_LOOPS_MS;

    if (now >= millisWait)
    {
      progState.lastEyeAudioMillis = 0;
      progState.currentEyeAudioIdx = 0;
    }

    return;
  }

  unsigned long millisTarget = baseMilllis + EYE_AUDIO_TIMINGS[idxRecipe][idxPattern];

  if (now < millisTarget)
  {
    return;
  }

  Serial.print(F("Eye audio pattern: recipe #"));
  Serial.print(idxRecipe);
  Serial.print(F(" item #"));
  Serial.println(idxPattern);

  uint8_t idxKnock = KNOCK_PATTERNS[idxRecipe][idxPattern];

  playTrack(AUDIO_TRACK_PINS[idxKnock]);

  ledEye.clear();
  ledEye.fill(LED_EYE_COLORS[idxKnock]);
  ledEye.show();

  progState.lastEyeAudioMillis = now;
  progState.currentEyeAudioIdx++;
  progState.ledEyeClearFlag = true;
}

Materials getMaterial(char *theTag)
{
  if (strlen(theTag) == 0)
  {
    return unknown;
  }

  for (uint8_t i = 0; i < NUM_TAGS_MATERIAL; i++)
  {
    if (SerialRFID::isEqualTag(theTag, tagsGold[i]))
    {
      return gold;
    }
    else if (SerialRFID::isEqualTag(theTag, tagsSilver[i]))
    {
      return silver;
    }
    else if (SerialRFID::isEqualTag(theTag, tagsBronze[i]))
    {
      return bronze;
    }
    else if (SerialRFID::isEqualTag(theTag, tagsMithril[i]))
    {
      return mithril;
    }
    else if (SerialRFID::isEqualTag(theTag, tagsOrichalcum[i]))
    {
      return orichalcum;
    }
  }

  return unknown;
}

int16_t getActiveRecipe()
{
  Materials readerMaterials[NUM_RFID];

  for (uint8_t idxRfid = 0; idxRfid < NUM_RFID; idxRfid++)
  {
    readerMaterials[idxRfid] = getMaterial(stateTags[idxRfid]);
  }

  KickSort<Materials>::quickSort(readerMaterials, NUM_RFID);

  for (uint8_t idxRecipe = 0; idxRecipe < NUM_RECIPES; idxRecipe++)
  {
    KickSort<Materials>::quickSort(recipes[idxRecipe], NUM_RFID);

    bool isEqual = true;

    for (uint8_t i = 0; i < NUM_RFID; i++)
    {
      if (recipes[idxRecipe][i] != readerMaterials[i])
      {
        isEqual = false;
        break;
      }
    }

    if (isEqual)
    {
      return idxRecipe;
    }
  }

  return -1;
}

void onTagInRangeChange(int idx, int v, int up)
{
  bool isInRange = v == 1;

  Serial.print(F("Tag in range #"));
  Serial.print(idx);
  Serial.print(F(": "));
  Serial.println(isInRange);

  if (isInRange)
  {
    return;
  }

  memset(stateTags[idx], 0, sizeof(stateTags[idx]));

  for (uint8_t i = 0; i < NUM_RFID_SOFT_SERIALS; i++)
  {
    if (softSerials[i].rfidIdx == idx)
    {
      Serial.print(F("Listening on SoftSerial #"));
      Serial.println(idx);

      softSerials[i].sSerial.listen();
    }
  }
}

void initRfidsTagInRange()
{
  const uint16_t debounceMs = 150;
  const bool activeLow = false;
  const bool pullUp = false;

  for (int i = 0; i < NUM_RFID; i++)
  {
    tagInRangeDigitals[i]
        .begin(TAG_IN_RANGE_PINS[i], debounceMs, activeLow, pullUp)
        .onChange(onTagInRangeChange, i);
  }
}

void readRfid(uint8_t readerIdx)
{
  if (readerIdx >= NUM_RFID)
  {
    return;
  }

  char newTag[SIZE_TAG_ID];

  if (rfids[readerIdx].readTag(newTag, sizeof(newTag)))
  {
    Serial.print(F("RFID #"));
    Serial.print(readerIdx);
    Serial.print(F(":"));
    Serial.print(newTag);
    Serial.println();
    Serial.flush();

    for (int k = 0; k < LEN_TAG_ID; k++)
    {
      stateTags[readerIdx][k] = newTag[k];
    }

    stateTagsMillis[readerIdx] = millis();
  }
}

void onTimerRfid(int idx, int v, int up)
{
  for (uint8_t i = 0; i < NUM_RFID; i++)
  {
    readRfid(i);
  }

  if (progState.isRgbSensorsPhaseCompleted || progState.isAnvilStepActive)
  {
    return;
  }

  if (getActiveRecipe() == progState.currentRecipe)
  {
    advanceToAnvilStep();
  }
}

void initTimerRfid()
{
  timerRfid
      .begin(TIMER_RFID_MS)
      .repeat(-1)
      .onTimer(onTimerRfid)
      .start();
}

bool isRgbSensorsStateValid()
{
  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    if (progState.rgbSensorsColorIndex[i] != RGB_SENSORS_COLOR_KEY[i])
    {
      return false;
    }
  }

  return true;
}

void readRgbSensor(uint8_t sensorIdx)
{
  if (sensorIdx >= NUM_RGB_SENSORS)
  {
    return;
  }

  uint16_t rawRed, rawGreen, rawBlue, rawClear;

  Serial.print(F("Reading RGB #"));
  Serial.println(sensorIdx);

  rgbSensors[sensorIdx].getRawData(&rawRed, &rawGreen, &rawBlue, &rawClear);

  Serial.print("R:\t");
  Serial.print(rawRed);
  Serial.print("\tG:\t");
  Serial.print(rawGreen);
  Serial.print("\tB:\t");
  Serial.print(rawBlue);
  Serial.print("\tC:\t");
  Serial.print(rawClear);
  Serial.println();

  if (rawClear < CLEAR_CHANNEL_THRESHOLD || rawClear == 0)
  {
    return;
  }

  uint32_t sum = rawRed + rawGreen + rawBlue;

  float redRatio = (float)rawRed / sum;
  float greenRatio = (float)rawGreen / sum;
  float blueRatio = (float)rawBlue / sum;

  Serial.print("R.R:\t");
  Serial.print(redRatio);
  Serial.print("\tR.G:\t");
  Serial.print(greenRatio);
  Serial.print("\tR.B:\t");
  Serial.print(blueRatio);
  Serial.println();

  for (uint8_t colorIdx = 0; colorIdx < NUM_RECOGNIZED_COLORS; colorIdx++)
  {
    if (redRatio >= RECOGNIZED_COLORS[colorIdx].redRatio &&
        greenRatio >= RECOGNIZED_COLORS[colorIdx].greenRatio &&
        blueRatio >= RECOGNIZED_COLORS[colorIdx].blueRatio)
    {
      Serial.print(F("Recognized color #"));
      Serial.println(colorIdx);
      progState.rgbSensorsColorIndex[sensorIdx] = colorIdx;
      return;
    }
  }
}

void updateLedRgbSensors()
{
  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    int16_t colorIdx = progState.rgbSensorsColorIndex[i];

    if (colorIdx >= 0 && colorIdx < NUM_RECOGNIZED_COLORS)
    {
      ledRgbSensors[i].setColor(RECOGNIZED_COLORS[colorIdx].ledColor);

      if (!ledRgbSensors[i].isRunning())
      {
        ledRgbSensors[i].start();
      }
    }
    else if (ledRgbSensors[i].isRunning())
    {
      ledRgbSensors[i].stop();
    }
  }
}

void onTimerRgbSensor(int idx, int v, int up)
{
  if (progState.isRgbSensorsPhaseCompleted)
  {
    return;
  }

  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    readRgbSensor(i);
  }

  updateLedRgbSensors();

  if (isRgbSensorsStateValid())
  {
    progState.isRgbSensorsPhaseCompleted = true;
    ledCoals.start();
  }
}

void initTimerRgbSensor()
{
  timerRgbSensor
      .begin(TIMER_RGB_MS)
      .repeat(-1)
      .onTimer(onTimerRgbSensor)
      .start();
}

void initLedRgbSensors()
{
  const uint16_t defaultSpeed = 600;
  const uint8_t defaultBrightness = 160;

  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    ledRgbSensors[i].init();
    ledRgbSensors[i].setBrightness(defaultBrightness);
    ledRgbSensors[i].setSpeed(defaultSpeed);
    ledRgbSensors[i].setMode(FX_MODE_COLOR_WIPE_INV);
    ledRgbSensors[i].stop();
  }
}

void initLedCoals()
{
  const uint16_t defaultSpeed = 300;
  const uint8_t defaultBrightness = 150;

  ledCoals.init();
  ledCoals.setBrightness(defaultBrightness);
  ledCoals.setSpeed(defaultSpeed);
  ledCoals.setMode(FX_MODE_FIRE_FLICKER);
  ledCoals.setColor(ORANGE);
  ledCoals.stop();
}

void initRgbSensors()
{
  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    Serial.print(F("Initializing RGB sensor #"));
    Serial.println(i);

    if (rgbSensors[i].begin(TCS34725_ADDRESS, &softWires[i]))
    {
      Serial.println(F("RGB Sensor initialized OK"));
    }
    else
    {
      Serial.println(F("Error initializing RGB sensor"));

      while (true)
      {
        delay(1000);
      }
    }
  }
}

void initLedKnockSensors()
{
  const uint8_t defaultBrightness = 150;

  for (int i = 0; i < NUM_KNOCK_SENSORS; i++)
  {
    ledKnockSensors[i].begin();
    ledKnockSensors[i].setBrightness(defaultBrightness);
    ledKnockSensors[i].clear();
    ledKnockSensors[i].show();
  }
}

bool isKnockSensorsStateValid()
{
  if (knockHistory.size() < SIZE_KNOCK_PATTERN)
  {
    return false;
  }

  for (uint8_t i = 0; i < SIZE_KNOCK_PATTERN; i++)
  {
    uint8_t bufIdx = knockHistory.size() - 1 - (SIZE_KNOCK_PATTERN - 1 - i);

    if (knockHistory[bufIdx] != KNOCK_PATTERNS[progState.currentRecipe][i])
    {
      return false;
    }
  }

  return true;
}

void onKnock(int idx, int v, int up)
{
  Serial.print(F("Knock #"));
  Serial.println(idx);

  if (!progState.isRgbSensorsPhaseCompleted || !progState.isAnvilStepActive)
  {
    return;
  }

  knockHistory.push(idx);

  ledKnockSensors[idx].fill(LED_KNOCK_COLORS[idx]);
  ledKnockSensors[idx].show();

  progState.ledKnockSensorsFillMillis[idx] = millis();

  if (isKnockSensorsStateValid())
  {
    advanceToNextMaterialsPhase();
  }
}

void initKnockSensors()
{
  for (uint8_t i = 0; i < NUM_KNOCK_SENSORS; i++)
  {
    knockAnalogs[i]
        .begin(KNOCK_PINS[i], KNOCK_SAMPLERATE)
        .range(KNOCK_RANGE_MIN, KNOCK_RANGE_MAX);

    knockControllers[i]
        .begin()
        .IF(knockAnalogs[i], '>', KNOCK_THRESHOLD)
        .onChange(true, onKnock, i);
  }
}

void clearLedKnockSensors()
{
  unsigned long now = millis();

  for (uint8_t idxKnock = 0; idxKnock < NUM_KNOCK_SENSORS; idxKnock++)
  {
    if (progState.ledKnockSensorsFillMillis[idxKnock] == 0)
    {
      continue;
    }
    else if (progState.ledKnockSensorsFillMillis[idxKnock] > now)
    {
      progState.ledKnockSensorsFillMillis[idxKnock] = 0;
      continue;
    }

    unsigned long diffMs = now - progState.ledKnockSensorsFillMillis[idxKnock];

    if (diffMs >= LED_KNOCK_DELAY_MS)
    {
      ledKnockSensors[idxKnock].clear();
      ledKnockSensors[idxKnock].show();
      progState.ledKnockSensorsFillMillis[idxKnock] = 0;
    }
  }
}

void clearAudioPins()
{
  if (progState.audioPlayMillis == 0)
  {
    return;
  }

  unsigned long now = millis();
  unsigned long diffMs = now - progState.audioPlayMillis;

  if (diffMs >= AUDIO_PLAY_DELAY_MS)
  {
    Serial.println(F("Clearing audio pins"));

    progState.audioPlayMillis = 0;

    for (uint8_t i = 0; i < NUM_KNOCK_SENSORS; i++)
    {
      pinMode(AUDIO_TRACK_PINS[i], INPUT);
    }
  }
}

void onTimerGeneral(int idx, int v, int up)
{
  clearLedKnockSensors();
  clearAudioPins();
  runEyeAudioPattern();
}

void initTimerGeneral()
{
  timerGeneral
      .begin(TIMER_GENERAL_MS)
      .repeat(-1)
      .onTimer(onTimerGeneral)
      .start();
}

void initSerials()
{
  const int bauds = 9600;

  Serial.begin(bauds);
  Serial1.begin(bauds);
  Serial2.begin(bauds);
  Serial3.begin(bauds);

  for (uint8_t i = 0; i < NUM_RFID_SOFT_SERIALS; i++)
  {
    softSerials[i].sSerial.begin(bauds);
  }
}

void serviceWS2812FX()
{
  ledCoals.service();
  ledProgress.service();

  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    ledRgbSensors[i].service();
  }
}

void setup(void)
{
  initSerials();
  initState();
  initRgbSensors();
  initTimerRgbSensor();
  initLedRgbSensors();
  initLedCoals();
  // initRfidsTagInRange();
  // initTimerRfid();
  // initKnockSensors();
  // initLedKnockSensors();
  // initTimerGeneral();
  // initLedProgress();
  // initAudio();
  // initLedEye();

  Serial.println(F("Starting Dracula Forge program"));
}

void loop(void)
{
  automaton.run();
  serviceWS2812FX();
}
