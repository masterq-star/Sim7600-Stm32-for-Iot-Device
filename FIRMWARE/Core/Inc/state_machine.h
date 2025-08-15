#ifndef STATE_MACHINE_H
#define STATE_MACHINE_H
#include "mqtt.h"
// Enum to represent the state
typedef enum {
    STATE_IDLE,
    STATE_CONNECT,
    STATE_UPLOAD,
    STATE_ERROR
} State;

// Function to execute state machine logic
void stateMachine(void);

// Global variable to store the current state
extern State currentState;

#endif // STATE_MACHINE_H
