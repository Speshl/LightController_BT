#include "storage.h"

std::string to_string( int x ) {
  int length = snprintf( NULL, 0, "%d", x );
  assert( length >= 0 );
  char* buf = new char[length + 1];
  snprintf( buf, length + 1, "%d", x );
  std::string str( buf );
  delete[] buf;
  return str;
}

void initializeStorage(Preferences* preferences){
  preferences->begin("Storage", false);
}

bool isFirstRun(Preferences* preferences){
  bool firstRun = preferences->getBool("FirstRun", true);
  if(firstRun == true){
    preferences->putBool("FirstRun", false);
  }
  return firstRun;
}

void saveSwitchState(Preferences* preferences, uint8_t value[], int valueSize){
  preferences->putBytes("Switch", value, valueSize);
}

void saveAnimationState(Preferences* preferences, uint8_t value[], int valueSize){
  preferences->putBytes("Animation", value, valueSize);
}

void saveChannelDetailState(Preferences* preferences, int index, uint8_t value[], int valueSize){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Detail" + temp;
  preferences->putBytes(key.c_str(), value, valueSize);
}

void saveChannelLocationState(Preferences* preferences, int index, uint8_t value[], int valueSize){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Loc" + temp;
  preferences->putBytes(key.c_str(), value, valueSize);
}

bool loadSwitchState(Preferences* preferences, uint8_t value[], int valueSize){
  int bytesRead = preferences->getBytes("Switch",value,valueSize);
  if(bytesRead == valueSize){
    return true;
  }else{
    return false;
  }
}

bool loadAnimationState(Preferences* preferences, uint8_t value[], int valueSize){
  int bytesRead = preferences->getBytes("Animation",value,valueSize);
  if(bytesRead == valueSize){
    return true;
  }else{
    return false;
  }
}

bool loadChannelDetailState(Preferences* preferences, int index, uint8_t value[], int valueSize){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Detail" + temp;
  int bytesRead = preferences->getBytes(key.c_str(),value,valueSize);
  if(bytesRead == valueSize){
    return true;
  }else{
    return false;
  }
}

bool loadChannelLocationState(Preferences* preferences, int index, uint8_t value[], int valueSize){
  char numstr[21];
  std::string temp = itoa(index, numstr, 10);
  std::string key = "Loc" + temp;
  int bytesRead = preferences->getBytes(key.c_str(),value, valueSize);
  if(bytesRead == valueSize){
    return true;
  }else{
    return false;
  }
}
