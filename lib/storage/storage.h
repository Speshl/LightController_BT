#ifndef STORAGE_H
#define STORAGE_H
#include <Preferences.h>

void initializeStorage(Preferences* preferences);

bool isFirstRun(Preferences* preferences);

void saveSwitchState(Preferences* preferences, uint8_t value[], int valueSize);

void saveAnimationState(Preferences* preferences, uint8_t value[], int valueSize);

void saveChannelDetailState(Preferences* preferences, int index, uint8_t value[], int valueSize);

void saveChannelLocationState(Preferences* preferences, int index, uint8_t value[], int valueSize);

bool loadSwitchState(Preferences* preferences, uint8_t value[], int valueSize);

bool loadAnimationState(Preferences* preferences, uint8_t value[], int valueSize);

bool loadChannelDetailState(Preferences* preferences, int index, uint8_t value[], int valueSize);

bool loadChannelLocationState(Preferences* preferences, int index, uint8_t value[], int valueSize);

#endif
