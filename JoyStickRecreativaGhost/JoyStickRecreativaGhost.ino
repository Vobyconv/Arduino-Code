#include <Arduino.h>
#include <Keyboard.h>
#include <Uduino.h>
#include <Automaton.h>

const int16_t UP = 1;
const int16_t DOWN = 2;
const int16_t LEFT = 3;
const int16_t RIGHT = 4;


const unsigned long PRESS_DELAY_MS = 80;

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

const uint8_t PIN_ACTION_BUTTON = A3;

Atm_button atmActionButton;
Atm_button atmButtons[NUM_BUTTONS];
Atm_led atmLeds[NUM_BUTTONS];
Atm_led atmLedBuiltin;

ButtonConfig btnConfs[NUM_BUTTONS] = {
    {.pinBtn = 6, .pinLed = 7},
    {.pinBtn = 8, .pinLed = 9},
    {.pinBtn = 10, .pinLed = 11},
    {.pinBtn = 12, .pinLed = A0},
    {.pinBtn = A1, .pinLed = A2}};

const uint8_t BUTTON_KEY_MAP[NUM_BUTTONS] = {
    'A', 'O', 'I', 'U', 'Y'};

const uint8_t ACTION_BUTTON_KEY = 'T';

const uint16_t HEARTBEAT_MS = 10000;

const uint16_t SET_ACTIVE_BUTTON_DEBOUNCE_MS = 500;

Atm_timer timerState;

const uint32_t TIMER_STATE_MS = 200;

typedef struct programState
{
  int gameCode;
  unsigned long gameStartMs;
  unsigned long lastHeartbeatMs;
  int idxActiveButton;
  unsigned long lastActiveButtonMs;
} ProgramState;

ProgramState progState = {
    .gameCode = 0,
    .gameStartMs = 0,
    .lastHeartbeatMs = 0,
    .idxActiveButton = -1,
    .lastActiveButtonMs = 0};

void pressKey(uint8_t code)
{
  Keyboard.press(code);
  delay(PRESS_DELAY_MS);
  Keyboard.releaseAll();
}

void onJoyPress(int idx, int v, int up)
{
  switch (idx)
  {
  case UP:
    Keyboard.press(KEY_UP_ARROW);
    break;
  case DOWN:
    Keyboard.press(KEY_DOWN_ARROW);
    break;
  case RIGHT:
    Keyboard.press(KEY_RIGHT_ARROW);
    break;
  case LEFT:
    Keyboard.press(KEY_LEFT_ARROW);
    break;
  default:
    break;
  }
}

void onJoyRelease(int idx, int v, int up)
{
  switch (idx)
  {
  case UP:
    Keyboard.release(KEY_UP_ARROW);
    break;
  case DOWN:
    Keyboard.release(KEY_DOWN_ARROW);
    break;
  case RIGHT:
    Keyboard.release(KEY_RIGHT_ARROW);
    break;
  case LEFT:
    Keyboard.release(KEY_LEFT_ARROW);
    break;
  default:
    break;
  }
}

void initJoystick()
{
  joyBtnUp
      .begin(joyInfo.pinUp)
      .onPress(onJoyPress, UP)
      .onRelease(onJoyRelease, UP);

  joyBtnDown
      .begin(joyInfo.pinDown)
      .onPress(onJoyPress, DOWN)
      .onRelease(onJoyRelease, DOWN);

  joyBtnLeft
      .begin(joyInfo.pinLeft)
      .onPress(onJoyPress, LEFT)
      .onRelease(onJoyRelease, LEFT);

  joyBtnRight
      .begin(joyInfo.pinRight)
      .onPress(onJoyPress, RIGHT)
      .onRelease(onJoyRelease, RIGHT);
}

void updateButtonLeds()
{
  for (uint8_t i = 0; i < NUM_BUTTONS; i++)
  {
    int onOff = progState.idxActiveButton == i ? atmLeds[i].EVT_ON : atmLeds[i].EVT_OFF;
    atmLeds[i].trigger(onOff);
  }
}

void onButtonPress(int idx, int v, int up)
{
  pressKey(BUTTON_KEY_MAP[idx]);
  progState.idxActiveButton = -1;
  updateButtonLeds();
}

void onActionButtonPress(int idx, int v, int up)
{
  pressKey(ACTION_BUTTON_KEY);
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

  atmActionButton
      .begin(PIN_ACTION_BUTTON)
      .onPress(onActionButtonPress);
}

void blinkLed()
{
  atmLedBuiltin.trigger(atmLedBuiltin.EVT_BLINK);
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

void setActiveButton()
{
  int numParams = uduino.getNumberOfParameters();

  if (numParams != 1)
  {
    return;
  }

  char *firstParam = uduino.getParameter(0);
  int idxButton = uduino.charToInt(firstParam);

  if (idxButton < 0 || idxButton >= NUM_BUTTONS)
  {
    return;
  }

  unsigned long now = millis();
  unsigned long diffMs = now - progState.lastActiveButtonMs;

  if (diffMs <= SET_ACTIVE_BUTTON_DEBOUNCE_MS)
  {
    return;
  }

  progState.lastActiveButtonMs = now;
  progState.idxActiveButton = idxButton;
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

void onTimerState(int idx, int v, int up)
{
  updateButtonLeds();
}

void initTimerState()
{
  timerState
      .begin(TIMER_STATE_MS)
      .repeat(-1)
      .onTimer(onTimerState)
      .start();
}

void setup()
{
  const uint32_t blinkDuration = 50;
  const uint32_t blinkPause = 50;
  const uint16_t blinkRepeat = 10;

  Serial.begin(9600);
  Keyboard.begin();
  atmLedBuiltin.begin(LED_BUILTIN).blink(blinkDuration, blinkPause, blinkRepeat);
  initJoystick();
  initButtons();
  initTimerState();
  uduino.addCommand("blinkLed", blinkLed);
  uduino.addCommand("initGame", initGame);
  uduino.addCommand("setActiveButton", setActiveButton);
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
