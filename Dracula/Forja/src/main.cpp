#include <Arduino.h>
#include <SlowSoftWire.h>
#include "Adafruit_TCS34725.h"

const int NUM_RGB_SENSORS = 4;

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

void readRgbSensor(int idx)
{
  const int lowerLimit = 0;
  const int upperLimit = 600;

  uint16_t clear, red, green, blue;

  rgbSensors[idx].setInterrupt(false);

  delay(60);

  rgbSensors[idx].getRawData(&red, &green, &blue, &clear);
  rgbSensors[idx].setInterrupt(true);

  int mappedRed = map(red, lowerLimit, upperLimit, 0, 255);
  int mappedGreen = map(green, lowerLimit, upperLimit, 0, 255);
  int mappedBlue = map(blue, lowerLimit, upperLimit, 0, 255);
  int mappedClear = map(clear, lowerLimit, upperLimit, 0, 255);

  Serial.print("C:\t");
  Serial.print(mappedClear);
  Serial.print("\tR:\t");
  Serial.print(mappedRed);
  Serial.print("\tG:\t");
  Serial.print(mappedGreen);
  Serial.print("\tB:\t");
  Serial.println(mappedBlue);
}

void initRgbSensors()
{
  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    Serial.print(F("Initializing RGB sensor #"));
    Serial.println(i);

    if (rgbSensors[i].begin(TCS34725_ADDRESS, &softWires[i]))
    {
      Serial.println(F("Sensor initialized OK"));
    }
    else
    {
      Serial.println(F("Error initializing sensor"));

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
  initRgbSensors();
}

void loop(void)
{
  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    Serial.print(F("Reading RGB sensor #"));
    Serial.println(i);
    readRgbSensor(i);
  }
}
