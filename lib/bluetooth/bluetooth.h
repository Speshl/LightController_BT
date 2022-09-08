#ifndef BLUETOOTH_H
#define BLUETOOTH_H

#define MAX_CHANNELS 16

#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>

const char generalServiceUUID[37] = {"beb5483e-36e1-4688-b7f0-ea07361b2000"};
const char animationCharacteristicUUID[37] = {"beb5483e-36e1-4688-b7f0-ea07361b2001"};
const char switchCharacteristicUUID[37] = {"beb5483e-36e1-4688-b7f0-ea07361b2002"};

const char channelServiceUUIDs[MAX_CHANNELS][2][37] = {
    {"beb5483e-36e1-4688-b7f0-ea07361b2600","beb5483e-36e1-4688-b7f0-ea07361b2601"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2610","beb5483e-36e1-4688-b7f0-ea07361b2611"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2620","beb5483e-36e1-4688-b7f0-ea07361b2621"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2630","beb5483e-36e1-4688-b7f0-ea07361b2631"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2640","beb5483e-36e1-4688-b7f0-ea07361b2641"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2650","beb5483e-36e1-4688-b7f0-ea07361b2651"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2660","beb5483e-36e1-4688-b7f0-ea07361b2661"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2670","beb5483e-36e1-4688-b7f0-ea07361b2671"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2680","beb5483e-36e1-4688-b7f0-ea07361b2681"},
    {"beb5483e-36e1-4688-b7f0-ea07361b2690","beb5483e-36e1-4688-b7f0-ea07361b2691"},
    {"beb5483e-36e1-4688-b7f0-ea07361b26A0","beb5483e-36e1-4688-b7f0-ea07361b26A1"},
    {"beb5483e-36e1-4688-b7f0-ea07361b26B0","beb5483e-36e1-4688-b7f0-ea07361b26B1"},
    {"beb5483e-36e1-4688-b7f0-ea07361b26C0","beb5483e-36e1-4688-b7f0-ea07361b26C1"},
    {"beb5483e-36e1-4688-b7f0-ea07361b26D0","beb5483e-36e1-4688-b7f0-ea07361b26D1"},
    {"beb5483e-36e1-4688-b7f0-ea07361b26E0","beb5483e-36e1-4688-b7f0-ea07361b26E1"},
    {"beb5483e-36e1-4688-b7f0-ea07361b26F0","beb5483e-36e1-4688-b7f0-ea07361b26F1"},
};

struct BLEChannelGroup{
    BLECharacteristic *details;
    BLECharacteristic *locations;
};

struct BluetoothState {
  BLEServer *server;
  BLEService *generalService;
  BLECharacteristic *animationCharacteristic;
  BLECharacteristic *switchCharacteristic;
  BLEChannelGroup bleChannelGroups[MAX_CHANNELS];
  BLEAdvertising *advertising;
};

void initializeBluetooth(BluetoothState* bluetooth);

int getBTClientCount(BluetoothState* bluetooth);

void getSwitchStateAsString(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize);
void getAnimationStateAsString(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize);
void getChannelDetailsAsString(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize);
void getChannelLocationsAsString(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize);

void setSwitchBLECharacteristic(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize);
void setAnimationBLECharacteristic(BluetoothState* bluetooth, uint8_t buffer[],int bufferSize);
void setChannelDetailsBLECharacteristic(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize);
void setChannelLocationsBLECharacteristic(BluetoothState* bluetooth, int i, uint8_t buffer[],int bufferSize);

#endif