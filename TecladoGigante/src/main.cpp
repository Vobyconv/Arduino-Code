#include <Arduino.h>
#include <CD74HC4067.h>
#include <Keyboard.h>

// S0, S1, S2, S3
CD74HC4067 muxOne(5, 4, 3, 2);
CD74HC4067 muxTwo(10, 9, 8, 7);

const int MUX_ONE_SIG = A1;
const int MUX_TWO_SIG = A2;

const int NUM_MUX_CHANNELS = 16;

/*
  ISO keyboard layout:

  +---+---+---+---+---+---+---+---+---+---+---+---+---+-------+
  |35 |1e |1f |20 |21 |22 |23 |24 |25 |26 |27 |2d |2e |BackSp |
  +---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-----+
  | Tab |14 |1a |08 |15 |17 |1c |18 |0c |12 |13 |2f |30 | Ret |
  +-----++--++--++--++--++--++--++--++--++--++--++--++--++    |
  |CapsL |04 |16 |07 |09 |0a |0b |0d |0e |0f |33 |34 |31 |    |
  +----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+---+----+
  |Shi.|32 |1d |1b |06 |19 |05 |11 |10 |36 |37 |38 |  Shift   |
  +----+---++--+-+-+---+---+---+---+---+--++---+---++----+----+
  |Ctrl|Win |Alt |                        |AlGr|Win |Menu|Ctrl|
  +----+----+----+------------------------+----+----+----+----+
*/

const uint8_t CODE_ENYE = 0x33;

const uint8_t MUX_ONE_MAP[NUM_MUX_CHANNELS] = {
    'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', 'A', 'S', 'D', 'F', 'G', 'H'};

const uint8_t MUX_TWO_MAP[NUM_MUX_CHANNELS] = {
    'J', 'K', 'L', CODE_ENYE, 'Z', 'X', 'C', 'V', 'B', 'N', 'M', KEY_RETURN, KEY_DELETE, ' ', ' ', ' '};

//CODE_ENYE

const unsigned long PRESS_DELAY_MS = 100;

int lastChannelMuxOne = -1;
int lastChannelMuxTwo = -1;

void setup()
{
  Serial.begin(9600);
  pinMode(MUX_ONE_SIG, INPUT_PULLUP);
  pinMode(MUX_TWO_SIG, INPUT_PULLUP);
  Keyboard.begin();
}

void loopMux(CD74HC4067 &theMux, int sig, int *lastChannel, const uint8_t *muxMap, int muxId)
{
  for (int idxChannel = 0; idxChannel < NUM_MUX_CHANNELS; idxChannel++)
  {
    theMux.channel(idxChannel);

    int level = digitalRead(sig);

    if (level == LOW && (*lastChannel) < 0)
    {
      Serial.print(muxId);
      Serial.print(F(" :: LOW :: "));
      Serial.println(idxChannel);

      Keyboard.press(muxMap[idxChannel]);
      delay(PRESS_DELAY_MS);
      Keyboard.releaseAll();

      (*lastChannel) = idxChannel;
    }
    else if (level == HIGH && (*lastChannel) == idxChannel)
    {
      Serial.print(muxId);
      Serial.print(F(" :: HIGH :: "));
      Serial.println(idxChannel);

      (*lastChannel) = -1;
    }
  }
}

void loop()
{
  loopMux(muxOne, MUX_ONE_SIG, &lastChannelMuxOne, MUX_ONE_MAP, 1);
  loopMux(muxTwo, MUX_TWO_SIG, &lastChannelMuxTwo, MUX_TWO_MAP, 2);
}
