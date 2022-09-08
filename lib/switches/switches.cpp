#include "switches.h"

void initializeSwitches(SwitchState * switches){
  switches->leftTurn = false;
  switches->rightTurn = false;
  switches->brake = false;
  switches->reverse = false;
  switches->interior = false;
  switches->leftTurn = false;
  //switches->uiOverrideSwitch = false;
}

bool getSwitchState(int pin){
  if(digitalRead(pin) == HIGH){//Switch is considered on when + signal is applied
    return true;//TODO: SET BACK TO FALSE
  }else{
    return false;
  }
}


//Commented out so UI will override the actual switches
bool updateSwitchState(SwitchState* switches){
  //Serial.println("Start Updating Switch State");
 /* if(switches->uiOverride == true){ //UI Override is on so we don't want to use state changes from the physical switches
    return false;
  }

  bool leftTurn = getSwitchState(LEFT_TURN_PIN);
  bool rightTurn = getSwitchState(RIGHT_TURN_PIN);
  bool brake = getSwitchState(BRAKE_PIN);
  bool reverse = getSwitchState(REVERSE_PIN);
  bool interior = getSwitchState(INTERIOR_LIGHT_PIN);
  bool updated = false;

  if(switches->leftTurn != leftTurn){
    switches->leftTurn = leftTurn;
    updated = true;
  }
  
  if(switches->rightTurn != rightTurn){
    switches->rightTurn = rightTurn;
    updated = true;
  }
  
  if(switches->brake != brake){
    switches->brake = brake;
    updated = true;
  }
  
  if(switches->reverse != reverse){
    switches->reverse = reverse;
    updated = true;
  }

  if(switches->interior  != interior){
    switches->interior = interior;
    updated = true;
  }
  //Serial.println("Finished Updating Switch State");
  return updated;*/
  return false;
}

void getStateAsJson(SwitchState* switches, JsonObject doc){
  doc["leftTurnSwitch"] = switches->leftTurn;
  doc["rightTurnSwitch"] = switches->rightTurn;
  doc["brakeSwitch"] = switches->brake;
  doc["reverseSwitch"] = switches->reverse;
  doc["interiorSwitch"] = switches->interior;
  doc["uiOverrideSwitch"] = switches->uiOverride;
}

void setStateFromJson(SwitchState* switches, JsonObject doc){
  switches->leftTurn = doc["leftTurnSwitch"];
  switches->rightTurn = doc["rightTurnSwitch"];
  switches->brake = doc["brakeSwitch"];
  switches->reverse = doc["reverseSwitch"];
  switches->interior = doc["interiorSwitch"];
  switches->uiOverride = doc["uiOverrideSwitch"];
}

void getStateAsBytes(SwitchState* switches, uint8_t state[SWITCH_STATE_LENGTH]){
  state[0] = switches->leftTurn;
  state[1] = switches->rightTurn;
  state[2] = switches->brake;
  state[3] = switches->reverse;
  state[4] = switches->interior;
  state[5] = switches->uiOverride;
}

void setStateFromBytes(SwitchState* switches, uint8_t state[SWITCH_STATE_LENGTH]){
  switches->leftTurn = state[0];
  switches->rightTurn = state[1];
  switches->brake = state[2];
  switches->reverse = state[3];
  switches->interior = state[4];
  switches->uiOverride = state[5];
}

std::string getStateAsString(SwitchState* switches){
  std::string returnString = "";
  returnString += switches->leftTurn;
  returnString += switches->rightTurn;
  returnString += switches->brake;
  returnString += switches->reverse;
  returnString += switches->interior;
  returnString += switches->uiOverride;
  return returnString;
}

void setStateFromString(SwitchState* switches, std::string inputValue){
  if(inputValue.length() != SWITCH_STATE_LENGTH){
    Serial.print("Expected switch state length of ");
    Serial.print(SWITCH_STATE_LENGTH);
    Serial.print(" - Found length of ");
    Serial.println(inputValue.length());
  }
  switches->leftTurn = inputValue[0];
  switches->rightTurn = inputValue[1];
  switches->brake = inputValue[2];
  switches->reverse = inputValue[3];
  switches->interior = inputValue[4];
  switches->uiOverride = inputValue[5];
}

void describeState(SwitchState* switches){
  Serial.println("Describing Switch State - ");
  Serial.print("LeftTurn: ");
  Serial.println(switches->leftTurn);//So it displays as number
  Serial.print("RightTurn: ");
  Serial.println(switches->rightTurn);//So it displays as number
  Serial.print("Brake: ");
  Serial.println(switches->brake);//So it displays as number
  Serial.print("Reverse: ");
  Serial.println(switches->reverse);//So it displays as numberer
  Serial.print("Interior: ");
  Serial.println(switches->interior);//So it displays as number
  Serial.print("UI Override: ");
  Serial.println(switches->uiOverride);//So it displays as number
}

std::string getLeftTurn(SwitchState* switches){
  if(switches->leftTurn){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getRightTurn(SwitchState* switches){
  if(switches->rightTurn){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getReverse(SwitchState* switches){
  if(switches->reverse){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getBrake(SwitchState* switches){
  if(switches->brake){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getInterior(SwitchState* switches){
  if(switches->interior){
    return "checked=true";
  }else{
    return "";
  }
}

std::string getUIOverride(SwitchState* switches){
  if(switches->uiOverride){
    return "checked=true";
  }else{
    return "";
  }
}
