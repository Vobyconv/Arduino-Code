#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <CircularBuffer.h>
#include <Automaton.h>

/**
 * Buttons
 */

const uint8_t NUM_BUTTONS = 3;
const uint8_t PIN_BUTTONS[NUM_BUTTONS] = {A0, A1, A2};

Atm_button buttons[NUM_BUTTONS];

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

const unsigned long MAX_AUDIO_DIFF_MILLIS = 200;

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
 * General-purpose timer
 */

const uint32_t TIMER_GENERAL_MS = 50;

Atm_timer timerGeneral;

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

  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
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

void onButtonPress(int idx, int v, int up)
{
  Serial.print(F("Button: "));
  Serial.println(idx);
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
  }
}

void processAudioQueue()
{
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
  }
}

void onTimerGeneral(int idx, int v, int up)
{
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

void setup()
{
  Serial.begin(9600);

  initAudio();
  initLeds();
  initButtons();
  initTimerGeneral();

  Serial.println(F(">> Bongos de Simon"));
}

void loop()
{
  automaton.run();
}