#ifndef SIGNALS_H
#define SIGNALS_H
#include <Arduino.h>
#include <bluetooth.h>
#include <channels.h>
#include <animations.h>
#include <location.h>
#include <switches.h>
#include <storage.h>
#include <messagequeue.h>

#define SIGNAL_CORE 0

struct State {
  AnimationState animation;
  SwitchState switches;
  ChannelState channels[MAX_CHANNELS];
  LocationState location;
  Preferences preferences;
  BluetoothState bluetooth;
};

void createSignals(TaskHandle_t* handle);

#endif