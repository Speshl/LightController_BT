#include "channels.h"

CRGB getColor(ChannelState* channel, CRGB color){
  //return color;
  return getColor(channel, color.red, color.green, color.blue);
}

CRGB getColor(ChannelState* channel,int red, int green, int blue){
  CRGB color;
  switch(channel->order){
    case 0: //RGB
      color.red = red;
      color.green = green;
      color.blue = blue;
      break;
    case 1: //RBG
      color.red = red;
      color.green = blue;
      color.blue = green;
      break;
    case 2: //BRG
      color.red = blue;
      color.green = red;
      color.blue = green;
      break;
    case 3: //BGR
      color.red = blue;
      color.green = green;
      color.blue = red;
      break;
    case 4: //GBR
      color.red = green;
      color.green = blue;
      color.blue = red;
      break;
    case 5: //GRB
      color.red = green;
      color.green = red;
      color.blue = blue;
      break;
  }
  return color;
}

void showAll(ChannelState channels[MAX_CHANNELS]){
  //Serial.println("Showing All");
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].updated == true && channels[i].controller != NULL){
      //Serial.print("Showing Channel ");
      //Serial.println(i);
      channels[i].controller->showLeds();
      channels[i].updated = false;
    }
  }
}

void applyBufferAndShow(ChannelState channels[MAX_CHANNELS], SwitchState* switches){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isActiveWithSpecialCases(&channels[i], switches) == true){
      //Serial.printf("Applying Buffer for channel: ");
      //Serial.println(i);
      channels[i].updated = true;
      for(int j=0; j<channels[i].numLEDs; j++){
        int actualPosition = j;
        if(channels[i].directionFlipped == true){
          actualPosition = (channels[i].numLEDs-1) - j;
        }
        channels[i].leds[actualPosition] = getColor(&channels[i], channels[i].ledsBuffer[j]);
        //channels[i].leds[j] = channels[i].ledsBuffer[j];
      }
    }
  }
  showAll(channels);
}

bool restartRequired(ChannelState channels[MAX_CHANNELS]){
  bool restart = false;
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].restartRequired == true){
      restart = true;
    }
  }
  return restart;
}

bool isActive(ChannelState* channel){
  if(channel->enabled == true && channel->numLEDs > 0){
    return true;
  }
  return false;
}

bool isActiveWithSpecialCases(ChannelState* channel, SwitchState* switches){
  if(channel->leftTurn == true && switches->leftTurn == true){ //Don't modify this channel since the signal is on
    return false;
  }

  if(channel->rightTurn == true && switches->rightTurn == true){
    return false;
  }

  if(channel->brake == true && switches->brake == true){
    return false;
  }

  if(channel->reverse == true && switches->reverse == true){
    return false;
  }

  if(channel->interior == true && switches->interior == true){
    return false;
  }
  return isActive(channel);
}

void getStateAsJson(ChannelState* channel, JsonObject doc, int index){
  char indexBuffer[5];
  itoa(index, indexBuffer,10);
  std::string indexString(indexBuffer);
  doc["enabled"+indexString] = channel->enabled;
  doc["interior"+indexString] = channel->interior;
  doc["directionFlipped"+indexString] = channel->directionFlipped;
  doc["leftTurn"+indexString] = channel->leftTurn;
  doc["rightTurn"+indexString] = channel->rightTurn;
  doc["brake"+indexString] = channel->brake;
  doc["reverse"+indexString] = channel->reverse;
  doc["type"+indexString] = channel->type;
  doc["order"+indexString] = channel->order;
  doc["numLEDs"+indexString] = channel->numLEDs;
}

void setStateFromJson(ChannelState* channel, JsonObject doc, int index){
  char indexBuffer[5];
  itoa(index, indexBuffer,10);
  std::string indexString(indexBuffer);

  channel->enabled = doc["enabled"+indexString];
  channel->interior = doc["interior"+indexString];
  channel->directionFlipped = doc["directionFlipped"+indexString];
  channel->leftTurn = doc["leftTurn"+indexString];
  channel->rightTurn = doc["rightTurn"+indexString];
  channel->brake = doc["brake"+indexString];
  channel->reverse = doc["reverse"+indexString];
  channel->type = doc["type"+indexString];
  channel->order = doc["order"+indexString];
  channel->numLEDs = doc["numLEDs"+indexString];
}

void getStateAsBytes(ChannelState * channel, uint8_t state[CHANNEL_STATE_LENGTH]){
  state[0] = channel->enabled;
  state[1] = channel->interior;
  state[2] = channel->directionFlipped;
  state[3] = channel->leftTurn;
  state[4] = channel->rightTurn;
  state[5] = channel->brake;
  state[6] = channel->reverse;
  state[7] = channel->type;
  state[8] = channel->order;
  
  ChannelsConversion converter;
  converter.wordValue = channel->numLEDs;
  state[9] = converter.byteValue[0];
  state[10] = converter.byteValue[1];
}

void getStateAsBytes(ChannelState channels[MAX_CHANNELS], int i, uint8_t state[CHANNEL_STATE_LENGTH]){
  //Serial.println("Getting channel state from string");
  getStateAsBytes(&channels[i],state);
}

std::string getStateAsString(ChannelState * channel){
  std::string returnValue = "";
  std::string builderString = "";
  builderString += channel->enabled;
  builderString += channel->interior;
  builderString += channel->directionFlipped;
  builderString += channel->leftTurn;
  builderString += channel->rightTurn;
  builderString += channel->brake;
  builderString += channel->reverse;
  builderString += channel->type;
  builderString += channel->order;
  
  ChannelsConversion converter;
  converter.wordValue = channel->numLEDs;
  builderString += converter.charValue[0];
  builderString += converter.charValue[1];    
  returnValue.append(builderString);
  return returnValue;
}

std::string getStateAsString(ChannelState channels[MAX_CHANNELS], int i){
  //Serial.println("Getting channel state from string");
  return getStateAsString(&channels[i]);
}

void setStateFromBytes(ChannelState * channel, uint8_t state [CHANNEL_STATE_LENGTH]){
  int currentPos = 0;
  bool newTypeFound = false;

  channel->enabled = state[0];
  channel->interior = state[1];
  channel->directionFlipped = state[2];
  channel->leftTurn = state[3];
  channel->rightTurn = state[4];
  channel->brake = state[5];
  channel->reverse = state[6];

  char newType = state[7];
  if(newType != channel->type){
    channel->type = newType;
    newTypeFound = true;
  }

  channel->order = state[8];

  ChannelsConversion converter;
  converter.byteValue[0] = state[9];
  converter.byteValue[1] = state[10];
  channel->numLEDs = converter.wordValue;
}

void setStateFromBytes(ChannelState channels[MAX_CHANNELS],int i, uint8_t state [CHANNEL_STATE_LENGTH]){
  //Serial.println("Setting channel state from string");
  setStateFromBytes(&channels[i], state);

  if(channels[i].enabled == true && channels[i].controller == NULL){
    getDefinition(&(channels[i]), i); //Get a controller where one didn't already exist
  }
  clear(channels);
  //Serial.println("Showing All after state from string update for channels");
  showAll(channels);
}

void setStateFromString(ChannelState * channel, std::string inputValue){
if (inputValue.length() < CHANNEL_SIZE){
    Serial.print("Invalid string used to set Channel State (Length: ");
    Serial.print(inputValue.length());
    Serial.print(") : ");
    Serial.println(inputValue.c_str());
    return;
  }
  int currentPos = 0;
  bool newTypeFound = false;

  channel->enabled = inputValue[currentPos];
  channel->interior = inputValue[currentPos+1];
  channel->directionFlipped = inputValue[currentPos+2];
  channel->leftTurn = inputValue[currentPos+3];
  channel->rightTurn = inputValue[currentPos+4];
  channel->brake = inputValue[currentPos+5];
  channel->reverse = inputValue[currentPos+6];

  char newType = inputValue[currentPos+7];
  if(newType != channel->type){
    channel->type = newType;
    newTypeFound = true;
  }

  channel->order = inputValue[currentPos+8];

  ChannelsConversion converter;
  converter.charValue[0] = inputValue[currentPos+9];
  converter.charValue[1] = inputValue[currentPos+10];
  channel->numLEDs = converter.wordValue;
}

void setStateFromString(ChannelState channels[MAX_CHANNELS],int i, std::string inputValue){
  //Serial.println("Setting channel state from string");
  setStateFromString(&channels[i], inputValue);

  if(channels[i].enabled == true && channels[i].controller == NULL){
    getDefinition(&(channels[i]), i); //Get a controller where one didn't already exist
  }
  clear(channels);
  //Serial.println("Showing All after state from string update for channels");
  showAll(channels);
}

void describeState(ChannelState channels[MAX_CHANNELS], int index){
  Serial.print("Describing Channel :");
  Serial.println(index);
  Serial.print("Enabled: ");
  Serial.println((uint8_t) channels[index].enabled);//So it displays as number
  Serial.print("Interior: ");
  Serial.println((uint8_t) channels[index].interior);//So it displays as number
  Serial.print("DirectionFlipped: ");
  Serial.println((uint8_t) channels[index].directionFlipped);//So it displays as number
  Serial.print("LeftTurn: ");
  Serial.println((uint8_t) channels[index].leftTurn);//So it displays as number
  Serial.print("RightTurn: ");
  Serial.println((uint8_t) channels[index].rightTurn);//So it displays as number
  Serial.print("Brake: ");
  Serial.println((uint8_t) channels[index].brake);//So it displays as number
  Serial.print("Reverse: ");
  Serial.println((uint8_t) channels[index].reverse);//So it displays as number
  Serial.print("Type: ");
  Serial.println((uint8_t) channels[index].type);//So it displays as number
  Serial.print("Order: ");
  Serial.println((uint8_t) channels[index].order);//So it displays as number 
  Serial.print("NumLEDs: ");
  Serial.println(channels[index].numLEDs);
}

void describeState(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Describing Channels State - ");
  for(int i=0; i<MAX_CHANNELS; i++){
    describeState(channels, i);
  }
}

void clear(ChannelState channels[MAX_CHANNELS]){
  for(int i=0; i<MAX_CHANNELS; i++){
    fill_solid(channels[i].leds, channels[i].numLEDs, CRGB(0,0,0));
    fill_solid(channels[i].ledsBuffer, channels[i].numLEDs, CRGB(0,0,0));
    channels[i].updated = true;
  }
}

void fill(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(isActive(&channels[i]) == true){
      fill_solid(channels[i].ledsBuffer, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillBrake(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].brake == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillReverse(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].reverse == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillLeftTurn(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].leftTurn == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void fillRightTurn(ChannelState channels[MAX_CHANNELS], CRGB color){
  for(int i=0; i<MAX_CHANNELS; i++){
    if(channels[i].rightTurn == true){
      fill_solid(channels[i].leds, channels[i].numLEDs, getColor(&channels[i],color));
      channels[i].updated = true;
    }
  }
}

void setColorAtPos(ChannelState* channel, int pos, CRGB color){
  channel->ledsBuffer[pos] = color;
}

CRGB getColorAtPos(ChannelState* channel, int pos){
  if(channel->numLEDs > pos){
    return channel->ledsBuffer[pos];
  }else{
    return CRGB::Black;
  }
}

void setChannelPins(){
  for(int i=0; i<MAX_CHANNELS; i++){
    Serial.print("Channel ");
    Serial.print(i);
    Serial.print(" on Pin ");
    Serial.println(channelpins[i]);
    pinMode(channelpins[i], OUTPUT);
  }
}

void setInitialState(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Start Setting Initial Channel State");

  for(int i=0; i<MAX_CHANNELS; i++){
    channels[i].enabled = false;
    channels[i].interior = false;
    channels[i].directionFlipped = false;
    channels[i].leftTurn = false;
    channels[i].rightTurn = false;
    channels[i].brake = false;
    channels[i].reverse = false;
    channels[i].type = 1;
    channels[i].order = 5;
    channels[i].numLEDs = 0;
    memset(channels[i].leds,0,sizeof(channels[i].leds));
    memset(channels[i].ledsBuffer,0,sizeof(channels[i].ledsBuffer));
    channels[i].updated = false;
    channels[i].restartRequired = false;
    getDefinition(&(channels[i]), i);
  }
  loadTestData(channels);
  Serial.println("Finished Setting Initial channel State");
}

/*std::string getChannelType(ChannelState* channel){
  char buffer[10];
  return itoa(channel->type,buffer, 10);
}

std::string getChannelOrder(ChannelState* channel){
  char buffer[10];
  return itoa(channel->order,buffer, 10);
}

std::string getChannelNumLEDs(ChannelState* channel){
 char buffer[10];
  return itoa(channel->numLEDs,buffer, 10);
}*/
















void getDefinition(ChannelState * channel, int index){
  //Serial.println("Start Getting strip definition");
//FIRST 8: GPIO_NUM_23,GPIO_NUM_22,GPIO_NUM_21,GPIO_NUM_19,GPIO_NUM_18,GPIO_NUM_5,GPIO_NUM_17,GPIO_NUM_16
//LAST 8:  ,GPIO_NUM_4,GPIO_NUM_0,GPIO_NUM_2,GPIO_NUM_15,GPIO_NUM_13,GPIO_NUM_12,GPIO_NUM_14,GPIO_NUM_27
  switch (index){
    case 0:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_0_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_0_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 23, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 1:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_1_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_1_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 22, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 2:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_2_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_2_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 3, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 3:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_3_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_3_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 21, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 4:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_4_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_4_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 19, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 5:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_5_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_5_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 18, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 6:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_6_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_6_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 5, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 7:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_7_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_7_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 4, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 8:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_8_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_8_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 0, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 9:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_9_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_9_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 2, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 10:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_10_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_10_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 15, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 11:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_11_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_11_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 25, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 12:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_12_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_12_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 26, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 13:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_13_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_13_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 14, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 14:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_14_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_14_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 12, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
    case 15:
      switch(channel->type){
        case 0:
          channel->controller = &FastLED.addLeds<WS2811, CHANNEL_15_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        case 1:
          channel->controller = &FastLED.addLeds<WS2812, CHANNEL_15_PIN, RGB>(channel->leds, MAX_LEDS);
          break;
        default:
          //FastLED.addLeds<NEOPIXEL, 13, RGB>(channel->leds, MAX_LEDS);
          break;
      }
      break;
  }
  //Serial.println("Finnished Getting strip definition");*/
}

void loadTestData(ChannelState channels[MAX_CHANNELS]){
  Serial.println("Start Settting DEFAULT Channel Data");
  channels[0].enabled = true; 
  channels[0].interior = false;
  channels[0].directionFlipped = true;
  channels[0].leftTurn = true;
  channels[0].rightTurn = false;
  channels[0].brake = false;
  channels[0].reverse = false;
  channels[0].type = 1;
  channels[0].order = 2;
  channels[0].numLEDs = 6;
  memset(channels[0].leds,0,sizeof(channels[0].leds));
  memset(channels[0].ledsBuffer,0,sizeof(channels[0].ledsBuffer));
  channels[0].updated = false;
  channels[0].restartRequired = false;
  getDefinition(&(channels[0]), 0);

  channels[1].enabled = true; 
  channels[1].interior = false;
  channels[1].directionFlipped = true;
  channels[1].leftTurn = true;
  channels[1].rightTurn = false;
  channels[1].brake = true;
  channels[1].reverse = false;
  channels[1].type = 1;
  channels[1].order = 2;
  channels[1].numLEDs = 6;
  memset(channels[1].leds,0,sizeof(channels[1].leds));
  memset(channels[1].ledsBuffer,0,sizeof(channels[1].ledsBuffer));
  channels[1].updated = false;
  channels[1].restartRequired = false;
  getDefinition(&(channels[1]), 1);

  channels[2].enabled = true; 
  channels[2].interior = false;
  channels[2].directionFlipped = true;
  channels[2].leftTurn = true;
  channels[2].rightTurn = false;
  channels[2].brake = true;
  channels[2].reverse = true;
  channels[2].type = 1;
  channels[2].order = 2;
  channels[2].numLEDs = 6;
  memset(channels[2].leds,0,sizeof(channels[2].leds));
  memset(channels[2].ledsBuffer,0,sizeof(channels[2].ledsBuffer));
  channels[2].updated = false;
  channels[2].restartRequired = false;
  getDefinition(&(channels[2]), 2);

  channels[3].enabled = true; 
  channels[3].interior = false;
  channels[3].directionFlipped = true;
  channels[3].leftTurn = false;
  channels[3].rightTurn = false;
  channels[3].brake = false;
  channels[3].reverse = false;
  channels[3].type = 1;
  channels[3].order = 2;
  channels[3].numLEDs = 6;
  memset(channels[3].leds,0,sizeof(channels[3].leds));
  memset(channels[3].ledsBuffer,0,sizeof(channels[3].ledsBuffer));
  channels[3].updated = false;
  channels[3].restartRequired = false;
  getDefinition(&(channels[3]), 3);

  channels[4].enabled = true; 
  channels[4].interior = false;
  channels[4].directionFlipped = true;
  channels[4].leftTurn = false;
  channels[4].rightTurn = false;
  channels[4].brake = false;
  channels[4].reverse = false;
  channels[4].type = 1;
  channels[4].order = 2;
  channels[4].numLEDs = 6;
  memset(channels[4].leds,0,sizeof(channels[4].leds));
  memset(channels[4].ledsBuffer,0,sizeof(channels[4].ledsBuffer));
  channels[4].updated = false;
  channels[4].restartRequired = false;
  getDefinition(&(channels[4]), 4);

  channels[5].enabled = true; 
  channels[5].interior = false;
  channels[5].directionFlipped = true;
  channels[5].leftTurn = false;
  channels[5].rightTurn = false;
  channels[5].brake = false;
  channels[5].reverse = false;
  channels[5].type = 1;
  channels[5].order = 2;
  channels[5].numLEDs = 6;
  memset(channels[5].leds,0,sizeof(channels[5].leds));
  memset(channels[5].ledsBuffer,0,sizeof(channels[5].ledsBuffer));
  channels[5].updated = false;
  channels[5].restartRequired = false;
  getDefinition(&(channels[5]), 5);

  channels[6].enabled = true; 
  channels[6].interior = false;
  channels[6].directionFlipped = true;
  channels[6].leftTurn = false;
  channels[6].rightTurn = false;
  channels[6].brake = false;
  channels[6].reverse = false;
  channels[6].type = 1;
  channels[6].order = 2;
  channels[6].numLEDs = 6;
  memset(channels[6].leds,0,sizeof(channels[6].leds));
  memset(channels[6].ledsBuffer,0,sizeof(channels[6].ledsBuffer));
  channels[6].updated = false;
  channels[6].restartRequired = false;
  getDefinition(&(channels[6]), 6);

  channels[7].enabled = true; 
  channels[7].interior = false;
  channels[7].directionFlipped = true;
  channels[7].leftTurn = false;
  channels[7].rightTurn = false;
  channels[7].brake = false;
  channels[7].reverse = false;
  channels[7].type = 1;
  channels[7].order = 2;
  channels[7].numLEDs = 6;
  memset(channels[7].leds,0,sizeof(channels[7].leds));
  memset(channels[7].ledsBuffer,0,sizeof(channels[7].ledsBuffer));
  channels[7].updated = false;
  channels[7].restartRequired = false;
  getDefinition(&(channels[7]), 7);

  channels[8].enabled = true; 
  channels[8].interior = false;
  channels[8].directionFlipped = true;
  channels[8].leftTurn = false;
  channels[8].rightTurn = false;
  channels[8].brake = false;
  channels[8].reverse = false;
  channels[8].type = 1;
  channels[8].order = 2;
  channels[8].numLEDs = 6;
  memset(channels[8].leds,0,sizeof(channels[8].leds));
  memset(channels[8].ledsBuffer,0,sizeof(channels[8].ledsBuffer));
  channels[8].updated = false;
  channels[8].restartRequired = false;
  getDefinition(&(channels[8]), 8);

  channels[9].enabled = true; 
  channels[9].interior = false;
  channels[9].directionFlipped = true;
  channels[9].leftTurn = false;
  channels[9].rightTurn = false;
  channels[9].brake = false;
  channels[9].reverse = false;
  channels[9].type = 1;
  channels[9].order = 2;
  channels[9].numLEDs = 6;
  memset(channels[9].leds,0,sizeof(channels[9].leds));
  memset(channels[9].ledsBuffer,0,sizeof(channels[9].ledsBuffer));
  channels[9].updated = false;
  channels[9].restartRequired = false;
  getDefinition(&(channels[9]), 9);

  channels[10].enabled = true; 
  channels[10].interior = false;
  channels[10].directionFlipped = true;
  channels[10].leftTurn = false;
  channels[10].rightTurn = false;
  channels[10].brake = false;
  channels[10].reverse = false;
  channels[10].type = 1;
  channels[10].order = 2;
  channels[10].numLEDs = 6;
  memset(channels[10].leds,0,sizeof(channels[10].leds));
  memset(channels[10].ledsBuffer,0,sizeof(channels[10].ledsBuffer));
  channels[10].updated = false;
  channels[10].restartRequired = false;
  getDefinition(&(channels[10]), 10);

  channels[11].enabled = true; 
  channels[11].interior = false;
  channels[11].directionFlipped = true;
  channels[11].leftTurn = false;
  channels[11].rightTurn = false;
  channels[11].brake = false;
  channels[11].reverse = false;
  channels[11].type = 1;
  channels[11].order = 2;
  channels[11].numLEDs = 6;
  memset(channels[11].leds,0,sizeof(channels[11].leds));
  memset(channels[11].ledsBuffer,0,sizeof(channels[11].ledsBuffer));
  channels[11].updated = false;
  channels[11].restartRequired = false;
  getDefinition(&(channels[11]), 11);

  channels[12].enabled = true; 
  channels[12].interior = false;
  channels[12].directionFlipped = true;
  channels[12].leftTurn = false;
  channels[12].rightTurn = false;
  channels[12].brake = false;
  channels[12].reverse = false;
  channels[12].type = 1;
  channels[12].order = 2;
  channels[12].numLEDs = 6;
  memset(channels[12].leds,0,sizeof(channels[12].leds));
  memset(channels[12].ledsBuffer,0,sizeof(channels[12].ledsBuffer));
  channels[12].updated = false;
  channels[12].restartRequired = false;
  getDefinition(&(channels[12]), 12);

  channels[13].enabled = true; 
  channels[13].interior = false;
  channels[13].directionFlipped = true;
  channels[13].leftTurn = false;
  channels[13].rightTurn = true;
  channels[13].brake = true;
  channels[13].reverse = true;
  channels[13].type = 1;
  channels[13].order = 2;
  channels[13].numLEDs = 6;
  memset(channels[13].leds,0,sizeof(channels[13].leds));
  memset(channels[13].ledsBuffer,0,sizeof(channels[13].ledsBuffer));
  channels[13].updated = false;
  channels[13].restartRequired = false;
  getDefinition(&(channels[13]), 13);

  channels[14].enabled = true; 
  channels[14].interior = false;
  channels[14].directionFlipped = true;
  channels[14].leftTurn = false;
  channels[14].rightTurn = true;
  channels[14].brake = true;
  channels[14].reverse = false;
  channels[14].type = 1;
  channels[14].order = 2;
  channels[14].numLEDs = 6;
  memset(channels[14].leds,0,sizeof(channels[14].leds));
  memset(channels[14].ledsBuffer,0,sizeof(channels[14].ledsBuffer));
  channels[14].updated = false;
  channels[14].restartRequired = false;
  getDefinition(&(channels[14]), 14);

  channels[15].enabled = true; 
  channels[15].interior = false;
  channels[15].directionFlipped = true;
  channels[15].leftTurn = false;
  channels[15].rightTurn = true;
  channels[15].brake = false;
  channels[15].reverse = false;
  channels[15].type = 1;
  channels[15].order = 2;
  channels[15].numLEDs = 6;
  memset(channels[15].leds,0,sizeof(channels[15].leds));
  memset(channels[15].ledsBuffer,0,sizeof(channels[15].ledsBuffer));
  channels[15].updated = false;
  channels[15].restartRequired = false;
  getDefinition(&(channels[15]), 15);

  Serial.println("Finnished Settting DEFAULT Channel Data");
}
