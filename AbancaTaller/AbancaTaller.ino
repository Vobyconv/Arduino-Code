#include "rdm630.h"
#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

// Configuracion RFID
const byte PIN_RFID_RX = 8;
const byte PIN_RFID_TX = 9;
RDM6300 rfid(PIN_RFID_RX, PIN_RFID_TX);

const int NUM_TRACKS = 5;
const int TAGS_PER_TRACK = 2;

// IDs de las tarjetas RFID para cada pista
const String TRACK_TAG_IDS[NUM_TRACKS][TAGS_PER_TRACK] = {
    {"6400D43A8100", "4300A80C13F4"},
    {"4D0066410900", "3C00D5A7AFE1"},
    {"4E0015B53E00", "4300A13863B9"},
    {"6B001C50FC00", "4300A8038B63"},
    {"4E00163AA400", "4300A78CBDD5"}};

// Configuracion Audio FX
const byte AUDIO_TRACK_PINS[NUM_TRACKS] = {A0, A1, A2, A3, A4};
const byte AUDIO_SECRET_PIN = A5;
const byte PIN_AUDIO_ACT = A7;
const unsigned long AUDIO_TRACK_MAX_MS = 30000;

// Tira LED para efectos de audio
const uint8_t PIN_LEDS_AUDIO = 11;
const uint16_t NUM_LEDS_AUDIO = 10;
Adafruit_NeoPixel audioStrip = Adafruit_NeoPixel(NUM_LEDS_AUDIO, PIN_LEDS_AUDIO, NEO_GRB + NEO_KHZ800);

// Colores para cada pista de audio
const uint32_t AUDIO_TRACK_COLORS[NUM_TRACKS] = {
    Adafruit_NeoPixel::Color(255, 0, 0),   // Rojo
    Adafruit_NeoPixel::Color(0, 255, 0),   // Verde
    Adafruit_NeoPixel::Color(0, 0, 255),   // Azul
    Adafruit_NeoPixel::Color(255, 255, 0), // Amarillo
    Adafruit_NeoPixel::Color(255, 0, 255)  // Magenta
};

// Configuracion de botones
const int NUM_BUTTONS = 6;
const byte BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7};
Atm_button buttons[NUM_BUTTONS];

// Secuencia secreta
const int SEQUENCE_LENGTH = 6;
const byte SECRET_SEQUENCE[SEQUENCE_LENGTH] = {0, 1, 2, 3, 4, 5};

// Buffer para la secuencia introducida
byte inputBuffer[SEQUENCE_LENGTH];
int bufferIndex = 0;

// Tira LED para secuencia secreta
const uint8_t PIN_LEDS_SECRET = 12;
const uint16_t NUM_LEDS_SECRET = 10;
Adafruit_NeoPixel secretStrip = Adafruit_NeoPixel(NUM_LEDS_SECRET, PIN_LEDS_SECRET, NEO_GRB + NEO_KHZ800);

// Estado del sistema
bool secretUnlocked = false;

// Declaraciones adelantadas
bool isSequenceCorrect();
void activateSecretStrip();
void onButtonPress(int idx, int v, int up);
void checkRFID();

// Funciones de audio
bool isTrackPlaying()
{
  bool playing = digitalRead(PIN_AUDIO_ACT) == LOW;
  return playing;
}

void playTrack(byte trackPin)
{
  if (isTrackPlaying())
  {
    return;
  }

  Serial.print(F("Audio: Iniciando reproduccion en pin "));
  Serial.println(trackPin);

  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
  delay(200);
  pinMode(trackPin, INPUT);
}

void displayAudioLedEffect(int trackIdx)
{
  Serial.print(F("LED: Iniciando efecto para pista "));
  Serial.println(trackIdx);

  uint32_t color = AUDIO_TRACK_COLORS[trackIdx];
  unsigned long start = millis();

  while (isTrackPlaying() && millis() - start < AUDIO_TRACK_MAX_MS)
  {
    for (uint16_t i = 0; i < NUM_LEDS_AUDIO; i++)
    {
      audioStrip.setPixelColor(i, color);
      audioStrip.show();
      delay(50);
    }
    audioStrip.clear();
    audioStrip.show();
  }

  Serial.println(F("LED: Efecto de audio finalizado"));
}

// Funciones de inicializacion
void initAudioPins()
{
  Serial.println(F("Init: Configurando pines de audio"));

  for (int i = 0; i < NUM_TRACKS; i++)
  {
    pinMode(AUDIO_TRACK_PINS[i], INPUT);
  }

  pinMode(AUDIO_SECRET_PIN, INPUT);
  pinMode(PIN_AUDIO_ACT, INPUT);

  Serial.println(F("Init: Pines de audio configurados"));
}

void initLeds()
{
  Serial.println(F("Init: Configurando tiras LED"));

  audioStrip.begin();
  audioStrip.setBrightness(200);
  audioStrip.clear();
  audioStrip.show();

  secretStrip.begin();
  secretStrip.setBrightness(200);
  secretStrip.clear();
  secretStrip.show();

  Serial.println(F("Init: Tiras LED configuradas"));
}

void initButtons()
{
  Serial.println(F("Init: Configurando botones"));

  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].begin(BUTTON_PINS[i]).onPress(onButtonPress, i);
  }

  Serial.println(F("Init: Botones configurados"));
}

void startupBlink()
{
  Serial.println(F("Init: Iniciando parpadeo de arranque"));

  const uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);

  for (int k = 0; k < 2; k++)
  {
    // Encender LEDs
    for (uint16_t i = 0; i < NUM_LEDS_AUDIO; i++)
    {
      audioStrip.setPixelColor(i, blue);
    }

    for (uint16_t i = 0; i < NUM_LEDS_SECRET; i++)
    {
      secretStrip.setPixelColor(i, blue);
    }

    audioStrip.show();
    secretStrip.show();
    delay(500);

    // Apagar LEDs
    audioStrip.clear();
    secretStrip.clear();
    audioStrip.show();
    secretStrip.show();
    delay(500);
  }

  Serial.println(F("Init: Parpadeo de arranque completado"));
}

// Funciones de botones y secuencia secreta
void onButtonPress(int idx, int v, int up)
{
  if (secretUnlocked)
  {
    Serial.println(F("Boton: Secreto ya desbloqueado"));
    return;
  }

  Serial.print(F("Boton: Pulsado boton "));
  Serial.println(idx);

  inputBuffer[bufferIndex] = idx;
  bufferIndex = (bufferIndex + 1) % SEQUENCE_LENGTH;

  Serial.print(F("Boton: Secuencia actual: "));
  for (int i = 0; i < SEQUENCE_LENGTH; i++)
  {
    Serial.print(inputBuffer[i]);
    Serial.print(F(" "));
  }
  Serial.println();

  if (isSequenceCorrect())
  {
    Serial.println(F("Boton: Secuencia correcta detectada"));
    activateSecretStrip();
    playTrack(AUDIO_SECRET_PIN);
    secretUnlocked = true;
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
  Serial.println(F("Secuencia: Patron correcto encontrado"));
  return true;
}

void activateSecretStrip()
{
  Serial.println(F("LED: Activando tira secreta"));

  for (uint16_t i = 0; i < NUM_LEDS_SECRET; i++)
  {
    secretStrip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0)); // Verde
  }

  secretStrip.show();
  Serial.println(F("LED: Tira secreta encendida por 3 segundos"));
  delay(3000);
  secretStrip.clear();
  secretStrip.show();
  Serial.println(F("LED: Tira secreta apagada"));
}

// Funciones RFID
void checkRFID()
{
  String tagId = rfid.getTagId();

  if (tagId.length() == 0)
  {
    return;
  }

  Serial.print(F("RFID: Tag detectado: "));
  Serial.println(tagId);

  for (int i = 0; i < NUM_TRACKS; i++)
  {
    for (int j = 0; j < TAGS_PER_TRACK; j++)
    {
      if (tagId.equals(TRACK_TAG_IDS[i][j]))
      {
        Serial.print(F("RFID: Reproduciendo pista "));
        Serial.println(i);
        playTrack(AUDIO_TRACK_PINS[i]);
        displayAudioLedEffect(i);
        return;
      }
    }
  }

  Serial.println(F("RFID: Tag no reconocido"));
}

// Funciones principales
void setup()
{
  Serial.begin(9600);
  Serial.println(F("Sistema: Iniciando configuracion"));

  Serial.println(F("Sistema: Configurando RFID"));
  rfid.begin();

  initAudioPins();
  initLeds();
  initButtons();
  startupBlink();

  Serial.println(F("Sistema: Configuracion completa"));
  Serial.println(F("Sistema: RFID + Botones listo"));
}

void loop()
{
  // Ejecutar maquinas de estado de Automaton
  automaton.run();

  // Verificar tarjetas RFID
  checkRFID();
}
