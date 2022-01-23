#include <Arduino.h>
#include <SlowSoftWire.h>
#include "Adafruit_TCS34725.h"
#include <Automaton.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <SerialRFID.h>
#include <KickSort.h>
#include <CircularBuffer.h>

/**
 * Relay.
 */

const uint8_t PIN_RELAY = 44;

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

Adafruit_NeoPixel ledRgbSensors[NUM_RGB_SENSORS] = {
    Adafruit_NeoPixel(LED_RGB_SENSOR_NUMS[0], LED_RGB_SENSOR_PINS[0], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_RGB_SENSOR_NUMS[1], LED_RGB_SENSOR_PINS[1], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_RGB_SENSOR_NUMS[2], LED_RGB_SENSOR_PINS[2], NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(LED_RGB_SENSOR_NUMS[3], LED_RGB_SENSOR_PINS[3], NEO_GRB + NEO_KHZ800)};

const uint16_t LED_COALS_NUM = 150;
const uint8_t LED_COALS_PIN = 34;

Adafruit_NeoPixel ledCoals = Adafruit_NeoPixel(LED_COALS_NUM, LED_COALS_PIN, NEO_GRB + NEO_KHZ800);

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

const uint8_t RGB_SENSORS_COLOR_KEY[NUM_RGB_SENSORS] = {3, 2, 1, 0};

/**
 * RGB sensor timer.
 */

const uint32_t TIMER_RGB_MS = 1000;

Atm_timer timerRgbSensor;

/**
 * RFID readers.
 */

const bool USE_MULTIPLE_SWSERIALS = false;

const uint8_t NUM_RFID = 5;

SoftwareSerial sSerial4 = SoftwareSerial(53, 62);
SoftwareSerial sSerial5 = SoftwareSerial(50, 51);

SerialRFID rfids[NUM_RFID] = {
    SerialRFID(Serial1),
    SerialRFID(Serial2),
    SerialRFID(Serial3),
    SerialRFID(Serial),
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
    {bronze, bronze, bronze, silver, gold},
    {orichalcum, orichalcum, orichalcum, mithril, mithril},
    {bronze, bronze, silver, silver, silver}};

const bool ONE_UNKNOWN_ALLOWED = true;

const uint8_t NUM_TAGS_MATERIAL = 4;

char tagsGold[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "09008EAA163B",
    "0700110E1F07",
    "0700115DB0FB",
    "0700118D47DC"};

char tagsSilver[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "070010B159FF",
    "0700109D0882",
    "09008F834441",
    "09008F9D7269"};

char tagsBronze[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "0700118AE77B",
    "09008F3A40FC",
    "0700113B270A",
    "07001093CE4A"};

char tagsMithril[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "09008EFCC7BC",
    "09008F67B554",
    "070010A0CB7C",
    "070010A80BB4"};

char tagsOrichalcum[NUM_TAGS_MATERIAL][SIZE_TAG_ID] = {
    "07001162A6D2",
    "0700110FE6FF",
    "070010A83E81",
    "09008FF8047A"};

/**
 * Knock sensors.
 */

const uint8_t NUM_KNOCK_SENSORS = 2;
const uint8_t KNOCK_PINS[NUM_KNOCK_SENSORS] = {A2, A3};
const int KNOCK_SAMPLERATE = 50;
const int KNOCK_RANGE_MIN = 0;
const int KNOCK_RANGE_MAX = 100;
const int KNOCK_THRESHOLD = 15;

const uint8_t SIZE_KNOCK_PATTERN = 5;

const uint8_t KNOCK_PATTERNS[NUM_RECIPES][SIZE_KNOCK_PATTERN] = {
    {0, 1, 0, 1, 0},
    {1, 1, 1, 0, 0},
    {0, 0, 1, 1, 0}};

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

const uint16_t LED_PROGRESS_NUM = 69;
const uint8_t LED_PROGRESS_PIN = 7;

Adafruit_NeoPixel ledProgress = Adafruit_NeoPixel(LED_PROGRESS_NUM, LED_PROGRESS_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t LED_PROGRESS_COLOR = Adafruit_NeoPixel::Color(255, 255, 0);

/**
 * LED strip for the eye of Odin.
 */

const uint16_t LED_EYE_NUM = 40;
const uint16_t LED_EYE_PIN = 6;

Adafruit_NeoPixel ledEye = Adafruit_NeoPixel(LED_EYE_NUM, LED_EYE_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t LED_EYE_COLORS[NUM_KNOCK_SENSORS] = {
    Adafruit_NeoPixel::Color(0, 0, 255),
    Adafruit_NeoPixel::Color(255, 255, 0)};

const unsigned long LED_EYE_DELAY_MS = 200;

/**
 * Knock (anvil) audio timings.
 */

const unsigned long EYE_AUDIO_TIMINGS[NUM_RECIPES][SIZE_KNOCK_PATTERN] = {
    {0, 1600, 1600, 1600, 1600},
    {0, 1600, 1600, 1600, 1600},
    {0, 1600, 1600, 1600, 1600}};

const unsigned long EYE_AUDIO_DELAY_LOOPS_MS = 10000;

/**
 * Audio FX.
 */

const uint8_t PIN_AUDIO_ACT = 8;
const uint8_t PIN_AUDIO_RST = 9;
const uint8_t AUDIO_TRACK_PINS[NUM_KNOCK_SENSORS] = {10, 11};
const uint8_t PIN_AUDIO_TRACK_COALS = 12;
const uint8_t PIN_AUDIO_TRACK_STAGE = 49;
const uint8_t PIN_AUDIO_TRACK_VICTORY = 52;
const unsigned long AUDIO_PLAY_DELAY_MS = 200;

/**
 * General-purpose timer.
 */

const uint32_t TIMER_GENERAL_MS = 50;

Atm_timer timerGeneral;

/**
 * Program state.
 */

char tagBuf[SIZE_TAG_ID];

int16_t rgbSensorsColorIndex[NUM_RGB_SENSORS];

char stateTags[NUM_RFID][SIZE_TAG_ID];
unsigned long stateTagsMillis[NUM_RFID];

const uint16_t KNOCK_BUF_SIZE = 30;
CircularBuffer<int, KNOCK_BUF_SIZE> knockHistory;

const uint16_t AUDIO_BUF_SIZE = 2;
CircularBuffer<int, AUDIO_BUF_SIZE> audioPinsQueue;

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
  bool isGameComplete;
  bool isCoalsEffectActive;
} ProgramState;

ProgramState progState;

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
  validateKnockPatterns();

  knockHistory.clear();
  audioPinsQueue.clear();

  progState.rgbSensorsColorIndex = rgbSensorsColorIndex;
  progState.ledKnockSensorsFillMillis = ledKnockSensorsFillMillis;

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
  progState.isGameComplete = false;
  progState.isCoalsEffectActive = false;
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

  pinMode(PIN_AUDIO_TRACK_COALS, INPUT);
  pinMode(PIN_AUDIO_TRACK_STAGE, INPUT);
  pinMode(PIN_AUDIO_TRACK_VICTORY, INPUT);

  pinMode(PIN_AUDIO_ACT, INPUT);
  pinMode(PIN_AUDIO_RST, INPUT);
}

void forceStopAudio()
{
  for (uint8_t i = 0; i < NUM_KNOCK_SENSORS; i++)
  {
    pinMode(AUDIO_TRACK_PINS[i], INPUT);
  }

  pinMode(PIN_AUDIO_TRACK_COALS, INPUT);
  pinMode(PIN_AUDIO_TRACK_STAGE, INPUT);
  pinMode(PIN_AUDIO_TRACK_VICTORY, INPUT);

  progState.audioPlayMillis = 0;
}

void enqueueTrack(uint8_t trackPin)
{
  audioPinsQueue.push(trackPin);
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
  uint16_t numPerPhase = floor((float)LED_PROGRESS_NUM / NUM_RECIPES);
  uint16_t count = min(numPerPhase * progState.currentRecipe, ledProgress.numPixels());

  ledProgress.clear();
  ledProgress.fill(LED_PROGRESS_COLOR, 0, count);
  ledProgress.show();
}

void initLedProgress()
{
  const uint8_t defaultBrightness = 150;

  ledProgress.begin();
  ledProgress.setBrightness(defaultBrightness);
  ledProgress.clear();
  ledProgress.show();
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

  if (progState.currentRecipe < NUM_RECIPES)
  {
    enqueueTrack(PIN_AUDIO_TRACK_STAGE);
  }

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

  Serial.print(F("Eye audio: recipe #"));
  Serial.print(idxRecipe);
  Serial.print(F(" item #"));
  Serial.print(idxPattern);
  Serial.print(F(" ("));
  Serial.print(millisTarget);
  Serial.print(F(" :: "));
  Serial.print(now);
  Serial.println(F(")"));

  const uint16_t diffMsWarn = TIMER_GENERAL_MS * 2;
  uint16_t diffMs = abs(now - millisTarget);

  if (diffMs > diffMsWarn)
  {
    Serial.println(F("[WARN] Eye audio: Diff with target: "));
    Serial.println(diffMs);
  }

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

bool equalMaterialArrays(Materials a[], Materials b[])
{
  bool isEqual = true;

  for (uint8_t i = 0; i < NUM_RFID; i++)
  {
    if (a[i] != b[i])
    {
      isEqual = false;
      break;
    }
  }

  return isEqual;
}

int16_t getActiveRecipe()
{
  Materials readerMaterials[NUM_RFID];

  for (uint8_t idxRfid = 0; idxRfid < NUM_RFID; idxRfid++)
  {
    readerMaterials[idxRfid] = getMaterial(stateTags[idxRfid]);
  }

  KickSort<Materials>::quickSort(readerMaterials, NUM_RFID);

  Materials bufMaterials[NUM_RFID];

  for (uint8_t idxRecipe = 0; idxRecipe < NUM_RECIPES; idxRecipe++)
  {
    if (ONE_UNKNOWN_ALLOWED)
    {
      for (uint8_t pivotUnknown = 0; pivotUnknown < NUM_RFID; pivotUnknown++)
      {
        for (uint8_t i = 0; i < NUM_RFID; i++)
        {
          bufMaterials[i] = i == pivotUnknown ? unknown : recipes[idxRecipe][i];
        }

        KickSort<Materials>::quickSort(bufMaterials, NUM_RFID);

        if (equalMaterialArrays(bufMaterials, readerMaterials))
        {
          return idxRecipe;
        }
      }
    }
    else
    {
      for (uint8_t i = 0; i < NUM_RFID; i++)
      {
        bufMaterials[i] = recipes[idxRecipe][i];
      }

      KickSort<Materials>::quickSort(bufMaterials, NUM_RFID);

      if (equalMaterialArrays(bufMaterials, readerMaterials))
      {
        return idxRecipe;
      }
    }
  }

  return -1;
}

void printRfidState()
{
  Materials readerMaterials[NUM_RFID];

  for (uint8_t idxRfid = 0; idxRfid < NUM_RFID; idxRfid++)
  {
    readerMaterials[idxRfid] = getMaterial(stateTags[idxRfid]);

    if (readerMaterials[idxRfid] == unknown)
    {
      continue;
    }

    Serial.print(F(">>> RFID #"));
    Serial.print(idxRfid);
    Serial.print(F(": "));

    if (readerMaterials[idxRfid] == gold)
    {
      Serial.println(F("Gold"));
    }
    else if (readerMaterials[idxRfid] == silver)
    {
      Serial.println(F("Silver"));
    }
    else if (readerMaterials[idxRfid] == orichalcum)
    {
      Serial.println(F("Orichalcum"));
    }
    else if (readerMaterials[idxRfid] == mithril)
    {
      Serial.println(F("Mithril"));
    }
    else if (readerMaterials[idxRfid] == bronze)
    {
      Serial.println(F("Bronze"));
    }
    else
    {
      Serial.println(F("UNKNOWN"));
    }
  }
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
  printRfidState();

  if (USE_MULTIPLE_SWSERIALS)
  {
    if (idx == 3)
    {
      sSerial4.listen();
      Serial.println(F("Listening on sSerial4"));
    }
    else if (idx == 4)
    {
      sSerial5.listen();
      Serial.println(F("Listening on sSerial5"));
    }
  }
  else
  {
    sSerial5.listen();
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

  bool tagFound = rfids[readerIdx].readTag(tagBuf, sizeof(tagBuf));

  if (!tagFound)
  {
    return;
  }

  const uint32_t blinkColor = Adafruit_NeoPixel::gamma32(Adafruit_NeoPixel::Color(255, 164, 0));
  const uint16_t blinkFirst = (int)(ledCoals.numPixels() * 0.8);

  if (progState.isRgbSensorsPhaseCompleted)
  {
    ledCoals.fill(blinkColor, blinkFirst);
    ledCoals.show();
  }

  // Serial.print(F("RFID #"));
  // Serial.print(readerIdx);
  // Serial.print(F(": "));
  // Serial.println(tagBuf);

  for (int k = 0; k < LEN_TAG_ID; k++)
  {
    stateTags[readerIdx][k] = tagBuf[k];
  }

  stateTagsMillis[readerIdx] = millis();

  printRfidState();
}

void onTimerRfid(int idx, int v, int up)
{
  for (uint8_t i = 0; i < NUM_RFID; i++)
  {
    readRfid(i);
  }

  if (!progState.isRgbSensorsPhaseCompleted || progState.isAnvilStepActive)
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
      uint32_t theColor = RECOGNIZED_COLORS[colorIdx].ledColor;
      ledRgbSensors[i].fill(theColor);
    }
    else
    {
      ledRgbSensors[i].clear();
    }

    ledRgbSensors[i].show();
  }
}

void onRgbSensorsStageEnd()
{
  enqueueTrack(PIN_AUDIO_TRACK_COALS);
  progState.isRgbSensorsPhaseCompleted = true;
  progState.isCoalsEffectActive = true;
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
    onRgbSensorsStageEnd();
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
  const uint8_t defaultBrightness = 160;

  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    ledRgbSensors[i].begin();
    ledRgbSensors[i].setBrightness(defaultBrightness);
    ledRgbSensors[i].clear();
    ledRgbSensors[i].show();
  }
}

void initLedCoals()
{
  const uint8_t defaultBrightness = 150;

  ledCoals.begin();
  ledCoals.setBrightness(defaultBrightness);
  ledCoals.clear();
  ledCoals.show();
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

  unsigned long now = millis();
  const uint16_t bounceMs = 150;

  if (progState.ledKnockSensorsFillMillis[idx] > 0 &&
      abs(now - progState.ledKnockSensorsFillMillis[idx]) <= bounceMs)
  {
    return;
  }

  Serial.print(F("Push Knock #"));
  Serial.println(idx);

  knockHistory.push(idx);

  ledKnockSensors[idx].fill(LED_KNOCK_COLORS[idx]);
  ledKnockSensors[idx].show();

  progState.ledKnockSensorsFillMillis[idx] = now;

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
    forceStopAudio();
  }
}

void runCoalsEffect()
{
  if (!progState.isCoalsEffectActive)
  {
    ledCoals.clear();
    ledCoals.show();
    return;
  }

  for (uint16_t i = 0; i < ledCoals.numPixels(); i++)
  {
    ledCoals.setPixelColor(i, random(100, 250), 0, 0);
  }

  ledCoals.show();
}

void runEndgame()
{
  const bool isGameComplete = progState.isRgbSensorsPhaseCompleted &&
                              progState.currentRecipe >= NUM_RECIPES;

  if (isGameComplete && !progState.isGameComplete)
  {
    Serial.println(F("The end"));
    progState.isGameComplete = true;
    stopEyeAudioPattern();
    openRelay(PIN_RELAY);
  }
  else if (progState.isGameComplete)
  {
    playTrack(PIN_AUDIO_TRACK_VICTORY);
  }
}

void processAudioQueue()
{
  if (audioPinsQueue.isEmpty() || isTrackPlaying())
  {
    return;
  }

  Serial.print(F("Audio queue size: "));
  Serial.println(audioPinsQueue.size());

  int trackPin = audioPinsQueue.shift();
  playTrack(trackPin);
}

void onTimerGeneral(int idx, int v, int up)
{
  clearLedKnockSensors();
  clearAudioPins();
  runEyeAudioPattern();
  runCoalsEffect();
  processAudioQueue();
  runEndgame();
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
  Serial.begin(9600);
  Serial1.begin(9600);
  Serial2.begin(9600);
  Serial3.begin(9600);
  sSerial4.begin(9600);
  sSerial5.begin(9600);
  sSerial5.listen();
}

void startupEffect()
{
  const uint8_t iters = 5;
  const uint16_t delayMs = 300;
  const uint32_t color = Adafruit_NeoPixel::Color(250, 250, 250);

  for (uint8_t i = 0; i < iters; i++)
  {
    ledEye.fill(color);
    ledEye.show();
    delay(delayMs);
    ledEye.clear();
    ledEye.show();
    delay(delayMs);
  }

  ledEye.clear();
  ledEye.show();
}

void setup(void)
{
  initSerials();
  initState();
  initRgbSensors();
  initTimerRgbSensor();
  initLedRgbSensors();
  initLedCoals();
  initRfidsTagInRange();
  initTimerRfid();
  initKnockSensors();
  initLedKnockSensors();
  initTimerGeneral();
  initLedProgress();
  initAudio();
  initLedEye();
  initRelay(PIN_RELAY);

  Serial.println(F("Forja Dracula"));
  startupEffect();
}

void loop(void)
{
  automaton.run();
}
