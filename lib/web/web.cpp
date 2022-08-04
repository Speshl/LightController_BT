#include "web.h"

void saveStateFromWeb(State* currentState){
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

void initializeAP(State* state){
  globalState = state;
  Serial.println("Starting AP...");
  WiFi.onEvent(OnWiFiEvent);
  WiFi.mode(WIFI_AP);
  WiFi.softAP(AP_SSID, AP_PASS);
  IPAddress IP = WiFi.softAPIP();
  Serial.print("SOFT AP IP: ");
  Serial.println(IP.toString());


  server.on("/", HTTP_GET, [](AsyncWebServerRequest * request){
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  });
  server.on("/switches", HTTP_GET, [](AsyncWebServerRequest * request){
      request->send(SPIFFS, "/switches.htm", String(), false, switchesProcessor);
  });
  server.on("/animation", HTTP_GET, [](AsyncWebServerRequest * request){
      request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  });
  server.on("/view", HTTP_GET, [](AsyncWebServerRequest * request){
      request->send(SPIFFS, "/view.htm", String(), false, viewProcessor);
  });
  server.on("/save", HTTP_POST, [](AsyncWebServerRequest * request){
      saveStateFromWeb(globalState);
      request->send(SPIFFS, "/switches.htm", String(), false, switchesProcessor);
  });
  server.on("/defaultLocations", HTTP_POST, [](AsyncWebServerRequest * request){
      
      removeAllEntries(&globalState->location);
      setInitialState(&globalState->location);
      saveStateFromWeb(globalState); //save so that it gets reloaded next time
      request->send(SPIFFS, "/switches.htm", String(), false, switchesProcessor);
  });
  server.on("/switches", HTTP_POST, [](AsyncWebServerRequest * request){
      if(request->hasParam("leftTurn",true)){
        Serial.println("LEFT TURN FOUND, TURNING ON");
        globalState->switches.leftTurn = true;
      }else{
        Serial.println("LEFT TURN NOT FOUND, OFF");
        globalState->switches.leftTurn = false;
      }

      if(request->hasParam("rightTurn",true)){
        globalState->switches.rightTurn = true;
      }else{
        globalState->switches.rightTurn = false;
      }

      if(request->hasParam("Reverse",true)){
        globalState->switches.reverse = true;
      }else{
        globalState->switches.reverse = false;
      }

      if(request->hasParam("Brake",true)){
        globalState->switches.brake = true;
      }else{
        globalState->switches.brake = false;
      }

      if(request->hasParam("Interior",true)){
        globalState->switches.interior = true;
      }else{
        globalState->switches.interior = false;
      }

      if(request->hasParam("UIOverride",true)){
        globalState->switches.uiOverride = true;
      }else{
        globalState->switches.uiOverride = false;
      }
      //describeState(&globalState->switches);
      saveSwitchState(&globalState->preferences, getStateAsString(&globalState->switches));
      
      request->send(SPIFFS, "/switches.htm", String(), false, switchesProcessor);
  });

  server.on("/animation", HTTP_POST, [](AsyncWebServerRequest * request){
    if(request->hasParam("animation",true)){
      AsyncWebParameter* p = request->getParam("animation", true);
      globalState->animation.animation = atoi(p->value().c_str());
    }

    if(request->hasParam("fps",true)){
      AsyncWebParameter* p = request->getParam("fps", true);
      globalState->animation.fps = atoi(p->value().c_str());
    }

    if(request->hasParam("brightness",true)){
      AsyncWebParameter* p = request->getParam("brightness", true);
      globalState->animation.brightness = atoi(p->value().c_str());
    }

    if(request->hasParam("blending",true)){
      globalState->animation.blending = LINEARBLEND;
    }else{
      globalState->animation.blending = NOBLEND;
    }

    if(request->hasParam("stepSize",true)){
      AsyncWebParameter* p = request->getParam("stepSize", true);
      globalState->animation.stepSize = atoi(p->value().c_str());
    }

    if(request->hasParam("colorPalette",true)){
      AsyncWebParameter* p = request->getParam("colorPalette", true);
      globalState->animation.palettePreset = atoi(p->value().c_str());
    }

    for(int i=0; i<MAX_COLORS; i++){
      char buffer[10];
      String stringNumber = itoa(i,buffer,10);
      String prefix = "color"+stringNumber;
      if(request->hasParam(prefix+"Pos", true)){
        AsyncWebParameter* p = request->getParam(prefix+"Pos", true);
        setColorPos(&globalState->animation, i, p->value().c_str());
      }
      if(request->hasParam(prefix, true)){
        AsyncWebParameter* p = request->getParam(prefix, true);
        setColorHex(&globalState->animation, i, p->value().c_str());
      }
    }
    animationUpdated(&globalState->animation, globalState->channels);
    saveAnimationState(&globalState->preferences, getStateAsString(&globalState->animation));
    setStateFromString(&globalState->animation, getStateAsString(&globalState->animation));
    request->send(SPIFFS, "/animation.htm", String(), false, animationProcessor);
  });

  server.on("/channelsPos", HTTP_POST, [](AsyncWebServerRequest * request){
    if(request->hasParam("INITIAL",true) == false){ //Read parameters if this isn't the initial load of the page
      if(request->hasParam("CHANNEL_INDEX",true)){
        AsyncWebParameter* p = request->getParam("CHANNEL_INDEX", true);
        int channelIndex = atoi(p->value().c_str());
        removeChannelEntries(&globalState->location, channelIndex);
        for(int i=0;i<globalState->channels[channelIndex].numLEDs; i++){
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

          AsyncWebParameter* rowParam = request->getParam(locRow.c_str(), true);
          AsyncWebParameter* colParam = request->getParam(locCol.c_str(), true);
          AsyncWebParameter* heightParam = request->getParam(locHeight.c_str(), true);

          int row = atoi(rowParam->value().c_str());
          int col = atoi(colParam->value().c_str());
          int height = atoi(heightParam->value().c_str());
          addEntry(&globalState->location,row, col, height, channelIndex, i);
        }
        uint8_t locationArray[MAX_LEDS*3];
        getChannelLocations(&globalState->location, channelIndex, globalState->channels[channelIndex].numLEDs, locationArray);
        saveChannelLocationState(&globalState->preferences, channelIndex, locationArray);
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
      if(request->hasParam(prefix+"enabled",true)){
        Serial.println("Enabling Channel");
        globalState->channels[channelIndex].enabled = true;
      }else{
        Serial.println("Disabling Channel");
        globalState->channels[channelIndex].enabled = false;
      }

      if(request->hasParam(prefix+"interior",true)){
        globalState->channels[channelIndex].interior = true;
      }else{
        globalState->channels[channelIndex].interior = false;
      }

      if(request->hasParam(prefix+"directionFlipped",true)){
        globalState->channels[channelIndex].directionFlipped = true;
      }else{
        globalState->channels[channelIndex].directionFlipped = false;
      }

      if(request->hasParam(prefix+"leftTurn",true)){
        globalState->channels[channelIndex].leftTurn = true;
      }else{
        globalState->channels[channelIndex].leftTurn = false;
      }

      if(request->hasParam(prefix+"rightTurn",true)){
        globalState->channels[channelIndex].rightTurn = true;
      }else{
        globalState->channels[channelIndex].rightTurn = false;
      }

      if(request->hasParam(prefix+"brake",true)){
        globalState->channels[channelIndex].brake = true;
      }else{
        globalState->channels[channelIndex].brake = false;
      }

      if(request->hasParam(prefix+"reverse",true)){
        globalState->channels[channelIndex].reverse = true;
      }else{
        globalState->channels[channelIndex].reverse = false;
      }

      if(request->hasParam(prefix+"type",true)){
        AsyncWebParameter* p = request->getParam(prefix+"type", true);
        int newType = atoi(p->value().c_str());
        if(newType != globalState->channels[channelIndex].type){
          globalState->channels[channelIndex].type = newType;
          globalState->channels[channelIndex].restartRequired = true;
        }
      }

      if(request->hasParam(prefix+"order",true)){
        AsyncWebParameter* p = request->getParam(prefix+"order", true);
        int newOrder = atoi(p->value().c_str());
        if(newOrder != globalState->channels[channelIndex].order){
          globalState->channels[channelIndex].order = newOrder;
          //globalState->channels[channelIndex].restartRequired = true;
        }
      }

      if(request->hasParam(prefix+"numLEDs",true)){
        AsyncWebParameter* p = request->getParam(prefix+"numLEDs", true);
        globalState->channels[channelIndex].numLEDs = atoi(p->value().c_str());
      }

      saveChannelDetailState(&globalState->preferences, channelIndex, getStateAsString(globalState->channels, channelIndex));
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