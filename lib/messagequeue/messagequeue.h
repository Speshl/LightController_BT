#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <Arduino.h>

//Request/Controller Commands
#define INACTIVE 0
#define ACTIVE 1

//Signal Commands
#define NO_SIGNAL 0
#define RAMP_UP 1
#define RAMP_DOWN 2

static QueueHandle_t controllerQueue;
static QueueHandle_t requestQueue;
static QueueHandle_t signalQueue;

#define SWITCH_MESSAGE_LENGTH 6
#define ANIMATION_MESSAGE_LENGTH 46
#define CHANNEL_MESSAGE_LENGTH 11
#define CHANNELPOS_MESSAGE_LENGTH 300

void initializeMessageQueue();

int peekControllerStatusFromQueue();
int getControllerStatusFromQueue();
bool sendControllerStatusToQueue(int command);

int peekRequestStatusFromQueue();
int getRequestStatusFromQueue();
bool sendRequestStatusToQueue(int command);

int getSignalCommandFromQueue();
bool sendSignalCommandToQueue(int command);

int peekControllerStatusFromQueueFromISR();
int getControllerStatusFromQueueFromISR();
bool sendControllerStatusToQueueFromISR(int command);

int peekRequestStatusFromQueueFromISR();
int getRequestStatusFromQueueFromISR();
bool sendRequestStatusToQueueFromISR(int command);

int getSignalCommandFromQueueFromISR();
bool sendSignalCommandToQueueFromISR(int command);


#endif