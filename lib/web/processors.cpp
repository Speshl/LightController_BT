#include "web.h"


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
  if(var == "SOLID_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 0){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPFRONT_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 1){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPREAR_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 2){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPRIGHT_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 3){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPLEFT_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 4){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPUP_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 5){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SWEEPDOWN_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 6){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "SPINCLOCK_ANIMATION_SELECTED"){
    if(globalState->animation.animation == 7){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "REPLACE_ANIMATION_FPS"){
    return getFPS(&globalState->animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_BRIGHTNESS"){
    return getBrightness(&globalState->animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_BLEND"){
    return getBlending(&globalState->animation).c_str();
  }
  if(var == "REPLACE_ANIMATION_STEPSIZE"){
    return getStepSize(&globalState->animation).c_str();
  }
  if(var == "CUSTOM_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 0){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "RAINBOW_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 1){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "RAINBOWSTRIPE_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 2){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "PARTY_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 3){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "LAVA_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 4){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "HEAT_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 5){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "CLOUD_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 6){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "OCEAN_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 7){
        return "selected";
    }else{
        return "";
    }
  }
  if(var == "FOREST_PALETTE_SELECTED"){
    if(globalState->animation.palettePreset == 8){
        return "selected";
    }else{
        return "";
    }
  }

  for(int i=0; i<MAX_COLORS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "COLOR_"+stringNumber;
    prefix += "_";
    if(var == prefix+"POS"){
        return getColorPos(&globalState->animation, i).c_str();
    }
    if(var == prefix+"HEX"){
      return getColorHex(&globalState->animation, i).c_str();
    }
  }
  return String();
}

String switchesProcessor(const String& var){
  if(var == "REPLACE_SWITCH_LEFTTURN"){
    return getLeftTurn(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_RIGHTTURN"){
    return getRightTurn(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_REVERSE"){
    return getReverse(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_BRAKE"){
    return getBrake(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_INTERIOR"){
    return getInterior(&globalState->switches).c_str();
  }
  if(var == "REPLACE_SWITCH_UIOVERRIDE"){
    return getUIOverride(&globalState->switches).c_str();
  }
  return String();
}

String channelProcessor(const String& var){
  for(int i=0; i<MAX_CHANNELS; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "CHANNEL_"+stringNumber;
    prefix += "_";
    if( var == prefix+"ENABLED"){
      return getChannelEnabled(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"INTERIOR"){
      return getChannelInterior(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"DIRECTIONFLIPPED"){
      return getChannelDirectionFlipped(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"LEFTTURN"){
      return getChannelLeftTurn(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"RIGHTTURN"){
      return getChannelRightTurn(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"BRAKE"){
      return getChannelBrake(&globalState->channels[i]).c_str();
    }
    if( var == prefix+"REVERSE"){
      return getChannelReverse(&globalState->channels[i]).c_str();
    }

    if( var == "TYPE0_"+prefix+"SELECTED"){
      if(globalState->channels[i].type == 0){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "TYPE1_"+prefix+"SELECTED"){
      if(globalState->channels[i].type == 1){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "RGB_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 0){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "RBG_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 1){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "BRG_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 2){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "BGR_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 3){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "GBR_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 4){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == "GRB_"+prefix+"SELECTED"){
      if(globalState->channels[i].order == 5){
        return "selected";
      }
      else{
        return "";
      }
    }

    if( var == prefix+"NUMLEDS"){
      return getChannelNumLEDs(&globalState->channels[i]).c_str();
    }
  }
  return String();
}

String channel0LocProcessor(const String& var){
  int channelIndex = 0;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "class=\"pos-grid\"";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel1LocProcessor(const String& var){
  int channelIndex = 1;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel2LocProcessor(const String& var){
  int channelIndex = 2;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel3LocProcessor(const String& var){
  int channelIndex = 3;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel4LocProcessor(const String& var){
  int channelIndex = 4;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel5LocProcessor(const String& var){
  int channelIndex = 5;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel6LocProcessor(const String& var){
  int channelIndex = 6;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel7LocProcessor(const String& var){
  int channelIndex = 7;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel8LocProcessor(const String& var){
  int channelIndex = 8;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel9LocProcessor(const String& var){
  int channelIndex = 9;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel10LocProcessor(const String& var){
  int channelIndex = 10;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel11LocProcessor(const String& var){
  int channelIndex = 11;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel12LocProcessor(const String& var){
  int channelIndex = 12;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel13LocProcessor(const String& var){
  int channelIndex = 13;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel14LocProcessor(const String& var){
  int channelIndex = 14;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
}

String channel15LocProcessor(const String& var){
  int channelIndex = 15;

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
    return String(globalState->channels[channelIndex].numLEDs);
  }

  for(int i=0; i<globalState->channels[channelIndex].numLEDs; i++){
    char buffer[10];
    String stringNumber = itoa(i,buffer,10);
    String prefix = "LOC_"+stringNumber+"_";
    if(var == prefix+"HIDDEN"){
      return "";
    }
    if(var == prefix+"ROW"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringRow.c_str();
    }
    if(var == prefix+"COL"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringCol.c_str();
    }
    if(var == prefix+"HEIGHT"){
      std::string stringRow;
      std::string stringCol;
      std::string stringHeight;
      getChannelLocationAtPosition(&globalState->location, channelIndex, i, &stringRow, &stringCol, &stringHeight);
      return stringHeight.c_str();
    }
  }
  return "hidden";
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
