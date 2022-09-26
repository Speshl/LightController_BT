#ifndef ANIMATIONS_H
#define ANIMATIONS_H

#include <FastLED.h>
#include <Arduino.h>
#include <channels.h>
#include <location.h>
#include "string"
#include <ArduinoJson.h>


#define ANIMATION_STATE_LENGTH 46
#define MAX_COLORS 10
#define STEP_SIZE_SCALE 20

struct AnimationState {
  uint8_t animation;
  uint8_t palettePreset;
  uint8_t brightness;
  uint8_t stepSize;
  TBlendType blending;
  uint8_t fps;
  byte paletteDescription[MAX_COLORS*4];
  //Below not used to rebuild state
  bool updated;
  CRGBPalette16 palette;
  uint8_t stepIndex;
  uint8_t xPos;
  uint8_t yPos;
  uint8_t zPos;
  uint8_t stepIndexScaler;
  unsigned long frameToFrameTime;
  unsigned long lastFrameTime;
  unsigned long numFrames;
};

union AnimationConversion {
  uint16_t wordValue;
  char charValue[2];
};

void setInitialState(AnimationState* animation);

void setStateFromBytes(AnimationState* animation, uint8_t state[ANIMATION_STATE_LENGTH]);

void getStateAsBytes(AnimationState* animation, uint8_t state[ANIMATION_STATE_LENGTH]);

std::string getStateAsString(AnimationState* animation);

void setStateFromString(AnimationState* animation, std::string input);

void getStateAsJson(AnimationState* animation, JsonObject doc);

void setStateFromJson(AnimationState* animation, JsonObject doc);

void describeState(AnimationState* animation);

uint16_t getStepDelay(AnimationState* animation, uint16_t frameTime);

void updateAnimation(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location);

void brakeAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches);

void reverseAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches);

void leftTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, CRGB color);

void rightTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, CRGB color);

void signalRampUp(ChannelState channels[MAX_CHANNELS], SwitchState* switches);

void signalRampDown(ChannelState channels[MAX_CHANNELS], SwitchState* switches);

std::string getAnimationIndex(AnimationState* animation);

std::string getStepSize(AnimationState* animation);

std::string getColorData(AnimationState* animation);

std::string getColorPos(AnimationState* animation, uint8_t index);

std::string getColorHex(AnimationState* animation, uint8_t index);

void setColorPos(AnimationState* animation, uint8_t index, std::string value);

void setColorHex(AnimationState* animation, uint8_t index, std::string value);

void setColorData(AnimationState* animation, std::string colorData);

std::string getBlending(AnimationState* animation);

void animationUpdated(AnimationState* animation, ChannelState channels[MAX_CHANNELS]);

std::string getFPS(AnimationState* animation);

std::string getBrightness(AnimationState* animation);

#endif
