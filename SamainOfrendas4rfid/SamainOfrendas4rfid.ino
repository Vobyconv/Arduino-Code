#include "rdm630.h"

/**
   Relay
*/

const byte PIN_RELAY = 12;

/**
   RFID modules
*/

const byte NUM_READERS = 4;

// RX, TX

RDM6300 rfid01(2, 3);
RDM6300 rfid02(4, 5);
RDM6300 rfid03(6, 7);
RDM6300 rfid04(8, 9);

RDM6300 rfidReaders[NUM_READERS] = {
    rfid01,
    rfid02,
    rfid03,
    rfid04
};

const unsigned int EMPTY_TOLERANCE = 1;

String currentTags[NUM_READERS];

const int NUM_OPTIONS = 2;

String validTags[NUM_READERS][NUM_OPTIONS] = {
    { "05005FBEEF00", "0200023BE200" },
    { "3C0088693700", "1D00277EC900" },
    { "05005FCFE800", "1D00278E7600" },
    { "0A00539F0600", "1D0027AA9B00" }
};

/**
  Structs.
*/

typedef struct programState {
    bool relayOpened;
    unsigned int emptyReadCount[NUM_READERS];
} ProgramState;

ProgramState progState = {
    .relayOpened = false,
    .emptyReadCount = { 0, 0, 0, 0 }
};

/**
   RFID modules functions
*/

void initRfidReaders()
{
    for (int i = 0; i < NUM_READERS; i++) {
        rfidReaders[i].begin();
    }
}

void pollRfidReaders()
{
    String tagId;

    for (int i = 0; i < NUM_READERS; i++) {
        tagId = rfidReaders[i].getTagId();

        if (tagId.length()) {
            progState.emptyReadCount[i] = 0;
        } else if (progState.emptyReadCount[i] <= EMPTY_TOLERANCE) {
            progState.emptyReadCount[i] += 1;
        }

        if (!tagId.length() && currentTags[i].length() && progState.emptyReadCount[i] <= EMPTY_TOLERANCE) {
            Serial.print("Ignoring empty read on reader: ");
            Serial.println(i);
            return;
        }

        currentTags[i] = tagId;
    }
}

bool isTagDefined(int idx)
{
    return currentTags[idx].length() > 0;
}

bool areCurrentTagsValid()
{
    bool isValid;

    for (int i = 0; i < NUM_READERS; i++) {
        if (!isTagDefined(i)) {
            return false;
        }

        isValid = false;

        for (int j = 0; j < NUM_OPTIONS; j++) {
            if (validTags[i][j].compareTo(currentTags[i]) == 0) {
                isValid = true;
                break;
            }
        }

        if (!isValid) {
            return false;
        }
    }

    return true;
}

void printCurrentTags()
{
    Serial.print("## Current tags :: ");
    Serial.println(millis());

    for (int i = 0; i < NUM_READERS; i++) {
        Serial.print(i);
        Serial.print(" :: ");
        Serial.println(currentTags[i]);
    }
}

/**
   Relay functions
*/

void lockRelay()
{
    digitalWrite(PIN_RELAY, LOW);
    progState.relayOpened = false;
}

void openRelay()
{
    digitalWrite(PIN_RELAY, HIGH);
    progState.relayOpened = true;
}

void initRelay()
{
    pinMode(PIN_RELAY, OUTPUT);
    lockRelay();
}

void checkStatusToUpdateRelay()
{
    if (areCurrentTagsValid() == true && progState.relayOpened == false) {
        Serial.println("Opening relay");
        openRelay();
    } else if (areCurrentTagsValid() == false && progState.relayOpened == true) {
        Serial.println("Locking relay");
        lockRelay();
    }
}

/**
   Entrypoint
*/

void setup()
{
    Serial.begin(9600);

    initRfidReaders();
    initRelay();

    Serial.println(">> Starting frames lock program");
}

void loop()
{
    pollRfidReaders();
    printCurrentTags();
    checkStatusToUpdateRelay();
}
