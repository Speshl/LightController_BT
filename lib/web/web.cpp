#include "web.h"

int getClientCount(){
    return clientsConnected;
}

void OnWiFiEvent(WiFiEvent_t event){
    switch(event){
        case SYSTEM_EVENT_STA_CONNECTED:
            Serial.println("ESP32 Connected to WiFi Network");
            break;
        case SYSTEM_EVENT_AP_START:
            Serial.println("ESP32 Soft AP Started");
            break;
        case SYSTEM_EVENT_AP_STACONNECTED:
            Serial.println("Station connected to ESP32 Soft AP");
            clientsConnected++;
            break;
        case SYSTEM_EVENT_AP_STADISCONNECTED:
            Serial.println("Station disconnected from ESP32 Soft AP");
            clientsConnected--;
            break;
        default: break;
    }
}

std::string getChannelPage(int index){
    if(index >= 0 && index <MAX_CHANNELS){
        char buffer[10];
        String stringNumber = itoa(index,buffer,10);
        std::string returnValue = "/channel";
        returnValue += stringNumber.c_str();
        returnValue += ".htm";
        return returnValue;
    }
    return "./channel0.htm";
}

class CaptiveRequestHandler : public AsyncWebHandler {
public:
  CaptiveRequestHandler() {}
  virtual ~CaptiveRequestHandler() {}

  bool canHandle(AsyncWebServerRequest *request){
    return true;
  }

  void handleRequest(AsyncWebServerRequest *request) {
    request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  }
};

void onUpload(AsyncWebServerRequest *request, String filename, size_t index, uint8_t *data, size_t len, bool final){
  if(!index){
    memset(uploadBuffer,0, sizeof(uploadBuffer));
  }

  if(len){
    for(int i=0; i<len; i++){
      uploadBuffer[index+i] = data[i];
    }
  }
  
  if(final){
    DynamicJsonDocument dynamicDoc(1024*15);
    DeserializationError error = deserializeJson(dynamicDoc, uploadBuffer, 1024*15);
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

    JsonObject object = dynamicDoc.as<JsonObject>();
    
    SwitchState switches;
    uint8_t switchBuffer[SWITCH_MESSAGE_LENGTH];
    setStateFromJson(&switches, object);
    getStateAsBytes(&switches, switchBuffer);
    sendSwitchToQueue(switchBuffer);
    sendCommandToQueue(READ_SWITCHES);

    AnimationState animation;
    uint8_t animationBuffer[ANIMATION_MESSAGE_LENGTH];
    setStateFromJson(&animation, object);
    getStateAsBytes(&animation, animationBuffer);
    sendAnimationToQueue(animationBuffer);
    sendCommandToQueue(READ_ANIMATION);

    for(int i=0; i<MAX_CHANNELS; i++){ //PUT BACK TO MAX CHANNELS
      //vTaskDelay(50);
      ChannelState channel;
      uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
      setStateFromJson(&channel, object, i);
      getStateAsBytes(&channel, channelBuffer);
      sendChannelToQueue(channelBuffer);
      sendCommandToQueue(READ_CHANNEL_START+i);

      //vTaskDelay(50);
      char indexBuffer[5];
      itoa(i, indexBuffer,10);
      std::string indexString(indexBuffer);
      uint8_t locationBuffer[CHANNELPOS_MESSAGE_LENGTH];
      std::string locationString = object["location"+indexString];
      char readBuffer[10];
      int bufferPos = 0;
      int pos = 0;
      for(int j=0; j<locationString.length(); j++){
        if(locationString.at(j) == ',' || locationString.at(j) == '\0'){
          locationBuffer[bufferPos] = atoi(readBuffer);
          memset(readBuffer, 0, sizeof(readBuffer));
          pos = 0;
          bufferPos++;
          if(bufferPos >= CHANNELPOS_MESSAGE_LENGTH){
            break;
          }
        }else{
          if(pos >= sizeof(readBuffer)){
            break;
          }
          readBuffer[pos] = locationString.at(j);
          pos++;
        }
      }

      sendChannelPosToQueue(locationBuffer);
      sendCommandToQueue(READ_CHANNELPOS_START+i);
    }
  }
}

void initializeAP(State* state){
  Serial.println("Starting AP...");
  WiFi.onEvent(OnWiFiEvent);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("SOFT AP IP: ");
  Serial.println(IP.toString());



  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
      sendCommandToQueue(DUMMY_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  });
  server.on("/switches", HTTP_GET, [](AsyncWebServerRequest * request){
      sendCommandToQueue(DUMMY_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/switches.htm", String(), false, switchesProcessor);
  });
  server.on("/animation", HTTP_GET, [](AsyncWebServerRequest * request){
      sendCommandToQueue(DUMMY_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  });
  server.on("/view", HTTP_GET, [](AsyncWebServerRequest * request){
      sendCommandToQueue(DUMMY_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/view.htm", String(), false, viewProcessor);
  });
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest * request){
      sendCommandToQueue(SAVE_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/utils.htm", String(), false, utilsProcessor);
  });
  server.on("/defaultLocations", HTTP_POST, [](AsyncWebServerRequest * request){
      sendCommandToQueue(RESET_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/utils.htm", String(), false, utilsProcessor);
  });
    server.on("/utils", HTTP_GET, [](AsyncWebServerRequest * request){
      sendCommandToQueue(DUMMY_COMMAND);
      vTaskDelay(50);
      request->send(SPIFFS, "/utils.htm", String(), false, utilsProcessor);
  });
  server.on("/toggle_on", HTTP_GET, [](AsyncWebServerRequest *request){
    sendCommandToQueue(DUMMY_COMMAND);
    vTaskDelay(50);
    request->send(SPIFFS, "/toggle_on.png", "image/png");
  });
  server.on("/toggle_off", HTTP_GET, [](AsyncWebServerRequest *request){
    sendCommandToQueue(DUMMY_COMMAND);
    vTaskDelay(50);
    request->send(SPIFFS, "/toggle_off.png", "image/png");
  });
  server.on("/import", HTTP_POST, [](AsyncWebServerRequest *request){
    sendCommandToQueue(DUMMY_COMMAND);
    vTaskDelay(50);
    request->send(SPIFFS, "/utils.htm", String(), false, utilsProcessor);
  }, onUpload);
  server.on("/export", HTTP_GET, [](AsyncWebServerRequest * request){
      sendCommandToQueue(DUMMY_COMMAND);

      AsyncJsonResponse * response = new AsyncJsonResponse(false, 1024 * 15);
      response->addHeader("Server","ESP Async Web Server");
      JsonObject root = response->getRoot();

      sendCommandToQueue(SEND_ANIMATION);
      uint8_t animationBuffer[ANIMATION_MESSAGE_LENGTH];
      AnimationState animation;
      getAnimationFromQueue(animationBuffer);
      setStateFromBytes(&animation,animationBuffer);
      getStateAsJson(&animation, root);

      sendCommandToQueue(SEND_SWITCHES);
      uint8_t switchBuffer[SWITCH_MESSAGE_LENGTH];
      SwitchState switches;
      getSwitchFromQueue(switchBuffer);
      setStateFromBytes(&switches, switchBuffer);
      getStateAsJson(&switches, root);

      for(int i=0; i<MAX_CHANNELS; i++){
        sendCommandToQueue(SEND_CHANNEL_START + i);
        uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
        ChannelState channel;
        getChannelFromQueue(channelBuffer);
        setStateFromBytes(&channel, channelBuffer);
        getStateAsJson(&channel, root, i);
      }

      for(int i=0; i<MAX_CHANNELS; i++){
        char indexBuffer[5];
        itoa(i, indexBuffer,10);
        std::string indexString(indexBuffer);
        sendCommandToQueue(SEND_CHANNELPOS_START + i);
        uint8_t locationBuffer[CHANNELPOS_MESSAGE_LENGTH];
        getChannelPosFromQueue(locationBuffer);
        std::string locationString;
        for(int j=0; j< CHANNELPOS_MESSAGE_LENGTH; j++){
          char charBuffer[10];
          locationString += itoa(locationBuffer[j], charBuffer, 10);
          if(j < CHANNELPOS_MESSAGE_LENGTH-1){
            locationString += ",";
          }
        }
        root["location"+indexString] = locationString;
      }
      response->setLength();
      request->send(response);
  });
  server.on("/switches", HTTP_POST, [](AsyncWebServerRequest * request){
    Serial.println("In Switch Post");
    sendCommandToQueue(SEND_SWITCHES);
    SwitchState newSwitchState;
    uint8_t buffer[SWITCH_STATE_LENGTH];
    getSwitchFromQueue(buffer);
    setStateFromBytes(&newSwitchState, buffer);

    Serial.println("Reading Params");
    if(request->hasParam("LEFT_TURN_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("LEFT_TURN_SWITCH", true);
      if(p->value().compareTo("OFF") == 0){
        newSwitchState.leftTurn = true;
      }else{
        newSwitchState.leftTurn = false;
      }
    }

    if(request->hasParam("RIGHT_TURN_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("RIGHT_TURN_SWITCH", true);
      if(p->value().compareTo("OFF") == 0){
        newSwitchState.rightTurn = true;
      }else{
        newSwitchState.rightTurn = false;
      }
    }

    if(request->hasParam("REVERSE_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("REVERSE_SWITCH", true);
      if(p->value().compareTo("OFF") == 0){
        newSwitchState.reverse = true;
      }else{
        newSwitchState.reverse = false;
      }
    }

    if(request->hasParam("BRAKE_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("BRAKE_SWITCH", true);
      if(p->value().compareTo("OFF") == 0){
        newSwitchState.brake = true;
      }else{
        newSwitchState.brake = false;
      }
    }

    if(request->hasParam("INTERIOR_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("INTERIOR_SWITCH", true);
      if(p->value().compareTo("OFF") == 0){
        newSwitchState.interior = true;
      }else{
        newSwitchState.interior = false;
      }
    }

    if(request->hasParam("UI_OVERRIDE_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("UI_OVERRIDE_SWITCH", true);
      if(p->value().compareTo("OFF") == 0){
        newSwitchState.uiOverride = true;
      }else{
        newSwitchState.uiOverride = false;
      }
    }
    getStateAsBytes(&newSwitchState, buffer);
    sendSwitchToQueue(buffer);
    sendCommandToQueue(READ_SWITCHES);
    vTaskDelay(50);
    request->send(SPIFFS, "/switches.htm", String(), false, switchesProcessor);
  });

  server.on("/animation", HTTP_POST, [](AsyncWebServerRequest * request){
    sendCommandToQueue(SEND_ANIMATION);
    AnimationState newAnimation;
    uint8_t buffer[ANIMATION_STATE_LENGTH];
    getAnimationFromQueue(buffer);
    setStateFromBytes(&newAnimation, buffer);

    if(request->hasParam("animation",true)){
      AsyncWebParameter* p = request->getParam("animation", true);
      newAnimation.animation = atoi(p->value().c_str());
    }

    if(request->hasParam("fps",true)){
      AsyncWebParameter* p = request->getParam("fps", true);
      newAnimation.fps = atoi(p->value().c_str());
    }

    if(request->hasParam("brightness",true)){
      AsyncWebParameter* p = request->getParam("brightness", true);
      newAnimation.brightness = atoi(p->value().c_str());
    }

    if(request->hasParam("blending",true)){
      newAnimation.blending = LINEARBLEND;
    }else{
      newAnimation.blending = NOBLEND;
    }

    if(request->hasParam("stepSize",true)){
      AsyncWebParameter* p = request->getParam("stepSize", true);
      newAnimation.stepSize = atoi(p->value().c_str());
    }

    if(request->hasParam("colorPalette",true)){
      AsyncWebParameter* p = request->getParam("colorPalette", true);
      newAnimation.palettePreset = atoi(p->value().c_str());
    }

    for(int i=0; i<MAX_COLORS; i++){
      char buffer[10];
      String stringNumber = itoa(i,buffer,10);
      String prefix = "color"+stringNumber;
      if(request->hasParam(prefix+"Pos", true)){
        AsyncWebParameter* p = request->getParam(prefix+"Pos", true);
        setColorPos(&newAnimation, i, p->value().c_str());
      }
      if(request->hasParam(prefix, true)){
        AsyncWebParameter* p = request->getParam(prefix, true);
        setColorHex(&newAnimation, i, p->value().c_str());
      }
    }
    getStateAsBytes(&newAnimation, buffer);
    sendAnimationToQueue(buffer);
    sendCommandToQueue(READ_ANIMATION);
    Serial.println("Send Animation Post Response");
    vTaskDelay(50); //Delay to we don't read animation from queue to fast in next calls
    request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  });

  server.on("/channelsPos", HTTP_POST, [](AsyncWebServerRequest * request){
    sendCommandToQueue(DUMMY_COMMAND);
    if(request->hasParam("INITIAL",true) == false){ //Read parameters if this isn't the initial load of the page
      if(request->hasParam("CHANNEL_INDEX",true)){
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());
        uint8_t posBuffer[CHANNELPOS_MESSAGE_LENGTH];
        for(int i=0;i<100; i++){ //HARD CODED MAX_LEDS so as not to include reference
          char buffer[10];
          std::string posString = itoa(i, buffer, 10);
          std::string locRow = "LOC_";
          std::string locCol = "LOC_";
          std::string locHeight = "LOC_";

          locRow += posString.c_str();
          locCol += posString.c_str();
          locHeight += posString.c_str();

          locRow += "_ROW";
          locCol += "_COL";
          locHeight += "_HEIGHT";

          if(request->hasParam(locRow.c_str(), true) == false ||
            request->hasParam(locCol.c_str(), true) == false ||
            request->hasParam(locHeight.c_str(), true) == false){
            break;//don't have data for anymore locations
          }

          AsyncWebParameter* rowParam = request->getParam(locRow.c_str(), true);
          AsyncWebParameter* colParam = request->getParam(locCol.c_str(), true);
          AsyncWebParameter* heightParam = request->getParam(locHeight.c_str(), true);

          posBuffer[(3*i)+0] = atoi(rowParam->value().c_str());
          posBuffer[(3*i)+1] = atoi(colParam->value().c_str());
          posBuffer[(3*i)+2] = atoi(heightParam->value().c_str());
        }
        sendChannelPosToQueue(posBuffer);
        sendCommandToQueue(READ_CHANNELPOS_START+channelIndex);
        vTaskDelay(50);
      }else{
        request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
      }
    }
    //Build page to display
    if(request->hasParam("CHANNEL_INDEX",true)){
      AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
      int channelIndex = atoi(p->value().c_str());
      request->send(SPIFFS, "/channelLoc.htm", String(), false, channelLocProcessor(channelIndex));
    }else{
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
    }
    
  });

  server.on("/channels", HTTP_POST, [](AsyncWebServerRequest * request){
    sendCommandToQueue(DUMMY_COMMAND);
    if(request->hasParam("INITIAL",true)){
      if(request->hasParam("CHANNEL_INDEX",true)){
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());
        request->send(SPIFFS, getChannelPage(channelIndex).c_str(), String(), false, channelProcessor);
      }else{
        request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
      }
    }else if(request->hasParam("CHANNEL_INDEX",true)){
      AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
      int channelIndex = atoi(p->value().c_str());
      String prefix = "CHANNEL_"+p->value();
      prefix += "_";

      sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
      ChannelState newChannel;
      uint8_t buffer[CHANNEL_MESSAGE_LENGTH];
      getChannelFromQueue(buffer);
      setStateFromBytes(&newChannel, buffer);

      if(request->hasParam(prefix+"enabled",true)){
        newChannel.enabled = true;
      }else{
        newChannel.enabled = false;
      }

      if(request->hasParam(prefix+"interior",true)){
        newChannel.interior = true;
      }else{
        newChannel.interior = false;
      }

      if(request->hasParam(prefix+"directionFlipped",true)){
        newChannel.directionFlipped = true;
      }else{
        newChannel.directionFlipped = false;
      }

      if(request->hasParam(prefix+"leftTurn",true)){
        newChannel.leftTurn = true;
      }else{
        newChannel.leftTurn = false;
      }

      if(request->hasParam(prefix+"rightTurn",true)){
        newChannel.rightTurn = true;
      }else{
        newChannel.rightTurn = false;
      }

      if(request->hasParam(prefix+"brake",true)){
        newChannel.brake = true;
      }else{
        newChannel.brake = false;
      }

      if(request->hasParam(prefix+"reverse",true)){
        newChannel.reverse = true;
      }else{
        newChannel.reverse = false;
      }

      if(request->hasParam(prefix+"type",true)){
        AsyncWebParameter* p = request->getParam(prefix+"type", true);
        int newType = atoi(p->value().c_str());
        if(newType != newChannel.type){
          newChannel.type = newType;
          newChannel.restartRequired = true;
        }
      }

      if(request->hasParam(prefix+"order",true)){
        AsyncWebParameter* p = request->getParam(prefix+"order", true);
        int newOrder = atoi(p->value().c_str());
        if(newOrder != newChannel.order){
          newChannel.order = newOrder;
          //newChannel.restartRequired = true;
        }
      }

      if(request->hasParam(prefix+"numLEDs",true)){
        AsyncWebParameter* p = request->getParam(prefix+"numLEDs", true);
        newChannel.numLEDs = atoi(p->value().c_str());
      }
      getStateAsBytes(&newChannel, buffer);
      sendChannelToQueue(buffer);
      sendCommandToQueue(READ_CHANNEL_START+channelIndex);
      vTaskDelay(50);
      request->send(SPIFFS, getChannelPage(channelIndex).c_str(), String(), false, channelProcessor);
    }else{
      Serial.println("Channel Index not found, showing index");
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
    }
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
  server.begin();
}

std::string getChecked(bool value){
  if(value){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getSelected(bool value){
  if(value){
      return "selected";
  }else{
      return "";
  }
}

String utilsProcessor(const String& var){
  return String();
}

String viewProcessor(const String& var){
  if(var == "NUM_ROWS"){
    return String(LOCATION_GRID_SIZE+1);
  }
  if(var == "NUM_COLS"){
    return String((LOCATION_GRID_SIZE*3)+10);
  }
  if(var == "LOCATION_MAP"){
    return "CUBED";//getLocationGrid(&globalState->location).c_str();
  }
  return String();
}

String animationProcessor(const String& var){
  sendCommandToQueue(SEND_ANIMATION);
  AnimationState animation;
  uint8_t buffer[ANIMATION_MESSAGE_LENGTH];
  getAnimationFromQueue(buffer);
  setStateFromBytes(&animation,buffer);

  if(var == "SOLID_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 0).c_str();
  }
  if(var == "SWEEPFRONT_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 1).c_str();
  }
  if(var == "SWEEPREAR_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 2).c_str();
  }
  if(var == "SWEEPRIGHT_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 3).c_str();
  }
  if(var == "SWEEPLEFT_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 4).c_str();
  }
  if(var == "SWEEPUP_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 5).c_str();
  }
  if(var == "SWEEPDOWN_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 6).c_str();
  }
  if(var == "SPINCLOCK_ANIMATION_SELECTED"){
    return getSelected(animation.animation == 7).c_str();
  }
  if(var == "REPLACE_ANIMATION_FPS"){
    return getFPS(&animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_BRIGHTNESS"){
    return getBrightness(&animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_BLEND"){
    return getBlending(&animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_STEPSIZE"){
    return getStepSize(&animation).c_str();
  }
  if(var == "CUSTOM_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 0).c_str();
  }
  if(var == "RAINBOW_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 1).c_str();
  }
  if(var == "RAINBOWSTRIPE_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 2).c_str();
  }
  if(var == "PARTY_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 3).c_str();
  }
  if(var == "LAVA_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 4).c_str();
  }
  if(var == "HEAT_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 5).c_str();
  }
  if(var == "CLOUD_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 6).c_str();
  }
  if(var == "OCEAN_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 7).c_str();
  }
  if(var == "FOREST_PALETTE_SELECTED"){
    return getSelected(animation.palettePreset == 8).c_str();
  }

  for(int i=0; i<MAX_COLORS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "COLOR_"+stringNumber;
    prefix += "_";
    if(var == prefix+"POS"){
        return getColorPos(&animation, i).c_str();
    }
    if(var == prefix+"HEX"){
      return getColorHex(&animation, i).c_str();
    }
  }
  return String();
}

String getSwitchImage(bool on){
  if(on){
    return String("toggle_on");
  }else{
    return String("toggle_off");
  }
}

String getSwitchAlt(bool on){
  if(on){
    return String("ON");
  }else{
    return String("OFF");
  }
}

String getSwitchText(bool on){
  if(on){
    return String("on-text");
  }else{
    return String("off-text");
  }
}

String switchesProcessor(const String& var){
  sendCommandToQueue(SEND_SWITCHES);
  SwitchState switches;
  uint8_t buffer[SWITCH_MESSAGE_LENGTH];
  getSwitchFromQueue(buffer);
  setStateFromBytes(&switches,buffer);

  if(var == "LEFT_TURN_SWITCH"){
    return getSwitchAlt(switches.leftTurn);
  }
  if(var == "RIGHT_TURN_SWITCH"){
    return getSwitchAlt(switches.rightTurn);
  }
  if(var == "BRAKE_SWITCH"){
    return getSwitchAlt(switches.brake);
  }
  if(var == "REVERSE_SWITCH"){
    return getSwitchAlt(switches.reverse);
  }
  if(var == "INTERIOR_SWITCH"){
    return getSwitchAlt(switches.interior);
  }
  if(var == "UI_OVERRIDE_SWITCH"){
    return getSwitchAlt(switches.uiOverride);
  }

  if(var == "LEFT_TURN_TEXT"){
    return getSwitchText(switches.leftTurn);
  }
  if(var == "RIGHT_TURN_TEXT"){
    return getSwitchText(switches.rightTurn);
  }
  if(var == "BRAKE_TEXT"){
    return getSwitchText(switches.brake);
  }
  if(var == "REVERSE_TEXT"){
    return getSwitchText(switches.reverse);
  }
  if(var == "INTERIOR_TEXT"){
    return getSwitchText(switches.interior);
  }
  if(var == "UI_OVERRIDE_TEXT"){
    return getSwitchText(switches.uiOverride);
  }

  if(var == "LEFT_TURN_IMG"){
    return getSwitchImage(switches.leftTurn);
  }
  if(var == "RIGHT_TURN_IMG"){
    return getSwitchImage(switches.rightTurn);
  }
  if(var == "BRAKE_IMG"){
    return getSwitchImage(switches.brake);
  }
  if(var == "REVERSE_IMG"){
    return getSwitchImage(switches.reverse);
  }
  if(var == "INTERIOR_IMG"){
    return getSwitchImage(switches.interior);
  }
  if(var == "UI_OVERRIDE_IMG"){
    return getSwitchImage(switches.uiOverride);
  }
  return String();
}

String channelProcessor(const String& var){
  for(int i=0; i<MAX_CHANNELS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);

    if(var.indexOf(stringNumber) == -1){ //Check if var is for this channel index
      continue;
    }

    sendCommandToQueue(SEND_CHANNEL_START+i);
    ChannelState channel;
    uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
    getChannelFromQueue(channelBuffer);
    setStateFromBytes(&channel, channelBuffer);

    String prefix = "CHANNEL_"+stringNumber;
    prefix += "_";
    if( var == prefix+"ENABLED"){
      return getChecked(channel.enabled).c_str();
    }
    if( var == prefix+"INTERIOR"){
      return getChecked(channel.interior).c_str();
    }
    if( var == prefix+"DIRECTIONFLIPPED"){
      return getChecked(channel.directionFlipped).c_str();
    }
    if( var == prefix+"LEFTTURN"){
      return getChecked(channel.leftTurn).c_str();
    }
    if( var == prefix+"RIGHTTURN"){
      return getChecked(channel.rightTurn).c_str();
    }
    if( var == prefix+"BRAKE"){
      return getChecked(channel.brake).c_str();
    }
    if( var == prefix+"REVERSE"){
      return getChecked(channel.reverse).c_str();
    }

    if( var == "TYPE0_"+prefix+"SELECTED"){
      return getSelected(channel.type == 0).c_str();
    }

    if( var == "TYPE1_"+prefix+"SELECTED"){
      return getSelected(channel.type == 1).c_str();
    }

    if( var == "RGB_"+prefix+"SELECTED"){
      return getSelected(channel.order == 0).c_str();
    }

    if( var == "RBG_"+prefix+"SELECTED"){
      return getSelected(channel.order == 1).c_str();
    }

    if( var == "BRG_"+prefix+"SELECTED"){
      return getSelected(channel.order == 2).c_str();
    }

    if( var == "BGR_"+prefix+"SELECTED"){
      return getSelected(channel.order == 3).c_str();
    }

    if( var == "GBR_"+prefix+"SELECTED"){
      return getSelected(channel.order == 4).c_str();
    }

    if( var == "GRB_"+prefix+"SELECTED"){
      return getSelected(channel.order == 5).c_str();
    }

    if( var == prefix+"NUMLEDS"){
      return String(channel.numLEDs);
    }
  }
  return String();
}

String channel0LocProcessor(const String& var){
  int channelIndex = 0;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "NUM_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel1LocProcessor(const String& var){
  int channelIndex = 1;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel2LocProcessor(const String& var){
  int channelIndex = 2;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel3LocProcessor(const String& var){
  int channelIndex = 3;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel4LocProcessor(const String& var){
  int channelIndex = 4;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel5LocProcessor(const String& var){
  int channelIndex = 5;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel6LocProcessor(const String& var){
  int channelIndex = 6;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel7LocProcessor(const String& var){
  int channelIndex = 7;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel8LocProcessor(const String& var){
  int channelIndex = 8;

  sendCommandToQueue(CHANNELPOS_MESSAGE+channelIndex);
  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel9LocProcessor(const String& var){
  int channelIndex = 9;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel10LocProcessor(const String& var){
  int channelIndex = 10;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel11LocProcessor(const String& var){
  int channelIndex = 11;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel12LocProcessor(const String& var){
  int channelIndex = 12;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel13LocProcessor(const String& var){
  int channelIndex = 13;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel14LocProcessor(const String& var){
  int channelIndex = 14;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

String channel15LocProcessor(const String& var){
  int channelIndex = 15;

  sendCommandToQueue(SEND_CHANNEL_START+channelIndex);
  ChannelState channel;
  uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
  getChannelFromQueue(channelBuffer);
  sendCommandToQueue(CHANNELPOS_MESSAGE_LENGTH+channelIndex);
  setStateFromBytes(&channel, channelBuffer);
  uint8_t locations[CHANNELPOS_MESSAGE_LENGTH];
  getChannelPosFromQueue(locations);

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  if(var == "MIN_COL" || var == "MIN_ROW" || var == "MIN_HEIGHT"){
    return "0";
  }
  if(var == "MAX_COL" || var == "MAX_ROW" || var == "MAX_HEIGHT"){
    return String(LOCATION_GRID_SIZE - 1);
  }
  if(var == "MAX_POS"){
    return String(channel.numLEDs);
  }

  for(int i=0; i<MAX_LEDS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";

    if(i >= channel.numLEDs){
      if(var == prefix+"HIDDEN"){
        return "style=\"display: none;\"";
      }
      continue;
    }

    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      return String(locations[(3*i)+0]);
    }
    if(var == prefix+"COL"){
      return String(locations[(3*i)+1]);
    }
    if(var == prefix+"HEIGHT"){
      return String(locations[(3*i)+2]);
    }
  }
  return "";
}

AwsTemplateProcessor channelLocProcessor(int channelIndex){
  switch(channelIndex){
    case 0:
      return channel0LocProcessor;
    case 1:
      return channel1LocProcessor;
    case 2:
      return channel2LocProcessor;
    case 3:
      return channel3LocProcessor;
    case 4:
      return channel4LocProcessor;
    case 5:
      return channel5LocProcessor;
    case 6:
      return channel6LocProcessor;
    case 7:
      return channel7LocProcessor;
    case 8:
      return channel8LocProcessor;
    case 9:
      return channel9LocProcessor;
    case 10:
      return channel10LocProcessor;
    case 11:
      return channel11LocProcessor;
    case 12:
      return channel12LocProcessor;
    case 13:
      return channel13LocProcessor;
    case 14:
      return channel14LocProcessor;
    case 15:
      return channel15LocProcessor;
    default:
      return channel0LocProcessor;
  }
}
