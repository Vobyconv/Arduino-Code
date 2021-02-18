// Librerias ---------------------------
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#include <SerialRFID.h>

//Ordenes basicas ---------------------
int ojos = 10;
int ledCielo = 11;
int rele = 12;
int ojosCarona = 12;
int estrellas = 8;

//pines de pulsadores
int pulsaTroll = A0;
int pulsaVaca = A1;
int pulsaOso = A2;
int pulsaMuerte = A3;

Adafruit_NeoPixel stripOjos = Adafruit_NeoPixel(ojosCarona, ojos, NEO_GRB + NEO_KHZ800);
Adafruit_NeoPixel stripCielo = Adafruit_NeoPixel(estrellas, ledCielo, NEO_GRB + NEO_KHZ800);

//* RFID reader.
const byte PIN_RFID_RX = 2;
const byte PIN_RFID_TX = 3;

SoftwareSerial sSerial(PIN_RFID_RX, PIN_RFID_TX);
SerialRFID rfid(sSerial);

char tag[SIZE_TAG_ID];

// ID para Tags
const int NUM_TRACKS = 3;
const int NUM_IDS_PER_TRACK = 2;

const char TRACK_TAG_IDS[NUM_TRACKS][NUM_IDS_PER_TRACK][SIZE_TAG_ID] = {
    {"2B00452097D9",
     "2B0045230B46"},
    {"2B0045A925E2",
     "2B0045A925E2"},
    {"2B0045E8D157",
     "2B0045E8D157"}};

//tarjeta de audio
const byte PIN_AUDIO_RST = 6;
const byte PIN_AUDIO_ACT = 7;

const byte AUDIO_TRACK_PINS[NUM_TRACKS] = {
    5, 6, 7};

const unsigned long AUDIO_TRACK_MAX_MS = 50000;
const int AUDIO_EFFECT_DELAY_MS = 500;

// * LED strip - Tags

const uint16_t NUM_LEDS = 10;
const uint16_t NUM_LEDS_STATIC = 25;

const uint8_t PIN_LEDS_01 = 4;
const uint8_t PIN_LEDS_02 = 3;
const uint8_t PIN_LEDS_STATIC = 5;

const int LED_EFFECT_STEP_MS = 3;
const int LED_BRIGHTNESS = 50;

Adafruit_NeoPixel pixelStrip01 = Adafruit_NeoPixel(
    NUM_LEDS,
    PIN_LEDS_01,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixelStrip02 = Adafruit_NeoPixel(
    NUM_LEDS,
    PIN_LEDS_02,
    NEO_GRB + NEO_KHZ800);

Adafruit_NeoPixel pixelStatic = Adafruit_NeoPixel(
    NUM_LEDS_STATIC,
    PIN_LEDS_STATIC,
    NEO_GRB + NEO_KHZ800);

const uint32_t COLOR_DEFAULT = Adafruit_NeoPixel::Color(126, 32, 198);
const uint32_t COLOR_STATIC = Adafruit_NeoPixel::Color(200, 200, 200);

const uint32_t AUDIO_TRACK_COLORS[NUM_TRACKS] = {
    Adafruit_NeoPixel::Color(255, 0, 0),
    Adafruit_NeoPixel::Color(255, 0, 10),
    Adafruit_NeoPixel::Color(255, 10, 0)};

//puntitos de las constelaciones
const int numPtsConst = 4;
const int constelLeds0[numPtsConst] = { 1, 9, 17, 19};
const int constelLeds1[numPtsConst] = { 2, 10, 18, 17};
const int constelLeds2[numPtsConst] = { 3, 11, 19, 21};
const int constelLeds3[numPtsConst] = { 4, 12, 20, 20};
const int constelLeds4[numPtsConst] = { 5, 13, 21, 19};
const int constelLeds5[numPtsConst] = { 6, 14, 22, 17};
const int constelLeds6[numPtsConst] = { 7, 15, 23, 21};
const int constelLeds7[numPtsConst] = { 8, 16, 24, 20};

// Funciones de constelaciones
void Constel_0_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds0[i], 10, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * TrollLED - ON * / ");
}

void Constel_0_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds0[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * TrollLED - OFF * / ");
}

void Constel_1_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], 250, 10, 10);
      stripCielo.show();
  }
      Serial.print(" / * VacaLED - ON * / ");
}

void Constel_1_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds1[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * VacaLED - OFF * / ");
}

void Constel_2_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], 250, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * OsoLED - ON * / ");
}

void Constel_2_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds2[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * OsoLED - OFF * / ");
}

void Constel_3_on(){
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], 10, 250, 10);
      stripCielo.show();
  }
      Serial.print(" / * MurteLed - ON * / ");
}

void Constel_3_off(){
  int orden = 0;
  for (int i = 0; i <= numPtsConst-1 ; i++) {
      stripCielo.setPixelColor(constelLeds3[i], 200, 200, 200);
      stripCielo.show();
  }
      Serial.print(" / * MuerteLED - OFF * / ");
}

//Funciones de audio -------------
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


//Contadores de victoria
int solucion = 2;
int solucionNum[] = { 1, 2, 3, 4, 5, 6, 7, 8};
int openRele[] = { true, true, false, false,};// false, false, false, false}
int openAll = 0;

//inicio de tioras led
void iniled() {
  stripOjos.begin();
  stripOjos.setBrightness(30);
  stripOjos.show();
  stripOjos.clear();
  
  stripCielo.begin();
  stripCielo.setBrightness(10);
  stripCielo.show();
  stripCielo.clear();

  pixelStrip01.begin();
  pixelStrip01.setBrightness(LED_BRIGHTNESS);
  pixelStrip01.show();

  pixelStrip02.begin();
  pixelStrip02.setBrightness(LED_BRIGHTNESS);
  pixelStrip02.show();

  pixelStatic.begin();
  pixelStatic.setBrightness(LED_BRIGHTNESS);
  pixelStatic.show();

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
    pixelStatic.clear();

    for (int i = 0; i < NUM_LEDS_STATIC; i++)
    {
        pixelStatic.setPixelColor(i, COLOR_STATIC);
    }

    pixelStatic.show();
}

//* RFID reader functions.
int readCurrentTagIndex()
{
    bool tagFound = rfid.readTag(tag, sizeof(tag));
  //Serial.println("leyendo");
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

// El Troll  -------------------------------------
void signoTroll() {
  Serial.print("++Nivel Troll : ");
    int pulsaTroll = analogRead(A0);
    int trollLevel = map(pulsaTroll, 0, 1024, 0, 10);
       
        Serial.print(pulsaTroll);
        Serial.print("/ medida: ");
        Serial.print(trollLevel);
        
    if (trollLevel >= 6) {
    Serial.print(" / * Trollpush - UP * /");
    Constel_0_on();
    solucionNum[0] = true;
    }
    else {
     Serial.print(" / * Trollpush - DOWN * /");
     Constel_0_off();
     solucionNum[0] = false;
    }
Serial.println(" - Troll OVER - //");
}

// La Vaca  --------------------------------------------
void signoVaca() {
  Serial.print("++Nivel Vaca  : ");
      
    int pulsaVaca = analogRead(A1);
    int vacaLevel = map(pulsaVaca, 0, 1024, 0, 10);
       Serial.print(pulsaVaca);
       Serial.print("/ medida: ");
       Serial.print(vacaLevel);
  if (vacaLevel >= 6) {
    Serial.print(" / * vacaPush - UP  * /");
    Constel_1_on();
    solucionNum[1] = true;
    }
    else {
    Serial.print(" / * VacaPush - DOWN * /");
    Constel_1_off();
    solucionNum[1] = false;
    }
  Serial.println(" - Vaca OVER - //");
}

// El Oso  ------------------------------------------------
void signoOso() {
  Serial.print("++Nivel  Oso  : ");
    int pulsaOso = analogRead(A2);
    int osoLevel = map(pulsaOso, 0, 1024, 0, 10);
        Serial.print(pulsaOso);
        Serial.print("/ medida: ");
        Serial.print(osoLevel);
        
    if (osoLevel >= 6) {
    Serial.print(" / * OsoPush - UP * /");
    Constel_2_on();
    solucionNum[2] = true;
    }
    else {
     Serial.print(" / * OsoPush - DOWN * /");
     Constel_2_off(); 
     solucionNum[2] = false;
    }
Serial.println(" - Oso over - //");
}

// La Muerte  --------------------------------------------
void signoMuerte() {
  Serial.print("++Nivel Muerte : ");
    int pulsaMuerte = analogRead(A3);
    int muerteLevel = map(pulsaMuerte, 0, 1024, 0, 10);
       
        Serial.print(pulsaMuerte);
        Serial.print("/ medida: ");
        Serial.print(muerteLevel);
        
    if (muerteLevel >= 6) {
    Serial.print(" / * Muertepush - UP * /");
    Constel_3_on();
    solucionNum[3] = true;
    }
    else {
     Serial.print(" / * Muertepush - DOWN * /");
     Constel_3_off();
     solucionNum[3] = false;
      
    }
Serial.println(" - Muerte OVER - //");
}

//Condición de Victoria  --------------------------------
void pinM(){
  pinMode(rele, OUTPUT);
}

void blink() {
  digitalWrite(rele, HIGH);
  delay(1000); // Wait for 1000 millisecond(s)
  digitalWrite(rele, LOW);
  delay(1000); // Wait for 1000 millisecond(s)
}

void releOpen() {
  int openAll = solucionNum[0]+solucionNum[1]+solucionNum[2]+solucionNum[3];
  Serial.print("openAll: ");
  Serial.println(openAll);
  int total = 4;
  int exito = 0;
  for (int i = 0; i <= total-1 ; i++) {
    if (solucionNum[i] == openRele[i]) {
    exito++;
    Serial.print(i);
    Serial.println(" // Coincidencia //");
  }
  else {
  Serial.print(i);
  Serial.println(" // Falla //");
  }
  
  }
  if (exito == total) {
    blink();
    Serial.println("Rele abierto");
  }
  else {
  Serial.println("Rele cerrado");
  }
}

void noche() {
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 105);
      stripCielo.show();
     }
  for (int i = 0; i <= estrellas ; i++) {
      stripCielo.setPixelColor(i, 100, 100, 105);
      stripCielo.show();
     }
  Serial.println("+cielo+ OK:");
 }

void constelaciones() {
  signoTroll();
  signoVaca();
  signoOso();
  signoMuerte();
  }

void setup()
{
  Serial.begin(9600);
  sSerial.begin(9600);
  Serial.println("++Starting Cielo++");
  initAudioPins();
  resetAudio();
  iniled();
  pinM();
  noche();
  playLedStartupPattern();
  sSerial.listen();
}

void loop()
{
// Boca
  readTagAndPlayAudio();
  showStaticLed();
// Cielo 
  constelaciones();
  releOpen();
  delay(100);
}
