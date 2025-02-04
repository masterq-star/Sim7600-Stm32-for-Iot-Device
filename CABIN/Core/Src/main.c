/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "ModbusMaster.h"
#include "uart_sim_async.h"
#include "String.h"
#include <stdio.h>
#include "FLASH_PAGE_F1.h"
#include "i2c-lcd.h"
#include "task.h"
#include "mqtt.h"
#include "FreeRTOS.h"
#include "event.h"
#include "cmsis_os.h"  // Ensure CMSIS-RTOS header is included
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c1;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;
UART_HandleTypeDef huart3;
DMA_HandleTypeDef hdma_usart1_rx;

osThreadId ModbusTaskHandle;
osThreadId LcdTaskHandle;
osThreadId TaskSimHandle;
osThreadId StatemachineTasHandle;
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_DMA_Init(void);
static void MX_I2C1_Init(void);
static void MX_USART1_UART_Init(void);
static void MX_USART3_UART_Init(void);
static void MX_SPI1_Init(void);
void StartModbusTask(void const * argument);
void StartLcd(void const * argument);
void StartSim(void const * argument);
void StartStateMachine(void const * argument);

/* USER CODE BEGIN PFP */
// task handle

extern TaskHandle_t xModbusTaskHandle;
extern TaskHandle_t xStateMachineTaskHandle;


// END Task handle
uint8_t RxData[256];
uint16_t Data[64];
uint32_t package_receive_MFM=0;
char mess_SIM[128];
uint8_t buffer[160] = {0};
char ATcommand[100];
float mfmdata[64];
uint8_t INIT_SIM=0;
char str[40];

MFMDATA Mfm;

uint8_t ATisOK = 0;
uint8_t CGREGisOK = 0;
//uint8_t Mqtt_connectOk = 0;
uint32_t previousTick;
uint32_t number_package_data_mfm;
const uint32_t timeOut =10000;

const char host[] = "tcp://broker.hivemq.com"; // Change this to your host
const int  port = 1883;


//const char topic[] = "HBCABIN_BD_001";
const char subrice_channel[] = "HBCABIN_BD_001_VL";
char charData[] = "hello test";
char publish_data[2000];


// sim rx
uint8_t rx_buffer[100] = {0};
uint8_t rx_index = 0;
uint8_t rx_data;
#define PREF_SMS_STORAGE "\"SM\""
char mobileNumber[] = "0967221346";
uint32_t Rx_Data_flash[30];
uint8_t slot = 0;
//uint8_t try_connect_mqtt=0;


void SIMTransmit(char *cmd);
void SimSendData(char * data);
//void MQTT_INIT(void);
void data_publish_parse(void);
void mqttPublish(void);

#define START_PAGE_SDT 0x08018000
#define RX_SIM_BUFFER_SIZE 500
uint8_t Rx_Sim_Buffer[RX_SIM_BUFFER_SIZE];
uint16_t size_sim_receive = 0;
Rx_Subrice_Sim  Rx_mess_subrice;
volatile bool modbusDataReceived = false;  // Flag indicating new Modbus data

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */


void HAL_UARTEx_RxEventCallback(UART_HandleTypeDef *huart, uint16_t Size)
{
	
//	Data[0] = RxData[3]<<8 | RxData[4];
//	Data[1] = RxData[5]<<8 | RxData[6];
//	Data[2] = RxData[7]<<8 | RxData[8];
//	Data[3] = RxData[9]<<8 | RxData[10];
//	mfmdata[0]= f_2uint_float(Data[0],Data[1]);

    if (huart->Instance == USART3)
    {
        // Process the received Modbus data
        Import_data_ARRAY(Data, RxData);
        Import_data_MFM(&Mfm, Data);
        package_receive_MFM++;
				modbusDataReceived = true;
        // Notify the state machine task
//       osSemaphoreRelease(semaphoreStateMachine);

        // Re-enable the UART receive interrupt
        HAL_UARTEx_ReceiveToIdle_IT(&huart3, RxData, 256);
    }

	if(huart->Instance == USART1)
	{
	 HAL_UARTEx_ReceiveToIdle_DMA(&huart1,Rx_Sim_Buffer,RX_SIM_BUFFER_SIZE);
		if(check_data_sim(Rx_Sim_Buffer,"CMTI")){
		if( sscanf((char*)Rx_Sim_Buffer, "\n+CMTI: " PREF_SMS_STORAGE ",%hhd", &slot)==1)
      {
        sprintf(ATcommand,"AT+CMGR=%d\r\n",slot);
        SIMTransmit_ONLY(ATcommand);
      }
		
		}
	 
		if(check_data_sim(Rx_Sim_Buffer,"CMQTTRXSTART")){ // if have message
					handle_payload_rx_sim(&Rx_mess_subrice,(char *)Rx_Sim_Buffer);
			    if(strcmp(Rx_mess_subrice.topic,subrice_channel) ==0)
					{
					 
						if(check_data_sim((uint8_t *)Rx_mess_subrice.mess , "sdt"))
						{
						 handle_sdt_rx_sim(Rx_mess_subrice.mess,mobileNumber);
							int numofwords = (strlen(mobileNumber)/4)+((strlen(mobileNumber)%4)!=0);

						 Flash_Write_Data(START_PAGE_SDT,(uint32_t * )mobileNumber,numofwords);
						}
					}
				}
	}
	
//	Data[4] = RxData[11]<<8 | RxData[12];
}
//void SimSendData(char * data){
//				sprintf(ATcommand,"AT+CMGF=1\r\n");
//	  SIMTransmit_ONLY(ATcommand);
//		vTaskDelay(1000);
//		sprintf(ATcommand,"AT+CMGS=\"%s\"\r\n",mobileNumber);
//	  SIMTransmit_ONLY(ATcommand);
//		vTaskDelay(100);
//		sprintf(ATcommand,"%s%c",data,0x1a);
//	  SIMTransmit_ONLY(ATcommand);
//		vTaskDelay(100);

//}
//void MQTT_SUBSCRIBE(char *Channel){
//	sprintf(ATcommand,"AT+CMQTTSUBTOPIC=0,%d,1\r\n",strlen(Channel));
//	SIMTransmit_ONLY(ATcommand);
//	vTaskDelay(100);
//	sprintf(ATcommand,"%s\r\n",Channel);
//	 SIMTransmit_ONLY(ATcommand);
//	 vTaskDelay(100);
// // SIMTransmit(ATcommand);
//	SIMTransmit_ONLY("AT+CMQTTSUB=0\r\n");


//}

//void MQTT_INIT(void)
//{
//	try_connect_mqtt =0;
//	while(CGREGisOK !=1){
//		INIT_SIM = 0;
//  ATisOK = 0;
//  CGREGisOK = 0;
//	Mqtt_connectOk = 0;

//  // Check for OK response for AT
//  previousTick =  HAL_GetTick();
//  while(!ATisOK && previousTick  + timeOut >  HAL_GetTick() )
//  {
//    SIMTransmit_ONLY("AT\r\n");
//    vTaskDelay(100);
//		if(check_data_sim(Rx_Sim_Buffer,"OK") && check_data_sim(Rx_Sim_Buffer,"AT"))ATisOK = 1;
//  }
////	sprintf(ATcommand,"AT+CMGF=1\r\n");
////  SIMTransmit_ONLY(ATcommand);
////  vTaskDelay(1000);
////  sprintf(ATcommand,"AT+CMGS=\"%s\"\r\n",mobileNumber);
////  SIMTransmit_ONLY(ATcommand);
////  vTaskDelay(100);
////	SIMTransmit_ONLY("THIS IS FIRST LINE FOR CUSTOMER \n This is two cmd you can use \n OnContactor: to ON CONTACTOR\n OffContactor: to OFF CONTACTOR \r\n");
//	//tick_test =0;

//  // Check for network registration.
//  if(ATisOK)
//  {
//    previousTick =  HAL_GetTick();
//    while(!CGREGisOK  && previousTick  + timeOut >  HAL_GetTick())
//    {
//      SIMTransmit_ONLY("AT+CGREG?\r\n");
//			vTaskDelay(100);
//			if(check_data_sim(Rx_Sim_Buffer,"+CGREG: 0,1")) CGREGisOK = 1;
//    }
//  }

//  // If registered
//  if(CGREGisOK)
//  {
//		SIMTransmit_ONLY("AT+CMQTTDISC=0,60\r\n"); // Start MQTT Service
//		vTaskDelay(100);
//		SIMTransmit_ONLY("AT+CMQTTREL=0\r\n"); // Start MQTT Service
//		vTaskDelay(100);
//		SIMTransmit_ONLY("AT+CMQTTSTOP\r\n"); // Start MQTT Service
//		vTaskDelay(100);
//   SIMTransmit_ONLY("AT+CMQTTSTART\r\n"); // Start MQTT Service
//		vTaskDelay(100);
//	SIMTransmit_ONLY("AT+CMQTTACCQ=0,\"HBHANOI_CABIN_BD\",0\r\n"); // Acquire a Client
//		vTaskDelay(100);
//	//	SIMTransmit("AT+CMQTTCONNECT=\r\n"); // Acquire a Client
//	//sprintf(ATcommand,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1\r\n",host,port); // Connect to a MQTT Server
//	// sprintf(ATcommand,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1,%s,%s\r\n",host,port,username,password);
//	//SIMTransmit_ONLY(ATcommand);
//	SIMTransmit_ONLY("AT+CMQTTCONNECT=0,\"tcp://broker.hivemq.com:1883\",60,1\r\n");
//	vTaskDelay(1000); 
//  if(check_data_sim(Rx_Sim_Buffer,"ERROR"))	{
//	Mqtt_connectOk = 0;
//	try_connect_mqtt ++;
//	if(try_connect_mqtt > 15)// if can't connect to mqtt sever reset module 
//	{
//	 try_connect_mqtt =0;
//		SIMTransmit_ONLY("AT+CRESET\r\n");	
//	}
//	}
//	else Mqtt_connectOk =1;
////	
//	//sprintf(ATcommand,"AT+CMQTTSUBTOPIC=0,%d,1\r\n",strlen(subrice_channel)); // Connect to a MQTT Server
//	if(Mqtt_connectOk == 1){
//		 
//			 MQTT_SUBSCRIBE((char *)subrice_channel);
//			 vTaskDelay(1000);
//			 if(check_data_sim(Rx_Sim_Buffer,"ERROR"))	CGREGisOK = 0;
//			//		
//				vTaskDelay(100);
//				}
//	else CGREGisOK = 0;
//}
//	
//}
//}

//void data_publish_parse(void){
//	uint8_t data_check_signal = 0;
//	if(HAL_GPIO_ReadPin(SIGNAL_CHECK_GPIO_Port,SIGNAL_CHECK_Pin) == 0) data_check_signal = 1;
//	else data_check_signal = 0;
//	sprintf(publish_data,"{\"VLN\": [%.2f, %.2f,%.2f, %.2f],\"VLL\": [%.2f, %.2f,%.2f, %.2f],\"I\": [%.2f, %.2f,%.2f, %.2f],\"KW\": [%.2f, %.2f,%.2f],\"FRE\": %.2f,\"KWH\": %.2f,\"STS\":%d}",
//	Mfm.VOLT_V1N,
//	Mfm.VOLT_V2N,
//	Mfm.VOLT_V3N,
//	Mfm.AVERAGE_V_LN,
//	Mfm.VOLT_V12,
//	Mfm.VOLT_V23,
//	Mfm.VOLT_V31,
//	Mfm.AVERAGE_V_LL,
//	Mfm.I1,
//	Mfm.I2,
//	Mfm.I3,
//	Mfm.AVERAGE_I,
//	Mfm.KW1,
//	Mfm.KW2,
//	Mfm.KW3,
//	Mfm.FREQUENCY,
//	Mfm.KWH,
//	data_check_signal
//	);

//}
//void mqttPublish(void)
//{
//	sprintf(ATcommand,"AT+CMQTTTOPIC=0,%d\r\n",strlen(topic)); // Set the topic for publish message
//	SIMTransmit_ONLY(ATcommand);
//		vTaskDelay(100);
//	sprintf(ATcommand,"%s\r\n",topic);
//	SIMTransmit_ONLY(ATcommand);
//  data_publish_parse();
//		//sprintf(publish_data,"HEKKKK %d",1);
//	sprintf(ATcommand,"AT+CMQTTPAYLOAD=0,%d\r\n",strlen(publish_data)); // Set the payload
//		vTaskDelay(100);
//	SIMTransmit_ONLY(ATcommand);
//		vTaskDelay(100);
//	SIMTransmit_ONLY(publish_data);
//		vTaskDelay(100);
//	SIMTransmit_ONLY("AT+CMQTTPUB=0,1,60\r\n"); // Publish
////	number_package_data_mfm = package_receive_MFM;	
////  }
//}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_DMA_Init();
  MX_I2C1_Init();
  MX_USART1_UART_Init();
  MX_USART3_UART_Init();
  MX_SPI1_Init();
  /* USER CODE BEGIN 2 */
  HAL_UARTEx_ReceiveToIdle_IT(&huart3, RxData, 256);
	HAL_UARTEx_ReceiveToIdle_DMA(&huart1, Rx_Sim_Buffer, RX_SIM_BUFFER_SIZE);
  __HAL_DMA_DISABLE_IT(&hdma_usart1_rx, DMA_IT_HT);
		//Flash_Read_Data(START_PAGE_SDT,Rx_Data_flash,20);
		//Convert_To_Str(Rx_Data_flash, mobileNumber);
//	while(!checkSimConnection()){}
//	while(!checkSignalQuality()){}
//	while(!checkMqttConnection()){}
//	while(!MQTT_INIT()){}
//	MX_IWDG_Init();
  /* USER CODE END 2 */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	
    /* Create semaphores */
//    osSemaphoreDef(semaphoreModbus);
//    semaphoreModbus = osSemaphoreCreate(osSemaphore(semaphoreModbus), 1);
//    
//    osSemaphoreDef(semaphoreStateMachine);
//    semaphoreStateMachine = osSemaphoreCreate(osSemaphore(semaphoreStateMachine), 1);

//    if ((semaphoreModbus == NULL) || (semaphoreStateMachine == NULL)) {
//        // Semaphore creation failed
//        Error_Handler();
//    }
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of ModbusTask */
  osThreadDef(ModbusTask, StartModbusTask, osPriorityNormal, 0, 128);
  ModbusTaskHandle = osThreadCreate(osThread(ModbusTask), NULL);

  /* definition and creation of LcdTask */
  osThreadDef(LcdTask, StartLcd, osPriorityAboveNormal, 0, 256);
  LcdTaskHandle = osThreadCreate(osThread(LcdTask), NULL);

  /* definition and creation of TaskSim */
  osThreadDef(TaskSim, StartSim, osPriorityHigh, 0, 128);
  TaskSimHandle = osThreadCreate(osThread(TaskSim), NULL);

  /* definition and creation of StatemachineTas */
  osThreadDef(StatemachineTas, StartStateMachine, osPriorityNormal, 0, 128);
  StatemachineTasHandle = osThreadCreate(osThread(StatemachineTas), NULL);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* Start scheduler */
  osKernelStart();
  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C1_Init(void)
{

  /* USER CODE BEGIN I2C1_Init 0 */

  /* USER CODE END I2C1_Init 0 */

  /* USER CODE BEGIN I2C1_Init 1 */

  /* USER CODE END I2C1_Init 1 */
  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 100000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C1_Init 2 */

  /* USER CODE END I2C1_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 115200;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief USART3 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART3_UART_Init(void)
{

  /* USER CODE BEGIN USART3_Init 0 */

  /* USER CODE END USART3_Init 0 */

  /* USER CODE BEGIN USART3_Init 1 */

  /* USER CODE END USART3_Init 1 */
  huart3.Instance = USART3;
  huart3.Init.BaudRate = 9600;
  huart3.Init.WordLength = UART_WORDLENGTH_8B;
  huart3.Init.StopBits = UART_STOPBITS_1;
  huart3.Init.Parity = UART_PARITY_NONE;
  huart3.Init.Mode = UART_MODE_TX_RX;
  huart3.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart3.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart3) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART3_Init 2 */

  /* USER CODE END USART3_Init 2 */

}

/**
  * Enable DMA controller clock
  */
static void MX_DMA_Init(void)
{

  /* DMA controller clock enable */
  __HAL_RCC_DMA1_CLK_ENABLE();

  /* DMA interrupt init */
  /* DMA1_Channel5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(DMA1_Channel5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(DMA1_Channel5_IRQn);

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};
/* USER CODE BEGIN MX_GPIO_Init_1 */
/* USER CODE END MX_GPIO_Init_1 */

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOE_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOE, LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
                          |LED5_Pin|BUZZER_Pin|TX_EN_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(PWR_SIM_GPIO_Port, PWR_SIM_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : LED1_Pin LED2_Pin LED3_Pin LED4_Pin
                           LED5_Pin BUZZER_Pin TX_EN_Pin */
  GPIO_InitStruct.Pin = LED1_Pin|LED2_Pin|LED3_Pin|LED4_Pin
                          |LED5_Pin|BUZZER_Pin|TX_EN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOE, &GPIO_InitStruct);

  /*Configure GPIO pin : PWR_SIM_Pin */
  GPIO_InitStruct.Pin = PWR_SIM_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_PULLDOWN;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(PWR_SIM_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : Button_on_Pin */
  GPIO_InitStruct.Pin = Button_on_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(Button_on_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pins : Button_off_Pin PC12 */
  GPIO_InitStruct.Pin = Button_off_Pin|GPIO_PIN_12;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_RISING;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);

  /*Configure GPIO pin : SIGNAL_CHECK_Pin */
  GPIO_InitStruct.Pin = SIGNAL_CHECK_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(SIGNAL_CHECK_GPIO_Port, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);

/* USER CODE BEGIN MX_GPIO_Init_2 */
/* USER CODE END MX_GPIO_Init_2 */
}

/* USER CODE BEGIN 4 */
//void StartTask_read_M(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
//  /* Infinite loop */
//  for(;;)
//  {
//    poll_request_read_res(0x01,0x04,0,64);
//		
//		
//    vTaskDelay(100);
//  }
//  /* USER CODE END 5 */
//}
//void StartCommunicati(void const * argument)
//{
//  /* USER CODE BEGIN StartCommunicati */
//  /* Infinite loop */
//  for(;;)
//  {
//    MQTT_INIT();
//	SimSendData("THIS IS FIRST LINE FOR CUSTOMER \
//it has 2 statements you can use: OnContactor \
//to close contactor \
//and OffContactor to disconnect contactor");
//  /* Infinite loop */
//  for(;;)
//  {
//		//if(package_receive_MFM != number_package_data_mfm)
//			mqttPublish();
//		if(Mfm.AVERAGE_I > 5)
//			SimSendData("CURRENT IS GREATER THAN 5A");
//    vTaskDelay(1000);
//  }
//  }
//  /* USER CODE END StartCommunicati */
//}
//void StartPeripheral(void const * argument)
//{
//  /* USER CODE BEGIN StartPeripheral */
//	lcd_clear();
//  /* Infinite loop */
//  for(;;)
//  {
//    	//lcd_clear();
//    if(Mqtt_connectOk == 1){
//		setCursor(0,0);
//		lcd_send_string("HBE CONTROLLER");
//		setCursor(0,1);
//		sprintf(str,"SDT:%s",mobileNumber);
//		lcd_send_string(str);
//		setCursor(0,2);
//		
//		HAL_GPIO_ReadPin(SIGNAL_CHECK_GPIO_Port,SIGNAL_CHECK_Pin) == 0 ? lcd_send_string("STATUS OUTPUT: CLOSE") : lcd_send_string("STATUS OUTPUT: OPEN ");
//		//lcd_send_string("STATUS OUTPUT:");
//		setCursor(0,3);
//		sprintf(str,"Package MFM:%d",package_receive_MFM);
//		lcd_send_string(str);
//		HAL_IWDG_Refresh(&hiwdg);
//    vTaskDelay(50);
//		
//		}
//		if(Mqtt_connectOk == 0){
//		setCursor(0,0);
//		lcd_send_string("HBE INIT");
//		setCursor(0,1);
//			ATisOK  == 1 ? lcd_send_string("AT RESPONSE: OK ") : lcd_send_string("AT RESPONSE: NOK");
//   	setCursor(0,2);		
//			CGREGisOK  == 1 ? lcd_send_string("INTERNET: OK ") : lcd_send_string("INTERNET: NOK");
//			setCursor(0,3);		
//			Mqtt_connectOk  == 1 ? lcd_send_string("MQTT: OK ") : lcd_send_string("MQTT: NOK");
//		
//			
//		} 
//  }
//  /* USER CODE END StartPeripheral */
//}

/* USER CODE END 4 */

/* USER CODE BEGIN Header_StartModbusTask */
/**
  * @brief  Function implementing the ModbusTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartModbusTask */
//void StartModbusTask(void const * argument)
//{
//  /* USER CODE BEGIN 5 */
////  /* Infinite loop */
////  for(;;)
////  {
////    poll_request_read_res(0x01,0x04,0,64);
////		
////		
////    vTaskDelay(100);
////  }
//  /* USER CODE END 5 */
//}

/* USER CODE BEGIN Header_StartLcd */
/////////**
////////* @brief Function implementing the LcdTask thread.
////////* @param argument: Not used
////////* @retval None
////////*/
/* USER CODE END Header_StartLcd */
//void StartLcd(void const * argument)
//{
//  /* USER CODE BEGIN StartLcd */
////  /* Infinite loop */
////  for(;;)
////  {
////    osDelay(1);
////  }
//  /* USER CODE END StartLcd */
//}

/* USER CODE BEGIN Header_StartSim */
/////////**
////////* @brief Function implementing the TaskSim thread.
////////* @param argument: Not used
////////* @retval None
////////*/
/* USER CODE END Header_StartSim */
//void StartSim(void const * argument)
//{
//  /* USER CODE BEGIN StartSim */
////  /* Infinite loop */
////  for(;;)
////  {
////    osDelay(1);
////  }
//  /* USER CODE END StartSim */
//}

/* USER CODE BEGIN Header_StartStateMachine */
/////////**
////////* @brief Function implementing the StatemachineTas thread.
////////* @param argument: Not used
////////* @retval None
////////*/
/* USER CODE END Header_StartStateMachine */
//void StartStateMachine(void const * argument)
//{
//  /* USER CODE BEGIN StartStateMachine */
//////  /* Infinite loop */
//////  for(;;)
//////  {
//////    osDelay(1);
//////  }
//  /* USER CODE END StartStateMachine */
//}

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM1 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM1) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
