#ifndef SWITCHES_H
#define SWITCHES_H
#include <Arduino.h>
#include <ArduinoJson.h>

//TODO: Define actual pins
#define LEFT_TURN_PIN 1
#define RIGHT_TURN_PIN 2
#define BRAKE_PIN 3
#define REVERSE_PIN 4
#define INTERIOR_LIGHT_PIN 5

#define SWITCH_STATE_LENGTH 6

struct SwitchState {
    bool leftTurn;
    bool rightTurn;
    bool reverse;
    bool brake;
    bool interior;
    bool uiOverride;
    //bool headlight;
    //bool showLights;
    //Below not used to rebuild state
    bool updated;
};

void initializeSwitches(SwitchState* switches);

bool getSwitchState(int pin);

bool updateSwitchState(SwitchState* switches); //Switch state will become dominant when this is called

void getStateAsBytes(SwitchState* switches, uint8_t state[SWITCH_STATE_LENGTH]);

void setStateFromBytes(SwitchState* switches, uint8_t state[SWITCH_STATE_LENGTH]);

std::string getStateAsString(SwitchState* switches);

void setStateFromString(SwitchState* switches, std::string inputValue);

void describeState(SwitchState* switches);

void getStateAsJson(SwitchState* switches, JsonObject doc);

void setStateFromJson(SwitchState* switches, JsonObject doc);

std::string getLeftTurn(SwitchState* switches);
std::string getRightTurn(SwitchState* switches);
std::string getReverse(SwitchState* switches);
std::string getBrake(SwitchState* switches);
std::string getInterior(SwitchState* switches);
std::string getUIOverride(SwitchState* switches);

#endif
