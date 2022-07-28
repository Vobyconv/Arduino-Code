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

const uint32_t LED_DURATION_MS = 100;
const uint32_t LED_DURATION_PAUSE_MS = 20;
const uint16_t LED_REPEAT_COUNT = 1;

/**
 * General-purpose timer
 */

const uint32_t TIMER_GENERAL_MS = 50;

Atm_timer timerGeneral;

/**
 * Audio FX
 */

const uint8_t PIN_AUDIO_ACT = 2;
const uint8_t PIN_AUDIO_RST = 3;

const uint8_t PIN_AUDIO_TRACK_BUTTONS[NUM_BUTTONS] = {4, 5, 6};

// On phase consolidation
const uint8_t PIN_AUDIO_TRACK_OK = 7;

// On button press fail
const uint8_t PIN_AUDIO_TRACK_FAIL = 8;

// On game victory
const uint8_t PIN_AUDIO_TRACK_VICTORY = 9;

// On new phase start
const uint8_t PIN_AUDIO_TRACK_PHASE = 10;

const uint16_t AUDIO_BUF_SIZE = 2;

typedef struct audioRequest
{
  unsigned long millis;
  uint8_t trackPin;
} AudioRequest;

CircularBuffer<AudioRequest, AUDIO_BUF_SIZE> audioRequestsQueue;

const unsigned long MAX_AUDIO_DIFF_MILLIS = 350;
const unsigned long CLEAR_AUDIO_WAIT_MILLIS = 100;

/**
 * LED strips
 */

const uint16_t LED_PROGRESS_NUM = 150;
const uint8_t LED_PROGRESS_PIN = 11;

const uint16_t LED_EFFECTS_NUM = 80;
const uint8_t LED_EFFECTS_PIN = 12;

Adafruit_NeoPixel ledProgress = Adafruit_NeoPixel(
    LED_PROGRESS_NUM,
    LED_PROGRESS_PIN,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel ledEffects = Adafruit_NeoPixel(
    LED_EFFECTS_NUM,
    LED_EFFECTS_PIN,
    NEO_GRB + NEO_KHZ800);

const uint8_t LED_BRIGHTNESS = 150;

/**
 * Game phases.
 */

const uint8_t NUM_PHASES = 2;
const uint8_t PHASE_SIZE = 9;

// Array values must be in range [-1, NUM_BUTTONS)

const int GAME_SOLUTION[NUM_PHASES][PHASE_SIZE] = {
    {0, 1, 2, 0, 1, -1, -1, -1, -1},
    {1, 2, 0, 1, 2, 0, 1, 2, 0}};

const unsigned long HINT_STEP_MS = 1200;
const unsigned long GAME_IDLE_MS = 6000;
const unsigned long HINT_LOOP_REPEAT_MS = 6000;

/**
 * Program state
 */

CircularBuffer<uint8_t, PHASE_SIZE> buttonsBuf;

typedef struct programState
{
  unsigned long lastPressMillis;
  uint8_t currentPhase;
  uint8_t currentHintStep;
  unsigned long lastHintMillis;
  unsigned long lastHintLoopEndMillis;
  unsigned long lastPlayMillis;
  unsigned long effectFailStartMillis;
  unsigned long effectOkStartMillis;
} ProgramState;

ProgramState progState;

void initState()
{
  progState.lastPressMillis = 0;
  progState.currentPhase = 0;
  progState.currentHintStep = 0;
  progState.lastHintMillis = 0;
  progState.lastHintLoopEndMillis = 0;
  progState.lastPlayMillis = 0;
  progState.effectFailStartMillis = 0;
  progState.effectOkStartMillis = 0;
}

void setEffectFail()
{
  progState.effectFailStartMillis = millis();
}

void setEffectOk()
{
  progState.effectOkStartMillis = millis();
}

uint8_t getPhaseSize(int phaseIdx)
{
  if (phaseIdx >= NUM_PHASES)
  {
    Serial.println(F("WARNING: Unexpected phase index"));
    return 0;
  }

  uint8_t ret = 0;

  for (int i = 0; i < PHASE_SIZE; i++)
  {
    if (GAME_SOLUTION[phaseIdx][i] >= 0)
    {
      ret++;
    }
    else
    {
      break;
    }
  }

  return ret;
}

uint8_t getCurrentMatchSize()
{
  if (buttonsBuf.capacity != PHASE_SIZE)
  {
    Serial.println(F("WARNING: Unexpected buffer capacity"));
  }

  uint8_t phaseSize = getPhaseSize(progState.currentPhase);
  uint8_t pivotLimit = min(buttonsBuf.size(), phaseSize);
  uint8_t matchSize = 0;

  for (uint8_t idx = 0; idx < pivotLimit; idx++)
  {
    if (buttonsBuf[idx] == GAME_SOLUTION[progState.currentPhase][idx])
    {
      matchSize++;
    }
    else
    {
      break;
    }
  }

  return matchSize;
}

bool isValidButtonsBuf()
{
  return getCurrentMatchSize() == buttonsBuf.size();
}

bool isValidPhase()
{
  return isValidButtonsBuf() &&
         buttonsBuf.size() == getPhaseSize(progState.currentPhase);
}

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

  progState.lastPlayMillis = millis();

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

  ledEffects.begin();
  ledEffects.setBrightness(LED_BRIGHTNESS);
  ledEffects.clear();
  ledEffects.show();
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

  buttonsBuf.clear();

  uint8_t currBtnIdx = GAME_SOLUTION[progState.currentPhase][progState.currentHintStep];

  if (currBtnIdx >= NUM_BUTTONS)
  {
    Serial.println(F("WARNING: Unexpected button index"));
    return;
  }

  unsigned long now = millis();

  progState.lastHintMillis = now;

  uint8_t phaseSize = getPhaseSize(progState.currentPhase);

  if (progState.currentHintStep < (phaseSize - 1))
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
  buttonsBuf.push(idx);

  if (isValidButtonsBuf())
  {
    enqueueTrack(PIN_AUDIO_TRACK_BUTTONS[idx]);
  }
  else
  {
    buttonsBuf.clear();
    setEffectFail();
    enqueueTrack(PIN_AUDIO_TRACK_FAIL);
  }
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
  unsigned long diffLastPlayMillis = millis() - progState.lastPlayMillis;

  if (diffLastPlayMillis >= CLEAR_AUDIO_WAIT_MILLIS)
  {
    clearAudioPins();
  }

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

void updateProgressLed()
{
  uint16_t totalSteps = 0;

  for (int phaseIdx = 0; phaseIdx < NUM_PHASES; phaseIdx++)
  {
    totalSteps += getPhaseSize(phaseIdx);
  }

  uint16_t ledsPerStep = floor(((float)ledProgress.numPixels()) / totalSteps);

  uint16_t currSteps = 0;

  for (int phaseIdx = 0; phaseIdx < progState.currentPhase; phaseIdx++)
  {
    currSteps += getPhaseSize(phaseIdx);
  }

  uint8_t matchSize = getCurrentMatchSize();

  currSteps += matchSize;

  uint16_t fillCount = currSteps * ledsPerStep;
  const uint32_t progressColor = Adafruit_NeoPixel::Color(0, 200, 0);

  ledProgress.clear();

  if (fillCount > 0)
  {
    ledProgress.fill(progressColor, 0, fillCount);
  }

  ledProgress.show();
}

void runSnakeEffect(
    unsigned long totalMs,
    uint16_t snakeSize,
    uint32_t color,
    unsigned long &stateStartMillis)
{
  unsigned long now = millis();
  unsigned long diffStartMs = now - stateStartMillis;

  if (diffStartMs >= totalMs || snakeSize > ledEffects.numPixels())
  {
    stateStartMillis = 0;
    ledEffects.clear();
    ledEffects.show();
    return;
  }

  uint16_t snakeSteps = ledEffects.numPixels() - snakeSize;
  uint16_t msPerStep = floor(((float)totalMs) / snakeSteps);
  uint16_t currStep = round(((float)diffStartMs) / msPerStep);

  ledEffects.clear();
  ledEffects.fill(color, currStep, snakeSize);
  ledEffects.show();
}

void runEffectFail()
{
  if (progState.effectFailStartMillis == 0)
  {
    return;
  }

  const unsigned long totalMs = 800;
  const uint16_t snakeSize = 10;
  const uint32_t color = Adafruit_NeoPixel::Color(220, 0, 0);

  runSnakeEffect(totalMs, snakeSize, color, progState.effectFailStartMillis);
}

void runEffectOk()
{
  if (progState.effectOkStartMillis == 0)
  {
    return;
  }

  const unsigned long totalMs = 800;
  const uint16_t snakeSize = 10;
  const uint32_t color = Adafruit_NeoPixel::Color(0, 220, 0);

  runSnakeEffect(totalMs, snakeSize, color, progState.effectOkStartMillis);
}

void updateEffectsLed()
{
  if (progState.effectFailStartMillis > 0 && progState.effectOkStartMillis > 0)
  {
    // Phase consolidation effect takes precedence
    progState.effectFailStartMillis = 0;
  }

  if (progState.effectFailStartMillis > 0)
  {
    runEffectFail();
  }
  else if (progState.effectOkStartMillis > 0)
  {
    runEffectOk();
  }
  else
  {
    ledEffects.clear();
    ledEffects.show();
  }
}

void updateProgressState()
{
  bool validPhase = isValidPhase();

  if (validPhase && progState.currentPhase >= (NUM_PHASES - 1))
  {
    initState();
    enqueueTrack(PIN_AUDIO_TRACK_VICTORY);
  }
  else if (validPhase)
  {
    setEffectOk();
    progState.currentPhase++;
    clearHintLoopState();
  }
}

void onTimerGeneral(int idx, int v, int up)
{
  showHint();
  processAudioQueue();
  updateProgressState();
  updateProgressLed();
  updateEffectsLed();
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

  initState();
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