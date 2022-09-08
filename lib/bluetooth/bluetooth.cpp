#include "bluetooth.h"

class Callbacks: public BLECharacteristicCallbacks {
  private:
    BluetoothState* bluetooth;

  public:
    Callbacks(BluetoothState* bt){
      bluetooth = bt;
    }
    void onWrite(BLECharacteristic *characteristic){
      std::string uuid = characteristic->getUUID().toString();
      //notify that an update happened
    }
};

void initializeBluetooth(BluetoothState* bluetooth){
  Serial.println("Initializing Bluetooth");
  
  BLEDevice::init("LightController");
  bluetooth->server = BLEDevice::createServer();
  bluetooth->generalService = bluetooth->server->createService(BLEUUID(generalServiceUUID),100,0);

  bluetooth->switchCharacteristic = bluetooth->generalService->createCharacteristic(BLEUUID(switchCharacteristicUUID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  bluetooth->switchCharacteristic->setCallbacks(new Callbacks(bluetooth));
  bluetooth->animationCharacteristic = bluetooth->generalService->createCharacteristic(BLEUUID(animationCharacteristicUUID), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
  bluetooth->animationCharacteristic->setCallbacks(new Callbacks(bluetooth));


  for(int i=0; i<MAX_CHANNELS; i++){
    bluetooth->bleChannelGroups[i].details = bluetooth->generalService->createCharacteristic(BLEUUID(channelServiceUUIDs[i][0]), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    bluetooth->bleChannelGroups[i].details->setCallbacks(new Callbacks(bluetooth));
    bluetooth->bleChannelGroups[i].locations = bluetooth->generalService->createCharacteristic(BLEUUID(channelServiceUUIDs[i][1]), BLECharacteristic::PROPERTY_READ | BLECharacteristic::PROPERTY_WRITE);
    bluetooth->bleChannelGroups[i].locations->setCallbacks(new Callbacks(bluetooth));
  }
  bluetooth->generalService->start();
  bluetooth->advertising = bluetooth->server->getAdvertising();
  bluetooth->advertising->start();

  Serial.println("Bluetooth Initialized");
}

int getBTClientCount(BluetoothState* bluetooth){
  if(bluetooth == NULL || bluetooth->server == NULL){
    return 0;
  }

  return bluetooth->server->getConnectedCount();
}

std::string getSwitchStateBLECharacteristic(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize){
  std::string returnedData = bluetooth->switchCharacteristic->getValue();
  memcpy(buffer, returnedData.data(), bufferSize);
}

void getAnimationStateBLECharacteristic(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize){
  std::string returnedData = bluetooth->animationCharacteristic->getValue();
  memcpy(buffer, returnedData.data(), bufferSize);
}

void getChannelDetailsBLECharacteristic(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize){
  std::string returnedData = bluetooth->bleChannelGroups[i].details->getValue();
  memcpy(buffer, returnedData.data(), bufferSize);
}

void getChannelLocationsBLECharacteristic(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize){
  std::string returnedData = bluetooth->bleChannelGroups[i].locations->getValue();
  memcpy(buffer, returnedData.data(), bufferSize);
}

void setSwitchBLECharacteristic(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize){
  bluetooth->switchCharacteristic->setValue(buffer,bufferSize);
}

void setAnimationBLECharacteristic(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize){
  bluetooth->animationCharacteristic->setValue(buffer,bufferSize);
}

void setChannelDetailsBLECharacteristic(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize){
  bluetooth->bleChannelGroups[i].details->setValue(buffer,bufferSize);
}

void setChannelLocationsBLECharacteristic(BluetoothState* bluetooth, int i, uint8_t buffer[], int bufferSize){
  bluetooth->bleChannelGroups[i].locations->setValue(buffer,bufferSize);
}