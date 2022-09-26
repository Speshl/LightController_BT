#include "animations.h"
void setInitialState(AnimationState* animation){
  Serial.println("Start setting Initial Animation State");
  animation->animation = 0;
  animation->palettePreset = 0; //0 is custom
  animation->fps = 20;
  animation->brightness = 255;
  animation->stepSize = 20;
  animation->blending = LINEARBLEND;

  for(int i=0; i<MAX_COLORS*4;i++){
    animation->paletteDescription[i] = 0;
  }
  
  animation->paletteDescription[0] = 0;
  animation->paletteDescription[1] = 255;
  animation->paletteDescription[2] = 0;
  animation->paletteDescription[3] = 255;

  animation->paletteDescription[4] = 255;
  animation->paletteDescription[5] = 0;
  animation->paletteDescription[6] = 255;
  animation->paletteDescription[7] = 255;

  animation->palette.loadDynamicGradientPalette(animation->paletteDescription);
  Serial.println("Finished setting Initial Animation State");
}

void setStateFromBytes(AnimationState* animation, uint8_t state[ANIMATION_STATE_LENGTH]){
  animation->animation = state[0];
  animation->palettePreset = state[1];
  animation->brightness = state[2];
  animation->stepSize = state[3];

  switch(state[4]){
    case 0:
      animation->blending = NOBLEND;
      break;
    case 1:
      animation->blending = LINEARBLEND;
      break;
    default:
      animation->blending = LINEARBLEND;
  }

  animation->fps = state[5];

  byte tempPalette[4*MAX_COLORS];
  for(int i=0; i< MAX_COLORS*4; i++){
    animation->paletteDescription[i] = state[i+6];
    tempPalette[i] = state[i+6];
  }
  animation->palette.loadDynamicGradientPalette(tempPalette);

  if(animation->stepSize == 0){
    animation->stepSize = 1;
  }
  if(animation->fps == 0){
    animation->fps = 1;
  }
  if(animation->brightness == 0){
    animation->brightness = 1;
  }
}

void getStateAsBytes(AnimationState* animation, uint8_t state[ANIMATION_STATE_LENGTH]){
  state[0] = animation->animation;
  state[1] = animation->palettePreset;
  state[2] = animation->brightness;
  state[3] = animation->stepSize;

  switch(animation->blending){
    case NOBLEND:
      state[4] = 0;
      break;
    case LINEARBLEND:
      state[4] = 1;
      break;
    default:
      state[4] = 1;
  }

  state[5] = animation->fps;

  for(int i=0; i<MAX_COLORS*4; i++){
    state[i+6] = animation->paletteDescription[i];
  }
}

void setStateFromJson(AnimationState* animation, JsonObject doc){
  animation->animation = doc["animation"];
  animation->palettePreset = doc["palettePreset"];
  animation->brightness = doc["brightness"];
  animation->stepSize = doc["stepSize"];
  animation->blending = doc["blending"];
  animation->fps = doc["fps"];

  byte tempPalette[4*MAX_COLORS];
  for(int i=0; i< MAX_COLORS*4; i++){
    animation->paletteDescription[i] = doc["paletteDescription"][i];
    tempPalette[i] = doc["paletteDescription"][i];
  }
  animation->palette.loadDynamicGradientPalette(tempPalette);
  

  if(animation->stepSize == 0){
    animation->stepSize = 1;
  }
  if(animation->fps == 0){
    animation->fps = 1;
  }
  if(animation->brightness == 0){
    animation->brightness = 1;
  }
}

void getStateAsJson(AnimationState* animation, JsonObject doc){
  char buffer[10];

  doc["animation"] = animation->animation;
  doc["palettePreset"] = animation->palettePreset;
  doc["brightness"] = animation->brightness;
  doc["stepSize"] = animation->stepSize;

  switch(animation->blending){
    case NOBLEND:
      doc["blending"] = 0;
      break;
    case LINEARBLEND:
      doc["blending"] = 1;
      break;
    default:
      doc["blending"] = 1;
  }

  doc["fps"] = animation->fps;

  for(int i=0; i< MAX_COLORS*4; i++){
    doc["paletteDescription"][i] = animation->paletteDescription[i];
  }
}

std::string getStateAsString(AnimationState* animation){
  std::string returnValue = "";
  returnValue += animation->animation;
  returnValue += animation->palettePreset;
  returnValue += animation->brightness;
  returnValue += animation->stepSize;
  switch(animation->blending){
    case NOBLEND:
      returnValue += '0';
      break;
    case LINEARBLEND:
      returnValue += '1';
      break;
    default:
      returnValue += '1';
  }

  returnValue += animation->fps;

  /*Save palette*/
  for(int i=0; i<MAX_COLORS*4; i++){
    returnValue += animation->paletteDescription[i];
  }
  
  return returnValue;
}

void setStateFromString(AnimationState* animation, std::string input){
  animation->animation = input[0];
  animation->palettePreset = input[1];
  animation->brightness = input[2];
  animation->stepSize = input[3];

  switch(input[4]){
    case 0:
      animation->blending = NOBLEND;
      break;
    case 1:
      animation->blending = LINEARBLEND;
      break;
    default:
      animation->blending = LINEARBLEND;
  }
  
  animation->fps = input[5];

  /*Read palette*/
  byte tempPalette[4*MAX_COLORS];
  for(int i=0; i<4*MAX_COLORS; i++){
    animation->paletteDescription[i] = input[i+6];
    tempPalette[i] = input[i+6];
  }
  switch(animation->palettePreset){
    case 0:
      animation->palette.loadDynamicGradientPalette(tempPalette);
      break;
    case 1:
      animation->palette = RainbowColors_p;
      break;
    case 2:
      animation->palette = RainbowStripeColors_p;
      break;
    case 3:
      animation->palette = PartyColors_p;
      break;
    case 4:
      animation->palette = LavaColors_p;
      break;
    case 5:
      animation->palette = HeatColors_p;
      break;
    case 6:
      animation->palette = CloudColors_p;
      break;
    case 7:
      animation->palette = OceanColors_p;
      break;
    case 8:
      animation->palette = ForestColors_p;
      break;
    default:
      animation->palette = RainbowColors_p;
  }
  if(animation->stepSize == 0){
    animation->stepSize = 1;
  }
  if(animation->fps == 0){
    animation->fps = 1;
  }
  if(animation->brightness == 0){
    animation->brightness = 1;
  }
}

void describePalette(AnimationState* animation){
  Serial.println("Start Describing Palette");
  for(int i=0; i<MAX_COLORS; i++){
    Serial.print(animation->paletteDescription[i*4]);
    Serial.print(" | ");
    Serial.print(animation->paletteDescription[i*4+1]);
    Serial.print(" | ");
    Serial.print(animation->paletteDescription[i*4+2]);
    Serial.print(" | ");
    Serial.println(animation->paletteDescription[i*4+3]);
  }
  Serial.println("Done Describing Palette");
}

void describeState(AnimationState* animation){
  Serial.println("Describing Animation State - ");
  Serial.print("Animation: ");
  Serial.println((uint8_t) animation->animation);//So it displays as number
  Serial.print("FPS: ");
  Serial.println((uint8_t) animation->fps);//So it displays as number
  Serial.print("Brightness: ");
  Serial.println((uint8_t) animation->brightness);//So it displays as number
  Serial.print("StepSize: ");
  Serial.println(animation->stepSize);//So it displays as number
  Serial.print("Palette Preset: ");
  Serial.println(animation->palettePreset);
  describePalette(animation);
}

std::string getColorPos(AnimationState* animation, uint8_t index){
  if(index >= 0 && index < MAX_COLORS){
    char buffer[5];
    uint8_t startPos = index * 4;
    return itoa(animation->paletteDescription[startPos],buffer, 10);
  }else{
    return "0";
  }
}

void setColorPos(AnimationState* animation, uint8_t index, std::string value){
  if(index >= 0 && index < MAX_COLORS){
    animation->paletteDescription[index*4] = atoi(value.c_str());
  }
}

std::string getColorHex(AnimationState* animation, uint8_t index){
  if(index >= 0 && index < MAX_COLORS){
    std::string returnValue = "#";
    char rValue[10];
    char gValue[10];
    char bValue[10];
    uint8_t startPos = index * 4;

    std::string rString = itoa(animation->paletteDescription[startPos+1],rValue, 16);
    std::string gString= itoa(animation->paletteDescription[startPos+2],gValue, 16);
    std::string bString= itoa(animation->paletteDescription[startPos+3],bValue, 16);

    if(rString.length() <2){
      returnValue += "0"+rString;
    }else{
      returnValue += rString;
    }

    if(gString.length() <2){
      returnValue += "0"+gString;
    }else{
      returnValue += gString;
    }

    if(bString.length() <2){
      returnValue += "0"+bString;
    }else{
      returnValue += bString;
    }
    return returnValue;
  }else{
    return "#000000";
  }
}

void setColorHex(AnimationState* animation, uint8_t index, std::string value){
  if(index >= 0 && index < MAX_COLORS && value.length() == 7){
    uint8_t startPos = index * 4;

    std::string rValue = value.substr(1,2);
    std::string gValue = value.substr(3,2);
    std::string bValue = value.substr(5,2);

    animation->paletteDescription[startPos+1] = (byte)strtol(rValue.c_str(), NULL, 16);
    animation->paletteDescription[startPos+2] = (byte)strtol(gValue.c_str(), NULL, 16);
    animation->paletteDescription[startPos+3] = (byte)strtol(bValue.c_str(), NULL, 16);
  }
}

std::string getAnimationIndex(AnimationState* animation){
  char buffer[5];
  return itoa(animation->animation,buffer, 10);
}

std::string getStepSize(AnimationState* animation){
  char buffer[10];
  return itoa(animation->stepSize,buffer, 10);
}

std::string getFPS(AnimationState* animation){
  char buffer[10];
  return itoa(animation->fps,buffer, 10);
}

std::string getBrightness(AnimationState* animation){
  char buffer[10];
  return itoa(animation->brightness,buffer, 10);
}

std::string getBlending(AnimationState* animation){
  if(animation->blending == 1){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getColorData(AnimationState* animation){
  std::string fullData;
  for(int i=0; i<40; i++){
    if(i%4 == 0){
      fullData += "\r\n";
    }
    char buffer[5];
    int temp = animation->paletteDescription[i];
    fullData += itoa(temp, buffer, 10);
    if(i != 39){
      fullData += ",";
    }
  }
  return fullData;
}

void setColorData(AnimationState* animation, std::string colorData){
  int pos = 0;
  std::string buffer[MAX_COLORS*4];

  for(int i=0; i<colorData.length(); i++){
    if(colorData[i] == ','){
      pos++;
    }else if(colorData[i] != ','){
      buffer[pos] += colorData[i];
    }
  }

  for(int i=0; i<MAX_COLORS*4; i++){
    int bufferInt = atoi(buffer[i].c_str());
    animation->paletteDescription[i] = bufferInt;
    Serial.print("Setting Palette ");
    Serial.print(i);
    Serial.print(" to value ");
    Serial.println(bufferInt);
  }
  animation->palette.loadDynamicGradientPalette(animation->paletteDescription);
}

void animationUpdated(AnimationState* animation, ChannelState channels[MAX_CHANNELS]){
  animation->stepIndex = 0;
  clear(channels);
}

void incrementStepIndex(AnimationState* animation, uint8_t stepSize){
  animation->stepIndexScaler += stepSize;
  while(animation->stepIndexScaler >= STEP_SIZE_SCALE){
    animation->stepIndexScaler -= STEP_SIZE_SCALE;
    animation->stepIndex++;
  }
}

void incrementStepIndex(AnimationState* animation){
  incrementStepIndex(animation, animation->stepSize);
}

void solidFromPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  setColorAll(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
}

void sweepFrontPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorUpRow(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepRearPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorDownRow(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepUpPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorUpHeight(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepDownPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorDownHeight(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepRightPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorUpCol(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Sweep Up From Palette command");
}

void sweepLeftPalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Sweep Up From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  shiftColorDownCol(location, channels, color);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Sweep Up From Palette command");
}

void showLine(ChannelState channels[MAX_CHANNELS], LocationState* location,CRGB color, int x1, int y1, int x2, int y2){
  const bool steep = (fabs(y2-y1) > fabs(x2-x1));
  if(steep){
    std::swap(x1, y1);
    std::swap(x2, y2);
  }

  if(x1 > x2){
    std::swap(x1, x2);
    std::swap(y1,y2);
  }

  const float dx = x2-x1;
  const float dy = fabs(y2-y1);
  float error = dx /2.0f;
  const int ystep = (y1 < y2) ? 1 : -1;
  int y = (int)y1;
  const int maxX = int(x2);

  for(int x=(int)x1; x<=maxX; x++){
    if(steep){
      setColorAtLocationIgnoreHeight(location, channels, y, x, color);
    }else{
      setColorAtLocationIgnoreHeight(location, channels, x, y, color);
    }

    error -= dy;
    if(error <0){
      y += ystep;
      error += dx;
    }
  }
}

void spinClockwisePalette(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  //Serial.println("Starting Spin Clockwise From Palette command");
  CRGB color = ColorFromPalette( animation->palette, animation->stepIndex, animation->brightness, animation->blending);
  if(animation->xPos < LOCATION_GRID_SIZE-1 && animation->yPos == 0){ //Start by sweeping up rows
    animation->xPos++;
    //Serial.println("Going Up Rows");
  }else if(animation->xPos >= LOCATION_GRID_SIZE-1 && animation->yPos < LOCATION_GRID_SIZE-1){ //Next sweeping "down" cols
    animation->yPos++;
    //Serial.println("Going Down Cols");
  }else if(animation->xPos <= LOCATION_GRID_SIZE -1 && animation->xPos > 0 && animation->yPos == LOCATION_GRID_SIZE -1){ //Third sweeping back down rows
    animation->xPos--;
    //Serial.println("Going Down Rows");
  }else if(animation->xPos <= LOCATION_GRID_SIZE -1 && animation->yPos > 0){ //Lastly sweep back up columns
    animation->yPos--;
    //Serial.println("Going Up Cols");
  }

  uint8_t midX = floor(LOCATION_GRID_SIZE / 2);
  uint8_t midY = floor(LOCATION_GRID_SIZE / 2);
  showLine(channels, location, color, animation->yPos, animation->xPos, midY, midX);
  applyBufferAndShow(channels,switches);
  incrementStepIndex(animation);
  //Serial.println("Finished Spin Clockwise From Palette command");
}

void brakeAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches){
  fillBrake(channels, CRGB::Red);
}

void reverseAnimation(ChannelState channels[MAX_CHANNELS], SwitchState* switches){
  fillReverse(channels, CRGB::White);
}

void leftTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, CRGB color){
  fillLeftTurn(channels, color);
}

void rightTurnAnimationWithColor(ChannelState channels[MAX_CHANNELS], SwitchState* switches, CRGB color){
  fillRightTurn(channels, color);
}

void signalRampUp(ChannelState channels[MAX_CHANNELS], SwitchState* switches){
  bool updated = false;
  if(switches->reverse){
    updated = true;
    reverseAnimation(channels, switches);
  }
  //brake light is next highest importance
  if(switches->brake){
    updated = true;
    Serial.println("Brake ON");
    brakeAnimation(channels, switches);
  }
  //turn signals are highest priority because they are intermittent and will default back to other signal if applicable
  if(switches->leftTurn){
    updated = true;
    Serial.println("Left Turn ON");
    leftTurnAnimationWithColor(channels, switches, CRGB::Orange); 
  }
  if(switches->rightTurn){
    updated = true;
    Serial.println("Right Turn ON");
    rightTurnAnimationWithColor(channels, switches, CRGB::Orange);
  }
  //Signals will show when main animation updates strips
}

void signalRampDown(ChannelState channels[MAX_CHANNELS], SwitchState* switches){
  //Set turn signals back to off
  bool updated = false;
  if(switches->leftTurn){
    updated = true;
    Serial.println("Left Turn OFF");
    leftTurnAnimationWithColor(channels, switches, CRGB::Black); 
  }
  if(switches->rightTurn){
    updated = true;
    Serial.println("Right Turn OFF");
    rightTurnAnimationWithColor(channels, switches, CRGB::Black);
  }
  //turn any turn signal back to to reverse color if they overlapped
  if(switches->reverse){
    updated = true;
    reverseAnimation(channels, switches);
  }
  //finally turn back on any overlapping breaklights
  if(switches->brake){
    updated = true;
    brakeAnimation(channels, switches);
  }
   //Signals will show when main animation updates strips
}

uint16_t getStepDelay(AnimationState* animation, uint16_t frameTime){
  int delayTarget = floor(1000/animation->fps);
  if(frameTime > delayTarget){
    return 1;
  }else{
      return delayTarget - frameTime;
  }
}

void updateAnimation(AnimationState* animation, ChannelState channels[MAX_CHANNELS], SwitchState* switches, LocationState* location){
  switch(animation->animation){
      case 0:
        solidFromPalette(animation, channels, switches, location);
        break;
      case 1:
        sweepFrontPalette(animation, channels, switches, location);
        break;
      case 2:
        sweepRearPalette(animation, channels, switches, location);
        break;
      case 3:
        sweepRightPalette(animation, channels, switches, location);
        break;
      case 4:
        sweepLeftPalette(animation, channels, switches, location);
        break;
      case 5:
        sweepUpPalette(animation, channels, switches, location);
        break;
      case 6:
        sweepDownPalette(animation, channels, switches, location);
        break;
      case 7:
        spinClockwisePalette(animation, channels, switches, location);
        break;
      default:
        solidFromPalette(animation, channels, switches, location);
  }
}