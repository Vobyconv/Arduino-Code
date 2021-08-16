#include <Arduino.h>
#include <SlowSoftWire.h>
#include "Adafruit_TCS34725.h"
#include <Automaton.h>
#include <Adafruit_NeoPixel.h>

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
 * Colors recognized by the RGB sensor.
 */

const uint16_t CLEAR_CHANNEL_THRESHOLD = 450;

typedef struct rgbColor
{
  float redRatio;
  float greenRatio;
  float blueRatio;
  uint32_t ledColor;
} RgbColor;

const uint8_t NUM_RECOGNIZED_COLORS = 4;

const RgbColor RECOGNIZED_COLORS[NUM_RECOGNIZED_COLORS] = {
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
} ProgramState;

ProgramState progState;

void initState()
{
  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    progState.rgbSensorsColorIndex[i] = -1;
  }
}

void readRgbSensor(uint8_t idx)
{
  if (idx >= NUM_RGB_SENSORS)
  {
    return;
  }

  uint16_t rawRed, rawGreen, rawBlue, rawClear;

  Serial.print(F("Reading RGB #"));
  Serial.println(idx);

  rgbSensors[idx].getRawDataOneShot(&rawRed, &rawGreen, &rawBlue, &rawClear);

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

  float redRatio;
  float greenRatio;
  float blueRatio;

  for (uint8_t colorIdx = 0; colorIdx < NUM_RECOGNIZED_COLORS; colorIdx++)
  {
    redRatio = ((float)rawRed / sum) * 255.0;
    greenRatio = ((float)rawGreen / sum) * 255.0;
    blueRatio = ((float)rawBlue / sum) * 255.0;

    if (redRatio >= RECOGNIZED_COLORS[colorIdx].redRatio &&
        greenRatio >= RECOGNIZED_COLORS[colorIdx].greenRatio &&
        blueRatio >= RECOGNIZED_COLORS[colorIdx].blueRatio)
    {
      Serial.print(F("Recognized color #"));
      Serial.println(colorIdx);
      progState.rgbSensorsColorIndex[idx] = colorIdx;
      return;
    }
  }
}

void onTimerRgbSensor(int idx, int v, int up)
{
  for (uint8_t i = 0; i < NUM_RGB_SENSORS; i++)
  {
    readRgbSensor(i);
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
}

void loop(void)
{
  automaton.run();
}
