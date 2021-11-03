#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

const int LED_NUM = 10;
const int LED_PIN = 10;
const uint32_t DEFAULT_COLOR = Adafruit_NeoPixel::Color(255, 255, 0);

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(LED_NUM, LED_PIN, NEO_RGB + NEO_KHZ800);

const int RELAY_PIN = 2;

const int NUM_BUTTONS = 5;
const int BUTTON_PINS[NUM_BUTTONS] = {4, 5, 6, 7, 8};
const int RESET_BUTTON_PIN = 12;

Atm_button buttons[NUM_BUTTONS];
Atm_button resetButton;

const bool BUTTON_CONFIGURATION[NUM_BUTTONS][LED_NUM] = {
    {true, false, true, true, false, true, true, true, false, true},
    {false, false, true, false, false, true, true, true, false, false},
    {true, false, true, false, false, false, false, false, false, false},
    {true, true, false, false, false, false, false, false, false, false},
    {true, true, true, true, true, true, true, true, true, true}};

const bool VALID_CONFIGURATION[LED_NUM] = {
    false, false, true, false, true, false, false, false, true, false};

bool currConfiguration[LED_NUM] = {
    true, true, true, true, true, true, true, true, true, true};

bool flagRelayOpen = false;

void lockRelay()
{
  digitalWrite(RELAY_PIN, LOW);
  flagRelayOpen = false;
}

void openRelay()
{
  digitalWrite(RELAY_PIN, HIGH);
  flagRelayOpen = true;
}

void initRelay()
{
  pinMode(RELAY_PIN, OUTPUT);
  lockRelay();
}

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

void blinkLeds()
{
  const int numLoops = 6;
  const int delayLoop = 150;

  ledStrip.clear();
  ledStrip.show();

  for (int i = 0; i < numLoops; i++)
  {
    ledStrip.fill(DEFAULT_COLOR);
    ledStrip.show();
    delay(delayLoop);
    ledStrip.clear();
    ledStrip.show();
    delay(delayLoop);
  }

  ledStrip.clear();
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

void checkState()
{
  if (isCurrentConfigValid() && !flagRelayOpen)
  {
    Serial.println(F("Valid config: opening relay"));
    openRelay();
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

void onReset(int idx, int v, int up)
{
  Serial.println(F("Reset"));

  for (int idxLed = 0; idxLed < LED_NUM; idxLed++)
  {
    currConfiguration[idxLed] = true;
  }
}

void initButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].begin(BUTTON_PINS[i]).onPress(onPress, i);
  }

  resetButton.begin(RESET_BUTTON_PIN).onPress(onReset);
}

void setup()
{
  Serial.begin(9600);
  initLeds();
  initButtons();
  initRelay();
  blinkLeds();
  Serial.println(F("Velas"));
}

void loop()
{
  automaton.run();
  updateLeds();
  checkState();
}