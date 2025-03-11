#include <SoftwareSerial.h>
#include <SerialRFID.h>
#include <Adafruit_NeoPixel.h>

/**
 * RFID reader. 
 */

const byte PIN_RFID_RX = 2;
const byte PIN_RFID_TX = 3;

SoftwareSerial sSerial(PIN_RFID_RX, PIN_RFID_TX);
SerialRFID rfid(sSerial);

char tag[SIZE_TAG_ID];

/**
 * Valid tag IDs.
 */

const int NUM_TRACKS = 6;
const int NUM_IDS_PER_TRACK = 2;

const char TRACK_TAG_IDS[NUM_TRACKS][NUM_IDS_PER_TRACK][SIZE_TAG_ID] = {
    {"1D0027A729B4",
     "4300A80C13F4"},//1 Asuntos fotovoltaicos y termicos, menuda memoria buena tengo
    {"1D00279848EA",
     "3C00D5A7AFE1"},//2 Las entrañas mas profundas, que bienm me explico
    {"4300A0ECA6A9",
     "4300A0EC9A95"},//3 Eso que obtenemos de la naturaleza... lo tengo muyu claro
    {"4300A88595FB",
     "4300A13863B9"},//4 Las turbinas de ese... liquido transparente, oh corcholis
    {"4300A7A4A0E0",
     "4300A8038B63"},//5 El movimiento del viento, adewmas rima
     {"4300A7C7C7E4",
     "4300A78CBDD5"}//6 Algo invisible... lo recuerdo `perfectamente
     };

/**
 * Audio FX.
 */

const byte PIN_AUDIO_RST = 6;
const byte PIN_AUDIO_ACT = 7;

const byte AUDIO_TRACK_PINS[NUM_TRACKS] = {
    8, 9, 10, 11, 12, 5};

const unsigned long AUDIO_TRACK_MAX_MS = 50000;
const int AUDIO_EFFECT_DELAY_MS = 500;

/**
 * LED strip.
 */

const uint16_t NUM_LEDS = 40;
const uint16_t NUM_LEDS_STATIC = 25;

const uint8_t PIN_LEDS_01 = 4;
const uint8_t PIN_LEDS_02 = 3;

const int LED_EFFECT_STEP_MS = 3;
const int LED_BRIGHTNESS = 250;

Adafruit_NeoPixel pixelStrip01 = Adafruit_NeoPixel(
    NUM_LEDS,
    PIN_LEDS_01,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixelStrip02 = Adafruit_NeoPixel(
    NUM_LEDS,
    PIN_LEDS_02,
    NEO_GRB + NEO_KHZ800);

const uint32_t COLOR_DEFAULT = Adafruit_NeoPixel::Color(126, 32, 198);

const uint32_t AUDIO_TRACK_COLORS[NUM_TRACKS] = {
    Adafruit_NeoPixel::Color(255, 190, 0),
    Adafruit_NeoPixel::Color(0, 255, 0),
    Adafruit_NeoPixel::Color(0, 50, 250),
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(128, 0, 128),
    Adafruit_NeoPixel::Color(0, 180, 128),
    };

/**
 * Audio FX functions.
 */

void playTrack(byte trackPin)
{
    if (isTrackPlaying())
    {
        Serial.println(F("Skipping: Audio still playing"));
        return;
    }

    Serial.print(F("Playing track on pin: "));
    Serial.println(trackPin);

    digitalWrite(trackPin, LOW);
    pinMode(trackPin, OUTPUT);
    delay(200);
    pinMode(trackPin, INPUT);
}

void initAudioPins()
{
    for (int i = 0; i < NUM_TRACKS; i++)
    {
        pinMode(AUDIO_TRACK_PINS[i], INPUT);
    }

    pinMode(PIN_AUDIO_ACT, INPUT);
    pinMode(PIN_AUDIO_RST, INPUT);
}

bool isTrackPlaying()
{
    return digitalRead(PIN_AUDIO_ACT) == LOW;
}

void resetAudio()
{
    Serial.println(F("Audio FX reset"));

    digitalWrite(PIN_AUDIO_RST, LOW);
    pinMode(PIN_AUDIO_RST, OUTPUT);
    delay(100);
    pinMode(PIN_AUDIO_RST, INPUT);

    Serial.println(F("Waiting for Audio FX startup"));

    delay(2000);
}

/**
 * LED strip functions.
 */

void displayAudioLedEffect(int tagIdx)
{
    int limitLo = floor(NUM_LEDS * 0.45);
    int limitHi = floor(NUM_LEDS * 1.00);

    uint32_t trackColor = AUDIO_TRACK_COLORS[tagIdx];

    unsigned long now;
    unsigned long ini = millis();
    bool timeout = false;

    int currTarget;
    int currLevel;

    delay(AUDIO_EFFECT_DELAY_MS);

    while (isTrackPlaying() && !timeout)
    {
        currTarget = random(limitLo, limitHi);
        currLevel = 0;

        clearLeds();

        for (int i = 0; i < currTarget; i++)
        {
            pixelStrip01.setPixelColor(i, trackColor);
            pixelStrip01.show();
            pixelStrip02.setPixelColor(i, trackColor);
            pixelStrip02.show();
            delay(LED_EFFECT_STEP_MS);
        }

        for (int i = (currTarget - 1); i >= 0; i--)
        {
            pixelStrip01.setPixelColor(i, 0);
            pixelStrip01.show();
            pixelStrip02.setPixelColor(i, 0);
            pixelStrip02.show();
            delay(LED_EFFECT_STEP_MS);
        }

        now = millis();

        if (now < ini)
        {
            Serial.println(F("Audio timeout: clock overflow"));
            timeout = true;
        }
        else if ((now - ini) > AUDIO_TRACK_MAX_MS)
        {
            Serial.println(F("Audio timeout"));
            timeout = true;
        }
    }

    clearLeds();
}

void initLeds()
{
    pixelStrip01.begin();
    pixelStrip01.setBrightness(LED_BRIGHTNESS);
    pixelStrip01.show();

    pixelStrip02.begin();
    pixelStrip02.setBrightness(LED_BRIGHTNESS);
    pixelStrip02.show();

    clearLeds();
}

void clearLeds()
{
    pixelStrip01.clear();
    pixelStrip01.show();

    pixelStrip02.clear();
    pixelStrip02.show();
}

void playLedStartupPattern()
{
    clearLeds();

    const int delayMs = 5;
    const uint32_t color = Adafruit_NeoPixel::Color(200, 200, 200);

    for (int i = 0; i < NUM_LEDS; i++)
    {
        pixelStrip01.setPixelColor(i, color);
        pixelStrip01.show();
        delay(delayMs);
    }

    clearLeds();
}

void showStaticLed()
{
    for (int i = 0; i <= 15 ; i++) {
      pixelStrip01.setPixelColor(i, random(15, 70), 0, 0);
      pixelStrip02.setPixelColor(i, 100, 0, 0);
      pixelStrip01.show();
      pixelStrip02.show();
  }

    
    delay(50);
}

/**
 * RFID reader functions.
 */

int readCurrentTagIndex()
{
    bool tagFound = rfid.readTag(tag, sizeof(tag));

    if (!tagFound)
    {
        return -1;
    }

    Serial.print(F("Tag: "));
    Serial.println(tag);

    for (int i = 0; i < NUM_TRACKS; i++)
    {
        for (int j = 0; j < NUM_IDS_PER_TRACK; j++)
        {
            if (SerialRFID::isEqualTag(tag, TRACK_TAG_IDS[i][j]))
            {
                Serial.print(F("Track match: "));
                Serial.println(i);
                return i;
            }
        }
    }

    return -1;
}

void readTagAndPlayAudio()
{
    int tagIdx = readCurrentTagIndex();

    if (tagIdx == -1)
    {
        return;
    }

    playTrack(AUDIO_TRACK_PINS[tagIdx]);
    displayAudioLedEffect(tagIdx);
}

/**
 * Entrypoint.
 */

void setup()
{
    Serial.begin(9600);
    sSerial.begin(9600);

    initAudioPins();
    resetAudio();
    initLeds();
    playLedStartupPattern();
    sSerial.listen();

    Serial.println(F(">> Starting Cerebrofono program"));
}

void loop()
{
    readTagAndPlayAudio();
    //showStaticLed();
}
