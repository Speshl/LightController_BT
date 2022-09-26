#include "web.h"
#include "web_animation.h"
#include "web_channelLoc.h"
#include "web_channel.h"
#include "web_utils.h"
#include "web_switches.h"
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

/*
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
    sendSwitchToQueueFromISR(switchBuffer);
    sendCommandToQueueFromISR(READ_SWITCHES);

    AnimationState animation;
    uint8_t animationBuffer[ANIMATION_MESSAGE_LENGTH];
    setStateFromJson(&animation, object);
    getStateAsBytes(&animation, animationBuffer);
    sendAnimationToQueueFromISR(animationBuffer);
    sendCommandToQueueFromISR(READ_ANIMATION);

    for(int i=0; i<MAX_CHANNELS; i++){ //PUT BACK TO MAX CHANNELS
      //vTaskDelay(50);
      ChannelState channel;
      uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
      setStateFromJson(&channel, object, i);
      getStateAsBytes(&channel, channelBuffer);
      sendChannelToQueueFromISR(channelBuffer);
      sendCommandToQueueFromISR(READ_CHANNEL_START+i);

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

      sendChannelPosToQueueFromISR(locationBuffer);
      sendCommandToQueueFromISR(READ_CHANNELPOS_START+i);
    }
  }
}
*/

void blockUntilReady(){
  Serial.println("Notify Controller of Recieved Request");
  while(sendRequestStatusToQueueFromISR(ACTIVE) != true){ //Let Controller know we have a request to process
    Serial.println("Failed Notifying Controller, Retrying");
    vTaskDelay(1);
  } 
  Serial.println("Waiting for Controller to be Ready");
  while(peekControllerStatusFromQueueFromISR() != INACTIVE){//Wait for controller to get into it's waiting loop
     vTaskDelay(5);
  } 
  Serial.println("Controller Ready");
}

void initializeAP(State* state){
  Serial.println("Starting AP...");
  WiFi.onEvent(OnWiFiEvent);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("SOFT AP IP: ");
  Serial.println(IP.toString());

  GlobalState = state; //Test this for data access

  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
      blockUntilReady();
      request->send_P(200, "text/html", animation_html, animationProcessor);
  });
  server.on("/switches", HTTP_GET, [](AsyncWebServerRequest * request){
      blockUntilReady();
      request->send_P(200, "text/html", switches_html, switchesProcessor);
  });
  server.on("/animation", HTTP_GET, [](AsyncWebServerRequest * request){
      blockUntilReady();
      request->send_P(200, "text/html", animation_html, animationProcessor);
  });

  server.on("/utils", HTTP_GET, [](AsyncWebServerRequest * request){
      blockUntilReady();
      request->send_P(200, "text/html", utils_html, utilsProcessor);
  });
  /*server.on("/toggle_on", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Toggle ON");
    size_t outputLength;
    unsigned char * decoded = base64_decode((const unsigned char *)toggle_on_image, strlen(toggle_on_image), &outputLength);
    request->send_P(200, "image/png", (char *)decoded);
  });
  server.on("/toggle_off", HTTP_GET, [](AsyncWebServerRequest *request){
    Serial.println("Toggle OFF");
    request->send_P(200, "image/png", toggle_off_image);
  });*/
  server.on("/defaultLocations", HTTP_POST, [](AsyncWebServerRequest * request){
    blockUntilReady();
    request->send_P(200, "text/html", utils_html, utilsProcessor);
  });
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest * request){
    blockUntilReady();
    request->send_P(200, "text/html", utils_html, utilsProcessor);
  });
/*  server.on("/import", HTTP_POST, [](AsyncWebServerRequest *request){
    blockUntilReady();
    request->send_P(200, "text/html", utils_html, utilsProcessor);
  }, onUpload);
  server.on("/export", HTTP_GET, [](AsyncWebServerRequest * request){
      blockUntilReady();

      AsyncJsonResponse * response = new AsyncJsonResponse(false, 1024 * 15);
      response->addHeader("Server","ESP Async Web Server");
      JsonObject root = response->getRoot();

      sendCommandToQueueFromISR(SEND_ANIMATION);
      uint8_t animationBuffer[ANIMATION_MESSAGE_LENGTH];
      AnimationState animation;
      getAnimationFromQueueFromISR(animationBuffer);
      setStateFromBytes(&animation,animationBuffer);
      getStateAsJson(&animation, root);

      sendCommandToQueueFromISR(SEND_SWITCHES);
      uint8_t switchBuffer[SWITCH_MESSAGE_LENGTH];
      SwitchState switches;
      getSwitchFromQueueFromISR(switchBuffer);
      setStateFromBytes(&switches, switchBuffer);
      getStateAsJson(&switches, root);

      for(int i=0; i<MAX_CHANNELS; i++){
        sendCommandToQueueFromISR(SEND_CHANNEL_START + i);
        uint8_t channelBuffer[CHANNEL_MESSAGE_LENGTH];
        ChannelState channel;
        getChannelFromQueueFromISR(channelBuffer);
        setStateFromBytes(&channel, channelBuffer);
        getStateAsJson(&channel, root, i);
      }

      for(int i=0; i<MAX_CHANNELS; i++){
        char indexBuffer[5];
        itoa(i, indexBuffer,10);
        std::string indexString(indexBuffer);
        sendCommandToQueueFromISR(SEND_CHANNELPOS_START + i);
        uint8_t locationBuffer[CHANNELPOS_MESSAGE_LENGTH];
        getChannelPosFromQueueFromISR(locationBuffer);
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
  });*/
  server.on("/switches", HTTP_POST, [](AsyncWebServerRequest * request){
    blockUntilReady();
    SwitchState newSwitchState = GlobalState->switches;

    Serial.println("Reading Params");
    if(request->hasParam("LEFT_TURN_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("LEFT_TURN_SWITCH", true);
      if(p->value().compareTo("off") == 0){
        newSwitchState.leftTurn = true;
      }else{
        newSwitchState.leftTurn = false;
      }
    }

    if(request->hasParam("RIGHT_TURN_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("RIGHT_TURN_SWITCH", true);
      if(p->value().compareTo("off") == 0){
        newSwitchState.rightTurn = true;
      }else{
        newSwitchState.rightTurn = false;
      }
    }

    if(request->hasParam("REVERSE_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("REVERSE_SWITCH", true);
      if(p->value().compareTo("off") == 0){
        newSwitchState.reverse = true;
      }else{
        newSwitchState.reverse = false;
      }
    }

    if(request->hasParam("BRAKE_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("BRAKE_SWITCH", true);
      if(p->value().compareTo("off") == 0){
        newSwitchState.brake = true;
      }else{
        newSwitchState.brake = false;
      }
    }

    if(request->hasParam("INTERIOR_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("INTERIOR_SWITCH", true);
      if(p->value().compareTo("off") == 0){
        newSwitchState.interior = true;
      }else{
        newSwitchState.interior = false;
      }
    }

    if(request->hasParam("UI_OVERRIDE_SWITCH",true)){
      AsyncWebParameter* p = request->getParam("UI_OVERRIDE_SWITCH", true);
      if(p->value().compareTo("off") == 0){
        newSwitchState.uiOverride = true;
      }else{
        newSwitchState.uiOverride = false;
      }
    }
    newSwitchState.updated = true;
    GlobalState->switches = newSwitchState;
    request->send_P(200, "text/html", switches_html, switchesProcessor);
  });

  server.on("/animation", HTTP_POST, [](AsyncWebServerRequest * request){
    blockUntilReady();
    AnimationState newAnimation = GlobalState->animation;
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
    newAnimation.updated = true;
    GlobalState->animation = newAnimation;
    Serial.println("Animation Post Complete, start processing template");
    request->send_P(200, "text/html", animation_html, animationProcessor);
  });

  server.on("/channelsPos", HTTP_POST, [](AsyncWebServerRequest * request){
    blockUntilReady();
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
        setChannelLocations(&GlobalState->location, channelIndex, CHANNELPOS_MESSAGE_LENGTH, posBuffer);
        GlobalState->location.updated = true;
      }else{
        request->send_P(200, "text/html", animation_html, animationProcessor);
      }
    }
    //Build page to display
    if(request->hasParam("CHANNEL_INDEX",true)){
      AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
      int channelIndex = atoi(p->value().c_str());
      request->send_P(200, "text/html", channelLoc_html, getChannelLocProcessor(channelIndex));
    }else{
      request->send_P(200, "text/html", animation_html, animationProcessor);
    }
  });

  server.on("/channels", HTTP_POST, [](AsyncWebServerRequest * request){
    blockUntilReady();
    if(request->hasParam("INITIAL",true)){
      if(request->hasParam("CHANNEL_INDEX",true)){
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());
        request->send_P(200, "text/html", channel_html, getChannelProcessor(channelIndex));
      }else{
        request->send_P(200, "text/html", animation_html, animationProcessor);
      }
    }else if(request->hasParam("CHANNEL_INDEX",true)){
      AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
      int channelIndex = atoi(p->value().c_str());
      String prefix = "CHANNEL_";

      ChannelState newChannel = GlobalState->channels[channelIndex];

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
      newChannel.updated = true;
      GlobalState->channels[channelIndex] = newChannel;
      request->send_P(200, "text/html", channel_html, getChannelProcessor(channelIndex));
    }else{
      Serial.println("Channel Index not found, showing index");
      request->send_P(200, "text/html", animation_html, animationProcessor);
    }
  });

  dnsServer.start(53, "*", WiFi.softAPIP());
  //server.addHandler(new CaptiveRequestHandler()).setFilter(ON_AP_FILTER);//only when requested from AP
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

String animationProcessor(const String& var){
  if(var == "LAST_LINE"){
    getRequestStatusFromQueueFromISR(); //Let Controller know request is over by removing status from queue
    Serial.println("Notified Controller request is done");
    return String();
  }

  AnimationState animation = GlobalState->animation;

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
    return "on";
  }else{
    return "off";
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
  if(var == "LAST_LINE"){
    getRequestStatusFromQueueFromISR(); //Let Controller know request is over
    Serial.println("Notified Controller request is done");
    return String();
  }

  SwitchState switches = GlobalState->switches;

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

String channelProcessor(const String& var, int channelIndex){
  if(var == "LAST_LINE"){
    getRequestStatusFromQueueFromISR(); //Let Controller know request is over
    Serial.println("Notified Controller request is done");
    return String();
  }

  ChannelState channel = GlobalState->channels[channelIndex];
  String prefix = "CHANNEL_";

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

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

  if( var == prefix+"TYPE0_SELECTED"){
    return getSelected(channel.type == 0).c_str();
  }

  if( var == prefix+"TYPE1_SELECTED"){
    return getSelected(channel.type == 1).c_str();
  }

  if( var == prefix+"RGB_SELECTED"){
    return getSelected(channel.order == 0).c_str();
  }

  if( var == prefix+"RBG_SELECTED"){
    return getSelected(channel.order == 1).c_str();
  }

  if( var == prefix+"BRG_SELECTED"){
    return getSelected(channel.order == 2).c_str();
  }

  if( var == prefix+"BGR_SELECTED"){
    return getSelected(channel.order == 3).c_str();
  }

  if( var == prefix+"GBR_SELECTED"){
    return getSelected(channel.order == 4).c_str();
  }

  if( var == prefix+"GRB_SELECTED"){
    return getSelected(channel.order == 5).c_str();
  }

  if( var == prefix+"NUMLEDS"){
    return String(channel.numLEDs);
  }
  return String();
}

String channelLocProcessor(const String& var, int channelIndex){
  if(var == "LAST_LINE"){
    getRequestStatusFromQueueFromISR(); //Let Controller know request is over
    Serial.println("Notified Controller request is done");
    return String();
  }

  if(var == "CHANNEL_INDEX"){
    char buffer[10];
    String stringNumber = itoa(channelIndex,buffer,10);
    return stringNumber;
  }

  ChannelState channel = GlobalState->channels[channelIndex];

  if(var == "NUM_POS"){
    return String(channel.numLEDs);
  }

  String varToNumber = var.substring(4,6);
  if(varToNumber.charAt(1) == '_'){
    varToNumber = varToNumber.substring(0,1);
  }

  std::string stringNumber = varToNumber.c_str();
  int i = atoi(stringNumber.c_str());
  String prefix = "LOC_"+varToNumber;
  
  if(i >= channel.numLEDs && var == prefix+"_HIDDEN"){
    return "style=\"display: none;\"";
  }

  if(var == prefix+"_HIDDEN"){
    return "";
  }

  std::string row="";
  std::string col="";
  std::string height="";
  getChannelLocationAtPosition(&GlobalState->location, channelIndex, i, &row, &col, &height);

  if(var == prefix+"_ROW"){
    return row.c_str();
  }
  if(var == prefix+"_COL"){
    return col.c_str();
  }
  if(var == prefix+"_HEIGHT"){
    return height.c_str();
  }
}













String channel0Processor(const String& var){
  return channelProcessor(var, 0);
}

String channel1Processor(const String& var){
  return channelProcessor(var, 1);
}

String channel2Processor(const String& var){
  return channelProcessor(var, 2);
}

String channel3Processor(const String& var){
  return channelProcessor(var, 3);
}

String channel4Processor(const String& var){
  return channelProcessor(var, 4);
}

String channel5Processor(const String& var){
  return channelProcessor(var, 5);
}

String channel6Processor(const String& var){
  return channelProcessor(var, 6);
}

String channel7Processor(const String& var){
  return channelProcessor(var, 7);
}

String channel8Processor(const String& var){
  return channelProcessor(var, 8);
}

String channel9Processor(const String& var){
  return channelProcessor(var, 9);
}

String channel10Processor(const String& var){
  return channelProcessor(var, 10);
}

String channel11Processor(const String& var){
  return channelProcessor(var, 11);
}

String channel12Processor(const String& var){
  return channelProcessor(var, 12);
}

String channel13Processor(const String& var){
  return channelProcessor(var, 13);
}

String channel14Processor(const String& var){
  return channelProcessor(var, 14);
}

String channel15Processor(const String& var){
  return channelProcessor(var, 15);
}

String channel0LocProcessor(const String& var){
  return channelLocProcessor(var, 0);
}

String channel1LocProcessor(const String& var){
  return channelLocProcessor(var, 1);
}

String channel2LocProcessor(const String& var){
  return channelLocProcessor(var, 2);
}

String channel3LocProcessor(const String& var){
  return channelLocProcessor(var, 3);
}

String channel4LocProcessor(const String& var){
  return channelLocProcessor(var, 4);
}

String channel5LocProcessor(const String& var){
  return channelLocProcessor(var, 5);
}

String channel6LocProcessor(const String& var){
  return channelLocProcessor(var, 6);
}

String channel7LocProcessor(const String& var){
  return channelLocProcessor(var, 7);
}

String channel8LocProcessor(const String& var){
  return channelLocProcessor(var, 8);
}

String channel9LocProcessor(const String& var){
  return channelLocProcessor(var, 9);
}

String channel10LocProcessor(const String& var){
  return channelLocProcessor(var, 10);
}

String channel11LocProcessor(const String& var){
  return channelLocProcessor(var, 11);
}

String channel12LocProcessor(const String& var){
  return channelLocProcessor(var, 12);
}

String channel13LocProcessor(const String& var){
  return channelLocProcessor(var, 13);
}

String channel14LocProcessor(const String& var){
  return channelLocProcessor(var, 14);
}

String channel15LocProcessor(const String& var){
  return channelLocProcessor(var, 15);
}

AwsTemplateProcessor getChannelProcessor(int channelIndex){
  switch(channelIndex){
    case 0:
      return channel0Processor;
    case 1:
      return channel1Processor;
    case 2:
      return channel2Processor;
    case 3:
      return channel3Processor;
    case 4:
      return channel4Processor;
    case 5:
      return channel5Processor;
    case 6:
      return channel6Processor;
    case 7:
      return channel7Processor;
    case 8:
      return channel8Processor;
    case 9:
      return channel9Processor;
    case 10:
      return channel10Processor;
    case 11:
      return channel11Processor;
    case 12:
      return channel12Processor;
    case 13:
      return channel13Processor;
    case 14:
      return channel14Processor;
    case 15:
      return channel15Processor;
    default:
      return channel0Processor;
  }
}

AwsTemplateProcessor getChannelLocProcessor(int channelIndex){
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
