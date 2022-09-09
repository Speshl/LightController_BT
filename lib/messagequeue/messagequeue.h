#ifndef MESSAGEQUEUE_H
#define MESSAGEQUEUE_H
#include <Arduino.h>

//Message Types
#define MessageType int
#define NO_MESSAGE 0
#define COMMAND_MESSAGE 1
#define ANIMATION_MESSAGE 2
#define SWITCH_MESSAGE 3
#define CHANNEL_MESSAGE 4
#define CHANNELPOS_MESSAGE 5

//Signal Commands
#define RAMP_UP 1
#define RAMP_DOWN 2

//Commands
#define Command int
#define NO_COMMAND 0
#define SAVE_COMMAND 1
#define RESET_COMMAND 2
#define SEND_SWITCHES 3
#define READ_SWITCHES 4
#define SEND_ANIMATION 5
#define READ_ANIMATION 6
#define DUMMY_COMMAND 7
#define SEND_CHANNEL_START 100 //Subtract 100 to get Channel to send
#define READ_CHANNEL_START 200 //Subtract 200 to get Channel to send
#define SEND_CHANNELPOS_START 300 //Subtract 300 to get ChannelPOS to send
#define READ_CHANNELPOS_START 400 //Subtract 400 to get ChannelPOS to send
#define END_COMMAND 500 //End of command range

static QueueHandle_t signalQueue;
static QueueHandle_t commandQueue;
static QueueHandle_t switchQueue;
static QueueHandle_t animationQueue;
static QueueHandle_t channelQueue;
static QueueHandle_t channelPosQueue;

#define SWITCH_MESSAGE_LENGTH 6
#define ANIMATION_MESSAGE_LENGTH 46
#define CHANNEL_MESSAGE_LENGTH 11
#define CHANNELPOS_MESSAGE_LENGTH 300

void initializeMessageQueue();

int getSignalCommandFromQueue(int wait);
void sendSignalCommandToQueue(int command);

int getCommandFromQueue(int wait);
void sendCommandToQueue(int command);

bool getSwitchFromQueue(uint8_t state[SWITCH_MESSAGE_LENGTH]);
void sendSwitchToQueue(uint8_t state[SWITCH_MESSAGE_LENGTH]);

bool getAnimationFromQueue(uint8_t state[ANIMATION_MESSAGE_LENGTH]);
void sendAnimationToQueue(uint8_t state[ANIMATION_MESSAGE_LENGTH]);

bool getChannelFromQueue(uint8_t state[CHANNEL_MESSAGE_LENGTH]);
void sendChannelToQueue(uint8_t state[CHANNEL_MESSAGE_LENGTH]);

bool getChannelPosFromQueue(uint8_t buffer[CHANNELPOS_MESSAGE_LENGTH]);
void sendChannelPosToQueue(uint8_t buffer[CHANNELPOS_MESSAGE_LENGTH]);

#endif