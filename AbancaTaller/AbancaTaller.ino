#include "rdm630.h"
#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

// Configuracion RFID
const byte PIN_RFID_RX = 8;
const byte PIN_RFID_TX = 9;
RDM6300 rfid(PIN_RFID_RX, PIN_RFID_TX);

const int NUM_TRACKS = 5;
const int TAGS_PER_TRACK = 2;

const String TRACK_TAG_IDS[NUM_TRACKS][TAGS_PER_TRACK] = {
  {"6400D43A8100", "4300A80C13F4"},
  {"4D0066410900", "3C00D5A7AFE1"},
  {"4E0015B53E00", "4300A13863B9"},
  {"6B001C50FC00", "4300A8038B63"},
  {"4E00163AA400", "4300A78CBDD5"}
};

const byte AUDIO_TRACK_PINS[NUM_TRACKS] = {A0, A1, A2, A3, A4};
const byte AUDIO_SECRET_PIN = A5;
const byte PIN_AUDIO_ACT = A7;
const unsigned long AUDIO_TRACK_MAX_MS = 30000;

const uint8_t PIN_LEDS_AUDIO = 11;
const uint16_t NUM_LEDS_AUDIO = 10;
Adafruit_NeoPixel audioStrip = Adafruit_NeoPixel(NUM_LEDS_AUDIO, PIN_LEDS_AUDIO, NEO_GRB + NEO_KHZ800);

const uint32_t AUDIO_TRACK_COLORS[NUM_TRACKS] = {
  Adafruit_NeoPixel::Color(255, 0, 0),
  Adafruit_NeoPixel::Color(0, 255, 0),
  Adafruit_NeoPixel::Color(0, 0, 255),
  Adafruit_NeoPixel::Color(255, 255, 0),
  Adafruit_NeoPixel::Color(255, 0, 255)
};

const int NUM_BUTTONS = 6;
const byte BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7};
Atm_button buttons[NUM_BUTTONS];

const int SEQUENCE_LENGTH = 6;
const byte SECRET_SEQUENCE[SEQUENCE_LENGTH] = {0, 1, 2, 3, 4, 5};

byte inputBuffer[SEQUENCE_LENGTH];
int bufferIndex = 0;

const uint8_t PIN_LEDS_SECRET = 12;
const uint16_t NUM_LEDS_SECRET = 10;
Adafruit_NeoPixel secretStrip = Adafruit_NeoPixel(NUM_LEDS_SECRET, PIN_LEDS_SECRET, NEO_GRB + NEO_KHZ800);

bool secretUnlocked = false;

// Variables para efecto LED no bloqueante
bool ledEffectRunning = false;
int currentTrackIdx = -1;
unsigned long lastUpdate = 0;
bool ledOn = false;
unsigned long effectStart = 0;

// Funciones
bool isSequenceCorrect();
void activateSecretStrip();
void onButtonPress(int idx, int v, int up);
void checkRFID();

bool isTrackPlaying() {
  return digitalRead(PIN_AUDIO_ACT) == LOW;
}

void playTrack(byte trackPin) {
  if (isTrackPlaying()) return;

  Serial.print(F("Audio: Iniciando reproduccion en pin "));
  Serial.println(trackPin);

  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
  delay(200);
  pinMode(trackPin, INPUT);
}

void displayAudioLedEffect(int trackIdx) {
  if (!ledEffectRunning) return;

  if (millis() - effectStart >= AUDIO_TRACK_MAX_MS) {
    audioStrip.clear();
    audioStrip.show();
    ledEffectRunning = false;
    Serial.println(F("LED: Efecto de audio finalizado"));
    return;
  }

  if (millis() - lastUpdate >= 100) {
    lastUpdate = millis();
    uint32_t color = AUDIO_TRACK_COLORS[trackIdx];

    if (ledOn) {
      audioStrip.clear();
    } else {
      for (uint16_t i = 0; i < NUM_LEDS_AUDIO; i++) {
        audioStrip.setPixelColor(i, color);
      }
    }

    audioStrip.show();
    ledOn = !ledOn;
  }
}

void initAudioPins() {
  for (int i = 0; i < NUM_TRACKS; i++) {
    pinMode(AUDIO_TRACK_PINS[i], INPUT);
  }
  pinMode(AUDIO_SECRET_PIN, INPUT);
  pinMode(PIN_AUDIO_ACT, INPUT);
}

void initLeds() {
  audioStrip.begin();
  audioStrip.setBrightness(200);
  audioStrip.clear();
  audioStrip.show();

  secretStrip.begin();
  secretStrip.setBrightness(200);
  secretStrip.clear();
  secretStrip.show();
}

void initButtons() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i].begin(BUTTON_PINS[i]).onPress(onButtonPress, i).debounce(50);
  }
}

void startupBlink() {
  const uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);

  for (int k = 0; k < 2; k++) {
    for (uint16_t i = 0; i < NUM_LEDS_AUDIO; i++) {
      audioStrip.setPixelColor(i, blue);
    }
    for (uint16_t i = 0; i < NUM_LEDS_SECRET; i++) {
      secretStrip.setPixelColor(i, blue);
    }
    audioStrip.show();
    secretStrip.show();
    delay(500);

    audioStrip.clear();
    secretStrip.clear();
    audioStrip.show();
    secretStrip.show();
    delay(500);
  }
}

void onButtonPress(int idx, int v, int up) {
  if (secretUnlocked) return;

  inputBuffer[bufferIndex] = idx;
  bufferIndex = (bufferIndex + 1) % SEQUENCE_LENGTH;

  if (isSequenceCorrect()) {
    activateSecretStrip();
    playTrack(AUDIO_SECRET_PIN);
    delay(300);
    currentTrackIdx = 0; // Puedes usar un color especial si lo deseas
    effectStart = millis();
    ledEffectRunning = true;
    secretUnlocked = true;
  }
}

bool isSequenceCorrect() {
  for (int i = 0; i < SEQUENCE_LENGTH; i++) {
    if (inputBuffer[(bufferIndex + i) % SEQUENCE_LENGTH] != SECRET_SEQUENCE[i]) {
      return false;
    }
  }
  return true;
}

void activateSecretStrip() {
  for (uint16_t i = 0; i < NUM_LEDS_SECRET; i++) {
    secretStrip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
  }
  secretStrip.show();
  delay(3000);
  secretStrip.clear();
  secretStrip.show();
}

void checkRFID() {
  String tagId = rfid.getTagId();
  if (tagId.length() == 0) return;

  for (int i = 0; i < NUM_TRACKS; i++) {
    for (int j = 0; j < TAGS_PER_TRACK; j++) {
      if (tagId.equals(TRACK_TAG_IDS[i][j])) {
        playTrack(AUDIO_TRACK_PINS[i]);
        delay(300);
        currentTrackIdx = i;
        effectStart = millis();
        ledEffectRunning = true;
        return;
      }
    }
  }
}

void setup() {
  Serial.begin(9600);
  rfid.begin();
  initAudioPins();
  initLeds();
  initButtons();
  startupBlink();
}

void loop() {
  automaton.run();
  checkRFID();
  displayAudioLedEffect(currentTrackIdx);
}
