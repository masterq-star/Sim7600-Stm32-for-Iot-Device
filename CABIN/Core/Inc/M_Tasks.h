#ifndef TASKS_H
#define TASKS_H

#include "cmsis_os.h"
#include <stdbool.h>

void StartModbusTask(void const * argument);
void StartLcd(void const * argument);
void StartSim(void const * argument);
void StartStateMachine(void const * argument);
void getMinifiedUniqueID(void) ;
extern bool isSimConnected;
typedef enum {
    MODBUS_STATUS_OK,
    MODBUS_STATUS_WAITING,
    MODBUS_STATUS_TIMEOUT
} ModbusStatus;
#endif // TASKS_H
