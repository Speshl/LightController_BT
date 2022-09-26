#include <Arduino.h>
#include <channels.h>
#include <bluetooth.h>
#include <animations.h>
#include <location.h>
#include <switches.h>
#include <storage.h>
#include <signals.h>
#include <web.h>

#define SAVED_STATE_VALID_INDICATOR 127
#define SHOW_CORE 0
#define SIGNAL_CORE 0
#define TOGGLE_WIFI_OR_BT 0 //1 for bluetooth

static State state;

static TaskHandle_t signalTaskHandle = 0;

void restart(bool shouldRestart){
  if(shouldRestart){
    Serial.println("Restart required to appply update to channels");
    ESP.restart();
  }
}


void describeState(State* currentState){  
  Serial.println("Listing LightController Detailed State");
  describeState(&(currentState->switches));
  describeState(&(currentState->animation));
  describeState(currentState->channels);
  //describeState(&(currentState->location));
}

void saveState(State* currentState){
  Serial.println("Saving State");
  uint8_t switchBuffer[SWITCH_STATE_LENGTH];
  getStateAsBytes(&currentState->switches, switchBuffer);
  saveSwitchState(&currentState->preferences, switchBuffer, SWITCH_STATE_LENGTH);

  uint8_t animationBuffer[ANIMATION_STATE_LENGTH];
  getStateAsBytes(&currentState->animation, animationBuffer);
  saveAnimationState(&currentState->preferences, animationBuffer, ANIMATION_STATE_LENGTH);

  for(int i=0; i<MAX_CHANNELS; i++){
    uint8_t channelBuffer[CHANNEL_STATE_LENGTH];
    getStateAsBytes(currentState->channels,i, channelBuffer);
    saveChannelDetailState(&currentState->preferences, i, channelBuffer, CHANNEL_STATE_LENGTH);

    uint8_t locationArray[MAX_LEDS*3];
    getChannelLocations(&currentState->location, i, locationArray);
    saveChannelLocationState(&currentState->preferences, i, locationArray, MAX_LEDS*3);
  }
  Serial.println("State Saved");
}

bool handleSignals(State* currentState, int command){
  std::string state;
  switch(command){
    case RAMP_UP:
      signalRampUp(currentState->channels, &currentState->switches);
      return true;
    case RAMP_DOWN:
      signalRampDown(currentState->channels, &currentState->switches);
      return true;
    default:
      return false;
  }
}

void saveIfRequired(State* currentState){
  if(currentState->switches.updated){
    currentState->switches.updated = false;
    uint8_t buffer[SWITCH_STATE_LENGTH];
    getStateAsBytes(&currentState->switches, buffer);
    saveSwitchState(&currentState->preferences, buffer, SWITCH_STATE_LENGTH);
  }

  if(currentState->animation.updated){
    currentState->animation.updated = false;
    animationUpdated(&currentState->animation, currentState->channels);
    uint8_t buffer[ANIMATION_STATE_LENGTH];
    getStateAsBytes(&currentState->animation, buffer);
    saveAnimationState(&currentState->preferences, buffer, ANIMATION_STATE_LENGTH);
  }

  for(int i=0; i<MAX_CHANNELS; i++){
    if(currentState->channels[i].updated){
      currentState->channels[i].updated = false;
      uint8_t channelBuffer[CHANNEL_STATE_LENGTH];
      getStateAsBytes(currentState->channels,i, channelBuffer);
      saveChannelDetailState(&currentState->preferences, i, channelBuffer, CHANNEL_STATE_LENGTH);
    }
    if(currentState->location.updated){ //Save for all channels if found
      uint8_t locationArray[MAX_LEDS*3];
      getChannelLocations(&currentState->location, i, locationArray);
      saveChannelLocationState(&currentState->preferences, i, locationArray, MAX_LEDS*3);
    }
  }
  currentState->location.updated=false;//reset location updated here because it is shared for all channels
}

bool delayAndPollForUpdate(State* currentState, int wifiConnections, int delay){
  bool returnValue = false;
  int signalCommand = 0;
  unsigned long time = millis();
  int webDelay = 0;
  SwitchState lastSwitchState = currentState->switches;//Save copy of switch state to compare to without using global while request threads run

  getControllerStatusFromQueue(); //Remove previous active status
  sendControllerStatusToQueue(INACTIVE); //Set Inactive status while waiting
  while(millis() < time + delay + webDelay || peekRequestStatusFromQueue() == ACTIVE){
    if(millis() > time + delay + webDelay && peekRequestStatusFromQueue() == ACTIVE){
      webDelay = 500; //Delay an extra half second
      time = millis(); //Reset delay start time to now
    }

    int tempSignalCommand = getSignalCommandFromQueue();
    if(tempSignalCommand != NO_SIGNAL){
      signalCommand = tempSignalCommand;
    }

    /*if(updateSwitchState(&lastSwitchState) == true){
      //returnValue = true;
      //break;
    }*/
    vTaskDelay(5);
  }
  getControllerStatusFromQueue();//Remove previous Inactive status
  sendControllerStatusToQueue(ACTIVE);//Set Active status until we call this function again to wait

  saveIfRequired(currentState);

  handleSignals(currentState, signalCommand);

  return returnValue;
}

void setupBluetooth(State* currentState){
  Serial.println("Bluetooth: Starting Up");
  initializeBluetooth(&currentState->bluetooth);

  uint8_t switchBuffer[SWITCH_STATE_LENGTH];
  getStateAsBytes(&currentState->switches, switchBuffer);
  setSwitchBLECharacteristic(&currentState->bluetooth, switchBuffer, SWITCH_STATE_LENGTH);

  uint8_t animationBuffer[ANIMATION_STATE_LENGTH];
  getStateAsBytes(&currentState->animation, animationBuffer);
  setAnimationBLECharacteristic(&currentState->bluetooth, animationBuffer, ANIMATION_STATE_LENGTH);
  for(int i=0; i<MAX_CHANNELS; i++){
    uint8_t channelBuffer[CHANNEL_STATE_LENGTH];
    getStateAsBytes(currentState->channels, i, channelBuffer);
    setChannelDetailsBLECharacteristic(&currentState->bluetooth, i, channelBuffer, CHANNEL_STATE_LENGTH);
    
    uint8_t locationArray[MAX_LEDS*3];
    getChannelLocations(&currentState->location, i, locationArray);
    setChannelLocationsBLECharacteristic(&currentState->bluetooth, i, locationArray, MAX_LEDS*3);
  }
  Serial.println("Bluetooth: Started");
}

void loadSavedIfFound(State* currentState){
  bool matchedAll = true; //TEMP SHOULD BE true

  if(isFirstRun(&currentState->preferences) == true || matchedAll == false){
    Serial.println("First Run, Using defaults");
    matchedAll = false;
  }else{
    Serial.println("Checking Saved Switch State...");
    uint8_t switchBuffer[SWITCH_STATE_LENGTH];
    if(loadSwitchState(&currentState->preferences, switchBuffer, SWITCH_STATE_LENGTH)){
      Serial.println("Found Saved Switch State");
      setStateFromBytes(&currentState->switches, switchBuffer);
      //describeState(&currentState->switches);
    }else{
      Serial.println("NO Found Saved Switch State");
      matchedAll = false;
    }

    Serial.println("Checking Saved Animation State...");
    uint8_t animationBuffer[ANIMATION_STATE_LENGTH];
    if(loadAnimationState(&currentState->preferences, animationBuffer, ANIMATION_STATE_LENGTH) && matchedAll){
      Serial.println("Found Saved Animation State");
      setStateFromBytes(&currentState->animation, animationBuffer);
      //describeState(&currentState->animation);
    }else{
      Serial.println("NO Found Saved Animation State");
      matchedAll = false;
    }

    Serial.println("Checking Saved Channel/Location States...");
    for(int i=0; i<MAX_CHANNELS; i++){
      uint8_t channelBuffer[CHANNEL_STATE_LENGTH];
      if(loadChannelDetailState(&currentState->preferences, i, channelBuffer, CHANNEL_STATE_LENGTH) && matchedAll){
        Serial.print("Channel ");
        Serial.print(i);
        Serial.println(" Detail State Found");
        setStateFromBytes(currentState->channels, i, channelBuffer);
        //describeState(currentState->channels, i);
      }else{
        Serial.print("NO Channel ");
        Serial.print(i);
        Serial.println(" Detail State Found");
        matchedAll = false;
        break;
      }
      uint8_t locationArray[MAX_LEDS*3];
      if(loadChannelLocationState(&currentState->preferences, i, locationArray, MAX_LEDS*3) && matchedAll){
        setChannelLocations(&currentState->location, i, state.channels[i].numLEDs, locationArray);
        //printChannelLocations(&currentState->location, i);
      }else{
        Serial.print("NO Channel ");
        Serial.print(i);
        Serial.println(" Location State Found");
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

void setup() {
  Serial.begin(115200);
  if(!SPIFFS.begin(true)){
    Serial.println("SPIFFS failed to begin;");
  }
  Serial.println("Setting Up...");

  setChannelPins();
  initializeSwitches(&state.switches);
  initializeMessageQueue();
  initializeStorage(&state.preferences);
  loadSavedIfFound(&state);
  describeState(&state);
  if(TOGGLE_WIFI_OR_BT == 0){
    initializeAP(&state);
  }else{
    setupBluetooth(&state);
  }
  createSignals(&signalTaskHandle);
  Serial.println("Setup Complete");
  Serial.println("Starting Animations");
}

void loop() {
  unsigned long frameStartTime = millis();
  updateAnimation(&state.animation, state.channels, &state.switches, &state.location);
  unsigned long currentTime = millis();
  //Serial.print("Frame Time: ");
  uint16_t frameTime = currentTime - frameStartTime;
  //Serial.println(frameTime);

  uint16_t adjustedStepDelay = getStepDelay(&state.animation, frameTime);
  if(delayAndPollForUpdate(&state,clientsConnected, adjustedStepDelay) == true){
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
}

