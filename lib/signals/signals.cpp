#include "signals.h"

bool delayAndPollForUpdate(State* state, int delay){
  unsigned long time = millis();
  while(millis() < time + delay){
    if(updateSwitchState(&state->switches) == true){
      return true;
    }
    vTaskDelay(10);
  }
  return false;
}

void signalTask(void *pvParameters){
  //Serial.println("Signal Task Started");
  State* state = (State*)pvParameters;
  for(;;){//following comments assume all switches are on
    if(paused == false && (state->switches.brake || state->switches.reverse || state->switches.leftTurn || state->switches.rightTurn)){
      Serial.println("Start Signals");
      //lowest priority goes first since any overlaps will be over written with higher priorities
      if(state->switches.reverse){
        reverseAnimation(state->channels, &state->switches);
      }

      //brake light is next highest importance
      if(state->switches.brake){
        Serial.println("Brake ON");
        brakeAnimation(state->channels, &state->switches);
      }
      
      //turn signals are highest priority because they are intermittent and will default back to other signal if applicable
      if(state->switches.leftTurn){
        Serial.println("Left Turn ON");
        leftTurnAnimationWithColor(state->channels, &state->switches, CRGB::Orange); 
      }

      if(state->switches.rightTurn){
        Serial.println("Right Turn ON");
        rightTurnAnimationWithColor(state->channels, &state->switches, CRGB::Orange);
      }

      //display lights with priority maintained
      showAll(state->channels);
      if(delayAndPollForUpdate(state, 500) == true){
        continue; //break out early if we get an update
      }

      //Set turn signals back to off
      if(state->switches.leftTurn){
        Serial.println("Left Turn OFF");
        leftTurnAnimationWithColor(state->channels, &state->switches, CRGB::Black); 
      }

      if(state->switches.rightTurn){
        Serial.println("Right Turn OFF");
        rightTurnAnimationWithColor(state->channels, &state->switches, CRGB::Black);
      }
      //turn any turn signal back to to reverse color if they overlapped
      if(state->switches.reverse){
        reverseAnimation(state->channels, &state->switches);
      }
      //finally turn back on any overlapping breaklights
      if(state->switches.brake){
        brakeAnimation(state->channels, &state->switches);
      }
      //Show the final prioritized state of the lights
      showAll(state->channels);
      if(delayAndPollForUpdate(state, 500) == true){
        continue; //break out early if we get an update
      }
      //Serial.println("End Signals");
    }else{
      delayAndPollForUpdate(state, 500);
    }
  }
}

void createSignals(State* state, TaskHandle_t* handle){
    xTaskCreatePinnedToCore(signalTask, "signalTask", 2048, (void*)state, 4, handle, SIGNAL_CORE);
}

void unPauseSignals(TaskHandle_t* handle){
  if(paused == true){
    //vTaskResume(handle);
    //xTaskCreatePinnedToCore(signalTask, "signalTask", 2048, (void*)&state, 3, handle, SIGNAL_CORE);
    paused = false;
  }
}

void pauseSignals(TaskHandle_t* handle){
  if(paused == false){
    //vTaskSuspend(handle);
    //vTaskDelete(handle);
    paused = true;
  }
}