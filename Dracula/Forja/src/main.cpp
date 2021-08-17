#include <Arduino.h>
#include <SlowSoftWire.h>
#include "Adafruit_TCS34725.h"
#include <Automaton.h>
#include <Adafruit_NeoPixel.h>
#include <WS2812FX.h>

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
    20, 20, 20, 20};

const uint16_t LED_RGB_SENSOR_PINS[NUM_RGB_SENSORS] = {
    30, 31, 32, 33};

Adafruit_NeoPixel ledRgbSensors[NUM_RGB_SENSORS] = {
    Adafruit_NeoPixel(
        LED_RGB_SENSOR_NUMS[0],
        LED_RGB_SENSOR_PINS[0],
        NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(
        LED_RGB_SENSOR_NUMS[1],
        LED_RGB_SENSOR_PINS[1],
        NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(
        LED_RGB_SENSOR_NUMS[2],
        LED_RGB_SENSOR_PINS[2],
        NEO_GRB + NEO_KHZ800),
    Adafruit_NeoPixel(
        LED_RGB_SENSOR_NUMS[3],
        LED_RGB_SENSOR_PINS[3],
        NEO_GRB + NEO_KHZ800)};

const uint16_t LED_COALS_NUM = 20;
const uint8_t LED_COALS_PIN = 34;

WS2812FX ledCoals = WS2812FX(LED_COALS_NUM, LED_COALS_PIN, NEO_GRB + NEO_KHZ800);

/**
 * Colors recognized by the RGB sensor.
 */

const uint16_t CLEAR_CHANNEL_THRESHOLD = 450;

typedef struct colorDefinition
{
  float redRatio;
  float greenRatio;
  float blueRatio;
  uint32_t ledColor;
} ColorDefinition;

const uint8_t NUM_RECOGNIZED_COLORS = 4;

const ColorDefinition RECOGNIZED_COLORS[NUM_RECOGNIZED_COLORS] = {
    {.redRatio = 0.8,
     .greenRatio = 0,
     .blueRatio = 0,
     .ledColor = Adafruit_NeoPixel::Color(255, 0, 0)},
    {.redRatio = 0,
     .greenRatio = 0.8,
     .blueRatio = 0,
     .ledColor = Adafruit_NeoPixel::Color(0, 255, 0)},
    {.redRatio = 0,
     .greenRatio = 0,
     .blueRatio = 0.8,
     .ledColor = Adafruit_NeoPixel::Color(0, 0, 255)},
    {.redRatio = 0.4,
     .greenRatio = 0.4,
     .blueRatio = 0,
     .ledColor = Adafruit_NeoPixel::Color(255, 255, 0)}};

const uint8_t RGB_SENSORS_COLOR_KEY[NUM_RGB_SENSORS] = {0, 1, 2, 3};

/**
 * RGB sensor timer.
 */

const uint32_t TIMER_RGB_MS = 1000;

Atm_timer timerRgbSensor;

/**
 * Program state.
 */

int16_t rgbSensorsColorIndex[NUM_RGB_SENSORS];

typedef struct programState
{
  int16_t *rgbSensorsColorIndex;
  bool isRgbSensorsPhaseCompleted;
} ProgramState;

ProgramState progState;

void initState()
{
  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    progState.rgbSensorsColorIndex[i] = -1;
  }

  progState.isRgbSensorsPhaseCompleted = false;
}

bool isCurrentRgbSensorsStateValid()
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

  rgbSensors[sensorIdx].getRawDataOneShot(&rawRed, &rawGreen, &rawBlue, &rawClear);

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
    ledRgbSensors[i].clear();

    if (progState.rgbSensorsColorIndex[i] >= 0 &&
        progState.rgbSensorsColorIndex[i] < NUM_RECOGNIZED_COLORS)
    {
      uint32_t theColor = RECOGNIZED_COLORS[progState.rgbSensorsColorIndex[i]].ledColor;
      ledRgbSensors[i].fill(theColor);
    }

    ledRgbSensors[i].show();
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

  if (isCurrentRgbSensorsStateValid())
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
  const uint8_t defaultBrightness = 120;

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
  const uint16_t defaultSpeed = 200;
  const uint8_t defaultBrightness = 120;

  ledCoals.init();
  ledCoals.setBrightness(defaultBrightness);
  ledCoals.setSpeed(defaultSpeed);
  ledCoals.setMode(FX_MODE_FIRE_FLICKER);
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

void setup(void)
{
  Serial.begin(9600);
  initState();
  initRgbSensors();
  initTimerRgbSensor();
  initLedRgbSensors();
  initLedCoals();
}

void loop(void)
{
  automaton.run();
  ledCoals.service();
}
