#include <Arduino.h>
#include <Keyboard.h>
#include <Uduino.h>
#include <Automaton.h>

const char *UDUINO_ID = "yourEnergyArduino";

Uduino uduino(UDUINO_ID);

/*
  ISO keyboard layout:

  +---+---+---+---+---+---+---+---+---+---+---+---+---+-------+
  |35 |1e |1f |20 |21 |22 |23 |24 |25 |26 |27 |2d |2e |BackSp |
  +---+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-----+
  | Tab |14 |1a |08 |15 |17 |1c |18 |0c |12 |13 |2f |30 | Ret |
  +-----++--++--++--++--++--++--++--++--++--++--++--++--++    |
  |CapsL |04 |16 |07 |09 |0a |0b |0d |0e |0f |33 |34 |31 |    |
  +----+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+---+----+
  |Shi.|32 |1d |1b |06 |19 |05 |11 |10 |36 |37 |38 |  Shift   |
  +----+---++--+-+-+---+---+---+---+---+--++---+---++----+----+
  |Ctrl|Win |Alt |                        |AlGr|Win |Menu|Ctrl|
  +----+----+----+------------------------+----+----+----+----+
*/

const uint8_t CODE_ENYE = 0x33;

const unsigned long PRESS_DELAY_MS = 80;
const unsigned long DELAY_LOOP_MS = 10;

typedef struct joystickInfo
{
  uint8_t pinUp;
  uint8_t pinDown;
  uint8_t pinLeft;
  uint8_t pinRight;
} JoystickInfo;

JoystickInfo joyInfo = {
    .pinUp = 2,
    .pinDown = 3,
    .pinLeft = 4,
    .pinRight = 5};

Atm_button joyBtnUp;
Atm_button joyBtnDown;
Atm_button joyBtnLeft;
Atm_button joyBtnRight;

const uint8_t NUM_BUTTONS = 5;

typedef struct buttonConfig
{
  uint8_t pinBtn;
  uint8_t pinLed;
} ButtonConfig;

Atm_button atmButtons[NUM_BUTTONS];
Atm_led atmLeds[NUM_BUTTONS];

ButtonConfig btnConfs[NUM_BUTTONS] = {
    {.pinBtn = 6, .pinLed = 7},
    {.pinBtn = 8, .pinLed = 9},
    {.pinBtn = 10, .pinLed = 11},
    {.pinBtn = 12, .pinLed = A0},
    {.pinBtn = A1, .pinLed = A2}};

const uint8_t BUTTON_KEY_MAP[NUM_BUTTONS] = {
    'P', 'O', 'I', 'U', 'Y'};

const uint16_t HEARTBEAT_MS = 10000;

typedef struct programState
{
  int gameCode;
  unsigned long gameStartMs;
  unsigned long lastHeartbeatMs;
} ProgramState;

ProgramState progState = {
    .gameCode = 0,
    .gameStartMs = 0,
    .lastHeartbeatMs = 0};

void pressKey(uint8_t code)
{
  Keyboard.press(code);
  delay(PRESS_DELAY_MS);
  Keyboard.releaseAll();
}

void onJoyUp(int idx, int v, int up)
{
  pressKey(KEY_UP_ARROW);
}

void onJoyDown(int idx, int v, int up)
{
  pressKey(KEY_DOWN_ARROW);
}

void onJoyLeft(int idx, int v, int up)
{
  pressKey(KEY_LEFT_ARROW);
}

void onJoyRight(int idx, int v, int up)
{
  pressKey(KEY_RIGHT_ARROW);
}

void initJoystick()
{
  joyBtnUp
      .begin(joyInfo.pinUp)
      .onPress(onJoyUp);

  joyBtnDown
      .begin(joyInfo.pinDown)
      .onPress(onJoyDown);

  joyBtnLeft
      .begin(joyInfo.pinLeft)
      .onPress(onJoyLeft);

  joyBtnRight
      .begin(joyInfo.pinRight)
      .onPress(onJoyRight);
}

void onButtonPress(int idx, int v, int up)
{
  pressKey(BUTTON_KEY_MAP[idx]);
}

void initButtons()
{
  for (uint8_t i = 0; i < NUM_BUTTONS; i++)
  {
    atmButtons[i]
        .begin(btnConfs[i].pinBtn)
        .onPress(onButtonPress, i);

    atmLeds[i]
        .begin(btnConfs[i].pinLed);

    atmLeds[i]
        .trigger(atmLeds[i].EVT_OFF);
  }
}

void blinkLed()
{
  const int delayBlinkMs = 50;
  int numBlinks = 15;

  int numParams = uduino.getNumberOfParameters();

  if (numParams > 0)
  {
    char *firstParam = uduino.getParameter(0);
    numBlinks = uduino.charToInt(firstParam);
  }

  for (int i = 0; i < numBlinks; i++)
  {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(delayBlinkMs);
    digitalWrite(LED_BUILTIN, LOW);
    delay(delayBlinkMs);
  }

  digitalWrite(LED_BUILTIN, LOW);

  uduino.println(numBlinks);
}

void initGame()
{
  int numParams = uduino.getNumberOfParameters();

  if (numParams == 0)
  {
    return;
  }

  char *firstParam = uduino.getParameter(0);
  int gameCode = uduino.charToInt(firstParam);

  progState.gameCode = gameCode;
  progState.gameStartMs = millis();
}

void heartbeat()
{
  unsigned long now = millis();
  unsigned long nextMs = progState.lastHeartbeatMs + HEARTBEAT_MS;

  if (now < nextMs)
  {
    return;
  }

  progState.lastHeartbeatMs = now;
  uduino.println(now);
}

void setup()
{
  Serial.begin(9600);
  Keyboard.begin();
  pinMode(LED_BUILTIN, OUTPUT);
  initJoystick();
  initButtons();
  uduino.addCommand("blinkLed", blinkLed);
  uduino.addCommand("initGame", initGame);
}

void loop()
{
  uduino.update();
  delay(DELAY_LOOP_MS);

  automaton.run();

  if (uduino.isConnected())
  {
    heartbeat();
  }
}