#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <Automaton.h>
#include <CircularBuffer.h>

const uint8_t NUM_BUTTONS = 4;

const uint8_t PIN_LED = 11;
const uint32_t COLOR_DEFAULT = Adafruit_NeoPixel::Color(200, 20, 20);

Adafruit_NeoPixel ledCandles = Adafruit_NeoPixel(
    NUM_BUTTONS,
    PIN_LED,
    NEO_GRB + NEO_KHZ800);

const uint8_t PIN_RELAY = 10;

Atm_button buttons[NUM_BUTTONS];

const uint8_t PIN_BUTTONS[NUM_BUTTONS] = {2, 3, 4, 5};

const uint8_t BUTTON_SOLUTION_ORDER[NUM_BUTTONS] = {0, 1, 2, 3};

const uint32_t TIMER_STATE_MS = 200;
Atm_timer timerState;

CircularBuffer<uint8_t, NUM_BUTTONS> buttonHistory;

bool candleState[NUM_BUTTONS];

void initState()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    candleState[i] = false;
  }
}

void initLeds()
{
  const uint8_t brightness = 200;

  ledCandles.begin();
  ledCandles.setBrightness(brightness);
  ledCandles.clear();
  ledCandles.show();

  const uint32_t delayMs = 150;
  const uint8_t numIters = 3;

  for (uint8_t i = 0; i < numIters; i++)
  {
    ledCandles.fill(COLOR_DEFAULT);
    delay(delayMs);
    ledCandles.clear();
    delay(delayMs);
  }
}

void updateCandleLeds()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    ledCandles.setPixelColor(i, candleState[i] ? COLOR_DEFAULT : 0);
  }

  ledCandles.show();
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

bool isSolved()
{
  if (buttonHistory.size() < NUM_BUTTONS)
  {
    return false;
  }

  for (int idx = 0; idx < NUM_BUTTONS; idx++)
  {
    int idxBuf = buttonHistory.size() - NUM_BUTTONS + idx;

    if (buttonHistory[idxBuf] != BUTTON_SOLUTION_ORDER[idx])
    {
      return false;
    }
  }

  return true;
}

void printCurrentBuffer()
{
  Serial.print(F("Buffer: "));

  for (int idxBuf = 0; idxBuf < buttonHistory.size(); idxBuf++)
  {
    Serial.print(buttonHistory[idxBuf]);
    Serial.print(F(" "));
  }

  Serial.println("");
}

void onButtonPress(int idx, int v, int up)
{
  Serial.print(F("Pressed button #"));
  Serial.println(idx);

  bool newState = !candleState[idx];
  candleState[idx] = newState;

  if (newState)
  {
    buttonHistory.push(idx);
  }

  printCurrentBuffer();
  updateCandleLeds();
}

void initControlPins()
{
  initRelay(PIN_RELAY);

  for (uint8_t idxButton = 0; idxButton < NUM_BUTTONS; idxButton++)
  {
    buttons[idxButton]
        .begin(PIN_BUTTONS[idxButton])
        .onPress(onButtonPress, idxButton);
  }
}

void onTimerState(int idx, int v, int up)
{
  if (isSolved())
  {
    openRelay(PIN_RELAY);
  }
  else
  {
    lockRelay(PIN_RELAY);
  }
}

void initTimerState()
{
  timerState
      .begin(TIMER_STATE_MS)
      .repeat(-1)
      .onTimer(onTimerState)
      .start();
}

void setup()
{
  Serial.begin(9600);
  initState();
  initControlPins();
  initLeds();
  initTimerState();
  updateCandleLeds();
  Serial.println(F("NaviVelas"));
}

void loop()
{
  automaton.run();
}