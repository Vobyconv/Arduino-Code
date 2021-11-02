#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

const int LED_NUM = 10;
const int LED_PIN = 10;
const uint32_t DEFAULT_COLOR = Adafruit_NeoPixel::Color(255, 255, 0);

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_RGB + NEO_KHZ800);

const int NUM_BUTTONS = 5;

Atm_button buttons[NUM_BUTTONS];

const int BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6};

const bool BUTTON_CONFIGURATION[NUM_BUTTONS][LED_NUM] = {
    {true, false, false, false, true, false, false, false, true, true},
    {false, false, false, false, true, false, true, false, true, true},
    {true, false, false, true, true, false, false, false, false, true},
    {false, false, true, false, true, false, false, false, false, false},
    {true, false, false, false, true, false, true, false, true, false}};

const bool VALID_CONFIGURATION[LED_NUM] = {
    false, false, false, false, false, false, false, true, false, true};

bool currConfiguration[LED_NUM] = {
    false, false, false, false, false, false, false, false, false, false};

void initLeds()
{
  const int brightness = 200;

  ledStrip.begin();
  ledStrip.setBrightness(brightness);
  ledStrip.show();
  ledStrip.clear();
}

void updateLeds()
{
  for (int idxLed = 0; idxLed < LED_NUM; idxLed++)
  {
    uint32_t color = currConfiguration[idxLed] ? DEFAULT_COLOR : 0;
    ledStrip.setPixelColor(idxLed, color);
  }

  ledStrip.show();
}

bool isCurrentConfigValid()
{
  for (int idxLed = 0; idxLed < LED_NUM; idxLed++)
  {
    if (currConfiguration[idxLed] != VALID_CONFIGURATION[idxLed])
    {
      return false;
    }
  }

  return true;
}

void checkConfig()
{
  if (isCurrentConfigValid())
  {
    Serial.println(F("Valid config"));
  }
}

void onPress(int idx, int v, int up)
{
  Serial.print(F("Button #"));
  Serial.println(idx);

  for (int idxLed = 0; idxLed < LED_NUM; idxLed++)
  {
    if (BUTTON_CONFIGURATION[idx][idxLed])
    {
      currConfiguration[idxLed] = !currConfiguration[idxLed];
    }
  }
}

void initButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].begin(BUTTON_PINS[i]).onPress(onPress, i);
  }
}

void setup()
{
  Serial.begin(9600);
  initLeds();
  initButtons();
  Serial.println(F("Velas"));
}

void loop()
{
  automaton.run();
  updateLeds();
  checkConfig();
}