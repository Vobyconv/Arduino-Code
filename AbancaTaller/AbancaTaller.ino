#include "rdm630.h"
#include <Adafruit_NeoPixel.h>

/** RFID **/
const byte PIN_RFID_RX = 8;
const byte PIN_RFID_TX = 9;
RDM6300 rfid(PIN_RFID_RX, PIN_RFID_TX);

const int NUM_TRACKS = 5;
const int TAGS_PER_TRACK = 2;

const String TRACK_TAG_IDS[NUM_TRACKS][TAGS_PER_TRACK] = {
    {"1D0027A729B4", "4300A80C13F4"},
    {"1D00279848EA", "3C00D5A7AFE1"},
    {"4300A88595FB", "4300A13863B9"},
    {"4300A7A4A0E0", "4300A8038B63"},
    {"4300A7C7C7E4", "4300A78CBDD5"}};

/** Audio FX **/
const byte AUDIO_TRACK_PINS[NUM_TRACKS] = {A0, A1, A2, A3, A4};
const byte AUDIO_SECRET_PIN = A5;
const byte PIN_AUDIO_ACT = A7;
const unsigned long AUDIO_TRACK_MAX_MS = 30000;

/** LED tira de audio **/
const uint8_t PIN_LEDS_AUDIO = 11;
const uint16_t NUM_LEDS_AUDIO = 10;
Adafruit_NeoPixel audioStrip = Adafruit_NeoPixel(NUM_LEDS_AUDIO, PIN_LEDS_AUDIO, NEO_GRB + NEO_KHZ800);

const uint32_t AUDIO_TRACK_COLORS[NUM_TRACKS] = {
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(0, 255, 0),
    Adafruit_NeoPixel::Color(0, 0, 255),
    Adafruit_NeoPixel::Color(255, 255, 0),
    Adafruit_NeoPixel::Color(255, 0, 255)};

/** Botones **/
const int NUM_BUTTONS = 6;
const byte BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7};
byte buttonStates[NUM_BUTTONS];              // Track previous state of each button
unsigned long lastDebounceTime[NUM_BUTTONS]; // Track last transition time for each button
const unsigned long DEBOUNCE_DELAY = 50;     // 50ms debounce window

const int SEQUENCE_LENGTH = 6;
const byte SECRET_SEQUENCE[SEQUENCE_LENGTH] = {0, 2, 1, 4, 3, 5};

byte inputBuffer[SEQUENCE_LENGTH];
int bufferIndex = 0;

/** LED tira de botones **/
const uint8_t PIN_LEDS_SECRET = 12;
const uint16_t NUM_LEDS_SECRET = 10;
Adafruit_NeoPixel secretStrip = Adafruit_NeoPixel(NUM_LEDS_SECRET, PIN_LEDS_SECRET, NEO_GRB + NEO_KHZ800);

/** Estado del secreto **/
bool secretUnlocked = false;

/** Funciones de audio **/
bool isTrackPlaying()
{
  return digitalRead(PIN_AUDIO_ACT) == LOW;
}

void playTrack(byte trackPin)
{
  if (isTrackPlaying())
    return;
  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
  delay(200);
  pinMode(trackPin, INPUT);
}

void displayAudioLedEffect(int trackIdx)
{
  uint32_t color = AUDIO_TRACK_COLORS[trackIdx];
  unsigned long start = millis();

  while (isTrackPlaying() && millis() - start < AUDIO_TRACK_MAX_MS)
  {
    for (int i = 0; i < NUM_LEDS_AUDIO; i++)
    {
      audioStrip.setPixelColor(i, color);
      audioStrip.show();
      delay(50);
    }
    audioStrip.clear();
    audioStrip.show();
  }
}

/** Inicialización **/
void initAudioPins()
{
  for (int i = 0; i < NUM_TRACKS; i++)
    pinMode(AUDIO_TRACK_PINS[i], INPUT);
  pinMode(AUDIO_SECRET_PIN, INPUT);
  pinMode(PIN_AUDIO_ACT, INPUT);
}

void initLeds()
{
  audioStrip.begin();
  audioStrip.setBrightness(200);
  audioStrip.clear();
  audioStrip.show();

  secretStrip.begin();
  secretStrip.setBrightness(200);
  secretStrip.clear();
  secretStrip.show();
}

void initButtons()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    pinMode(BUTTON_PINS[i], INPUT_PULLUP);
    buttonStates[i] = HIGH;  // Initialize as released
    lastDebounceTime[i] = 0; // Initialize debounce timer
  }
}

void startupBlink()
{
  const uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);
  for (int k = 0; k < 2; k++)
  {
    for (int i = 0; i < NUM_LEDS_AUDIO; i++)
    {
      audioStrip.setPixelColor(i, blue);
    }
    for (int i = 0; i < NUM_LEDS_SECRET; i++)
    {
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

/** Botones y secuencia **/
void checkButtons()
{
  if (secretUnlocked)
    return;

  unsigned long currentTime = millis();

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    byte currentState = digitalRead(BUTTON_PINS[i]);

    // Detect press: transition from HIGH (released) to LOW (pressed)
    if (buttonStates[i] == HIGH && currentState == LOW)
    {
      // Check if enough time has passed since last transition (debounce)
      if (currentTime - lastDebounceTime[i] >= DEBOUNCE_DELAY)
      {
        Serial.print("Botón pulsado: ");
        Serial.println(i);

        inputBuffer[bufferIndex] = i;
        bufferIndex = (bufferIndex + 1) % SEQUENCE_LENGTH;

        if (isSequenceCorrect())
        {
          activateSecretStrip();
          playTrack(AUDIO_SECRET_PIN);
          secretUnlocked = true;
        }

        lastDebounceTime[i] = currentTime;
      }
    }

    buttonStates[i] = currentState;
  }
}

bool isSequenceCorrect()
{
  for (int i = 0; i < SEQUENCE_LENGTH; i++)
  {
    if (inputBuffer[(bufferIndex + i) % SEQUENCE_LENGTH] != SECRET_SEQUENCE[i])
    {
      return false;
    }
  }
  return true;
}

void activateSecretStrip()
{
  Serial.println("¡Secuencia correcta! Activando LED secreto");
  for (int i = 0; i < NUM_LEDS_SECRET; i++)
  {
    secretStrip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0)); // Verde
  }
  secretStrip.show();
  delay(3000);
  secretStrip.clear();
  secretStrip.show();
}

/** Setup y loop **/
void setup()
{
  Serial.begin(9600);
  rfid.begin();
  initAudioPins();
  initLeds();
  initButtons();
  startupBlink();

  Serial.println(">> Sistema RFID + Botones iniciado");
}

void loop()
{
  // RFID
  String tagId = rfid.getTagId();
  if (tagId.length() > 0)
  {
    Serial.print("Tag detectado: ");
    Serial.println(tagId);

    for (int i = 0; i < NUM_TRACKS; i++)
    {
      for (int j = 0; j < TAGS_PER_TRACK; j++)
      {
        if (tagId.equals(TRACK_TAG_IDS[i][j]))
        {
          Serial.print("Reproduciendo pista ");
          Serial.println(i);
          playTrack(AUDIO_TRACK_PINS[i]);
          displayAudioLedEffect(i);
          return;
        }
      }
    }
  }

  // Botones
  checkButtons();
}
