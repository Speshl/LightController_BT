#include "signals.h"

void signalTask(void *pvParameters){
  // Initialise the xLastWakeTime variable with the current time.
  TickType_t xLastWakeTime = xTaskGetTickCount();
  const TickType_t xFrequency = 500;
  for(;;){//following comments assume all switches are on
    if(peekRequestStatusFromQueue() != ACTIVE){
      sendSignalCommandToQueue(RAMP_UP);
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
    if(peekRequestStatusFromQueue() != ACTIVE){
      sendSignalCommandToQueue(RAMP_DOWN);
    }
    vTaskDelayUntil(&xLastWakeTime, xFrequency);
  }
}

void createSignals(TaskHandle_t* handle){
    xTaskCreatePinnedToCore(signalTask, "signalTask", 2048, (void*)NULL, 4, handle, SIGNAL_CORE);
}