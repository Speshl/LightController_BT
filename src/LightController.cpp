#include <Arduino.h>
#include <channels.h>
#include <animations.h>
#include <location.h>
#include <switches.h>
#include <storage.h>
#include <signals.h>
#include <web.h>

#define SAVED_STATE_VALID_INDICATOR 127
#define SHOW_CORE 0
#define SIGNAL_CORE 0

State state;

static TaskHandle_t signalTaskHandle = 0;

void describeState(State* currentState){  
  Serial.println("Listing LightController Detailed State");
  describeState(&(currentState->switches));
  describeState(&(currentState->animation));
  describeState(currentState->channels);
  //describeState(&(currentState->location));
}

void saveState(State* currentState){
  Serial.println("Saving State");
  saveSwitchState(&currentState->preferences, getStateAsString(&currentState->switches));
  saveAnimationState(&currentState->preferences, getStateAsString(&currentState->animation));
  for(int i=0; i<MAX_CHANNELS; i++){
    saveChannelDetailState(&currentState->preferences, i, getStateAsString(currentState->channels, i));
    uint8_t locationArray[MAX_LEDS*3];
    getChannelLocations(&currentState->location, i, currentState->channels[i].numLEDs, locationArray);
    saveChannelLocationState(&currentState->preferences, i, locationArray);
  }
  Serial.println("State Saved");
}

void loadSavedIfFound(State* currentState){
  Serial.println("Checking for Saved States");
  bool matchedAll = true; //TEMP SHOULD BE true

  if(isFirstRun(&currentState->preferences) == true || matchedAll == false){
    Serial.println("First Run, Using defaults");
    matchedAll = false;
  }else{
    Serial.println("Checking Saved States...");
    std::string loadedSwitchState = loadSwitchState(&currentState->preferences);
    if(loadedSwitchState.length() == SWITCH_STATE_LENGTH){
      Serial.println("Found Saved Switch State");
      setStateFromString(&currentState->switches, loadedSwitchState);
    }else{
      Serial.println("NO Found Saved Switch State");
      matchedAll = false;
    }

    std::string loadedAnimationState = loadAnimationState(&currentState->preferences);
    if(loadedAnimationState.length() == ANIMATION_STATE_LENGTH && matchedAll){
      Serial.println("Found Saved Animation State");
      setStateFromString(&currentState->animation, loadedAnimationState);
    }else{
      Serial.println("NO Found Saved Animation State");
      matchedAll = false;
    }

    for(int i=0; i<MAX_CHANNELS; i++){
      std::string loadedChannelState = loadChannelDetailState(&currentState->preferences, i);
      if(loadedChannelState.length() == CHANNEL_STATE_LENGTH && matchedAll){
        Serial.print("Channel ");
        Serial.print(i);
        Serial.println(" Detail State Found");
        setStateFromString(currentState->channels, i, loadedChannelState);
      }else{
        Serial.print("NO Channel ");
        Serial.print(i);
        Serial.print(" Detail State Found. Length: ");
        Serial.println(loadedChannelState.length());
        matchedAll = false;
        break;
      }
      uint8_t locationArray[MAX_LEDS*3];
      loadChannelLocationState(&currentState->preferences, i, locationArray);
      
      if(matchedAll){//Add check to make sure location data was read
        setChannelLocations(&currentState->location, i, state.channels[i].numLEDs, locationArray);
      }else{
        Serial.println("Channel Location State String Size Mismatch");
        matchedAll = false;
        break;
      }
    }
  }
  
  if(matchedAll == false){
    Serial.println("Loading Default Storage Values");
    updateSwitchState(&currentState->switches);
    setInitialState(&state.animation);
    setInitialState(currentState->channels);
    setInitialState(&currentState->location);
    saveState(currentState); //save so that it gets reloaded next time
  }else{
    Serial.println("Loaded Save State Successfully");
  }
}

bool delayAndPollForUpdate(SwitchState* switches, int wifiConnections, int delay){
  unsigned long time = millis();
  while(millis() < time + delay){
    if(wifiConnections > 0){
      
    }else if(updateSwitchState(switches) == true){
      return true;
    }
  }
  return false;
}

void restart(bool shouldRestart){
  if(shouldRestart){
    Serial.println("Restart required to appply update to channels");
    ESP.restart();
  }
}

void setup() {
  Serial.begin(115200);
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS failed to begin;");
  }
  Serial.println("Setting Up...");
  setChannelPins();
  initializeStorage(&state.preferences);
  loadSavedIfFound(&state);
  describeState(&state);
  initializeAP(&state);
  createSignals(&state, &signalTaskHandle);
  Serial.println("Setup Complete");
  Serial.println("Starting Animations");
}

void loop() {
  //Serial.println("Looping");
  if(getClientCount() == 0){
    unPauseSignals(&signalTaskHandle);
    unsigned long frameStartTime = millis();
    updateAnimation(&state.animation, state.channels, &state.switches, &state.location);
    unsigned long currentTime = millis();
    //Serial.print("Frame Time: ");
    uint16_t frameTime = currentTime - frameStartTime;
    //Serial.println(frameTime);

    uint16_t adjustedStepDelay = getStepDelay(&state.animation, frameTime);
    if(delayAndPollForUpdate(&state.switches,clientsConnected, adjustedStepDelay) == true){
      Serial.println("New BLE value found, breaking out of delay early to update.");
    }
    /*Serial.print("FrameToFrame Time: ");
    currentTime = millis();
    state.animation.frameToFrameTime = currentTime - state.animation.lastFrameTime;
    state.animation.lastFrameTime = currentTime;
    Serial.println(state.animation.frameToFrameTime);*/
    /*Serial.print("Free Heap: ");
    Serial.println(xPortGetFreeHeapSize());*/
    runGarbageCollector(&state.location);
  }else{
    pauseSignals(&signalTaskHandle);
  }
}

