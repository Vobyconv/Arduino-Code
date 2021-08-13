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

byte gammaTable[256];

void buildGammaTable()
{
  for (int i = 0; i < 256; i++)
  {
    float x = i;
    x /= 255;
    x = pow(x, 2.5);
    x *= 255;
    gammaTable[i] = 255 - x;
  }
}

void readRgbSensor(int idx)
{
  float red, green, blue;

  rgbSensors[idx].setInterrupt(false);

  delay(60);

  rgbSensors[idx].getRGB(&red, &green, &blue);
  rgbSensors[idx].setInterrupt(true);

  Serial.print("R:\t");
  Serial.print(int(gammaTable[(int)red]));
  Serial.print("\tG:\t");
  Serial.print(int(gammaTable[(int)green]));
  Serial.print("\tB:\t");
  Serial.print(int(gammaTable[(int)blue]));
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
  buildGammaTable();
}

void loop(void)
{
  for (int i = 0; i < NUM_RGB_SENSORS; i++)
  {
    Serial.print(F("Reading RGB sensor #"));
    Serial.println(i);
    readRgbSensor(i);
    delay(1000);
  }
}
