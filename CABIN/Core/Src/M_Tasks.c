#include "M_Tasks.h"
#include "i2c-lcd.h"
#include "state_machine.h"
#include "ModbusMaster.h"
#include "uart_sim_async.h"
#include "event.h"
#include "string.h"
#include "stdio.h"
#include "main.h"
#include "cmsis_os.h"
//#include "usb_device.h"
extern osThreadId ModbusTaskHandle;
extern osThreadId StatemachineTasHandle;

extern  bool modbusDataReceived;
// USER CODE BEGIN COUNTER MODBUS
uint8_t timeoutCounter=0;
uint8_t timeoutThreshold = 30;
ModbusStatus modbusStatus = MODBUS_STATUS_WAITING;
// USER CODE END COUNTER MODBUS
// USER CODE BEGIN WATCHDOG TIMER
// USER CODE END WATCHDOG TIME
// USER CODE STATUS INTERNET
bool currentConnectionState;
bool currentInternetState;
bool currentMqttState;
bool currentMqttInit;
extern State currentState;

// USER CODE END STATUS INTERNET

extern uint8_t RxData[256];

extern char lcdContent[LCD_HEIGHT][LCD_WIDTH + 1];  // +1 for null-terminator
char lastModbusData[20];
char lastStateMachine[20];
char minifiedID[9];  // Buffer to store the minified unique device ID
void getMinifiedUniqueID(void) {
    uint32_t *UID = (uint32_t *)0x1FFFF7E8;
    uint32_t checksum = UID[0] ^ UID[1] ^ UID[2];  // Simple XOR checksum
    snprintf(minifiedID, sizeof(minifiedID), "%08lX", (unsigned long)checksum);
}
/* USER CODE BEGIN ModbusTask */
void StartModbusTask(void const * argument)
{
      while (1)
    {
         // Poll data from Modbus
          poll_request_read_res(0x01, 0x04, 0, 64);
					if (modbusDataReceived) 
        {
					  modbusStatus = MODBUS_STATUS_OK;
            modbusDataReceived = false; // Reset the flag
            timeoutCounter = 0; // Reset the timeout counter
        } 
        else 
        {
            
            if (timeoutCounter >= timeoutThreshold) 
            {
							modbusStatus = MODBUS_STATUS_TIMEOUT;
            }
						else {
							// ONLY COUNT if in WAITTING MODE
							timeoutCounter++;
							modbusStatus = MODBUS_STATUS_WAITING;
						
						}
        }
				if(modbusStatus == MODBUS_STATUS_OK)snprintf(lastModbusData, sizeof(lastModbusData), "Modbus device: OK    ");
				else if(modbusStatus == MODBUS_STATUS_TIMEOUT) snprintf(lastModbusData, sizeof(lastModbusData), "Modbus device: TO    ");
				else if(modbusStatus == MODBUS_STATUS_WAITING) snprintf(lastModbusData, sizeof(lastModbusData), "Modbus device: %d    ",timeoutCounter);
				
				//toggle led
				HAL_GPIO_TogglePin(LED1_GPIO_Port,LED1_Pin);
				// reset watchdog timer
//				HAL_IWDG_Refresh(&hiwdg);  // Feed the watchdog
				modbusStatus = MODBUS_STATUS_OK;
				if (currentMqttInit && modbusStatus == MODBUS_STATUS_OK ){
				   if (StatemachineTasHandle != NULL )
            {
                xTaskNotifyGive(StatemachineTasHandle);
            }
				}
				osDelay(1000);
       
    }
}
/* USER CODE END ModbusTask */

/* USER CODE BEGIN LcdTask */
void StartLcd(void const * argument)
{
    lcd_init();
    lcd_clear();
    getMinifiedUniqueID();  // Fetch the minified unique device ID

    // Initialize the LCD content
    memset(lcdContent, ' ', sizeof(lcdContent));  // Fill with spaces
    lcdContent[LCD_HEIGHT - 1][LCD_WIDTH] = '\0';  // Ensure null-termination for the last row

    // Display static information
    updateLcdContent(0, "ID: ");
    updateLcdContent(0, minifiedID);
	  updateLcdContent(1, "TRY TO CONNNECTTING");
    updateLcdContent(2, lastModbusData);
	  updateLcdContent(3, "STATE:  IDLE");
	
	
    bool previousConnectionState = false;
    bool previousInternetState = false;
    bool previousMqttState = false;
	  State previosState = STATE_IDLE;

    while (1)
    {
       //
	//		HAL_IWDG_Refresh(&hiwdg);  // Feed the watchdog
       if (currentConnectionState != previousConnectionState)
        {
            previousConnectionState = currentConnectionState;
            if (!currentConnectionState)
            {
                updateLcdContent(1, "Try Connect To MSim!");
            }
            else
            {
                updateLcdContent(1, "Conn: SIM OK        ");
            }
        }

        if (currentConnectionState && currentInternetState != previousInternetState)
        {
            previousInternetState = currentInternetState;
            if (!currentInternetState)
            {
                updateLcdContent(1, "Try Connect To Inte!");
            }
            else
            {
                updateLcdContent(1, "Conn: INTERNET OK   ");
            }
        }

        if (currentInternetState && currentMqttInit != previousMqttState)
        {
            previousMqttState = currentMqttInit;
            if (!currentMqttInit)
            {
                updateLcdContent(1, "Try Connect To Mqtt!");
            }
            else
            {
                updateLcdContent(1, "Conn: MQTT OK       ");
            }
        }
				setCursor(0, 2);
        lcd_send_string(lastModbusData);
        if(currentState != previosState){
					if(currentState == STATE_IDLE) updateLcdContent(3, "STATE:  IDLE");
					if(currentState == STATE_CONNECT) updateLcdContent(3, "STATE:  CONNECT");
					if(currentState == STATE_UPLOAD) updateLcdContent(3, "STATE:  UPLOAD");
					if(currentState == STATE_ERROR) updateLcdContent(3, "STATE:  ERROR");
				
				}
        osDelay(100); // Delay for 100 ms and check again
    }
}
/* USER CODE END LcdTask */

/* USER CODE BEGIN SimTask */
void StartSim(void const * argument)
{
				
	 while(!currentConnectionState){ currentConnectionState = checkSimConnection(); }
	 while(!currentInternetState){ currentInternetState = currentConnectionState && checkSignalQuality() && checkInternetConnection(); }
	 while(!currentMqttState){ currentMqttState = currentInternetState && checkMqttConnection(); }
	 while(!currentMqttInit){currentMqttInit = MQTT_INIT();}
    while (1)
    {
//			currentConnectionState = checkSimConnection();
//        currentInternetState = currentConnectionState && checkSignalQuality() && checkInternetConnection();
//       currentMqttState = currentInternetState && checkMqttConnection();
			// only IDLE STATE WANT CHECK
//			  if(currentState == STATE_IDLE){
//        currentConnectionState = checkSimConnection();
//        currentInternetState = currentConnectionState && checkSignalQuality() && checkInternetConnection();
//        currentMqttState = currentInternetState && checkMqttConnection();
//        }
//        if (currentConnectionState)
//        {
//            if (ModbusTaskHandle != NULL)
//            {
//                xTaskNotifyGive(ModbusTaskHandle);
//            }
//            if (StatemachineTasHandle != NULL)
//            {
//                xTaskNotifyGive(StatemachineTasHandle);
//            }
//        }
//			  mqttUploadData();

			   

        // Toggle LED for debugging
        HAL_GPIO_TogglePin(LED2_GPIO_Port, LED2_Pin);  // Example GPIO pin
        
       osDelay(1000);
    }
}
/* USER CODE END SimTask */

/* USER CODE BEGIN StateMachineTask */
void StartStateMachine(void const * argument)
{

  while (1)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    stateMachine();
			
		//	mqttUploadData();
    
		//mqttUploadData();
		HAL_GPIO_TogglePin(LED3_GPIO_Port, LED3_Pin);  // Example GPIO pin
		osDelay(100);
		
  }
}
/* USER CODE END StateMachineTask */
