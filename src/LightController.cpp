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

void sendSwitches(State* currentState){
  uint8_t buffer[SWITCH_STATE_LENGTH];
  getStateAsBytes(&currentState->switches, buffer);
  sendSwitchToQueue(buffer);
}

void readSwitches(State* currentState){
  uint8_t buffer[SWITCH_STATE_LENGTH];
  getSwitchFromQueue(buffer);
  setStateFromBytes(&currentState->switches, buffer);
  //describeState(&currentState->switches);
}

void sendAnimation(State* currentState){
  uint8_t buffer[ANIMATION_STATE_LENGTH];
  getStateAsBytes(&currentState->animation, buffer);
  sendAnimationToQueue(buffer);
}

void readAnimation(State* currentState){
  uint8_t buffer[ANIMATION_STATE_LENGTH];
  getAnimationFromQueue(buffer);
  setStateFromBytes(&currentState->animation, buffer);
  describeState(&currentState->animation);
  animationUpdated(&currentState->animation, currentState->channels);
  saveAnimationState(&currentState->preferences, buffer, ANIMATION_STATE_LENGTH);
}

void sendChannel(State* currentState, int index){
  uint8_t buffer[CHANNEL_STATE_LENGTH];
  getStateAsBytes(currentState->channels, index, buffer);
  sendChannelToQueue(buffer);
}

void readChannel(State* currentState, int index){
  uint8_t buffer[CHANNEL_STATE_LENGTH];
  getChannelFromQueue(buffer);
  setStateFromBytes(currentState->channels, index, buffer);
  describeState(currentState->channels, index);
  saveChannelDetailState(&currentState->preferences, index, buffer, CHANNEL_STATE_LENGTH);
}

void sendChannelPos(State* currentState, int index){
  uint8_t buffer[MAX_LEDS*3];
  getChannelLocations(&currentState->location, index, buffer);
  sendChannelPosToQueue(buffer);
}

void readChannelPos(State* currentState, int index){
  uint8_t buffer[MAX_LEDS*3];
  getChannelPosFromQueue(buffer);
  setChannelLocations(&currentState->location, index, currentState->channels[index].numLEDs, buffer);
  saveChannelLocationState(&currentState->preferences, index, buffer, MAX_LEDS*3);
}

bool handleMessages(State* currentState){
  int command = getCommandFromQueue(0);
  std::string state;
  switch(command){
    case NO_COMMAND:
      return false;
    case DUMMY_COMMAND:
      return true;
    case SAVE_COMMAND:
      saveState(currentState);
      return true;
    case RESET_COMMAND:
      removeAllEntries(&currentState->location);
      setInitialState(&currentState->location);
      saveState(currentState);
      return true;
    case SEND_SWITCHES:
      sendSwitches(currentState);
      return true;
    case READ_SWITCHES:
      readSwitches(currentState);
      return true;
    case SEND_ANIMATION:
      sendAnimation(currentState);
      return true;
    case READ_ANIMATION:
      readAnimation(currentState);
      return true;
    case SEND_CHANNEL_START...(READ_CHANNEL_START-1):
      sendChannel(currentState, (command-SEND_CHANNEL_START));
      return true;
    case READ_CHANNEL_START...(SEND_CHANNELPOS_START-1):
      readChannel(currentState, (command-READ_CHANNEL_START));
      return true;
    case SEND_CHANNELPOS_START...(READ_CHANNELPOS_START-1):
      sendChannelPos(currentState, (command-SEND_CHANNELPOS_START));
      return true;
    case READ_CHANNELPOS_START...(END_COMMAND-1):
      readChannelPos(currentState, (command-READ_CHANNELPOS_START));
      return true;
    default:
      return false;
  }
}

bool handleSignals(State* currentState){
  int command = getSignalCommandFromQueue(0);
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

bool delayAndPollForUpdate(State* currentState, int wifiConnections, int delay){
  unsigned long time = millis();
  int webDelay = 0;
  int realDelay = delay;
  if(realDelay < 10){ //TEMP REMOVE WHEN DONE
    realDelay = 10;
  }

  while(millis() < time + realDelay + webDelay){
    if(updateSwitchState(&currentState->switches) == true){
      return true;
    }
    if(webDelay == 0 && handleSignals(currentState)){
      //stuff here if we got a signal command
    }
    if(handleMessages(currentState)){
      webDelay = 1000;
      time = millis();
    }
  }
  return false;
}
/*
void testFunc(State * currentState){
  Serial.println("***************START OF TESTING SHIT***********************");
  ChannelState channel;
  uint8_t loadBuffer[CHANNEL_STATE_LENGTH];
  loadChannelDetailState(&currentState->preferences, 0, loadBuffer, CHANNEL_STATE_LENGTH);

  Serial.println("Loaded Buffer: ");
  for(int i=0; i<CHANNEL_STATE_LENGTH;i++){
    Serial.print(loadBuffer[i]);
  }
  Serial.println("END BUFFER");

  setStateFromBytes(&channel, loadBuffer);
  describeStateSolo(&channel);

  uint8_t readBuffer[CHANNEL_STATE_LENGTH];
  getStateAsBytes(&channel, readBuffer);

  Serial.println("Read Buffer: ");
  for(int i=0; i<CHANNEL_STATE_LENGTH;i++){
    Serial.print(loadBuffer[i]);
  }
  Serial.println("END BUFFER");



  Serial.println("**************************************END OF TESTING SHIT**************************************");
}
*/
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
    Serial.println("Checking Saved States...");
    uint8_t switchBuffer[SWITCH_STATE_LENGTH];
    if(loadSwitchState(&currentState->preferences, switchBuffer, SWITCH_STATE_LENGTH)){
      Serial.println("Found Saved Switch State");
      setStateFromBytes(&currentState->switches, switchBuffer);
      //describeState(&currentState->switches);
    }else{
      Serial.println("NO Found Saved Switch State");
      matchedAll = false;
    }

    uint8_t animationBuffer[ANIMATION_STATE_LENGTH];
    if(loadAnimationState(&currentState->preferences, animationBuffer, ANIMATION_STATE_LENGTH) && matchedAll){
      Serial.println("Found Saved Animation State");
      setStateFromBytes(&currentState->animation, animationBuffer);
      //describeState(&currentState->animation);
    }else{
      Serial.println("NO Found Saved Animation State");
      matchedAll = false;
    }

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
  //Serial.println("Looping");
  //if(getClientCount() == 0 && getBTClientCount(&state.bluetooth) == 0){
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
  /*}else{
    uint16_t adjustedStepDelay = getStepDelay(&state.animation, 0);
    if(delayAndPollForUpdate(&state,clientsConnected, adjustedStepDelay) == true){
      Serial.println("New BLE value found, breaking out of delay early to update.");
    }
  }*/
}

