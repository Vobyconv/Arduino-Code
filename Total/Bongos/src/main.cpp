#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <CircularBuffer.h>
#include <Automaton.h>

/**
 * Buttons
 */

const uint8_t NUM_BUTTONS = 3;
const uint8_t PIN_BUTTONS[NUM_BUTTONS] = {A0, A1, A2};
const uint8_t PIN_BUTTON_LEDS[NUM_BUTTONS] = {A3, A4, A5};

Atm_button buttons[NUM_BUTTONS];
Atm_led buttonLeds[NUM_BUTTONS];

const uint32_t LED_DURATION_MS = 150;
const uint32_t LED_DURATION_PAUSE_MS = 50;
const uint16_t LED_REPEAT_COUNT = 1;

/**
 * General-purpose timer
 */

const uint32_t TIMER_GENERAL_MS = 100;

Atm_timer timerGeneral;

/**
 * Audio FX
 */

const uint8_t PIN_AUDIO_ACT = 2;
const uint8_t PIN_AUDIO_RST = 3;

const uint8_t PIN_AUDIO_TRACK_BUTTONS[NUM_BUTTONS] = {4, 5, 6};

const uint8_t PIN_AUDIO_TRACK_OK = 7;
const uint8_t PIN_AUDIO_TRACK_FAIL = 8;
const uint8_t PIN_AUDIO_TRACK_VICTORY = 9;
const uint8_t PIN_AUDIO_TRACK_PHASE = 10;

const uint16_t AUDIO_BUF_SIZE = 2;

typedef struct audioRequest
{
  unsigned long millis;
  uint8_t trackPin;
} AudioRequest;

CircularBuffer<AudioRequest, AUDIO_BUF_SIZE> audioRequestsQueue;

const unsigned long MAX_AUDIO_DIFF_MILLIS = 350;

/**
 * LED strips
 */

const uint16_t LED_PROGRESS_NUM = 30;
const uint8_t LED_PROGRESS_PIN = 11;

const uint16_t LED_CLICK_NUM = 30;
const uint8_t LED_CLICK_PIN = 12;

Adafruit_NeoPixel ledProgress = Adafruit_NeoPixel(
    LED_PROGRESS_NUM,
    LED_PROGRESS_PIN,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledClick = Adafruit_NeoPixel(
    LED_CLICK_NUM,
    LED_CLICK_PIN,
    NEO_GRB + NEO_KHZ800);

const uint8_t LED_BRIGHTNESS = 150;

/**
 * Game phases.
 */

const uint8_t NUM_PHASES = 2;
const uint8_t PHASE_SIZE = 6;

// Array values must be in range [0, NUM_BUTTONS)

const uint8_t GAME_SOLUTION[NUM_PHASES][PHASE_SIZE] = {
    {0, 1, 2, 0, 1, 2},
    {1, 2, 0, 1, 2, 0}};

const unsigned long HINT_STEP_MS = 1200;
const unsigned long GAME_IDLE_MS = 6000;
const unsigned long HINT_LOOP_REPEAT_MS = 6000;

/**
 * Program state
 */

const uint16_t BUTTONS_BUF_SIZE = PHASE_SIZE;

CircularBuffer<uint8_t, BUTTONS_BUF_SIZE> buttonsBuf;

typedef struct programState
{
  unsigned long lastPressMillis;
  uint8_t currentPhase;
  uint8_t currentHintStep;
  unsigned long lastHintMillis;
  unsigned long lastHintLoopEndMillis;
} ProgramState;

ProgramState progState = {
    .lastPressMillis = 0,
    .currentPhase = 0,
    .currentHintStep = 0,
    .lastHintMillis = 0,
    .lastHintLoopEndMillis = 0};

bool isTrackPlaying()
{
  return digitalRead(PIN_AUDIO_ACT) == LOW;
}

void playTrack(uint8_t trackPin)
{
  if (isTrackPlaying())
  {
    Serial.println(F("Skipping: Audio still playing"));
    return;
  }

  Serial.print(F("Playing pin: "));
  Serial.println(trackPin);

  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
}

void clearAudioPins()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    pinMode(PIN_AUDIO_TRACK_BUTTONS[i], INPUT);
  }

  pinMode(PIN_AUDIO_TRACK_OK, INPUT);
  pinMode(PIN_AUDIO_TRACK_FAIL, INPUT);
  pinMode(PIN_AUDIO_TRACK_VICTORY, INPUT);
  pinMode(PIN_AUDIO_TRACK_PHASE, INPUT);
}

void initAudioPins()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    pinMode(PIN_AUDIO_TRACK_BUTTONS[i], INPUT);
  }

  pinMode(PIN_AUDIO_TRACK_OK, INPUT);
  pinMode(PIN_AUDIO_TRACK_FAIL, INPUT);
  pinMode(PIN_AUDIO_TRACK_VICTORY, INPUT);
  pinMode(PIN_AUDIO_TRACK_PHASE, INPUT);

  pinMode(PIN_AUDIO_ACT, INPUT);
  pinMode(PIN_AUDIO_RST, INPUT);
}

void enqueueTrack(uint8_t trackPin)
{
  AudioRequest audioReq = {.millis = millis(), .trackPin = trackPin};
  audioRequestsQueue.push(audioReq);
}

void resetAudio()
{
  const unsigned long resetDelayMs = 100;
  const unsigned long waitDelayMs = 2000;

  Serial.println(F("Audio FX reset"));

  digitalWrite(PIN_AUDIO_RST, LOW);
  pinMode(PIN_AUDIO_RST, OUTPUT);
  delay(resetDelayMs);
  pinMode(PIN_AUDIO_RST, INPUT);

  Serial.println(F("Waiting for Audio FX startup"));

  delay(waitDelayMs);
}

void initAudio()
{
  initAudioPins();
  resetAudio();
}

void initLeds()
{
  ledProgress.begin();
  ledProgress.setBrightness(LED_BRIGHTNESS);
  ledProgress.clear();
  ledProgress.show();

  ledClick.begin();
  ledClick.setBrightness(LED_BRIGHTNESS);
  ledClick.clear();
  ledClick.show();
}

bool isHintEnabled()
{
  unsigned long now = millis();

  if ((now - progState.lastPressMillis) < GAME_IDLE_MS)
  {
    return false;
  }

  if ((now - progState.lastHintMillis) < HINT_STEP_MS)
  {
    return false;
  }

  if ((now - progState.lastHintLoopEndMillis) < HINT_LOOP_REPEAT_MS)
  {
    return false;
  }

  return true;
}

void showHint()
{
  if (!isHintEnabled())
  {
    return;
  }

  uint8_t currBtnIdx = GAME_SOLUTION[progState.currentPhase][progState.currentHintStep];

  if (currBtnIdx >= NUM_BUTTONS)
  {
    Serial.println(F("WARNING: Unexpected button index"));
    return;
  }

  unsigned long now = millis();

  progState.lastHintMillis = now;

  if (progState.currentHintStep < (PHASE_SIZE - 1))
  {
    progState.currentHintStep++;
  }
  else
  {
    progState.currentHintStep = 0;
    progState.lastHintLoopEndMillis = now;
  }

  buttonLeds[currBtnIdx].trigger(Atm_led::EVT_BLINK);
  enqueueTrack(PIN_AUDIO_TRACK_BUTTONS[currBtnIdx]);
}

void clearHintLoopState()
{
  progState.currentHintStep = 0;
  progState.lastHintMillis = 0;
  progState.lastHintLoopEndMillis = 0;
}

void onButtonPress(int idx, int v, int up)
{
  Serial.print(F("Button: "));
  Serial.println(idx);

  clearHintLoopState();
  progState.lastPressMillis = millis();
  buttonLeds[idx].trigger(Atm_led::EVT_BLINK);
  enqueueTrack(PIN_AUDIO_TRACK_BUTTONS[idx]);
  buttonsBuf.push(idx);
}

void initButtons()
{
  const int debounceDelayMs = 50;

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i]
        .begin(PIN_BUTTONS[i])
        .debounce(debounceDelayMs)
        .onPress(onButtonPress, i);

    buttonLeds[i]
        .begin(PIN_BUTTON_LEDS[i])
        .blink(LED_DURATION_MS, LED_DURATION_PAUSE_MS, LED_REPEAT_COUNT);

    buttonLeds[i]
        .trigger(Atm_led::EVT_OFF);
  }
}

void processAudioQueue()
{
  clearAudioPins();

  if (audioRequestsQueue.isEmpty() || isTrackPlaying())
  {
    return;
  }

  while (!audioRequestsQueue.isEmpty())
  {
    AudioRequest audioReq = audioRequestsQueue.shift();
    unsigned long diffMillis = millis() - audioReq.millis;

    if (diffMillis <= MAX_AUDIO_DIFF_MILLIS)
    {
      playTrack(audioReq.trackPin);
      break;
    }
    else
    {
      Serial.print(F("Discarded audio: "));
      Serial.print(diffMillis);
      Serial.println(F(" ms"));
    }
  }
}

void onTimerGeneral(int idx, int v, int up)
{
  showHint();
  processAudioQueue();
}

void initTimerGeneral()
{
  timerGeneral
      .begin(TIMER_GENERAL_MS)
      .repeat(-1)
      .onTimer(onTimerGeneral)
      .start();
}

void startupEffect()
{
  const uint8_t numIters = 5;
  const uint16_t delayMs = 200;
  const uint32_t color = Adafruit_NeoPixel::Color(200, 200, 200);

  for (uint8_t i = 0; i < numIters; i++)
  {
    ledProgress.fill(color);
    ledProgress.show();
    delay(delayMs);
    ledProgress.clear();
    ledProgress.show();
    delay(delayMs);
  }

  ledProgress.clear();
  ledProgress.show();

  for (uint8_t i = 0; i < NUM_BUTTONS; i++)
  {
    buttonLeds[i].trigger(Atm_led::EVT_BLINK);
  }
}

void setup()
{
  Serial.begin(9600);

  initAudio();
  initLeds();
  initButtons();
  initTimerGeneral();
  startupEffect();

  Serial.println(F(">> Bongos de Simon"));
}

void loop()
{
  automaton.run();
}