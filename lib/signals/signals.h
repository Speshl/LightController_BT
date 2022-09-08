#ifndef SIGNALS_H
#define SIGNALS_H
#include <Arduino.h>
#include <bluetooth.h>
#include <channels.h>
#include <animations.h>
#include <location.h>
#include <switches.h>
#include <storage.h>

#define SIGNAL_CORE 0

static bool paused = false;

struct State {
  AnimationState animation;
  SwitchState switches;
  ChannelState channels[MAX_CHANNELS];
  LocationState location;
  Preferences preferences;
  BluetoothState bluetooth;
};

void createSignals(State* state, TaskHandle_t* handle);

void pauseSignals(TaskHandle_t* handle);

void unPauseSignals(TaskHandle_t* handle);

#endif