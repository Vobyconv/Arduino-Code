#include <Automaton.h>
#include <Atm_button.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN     12
#define RELAY_PIN   11
#define BUZZER_PIN   7
#define NUM_LEDS    15

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_RGB + NEO_KHZ800);

Atm_button sensor[5];
int posiciones[5] = {0, 0, 0, 0, 0};

int combinacionObjetivo[5] = {1, 2, 3, 1, 2};

// --- NUEVO: variables para el modo victoria ---
bool modoVictoria = false;
unsigned long tiempoVictoria = 0;
const unsigned long duracionVictoria = 15000; // 15 segundos

// ---------------------------------------------------------
// LEDS: SOLO UN LED ENCENDIDO POR TRAMO
// ---------------------------------------------------------
void setSegmentColor(int segment, int pos) {
  int start = segment * 3;

  // Apagar tramo
  for (int i = 0; i < 3; i++) {
    strip.setPixelColor(start + i, strip.Color(0, 0, 0));
  }

  uint32_t color;
  switch (pos) {
    case 1: color = strip.Color(255, 255, 0); break;
    case 2: color = strip.Color(255, 165, 0); break;
    case 3: color = strip.Color(255, 0, 0); break;
  }

  strip.setPixelColor(start + (pos - 1), color);
  strip.show();
}

void setAllGreen() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0));
  }
  strip.show();
}

// ---------------------------------------------------------
// COMPROBAR COMBINACIÓN
// ---------------------------------------------------------
bool checkCombination() {
  for (int i = 0; i < 5; i++) {
    if (posiciones[i] != combinacionObjetivo[i]) return false;
  }
  return true;
}

// ---------------------------------------------------------
// BUZZER
// ---------------------------------------------------------
void beepShort() {
  tone(BUZZER_PIN, 2000, 100);
}

void fanfare() {
  int melody[]   = {523, 659, 784, 1046};
  int duration[] = {200, 200, 200, 400};

  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, melody[i], duration[i]);
    delay(duration[i] * 1.3);
  }
  noTone(BUZZER_PIN);
}

// ---------------------------------------------------------
// CALLBACK DE AUTOMATON
// ---------------------------------------------------------
void onSensorPress(int idx) {

  // Si estamos en modo victoria, ignorar sensores
  if (modoVictoria) return;

  posiciones[idx]++;
  if (posiciones[idx] > 3) posiciones[idx] = 1;

  setSegmentColor(idx, posiciones[idx]);
  beepShort();

  if (checkCombination()) {
    digitalWrite(RELAY_PIN, HIGH);
    setAllGreen();
    fanfare();

    // Activar modo victoria
    modoVictoria = true;
    tiempoVictoria = millis();
  } else {
    digitalWrite(RELAY_PIN, LOW);
  }
}

// ---------------------------------------------------------
// SETUP
// ---------------------------------------------------------
void setup() {
  strip.begin();
  strip.show();

  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);

  pinMode(BUZZER_PIN, OUTPUT);

  for (int i = 0; i < 5; i++) {
    sensor[i]
      .begin(2 + i)
      .onPress(onSensorPress, i);
  }
}

// ---------------------------------------------------------
// LOOP
// ---------------------------------------------------------
void loop() {
  automaton.run();

  // Si estamos en modo victoria, comprobar si pasaron 15 segundos
  if (modoVictoria) {
    if (millis() - tiempoVictoria >= duracionVictoria) {

      // Salir del modo victoria
      modoVictoria = false;

      // Apagar relé
      digitalWrite(RELAY_PIN, LOW);

      // Restaurar LEDs según posiciones actuales
      for (int i = 0; i < 5; i++) {
        setSegmentColor(i, posiciones[i]);
      }
    }
  }
}
