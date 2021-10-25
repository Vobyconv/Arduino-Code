#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Control pins
const int PIN_INPUT_ACTIVATION = 11;
const int PIN_OUTPUT_RELAY = 12;

// Program state
bool isActive = false;
bool isRelayOpen = false;

const int NUM_MURCIELAGOS = 7;
const int NUM_LEDS = 2;

int tiraEyes = 10;
int puntosEyes = 14;
int sensibilidad = 6;
int tope = 20;
int limiteChillido = 48;
int agudo = 1100;
int tiempoChillido = 4;

Adafruit_NeoPixel stripBat = Adafruit_NeoPixel(puntosEyes, tiraEyes, NEO_GRB + NEO_KHZ800);

// Pines de murcielagos
int batSensores[NUM_MURCIELAGOS] = {A0, A1, A2, A3, A4, A5, A6};

// Chillidos
int batSound[NUM_MURCIELAGOS] = {2, 3, 4, 5, 6, 7, 8};

// Puntitos de Ojos murcielago
int batLeds[NUM_MURCIELAGOS][NUM_LEDS] = {{0, 1}, {2, 3}, {4, 5}, {6, 7}, {8, 9}, {10, 11}, {12, 13}};

// Contadores de luz
int batLights[NUM_MURCIELAGOS] = {0, 0, 0, 0, 0, 0, 0};

// Contadores de victoria
bool solucionNum[NUM_MURCIELAGOS] = {false, false, false, false, false, false, false};

// Para omitir comprobaciones
bool batCheck[NUM_MURCIELAGOS] = {false, false, false, false, false, false, false};

/**
 * https://www.avdweb.nl/arduino/misc/handy-functions
 * When the output is high, the impedance must be high; this is done by making the pinmode INPUT. 
 * When the output is low, it must be 0V; this is done by making the pinmode OUTPUT and the output level LOW.
 * The receiver must use INPUT_PULLUP. 
 * The signal will be LOW if the transmitter is powered down.
 */
void setOpenDrainOutput(uint8_t pin, bool value)
{
  if (value)
  {
    pinMode(pin, INPUT);
  }
  else
  {
    pinMode(pin, OUTPUT);
    digitalWrite(pin, LOW);
  }
}

void initLeds()
{
  stripBat.begin();
  stripBat.setBrightness(10);
  stripBat.show();
  stripBat.clear();
}

void batDark(int murci)
{
  int chillido = random(0, 50);

  if (chillido >= 40)
  {
    stripBat.setPixelColor(batLeds[murci][0], random(100), 0, 0);
    stripBat.setPixelColor(batLeds[murci][1], random(100), 0, 0);
    stripBat.show();
  }
  else
  {
    stripBat.setPixelColor(batLeds[murci][0], 0, 0, 0);
    stripBat.setPixelColor(batLeds[murci][1], 0, 0, 0);
    stripBat.show();
  }
}

void batLight(int murci)
{
  stripBat.setPixelColor(batLeds[murci][0], 200, 200, 0);
  stripBat.setPixelColor(batLeds[murci][1], 200, 200, 0);
  stripBat.show();
}

void batBright(int murci)
{
  stripBat.setPixelColor(batLeds[murci][0], 0, 0, 200);
  stripBat.setPixelColor(batLeds[murci][1], 0, 0, 200);
  stripBat.show();
}

void releOpen()
{
  int qttWin = 0;

  for (int i = 0; i < NUM_MURCIELAGOS; i++)
  {
    if (solucionNum[i] == true)
    {
      qttWin++;
    }
    else
    {
      qttWin = 0;
      break;
    }
  }

  if (qttWin == NUM_MURCIELAGOS)
  {
    Serial.println("Opening relay");
    isRelayOpen = true;
  }
  else
  {
    isRelayOpen = false;
  }
}

void checkBats(int murci)
{
  int batSensor = analogRead(batSensores[murci]);
  int batLevel = map(batSensor, 0, 700, 0, 10);

  int chillido = random(0, 50);

  if (chillido >= limiteChillido)
  {
    tone(batSound[murci], agudo, tiempoChillido);
  }

  if (batLevel >= sensibilidad)
  {
    batLight(murci);
    batCheck[murci] = true;
    batLights[murci] = batLights[murci] + 1;
    tone(batSound[murci], agudo + batLights[murci] * 50, tiempoChillido * 3);
  }
  else
  {
    batCheck[murci] = false;
  }

  if (batLights[murci] >= tope)
  {
    batBright(murci);
    solucionNum[murci] = true;
  }
  else
  {
    batDark(murci);
    solucionNum[murci] = false;
  }
}

void murcielagos()
{
  for (int i = 0; i < NUM_MURCIELAGOS; i++)
  {
    if (!solucionNum[i])
    {
      checkBats(i);
    }
  }
}

void resetOjos()
{
  for (int i = 0; i < NUM_MURCIELAGOS; i++)
  {
    if (!batCheck[i])
    {
      stripBat.setPixelColor(batLeds[i][0], 0, 0, 0);
      stripBat.setPixelColor(batLeds[i][1], 0, 0, 0);
      stripBat.show();
    }
  }
}

void ojitos()
{
  int murcielagoRND = 0;
  int resetRND = 0;

  resetRND = random(14);
  murcielagoRND = random(0, NUM_MURCIELAGOS);

  if (resetRND <= 4 && !batCheck[murcielagoRND])
  {
    resetOjos();
  }

  if (!batCheck[murcielagoRND])
  {
    stripBat.setPixelColor(batLeds[murcielagoRND][0], 200, 0, 0);
    stripBat.setPixelColor(batLeds[murcielagoRND][1], 200, 0, 0);
    stripBat.show();
  }
}

void updateActivationState()
{
  const int readDelayMs = 200;
  const int numReads = 15;

  for (int i = 0; i < numReads; i++)
  {
    if (digitalRead(PIN_INPUT_ACTIVATION) != HIGH)
    {
      return;
    }

    delay(readDelayMs);
  }

  Serial.println(F("Detected activation pulse"));
  isActive = true;
}

void setup()
{
  Serial.begin(9600);
  setOpenDrainOutput(PIN_OUTPUT_RELAY, false);
  pinMode(PIN_INPUT_ACTIVATION, INPUT_PULLUP);
  initLeds();
  Serial.println(F("Starting Murcielagos"));
}

void loop()
{
  const int delayLoopMs = 80;

  if (isActive)
  {
    if (isRelayOpen)
    {
      setOpenDrainOutput(PIN_OUTPUT_RELAY, true);
    }
    else
    {
      ojitos();
      murcielagos();
      releOpen();
      setOpenDrainOutput(PIN_OUTPUT_RELAY, false);
    }

    delay(delayLoopMs);
  }
  else
  {
    updateActivationState();
  }
}