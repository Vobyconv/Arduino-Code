#include <Automaton.h>

/**
 * Sensores Hall
 */

const int SENSOR_NUM = 3;
const int SENSOR_PINS[SENSOR_NUM] = {2, 3, 4};

Atm_button sensorButtons[SENSOR_NUM];
Atm_controller sensorController;

/**
 * Relays
 */

const int RELAY_PIN = 11;

// Milisegundos que el relay queda abierto antes de cerrarse automaticamente
const unsigned long RELAY_CLOSE_DELAY_MS = 5000;

/**
 * Potenciometros
 */

const int NUM_POTS = 2;

// Valores minimo y maximo que puede tomar un potenciometro
const int POTS_RANGE_LO = 0;
const int POTS_RANGE_HI = 9;

int potPins[NUM_POTS] = {A4, A3};

// Este array define los rangos validos de los potenciometros:
// El elemento [x][0] es el valor inferior del rango para el potenciometro x
// El elemento [x][1] es el valor superior del rango para el potenciometro x
const int potValidRanges[NUM_POTS][2] = {
    {2, 7},
    {2, 7}};

Atm_analog pots[NUM_POTS];

struct ProgramState
{
    unsigned millisSensorsOff;
};

ProgramState progState;

void initState()
{
    progState.millisSensorsOff = 0;
}

void lockRelay()
{
    digitalWrite(RELAY_PIN, LOW);
}

void openRelay()
{
    digitalWrite(RELAY_PIN, HIGH);
}

void initRelay()
{
    pinMode(RELAY_PIN, OUTPUT);
    lockRelay();
}

void checkRelay()
{
    if (progState.millisSensorsOff == 0)
    {
        return;
    }

    unsigned long now = millis();
    unsigned long millisLock = progState.millisSensorsOff + RELAY_CLOSE_DELAY_MS;

    if (now >= millisLock)
    {
        lockRelay();
        progState.millisSensorsOff = 0;
    }
}

bool isValidPotsCombination()
{
    int currState[NUM_POTS];

    for (int i = 0; i < NUM_POTS; i++)
    {
        currState[i] = pots[i].state();
    }

    for (int i = 0; i < NUM_POTS; i++)
    {
        if (currState[i] < potValidRanges[i][0] ||
            currState[i] > potValidRanges[i][1])
        {
            return false;
        }
    }

    return true;
}

bool allSensorsActive(int)
{
    for (int i = 0; i < SENSOR_NUM; i++)
    {
        if (sensorButtons[i].state() == Atm_button::BTN_RELEASE)
        {
            return false;
        }
    }

    return isValidPotsCombination();
}

void onSensorPress(int idx, int v, int up)
{
    Serial.print("Sensor ON #");
    Serial.println(idx);
}

void onControllerUp(int, int, int)
{
    Serial.println(F("Sensor controller up"));
    progState.millisSensorsOff = 0;
    openRelay();
}

void onControllerDown(int, int, int)
{
    Serial.println(F("Sensor controller down"));
    progState.millisSensorsOff = millis();
}

void onPotChange(int idx, int v, int up)
{
    Serial.print("Pot #");
    Serial.print(idx);
    Serial.print(": ");
    Serial.println(v);
}

void initSensors()
{
    for (int i = 0; i < NUM_POTS; i++)
    {
        pots[i]
            .begin(potPins[i])
            .range(POTS_RANGE_LO, POTS_RANGE_HI)
            .onChange(onPotChange, i);
    }

    for (int i = 0; i < SENSOR_NUM; i++)
    {
        sensorButtons[i]
            .begin(SENSOR_PINS[i])
            .onPress(onSensorPress, i);
    }

    sensorController
        .begin()
        .IF(allSensorsActive)
        .onChange(true, onControllerUp)
        .onChange(false, onControllerDown);
}

void setup()
{
    Serial.begin(9600);

    initState();
    initSensors();
    initRelay();

    Serial.println(">> Imanes y potenciometros");
}

void loop()
{
    automaton.run();
    checkRelay();
}