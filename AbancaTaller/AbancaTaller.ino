#include "rdm630.h"
#include <Adafruit_NeoPixel.h>
#include <Automaton.h>

// -----------------------------------------------------------------------------
// Configuracion del lector RFID
// -----------------------------------------------------------------------------
// Pines de comunicacion serial con el modulo RDM6300
const byte PIN_RFID_RX = 8;  // Pin de recepcion (RX)
const byte PIN_RFID_TX = A7; // Pin de transmision (TX)
RDM6300 rfid(PIN_RFID_RX, PIN_RFID_TX);

// Intervalo minimo entre lecturas RFID para evitar lecturas repetidas
// Valor recomendado: 1000ms (1 segundo)
const unsigned long RFID_READ_INTERVAL_MS = 2000;
unsigned long lastRfidReadTime = 0;

// -----------------------------------------------------------------------------
// Configuracion de tiempos y retardos
// -----------------------------------------------------------------------------
// Duracion del pulso bajo para activar una pista de audio en el modulo DFPlayer
const unsigned long AUDIO_TRIGGER_DELAY_MS = 200;

// Retardo tras iniciar la reproduccion de una pista antes de activar efectos LED
const unsigned long AUDIO_TRACK_TRIGGER_DELAY_MS = 300;

// Intervalo de actualizacion del efecto de parpadeo LED (menor = mas rapido)
const unsigned long LED_BLINK_INTERVAL_MS = 100;

// Duracion de cada fase (encendido/apagado) del parpadeo de inicio
const unsigned long STARTUP_BLINK_DELAY_MS = 500;

// Tiempo que permanece encendida la tira LED secreta tras desbloquearla
const unsigned long SECRET_STRIP_DISPLAY_MS = 3000;

// Duracion maxima de reproduccion de cada pista de audio (timeout de seguridad)
const unsigned long AUDIO_TRACK_MAX_MS = 30000;

// -----------------------------------------------------------------------------
// Configuracion de tiras LED
// -----------------------------------------------------------------------------
// Nivel de brillo para todas las tiras LED (0-255, donde 255 es brillo maximo)
// Valor recomendado: 200 para evitar consumo excesivo de corriente
const uint8_t LED_BRIGHTNESS = 200;
const uint8_t LED_BRIGHTNESS_LOW = 120;

// Pin de datos y numero de LEDs de la tira de audio
const uint8_t PIN_LEDS_AUDIO = 11;
const uint16_t NUM_LEDS_AUDIO = 10;

// Pin de datos y numero de LEDs de la tira secreta
const uint8_t PIN_LEDS_SECRET = 12;
const uint16_t NUM_LEDS_SECRET = 10;

// Pin de datos y numero de LEDs de la tira de debug
const uint8_t PIN_LEDS_DEBUG = 9;
const uint16_t NUM_LEDS_DEBUG = 60;

const uint32_t INFO_STRIP_COLOR = Adafruit_NeoPixel::Color(0, 255, 255);

// -----------------------------------------------------------------------------
// Configuracion de feedback de botones
// -----------------------------------------------------------------------------
// Duracion del efecto de feedback visual en la tira de info (ms)
const unsigned long BUTTON_FEEDBACK_DURATION_MS = 200;

// Color del efecto de feedback (blanco suave)
const uint32_t BUTTON_FEEDBACK_COLOR = Adafruit_NeoPixel::Color(200, 200, 200);

// -----------------------------------------------------------------------------
// Configuracion de botones
// -----------------------------------------------------------------------------
// Tiempo de antirrebote para los botones (evita lecturas multiples por pulsacion)
const unsigned long BUTTON_DEBOUNCE_MS = 50;

// Numero total de botones en el sistema
const int NUM_BUTTONS = 6;

// Pines GPIO donde estan conectados los botones
const byte BUTTON_PINS[NUM_BUTTONS] = {2, 3, 4, 5, 6, 7};

// -----------------------------------------------------------------------------
// Configuracion de la secuencia secreta
// -----------------------------------------------------------------------------
// Longitud de la secuencia secreta que desbloquea el easter egg
const int SEQUENCE_LENGTH = 4;

// Secuencia de botones que debe pulsarse para desbloquear el contenido secreto
// Los numeros corresponden a los indices de BUTTON_PINS (0-5)
const byte SECRET_SEQUENCE[SEQUENCE_LENGTH] = {0, 4, 1, 2};

// -----------------------------------------------------------------------------
// Configuracion de comunicacion serial
// -----------------------------------------------------------------------------
// Velocidad de comunicacion del puerto serie para depuracion
const unsigned long SERIAL_BAUD_RATE = 9600;

// -----------------------------------------------------------------------------
// Configuracion de animacion de inicio
// -----------------------------------------------------------------------------
// Numero de veces que parpadean los LEDs al arrancar el sistema
const int STARTUP_BLINK_CYCLES = 2;

// -----------------------------------------------------------------------------
// Configuracion de pistas de audio y etiquetas RFID
// -----------------------------------------------------------------------------
// Numero de pistas de audio disponibles
const int NUM_TRACKS = 5;

// Numero de etiquetas RFID asignadas a cada pista (redundancia)
const int TAGS_PER_TRACK = 2;

// Mapeo de identificadores de etiquetas RFID a pistas de audio
// Cada fila representa una pista, cada columna una etiqueta alternativa
const String TRACK_TAG_IDS[NUM_TRACKS][TAGS_PER_TRACK] = {
    {"6400D43A8100", "4300A80C13F4"}, // Pista 0
    {"4D0066410900", "3C00D5A7AFE1"}, // Pista 1
    {"4E0015B53E00", "4300A13863B9"}, // Pista 2
    {"6B001C50FC00", "4300A8038B63"}, // Pista 3
    {"4E00163AA400", "4300A78CBDD5"}  // Pista 4
};

// Pines analogicos para activar cada pista de audio en el modulo DFPlayer
const byte AUDIO_TRACK_PINS[NUM_TRACKS] = {A0, A1, A2, A3, A4};

// Pin analogico para activar la pista de audio secreta
const byte AUDIO_SECRET_PIN = A5;

// Pin que indica si el modulo de audio esta reproduciendo (LOW = reproduciendo)
const byte PIN_AUDIO_ACT = 10;

// Colores RGB asignados a cada pista para el efecto LED
// Formato: Color(Rojo, Verde, Azul) con valores de 0-255
const uint32_t AUDIO_TRACK_COLORS[NUM_TRACKS] = {
    Adafruit_NeoPixel::Color(255, 0, 0),   // Pista 0: Rojo
    Adafruit_NeoPixel::Color(0, 255, 0),   // Pista 1: Verde
    Adafruit_NeoPixel::Color(0, 0, 255),   // Pista 2: Azul
    Adafruit_NeoPixel::Color(255, 255, 0), // Pista 3: Amarillo
    Adafruit_NeoPixel::Color(255, 0, 255)  // Pista 4: Magenta
};

// =============================================================================
// INICIALIZACION DE OBJETOS
// =============================================================================

// Tira LED para efectos visuales de audio
Adafruit_NeoPixel audioStrip = Adafruit_NeoPixel(NUM_LEDS_AUDIO, PIN_LEDS_AUDIO, NEO_GRB + NEO_KHZ800);

// Tira LED para indicar desbloqueo del contenido secreto
Adafruit_NeoPixel secretStrip = Adafruit_NeoPixel(NUM_LEDS_SECRET, PIN_LEDS_SECRET, NEO_GRB + NEO_KHZ800);

// Tira LED de debug
Adafruit_NeoPixel infoStrip = Adafruit_NeoPixel(NUM_LEDS_DEBUG, PIN_LEDS_DEBUG, NEO_GRB + NEO_KHZ800);

// Array de botones gestionados por la libreria Automaton
Atm_button buttons[NUM_BUTTONS];

// =============================================================================
// VARIABLES DE ESTADO DEL SISTEMA
// =============================================================================

// Buffer circular para almacenar la secuencia de botones pulsados
byte inputBuffer[SEQUENCE_LENGTH];
int bufferIndex = 0;

// Indica si el contenido secreto ya ha sido desbloqueado
bool secretUnlocked = false;

// Variables de control para el efecto LED no bloqueante
bool ledEffectRunning = false; // Indica si hay un efecto LED en ejecucion
int currentTrackIdx = -1;      // Indice de la pista actual (determina el color)
unsigned long lastUpdate = 0;  // Timestamp de la ultima actualizacion del efecto
bool ledOn = false;            // Estado actual del parpadeo (encendido/apagado)
unsigned long effectStart = 0; // Timestamp de inicio del efecto LED actual

// Variables de control para el feedback de botones en infoStrip
bool buttonFeedbackActive = false;     // Indica si hay un efecto de feedback activo
unsigned long buttonFeedbackStart = 0; // Timestamp de inicio del efecto de feedback

// =============================================================================
// DECLARACION DE FUNCIONES
// =============================================================================
bool isSequenceCorrect();
void activateSecretStrip();
void onButtonPress(int idx, int v, int up);
void checkRFID();
void updateButtonFeedback();

// -----------------------------------------------------------------------------
// Funciones de gestion de audio
// -----------------------------------------------------------------------------

/**
 * Comprueba si el modulo de audio esta reproduciendo alguna pista
 * @return true si esta reproduciendo, false en caso contrario
 */
bool isTrackPlaying()
{
  return digitalRead(PIN_AUDIO_ACT) == LOW;
}

/**
 * Reproduce una pista de audio mediante pulso bajo en el pin especificado
 * @param trackPin Pin analogico conectado a la pista del modulo DFPlayer
 */
void playTrack(byte trackPin)
{
  if (isTrackPlaying())
  {
    Serial.println(F("Audio: Reproduccion bloqueada - ya hay audio en curso"));
    return;
  }

  Serial.print(F("Audio: Iniciando reproduccion en pin "));
  Serial.println(trackPin);

  digitalWrite(trackPin, LOW);
  pinMode(trackPin, OUTPUT);
  delay(AUDIO_TRIGGER_DELAY_MS);
  pinMode(trackPin, INPUT);

  Serial.println(F("Audio: Pulso de activacion enviado correctamente"));
}

// -----------------------------------------------------------------------------
// Funciones de efectos LED
// -----------------------------------------------------------------------------

/**
 * Gestiona el efecto de parpadeo LED asociado a la reproduccion de audio
 * Implementacion no bloqueante que permite la ejecucion concurrente del programa
 * @param trackIdx Indice de la pista (determina el color del efecto)
 */
void displayAudioLedEffect(int trackIdx)
{
  if (!ledEffectRunning)
  {
    return;
  }

  if (!isTrackPlaying())
  {
    audioStrip.clear();
    audioStrip.show();
    ledEffectRunning = false;
    Serial.println(F("LED: No hay audio reproduciendose, efecto detenido"));
    return;
  }

  // Issue #1: Validar que trackIdx este dentro de los limites del array
  if (trackIdx < 0 || trackIdx >= NUM_TRACKS)
  {
    audioStrip.clear();
    audioStrip.show();
    ledEffectRunning = false;
    Serial.println(F("LED: Indice de pista invalido, efecto detenido"));
    return;
  }

  if (millis() - effectStart >= AUDIO_TRACK_MAX_MS)
  {
    audioStrip.clear();
    audioStrip.show();
    ledEffectRunning = false;
    Serial.println(F("LED: Efecto de audio finalizado por timeout"));
    return;
  }

  if (millis() - lastUpdate >= LED_BLINK_INTERVAL_MS)
  {
    lastUpdate = millis();
    uint32_t color = AUDIO_TRACK_COLORS[trackIdx];

    if (ledOn)
    {
      audioStrip.clear();
    }
    else
    {
      for (uint16_t i = 0; i < NUM_LEDS_AUDIO; i++)
      {
        audioStrip.setPixelColor(i, color);
      }
    }

    audioStrip.show();
    ledOn = !ledOn;
  }
}

/**
 * Gestiona el efecto de feedback visual en la tira de info cuando se pulsa un boton
 * Implementacion no bloqueante que restaura el estado original rapidamente
 */
void updateButtonFeedback()
{
  if (!buttonFeedbackActive)
  {
    return;
  }

  unsigned long elapsed = millis() - buttonFeedbackStart;

  if (elapsed >= BUTTON_FEEDBACK_DURATION_MS)
  {
    // Restaurar el color original de la tira de info
    infoStrip.fill(INFO_STRIP_COLOR);
    infoStrip.show();
    buttonFeedbackActive = false;
  }
}

/**
 * Inicia el efecto de feedback visual en la tira de info
 * Muestra un flash rapido de color blanco
 */
void triggerButtonFeedback()
{
  infoStrip.fill(BUTTON_FEEDBACK_COLOR);
  infoStrip.show();
  buttonFeedbackActive = true;
  buttonFeedbackStart = millis();
}

// -----------------------------------------------------------------------------
// Funciones de inicializacion
// -----------------------------------------------------------------------------

/**
 * Configura los pines del modulo de audio como entradas
 */
void initAudioPins()
{
  Serial.println(F("Inicializacion: Configurando pines de audio"));
  for (int i = 0; i < NUM_TRACKS; i++)
  {
    pinMode(AUDIO_TRACK_PINS[i], INPUT);
  }
  pinMode(AUDIO_SECRET_PIN, INPUT);
  pinMode(PIN_AUDIO_ACT, INPUT_PULLUP);
  Serial.println(F("Inicializacion: Pines de audio configurados"));
}

/**
 * Inicializa las tiras LED (audio, secreta y debug) y las apaga
 */
void initLeds()
{
  Serial.println(F("Inicializacion: Configurando tiras LED"));
  audioStrip.begin();
  audioStrip.setBrightness(LED_BRIGHTNESS);
  audioStrip.clear();
  audioStrip.show();

  secretStrip.begin();
  secretStrip.setBrightness(LED_BRIGHTNESS);
  secretStrip.clear();
  secretStrip.show();

  infoStrip.begin();
  infoStrip.setBrightness(LED_BRIGHTNESS_LOW);
  infoStrip.fill(INFO_STRIP_COLOR);
  infoStrip.show();
}

/**
 * Configura los botones con sus pines y callbacks, aplicando debounce
 */
void initButtons()
{
  Serial.println(F("Inicializacion: Configurando botones"));
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    buttons[i].begin(BUTTON_PINS[i]).onPress(onButtonPress, i).debounce(BUTTON_DEBOUNCE_MS);
  }
  Serial.print(F("Inicializacion: "));
  Serial.print(NUM_BUTTONS);
  Serial.println(F(" botones configurados con debounce"));
}

/**
 * Animacion de inicio: parpadea todas las tiras LED en azul
 */
void startupBlink()
{
  Serial.println(F("Prueba de luces: Iniciando animacion de arranque"));

  const uint32_t blue = Adafruit_NeoPixel::Color(0, 0, 255);

  for (int k = 0; k < STARTUP_BLINK_CYCLES; k++)
  {
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
    delay(STARTUP_BLINK_DELAY_MS);

    audioStrip.clear();
    secretStrip.clear();
    audioStrip.show();
    secretStrip.show();
    delay(STARTUP_BLINK_DELAY_MS);
  }

  Serial.println(F("Prueba de luces: Animacion completada exitosamente"));
}

// -----------------------------------------------------------------------------
// Funciones de gestion de secuencia secreta
// -----------------------------------------------------------------------------

/**
 * Callback ejecutado cuando se pulsa un boton
 * Almacena la pulsacion y comprueba si se ha completado la secuencia secreta
 * @param idx Indice del boton pulsado (0-5)
 * @param v Valor (no utilizado)
 * @param up Estado up/down (no utilizado)
 */
void onButtonPress(int idx, int v, int up)
{
  Serial.print(F("Boton: Pulsado boton numero "));
  Serial.println(idx);

  // Activar feedback visual inmediato
  triggerButtonFeedback();

  if (secretUnlocked)
  {
    Serial.println(F("Boton: Secreto ya desbloqueado, ignorando pulsacion"));
    return;
  }

  inputBuffer[bufferIndex] = idx;
  bufferIndex = (bufferIndex + 1) % SEQUENCE_LENGTH;

  if (isSequenceCorrect())
  {
    Serial.println(F("Secreto: Secuencia correcta detectada"));

    // Issue #6: Verificar que no haya audio reproduciendose antes de activar el secreto
    if (isTrackPlaying())
    {
      Serial.println(F("Secreto: Esperando fin de reproduccion actual"));
      return;
    }

    activateSecretStrip();
    playTrack(AUDIO_SECRET_PIN);
    delay(AUDIO_TRACK_TRIGGER_DELAY_MS);
    currentTrackIdx = 0; // Usar color rojo para pista secreta
    effectStart = millis();
    ledEffectRunning = true;
    secretUnlocked = true;
    Serial.println(F("Secreto: Contenido secreto desbloqueado exitosamente"));
  }
}

/**
 * Verifica si la secuencia de botones almacenada coincide con la secuencia secreta
 * @return true si la secuencia es correcta, false en caso contrario
 */
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

/**
 * Activa la tira LED secreta con color verde durante unos segundos
 */
void activateSecretStrip()
{
  Serial.println(F("LED Secreto: Activando tira LED verde"));
  for (uint16_t i = 0; i < NUM_LEDS_SECRET; i++)
  {
    secretStrip.setPixelColor(i, Adafruit_NeoPixel::Color(0, 255, 0));
  }
  secretStrip.show();
}

// -----------------------------------------------------------------------------
// Funciones de gestion RFID
// -----------------------------------------------------------------------------

/**
 * Lee el lector RFID y reproduce la pista correspondiente si se detecta una etiqueta valida
 * Incluye rate limiting para evitar sobrecargar el lector RFID
 */
void checkRFID()
{
  // Limitar la lectura del RFID a una frecuencia determinada
  unsigned long currentTime = millis();
  if (currentTime - lastRfidReadTime < RFID_READ_INTERVAL_MS)
  {
    return;
  }

  // Actualizar el tiempo de la ultima lectura ANTES de intentar leer
  // Esto evita sobrecargar el lector incluso cuando no hay etiquetas
  lastRfidReadTime = currentTime;

  Serial.println(F("RFID: Esperando etiqueta"));
  String tagId = rfid.getTagId();

  if (tagId.length() == 0)
  {
    Serial.println(F("RFID: No se ha detectado ninguna etiqueta"));
    return;
  }

  // Issue #10: Agregar debug de RFID
  Serial.print(F("RFID: Etiqueta detectada: "));
  Serial.println(tagId);

  for (int i = 0; i < NUM_TRACKS; i++)
  {
    for (int j = 0; j < TAGS_PER_TRACK; j++)
    {
      if (tagId.equals(TRACK_TAG_IDS[i][j]))
      {
        Serial.print(F("RFID: Coincidencia encontrada - Pista "));
        Serial.print(i);
        Serial.print(F(" (Tag "));
        Serial.print(j);
        Serial.println(F(")"));
        playTrack(AUDIO_TRACK_PINS[i]);
        delay(AUDIO_TRACK_TRIGGER_DELAY_MS);
        currentTrackIdx = i;
        effectStart = millis();
        ledEffectRunning = true;
        Serial.println(F("RFID: Iniciando efecto LED de audio"));
        return;
      }
    }
  }

  // Issue #10: Informar cuando la etiqueta no coincide
  Serial.println(F("RFID: Etiqueta no reconocida en el sistema"));
}

// =============================================================================
// FUNCIONES PRINCIPALES DE ARDUINO
// =============================================================================

/**
 * Configuracion inicial del sistema
 */
void setup()
{
  Serial.begin(SERIAL_BAUD_RATE);
  Serial.println(F("==========================================="));
  Serial.println(F("Sistema: Iniciando configuracion"));
  Serial.println(F("==========================================="));

  Serial.println(F("RFID: Inicializando modulo lector"));
  rfid.begin();
  Serial.println(F("RFID: Modulo inicializado correctamente"));

  initAudioPins();
  initLeds();
  initButtons();
  startupBlink();

  Serial.println(F("==========================================="));
  Serial.println(F("Sistema: Configuracion completada"));
  Serial.println(F("Sistema: Programa principal en ejecucion"));
  Serial.println(F("==========================================="));
}

/**
 * Bucle principal del programa
 */
void loop()
{
  automaton.run();
  checkRFID();
  displayAudioLedEffect(currentTrackIdx);
  updateButtonFeedback();
}
