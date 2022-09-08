#include "messagequeue.h"

void initializeMessageQueue(){
    Serial.println("Building Message Queues");
    commandQueue = xQueueCreate( 1, sizeof(int));
    if(commandQueue == NULL){
        Serial.println("Error creating commandQueue");
    }

    switchQueue = xQueueCreate(1, SWITCH_MESSAGE_LENGTH);
    if(switchQueue == NULL){
        Serial.println("Error creating switchQueue");
    }

    animationQueue = xQueueCreate(1, ANIMATION_MESSAGE_LENGTH);
    if(animationQueue == NULL){
        Serial.println("Error creating animationQueue");
    }

    channelQueue = xQueueCreate(1, CHANNEL_MESSAGE_LENGTH);
    if(channelQueue == NULL){
        Serial.println("Error creating channelQueue");
    }

    channelPosQueue = xQueueCreate(1, CHANNELPOS_MESSAGE_LENGTH);
    if(channelPosQueue == NULL){
        Serial.println("Error creating channelPosQueue");
    }
}

int getCommandFromQueue(int wait){
    int command;
    bool found = xQueueReceive(commandQueue, &command, wait);
    if(found){
        return command;
    }else{
        return NO_COMMAND;
    }
}

void sendCommandToQueue(int command){
    xQueueSend(commandQueue, &command, portMAX_DELAY);
}

bool getSwitchFromQueue(uint8_t state[ANIMATION_MESSAGE_LENGTH]){
    return xQueueReceive(switchQueue, state, portMAX_DELAY);
}

void sendSwitchToQueue(uint8_t state[ANIMATION_MESSAGE_LENGTH]){
    xQueueSend(switchQueue, state, portMAX_DELAY);
}

bool getAnimationFromQueue(uint8_t state[ANIMATION_MESSAGE_LENGTH]){
    return xQueueReceive(animationQueue, state, portMAX_DELAY);
}

void sendAnimationToQueue(uint8_t state[ANIMATION_MESSAGE_LENGTH]){
    xQueueSend(animationQueue,state, portMAX_DELAY);
}

bool getChannelFromQueue(uint8_t state[CHANNEL_MESSAGE_LENGTH]){
    return xQueueReceive(channelQueue, state, portMAX_DELAY);
}

void sendChannelToQueue(uint8_t state[CHANNEL_MESSAGE_LENGTH]){
    xQueueSend(channelQueue, state, portMAX_DELAY);
}

bool getChannelPosFromQueue(byte state[CHANNELPOS_MESSAGE_LENGTH]){
    return xQueueReceive(channelPosQueue, state, portMAX_DELAY);
}

void sendChannelPosToQueue(byte state[CHANNELPOS_MESSAGE_LENGTH]){
    xQueueSend(channelPosQueue, state, portMAX_DELAY);
}