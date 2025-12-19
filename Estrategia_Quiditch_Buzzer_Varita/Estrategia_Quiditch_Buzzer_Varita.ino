#include <Automaton.h>
#include <Atm_button.h>
#include <Adafruit_NeoPixel.h>

#define LED_PIN   12
#define RELAY_PIN 11
#define BUZZER_PIN 7
#define NUM_LEDS  15

Adafruit_NeoPixel strip(NUM_LEDS, LED_PIN, NEO_GRB + NEO_KHZ800);

Atm_button sensor[5];   // 5 sensores como botones
int posiciones[5] = {0, 0, 0, 0, 0}; // Estado de cada tramo (0=apagado, 1=amarillo, 2=naranja, 3=rojo)

// --- CONFIGURACIÓN DE COMBINACIÓN ---
int combinacionObjetivo[5] = {1, 2, 3, 1, 2}; 
// Ejemplo: sensor1=amarillo, sensor2=naranja, sensor3=rojo, sensor4=amarillo, sensor5=naranja

// --- FUNCIONES AUXILIARES ---
void setSegmentColor(int segment, int pos) {
  int start = segment * 3; // inicio del tramo
  uint32_t color;

  switch (pos) {
    case 1: color = strip.Color(255, 255, 0); break;   // Amarillo
    case 2: color = strip.Color(255, 165, 0); break;   // Naranja
    case 3: color = strip.Color(255, 0, 0); break;     // Rojo
    default: color = strip.Color(0, 0, 0); break;      // Apagado
  }

  for (int i = 0; i < 3; i++) {
    strip.setPixelColor(start + i, color);
  }
  strip.show();
}

void setAllGreen() {
  for (int i = 0; i < NUM_LEDS; i++) {
    strip.setPixelColor(i, strip.Color(0, 255, 0)); // Verde
  }
  strip.show();
}

bool checkCombination() {
  for (int i = 0; i < 5; i++) {
    if (posiciones[i] != combinacionObjetivo[i]) {
      return false;
    }
  }
  return true;
}

// --- BUZZER ---
void beepShort() {
  tone(BUZZER_PIN, 2000, 100); // Pitido agudo corto (2000 Hz, 100 ms)
}

void fanfare() {
  // Melodía sencilla tipo fanfarria
  int melody[] = { 523, 659, 784, 1046 }; // Notas: Do, Mi, Sol, Do alto
  int duration[] = { 200, 200, 200, 400 }; // Duraciones en ms

  for (int i = 0; i < 4; i++) {
    tone(BUZZER_PIN, melody[i], duration[i]);
    delay(duration[i] * 1.3); // pequeña pausa entre notas
  }
  noTone(BUZZER_PIN);
}

// --- CALLBACK DE SENSORES ---
void onSensorPress(int idx) {
  posiciones[idx]++;
  if (posiciones[idx] > 3) posiciones[idx] = 1; // ciclo 1→2→3→1
  setSegmentColor(idx, posiciones[idx]);

  beepShort(); // pitido cada vez que se pulsa un sensor

  if (checkCombination()) {
    digitalWrite(RELAY_PIN, HIGH); // Activa el relé
    setAllGreen();                 // Todos los LEDs en verde
    fanfare();                     // Suena la fanfarria
  } else {
    digitalWrite(RELAY_PIN, LOW);  // Relé apagado
  }
}

// --- SETUP ---
void setup() {
  strip.begin();
  strip.show(); // inicializa apagado
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW);
  pinMode(BUZZER_PIN, OUTPUT);

  // Inicializamos los sensores como botones
  for (int i = 0; i < 5; i++) {
    sensor[i].begin(2 + i) // pines 2,3,4,5,6
      .onPress( onSensorPress, i ); // callback con índice
  }
}

// --- LOOP ---
void loop() {
  automaton.run(); // corre todas las máquinas
}
