#include "messagequeue.h"

void initializeMessageQueue(){
    Serial.println("Building Message Queues");
    controllerQueue = xQueueCreate( 1, sizeof(int));
    if(controllerQueue == NULL){
        Serial.println("Error creating controllerQueue");
    }

    requestQueue = xQueueCreate( 1, sizeof(int));
    if(requestQueue == NULL){
        Serial.println("Error creating requestQueue");
    }

    signalQueue = xQueueCreate( 1, sizeof(int));
    if(signalQueue == NULL){
        Serial.println("Error creating signalQueue");
    }
}

int peekControllerStatusFromQueue(){
    int command;
    bool found = xQueuePeek(controllerQueue, &command,0);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}

int getControllerStatusFromQueue(){
    int command;
    bool found = xQueueReceive(controllerQueue, &command, 0);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}
bool sendControllerStatusToQueue(int command){
    return xQueueSend(controllerQueue, &command, 0);
}

int peekRequestStatusFromQueue(){
    int command;
    bool found = xQueuePeek(requestQueue, &command ,0);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}

int getRequestStatusFromQueue(){
    int command;
    bool found = xQueueReceive(requestQueue, &command, portMAX_DELAY);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}

bool sendRequestStatusToQueue(int command){
    return xQueueSend(requestQueue, &command, portMAX_DELAY);
}

int getSignalCommandFromQueue(){
    int command;
    bool found = xQueueReceive(signalQueue, &command, 0);
    if(found){
        return command;
    }else{
        return NO_SIGNAL;
    }
}

bool sendSignalCommandToQueue(int command){
    return xQueueSend(signalQueue, &command, portMAX_DELAY);
}

int peekControllerStatusFromQueueFromISR(){
    int command;
    bool found = xQueuePeekFromISR(controllerQueue, &command);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}

int getControllerStatusFromQueueFromISR(){
    BaseType_t taskWoke = pdFALSE;
    int command;
    bool found = xQueueReceiveFromISR(controllerQueue, &command, &taskWoke);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}
bool sendControllerStatusToQueueFromISR(int command){
    BaseType_t taskWoke = pdFALSE;
    return xQueueSendFromISR(controllerQueue, &command, &taskWoke);
}

int peekRequestStatusFromQueueFromISR(){
    BaseType_t taskWoke = pdFALSE;
    int command;
    bool found = xQueuePeekFromISR(requestQueue, &command);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}

int getRequestStatusFromQueueFromISR(){
    BaseType_t taskWoke = pdFALSE;
    int command;
    bool found = xQueueReceiveFromISR(requestQueue, &command, &taskWoke);
    if(found){
        return command;
    }else{
        return INACTIVE;
    }
}

bool sendRequestStatusToQueueFromISR(int command){
    BaseType_t taskWoke = pdFALSE;
    return xQueueSendFromISR(requestQueue, &command, &taskWoke);
}

int getSignalCommandFromQueueFromISR(){
    BaseType_t taskWoke = pdFALSE;
    int command;
    bool found = xQueueReceiveFromISR(signalQueue, &command, &taskWoke);
    if(found){
        return command;
    }else{
        return NO_SIGNAL;
    }
}

bool sendSignalCommandToQueueFromISR(int command){
    BaseType_t taskWoke = pdFALSE;
    return xQueueSendFromISR(signalQueue, &command, &taskWoke);
}