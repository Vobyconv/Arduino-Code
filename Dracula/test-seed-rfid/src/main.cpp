#include <Arduino.h>
#include <SoftwareSerial.h>

SoftwareSerial sSerial(2, 3);
unsigned char buffer[64];
int count = 0;

void clearBufferArray()
{
  for (int i = 0; i < count; i++)
  {
    buffer[i] = NULL;
  }
}

void setup()
{
  sSerial.begin(9600);
  Serial.begin(9600);
  sSerial.listen();
}

void loop()
{
  if (sSerial.available())
  {
    while (sSerial.available())
    {
      buffer[count++] = sSerial.read();

      if (count == 64)
      {
        break;
      }
    }

    Serial.write(buffer, count);
    clearBufferArray();
    count = 0;
  }
}