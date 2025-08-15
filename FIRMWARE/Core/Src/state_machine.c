#include "state_machine.h"

// Include necessary headers for MQTT functions and error handling


// Initialize the current state to IDLE

State currentState = STATE_IDLE;

extern bool currentMqttInit;
void stateMachine(void) {
    switch (currentState) {
        case STATE_IDLE:
            if (!currentMqttInit) {
                currentState = STATE_CONNECT;
            } else {
                currentState = STATE_UPLOAD;
            }
            break;
        case STATE_CONNECT:
             
				     currentMqttInit = MQTT_INIT();
				     currentState = STATE_IDLE;
				
            break;
        case STATE_UPLOAD:
            mqttUploadData();
				    currentState = STATE_IDLE;
            break;
        case STATE_ERROR:
             handleError();
            currentState = STATE_CONNECT;
            break;
    }
}
