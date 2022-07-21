#include <Automaton.h>
#include <Adafruit_NeoPixel.h>

/**
 * Proximity sensors.
 */

const uint8_t SENSOR_NUM = 5;

const uint8_t SENSOR_PINS[SENSOR_NUM] = {
    2, 3, 4, 5, 6};

Atm_digital sensorDigitals[SENSOR_NUM];

/**
 * LEDs.
 */

const uint8_t LED_BRIGHTNESS = 200;
const uint16_t LED_NUM = 60;
const int16_t LED_PIN = 7;

Adafruit_NeoPixel ledStrip = Adafruit_NeoPixel(
    LED_NUM, LED_PIN, NEO_GRB + NEO_KHZ800);

const uint32_t LED_COLOR = Adafruit_NeoPixel::Color(0, 0, 250);
const uint32_t COLOR_VICTORY = Adafruit_NeoPixel::Color(0, 250, 0);

/**
 * Program state.
 */

const uint32_t TIMER_STATE_MS = 100;

Atm_timer timerState;

const uint16_t POINTS_LIMIT = 20;

typedef struct programState
{
  uint16_t totalPoints;
} ProgramState;

ProgramState progState = {
    .totalPoints = 0};

uint16_t getCurrentActiveLeds()
{
  float pixelsPerPoint = ((float)ledStrip.numPixels()) / POINTS_LIMIT;
  uint16_t currPoints = progState.totalPoints % POINTS_LIMIT;
  uint16_t currPixels = min(floor(pixelsPerPoint * currPoints), ledStrip.numPixels());
  return currPixels;
}

void showVictoryEffect()
{
  const uint8_t numIters = 8;
  const unsigned long delayMs = 200;

  for (uint8_t i = 0; i < numIters; i++)
  {
    ledStrip.fill(COLOR_VICTORY);
    ledStrip.show();
    delay(delayMs);
    ledStrip.clear();
    ledStrip.show();
    delay(delayMs);
  }

  ledStrip.clear();
  ledStrip.show();
}

void updateLeds()
{
  ledStrip.clear();
  uint16_t count = getCurrentActiveLeds();
  ledStrip.fill(LED_COLOR, 0, count);
  ledStrip.show();
}

void onSensorPress(int idx, int v, int up)
{
  Serial.print(F("Sensor: "));
  Serial.print(idx);

  progState.totalPoints++;

  if (progState.totalPoints >= POINTS_LIMIT)
  {
    Serial.println(F("Showing victory effect"));
    showVictoryEffect();
    progState.totalPoints = 0;
  }

  updateLeds();
}

void initSensorDigitals()
{
  const int minDurationMs = 20;
  const bool activeLow = false;
  const bool pullUp = false;

  for (int i = 0; i < SENSOR_NUM; i++)
  {
    sensorDigitals[i]
        .begin(SENSOR_PINS[i], minDurationMs, activeLow, pullUp)
        .onChange(HIGH, onSensorPress, i);
  }
}

void initLeds()
{
  ledStrip.begin();
  ledStrip.setBrightness(LED_BRIGHTNESS);
  ledStrip.clear();
  ledStrip.show();
}

void setup()
{
  Serial.begin(9600);

  initLeds();
  initSensorDigitals();

  Serial.println(F(">> Lanza Bolas"));
}

void loop()
{
  automaton.run();
}