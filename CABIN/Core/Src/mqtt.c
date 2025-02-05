#include "mqtt.h"
#include "uart_sim_async.h"
#include "cmsis_os.h"
#include "task.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include "ModbusMaster.h"
#include "M_Tasks.h"
#include "C_Print.h"
extern char ATcommand[100];
extern char publish_data[500];
extern uint8_t Rx_Sim_Buffer[500];
extern MFMDATA Mfm;
extern char minifiedID[9];
#define MAX_TOPIC_LENGTH 100  // Adjust as needed for topic length

char topic[MAX_TOPIC_LENGTH];  // Buffer for the topic string
// USER DEFINE MQTT SERVER
const char* PUB_TOPIC(void) {
    // Ensure minifiedID is populated
    getMinifiedUniqueID();
    
    // Construct the topic
    snprintf(topic, sizeof(topic), "HANOI/MFMDATA/%s/PUBDATA", minifiedID);
    return topic;
}


#define MQTT_BROKER_ADDRESS ""
#define MQTT_PORT 1883
#define MQTT_TOPIC PUB_TOPIC()
#define MQTT_USERNAME ""
#define MQTT_PASSWORD ""

#define PUBLISH_DATA_SIZE 500


bool Mqtt_connectOk= false;
uint8_t try_connect_mqtt =0;

char clientID[20]; // Adjust the size as needed

bool mqttConnect(void)
{

//	  HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin);
	SIMTransmit_ONLY("AT+CMQTTSTART\r\n");
	 // sendATCommand("AT+CMQTTSTART\r\n","OK",2000); 
    // Example commands to connect to the MQTT broker
	vTaskDelay(pdMS_TO_TICKS(100));
	
   sprintf(ATcommand, "AT+CMQTTCONNECT=0,\"ssl://emqx.skynoce.com:1883\",60,1,\"admin\",\"admin\"\r\n");
	SIMTransmit_ONLY(ATcommand);
	vTaskDelay(pdMS_TO_TICKS(100));
	HAL_UART_Receive(&huart1, Rx_Sim_Buffer, sizeof(Rx_Sim_Buffer), 1000);
	  return check_data_sim(Rx_Sim_Buffer,"OK");
}
// Function to generate a random string
void generate_random_string(char *str, size_t length) {
    static char charset[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
    for (size_t i = 0; i < length; i++) {
        int key = rand() % (int)(sizeof(charset) - 1);
        str[i] = charset[key];
    }
    str[length] = '\0';
}

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

bool mqttUploadData(void)
{
    // Example data publishing code
    sprintf(ATcommand, "AT+CMQTTTOPIC=0,%d\r\n", strlen(MQTT_TOPIC));
    SIMTransmit_ONLY(ATcommand);
    vTaskDelay(pdMS_TO_TICKS(100));
	
    memset(ATcommand,'\0',strlen(ATcommand));
    sprintf(ATcommand, "%s\r\n",MQTT_TOPIC);
    SIMTransmit_ONLY(ATcommand);
    vTaskDelay(pdMS_TO_TICKS(100));

    // Prepare the data to be published
    // This function needs to be implemented to prepare `publish_data`
    data_publish_parse();
    
	  memset(ATcommand,'\0',strlen(ATcommand));
    sprintf(ATcommand, "AT+CMQTTPAYLOAD=0,%d\r\n", strlen(publish_data));
    SIMTransmit_ONLY(ATcommand);
    vTaskDelay(pdMS_TO_TICKS(100));

    SIMTransmit_ONLY(publish_data);
    vTaskDelay(pdMS_TO_TICKS(100));

    SIMTransmit_ONLY("AT+CMQTTPUB=0,1,60\r\n"); // Publish
		memset(ATcommand,'\0',strlen(ATcommand));
		vTaskDelay(pdMS_TO_TICKS(100));
		
//    vTaskDelay(pdMS_TO_TICKS(100));
//    
    if (check_data_sim(Rx_Sim_Buffer, "ERROR"))
    {
        return false;
    }
    return true;
}

void handleError(void)
{
    // Example error handling
    //SIMTransmit_ONLY("AT+CRESET\r\n");
	  HAL_GPIO_TogglePin(LED5_GPIO_Port,LED5_Pin);
    vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete
//	  HAL_GPIO_WritePin(BUZZER_GPIO_Port,BUZZER_Pin,GPIO_PIN_RESET);
}

void data_publish_parse(void)
{
	 sprintf(publish_data,"NEW DATA TEST, %s","	MTQ");

//	memset(publish_data,'\0',sizeof(publish_data));

//    sprintf(publish_data, 
//        "{\"ID\":\"%s\","
//        "\"VOLT_LN\":[%.2f,%.2f,%.2f,%.2f],"
//        "\"VOLT_LL\":[%.2f,%.2f,%.2f,%.2f],"
//        "\"CURR\":[%.2f,%.2f,%.2f,%.2f],"
//        "\"KW\":[%.2f,%.2f,%.2f],"
//        "\"KVA\":[%.2f,%.2f,%.2f],"
//        "\"KVAr\":[%.2f,%.2f,%.2f],"
//        "\"TOTALS\":[%.2f,%.2f,%.2f],"
//        "\"PF\":[%.2f,%.2f,%.2f,%.2f],"
//        "\"FRE\":%.2f,"
//        "\"ENERGY\":[%.2f,%.2f,%.2f]}",
//        minifiedID,
//        Mfm.VOLT_V1N, Mfm.VOLT_V2N, Mfm.VOLT_V3N, Mfm.AVERAGE_V_LN,
//        Mfm.VOLT_V12, Mfm.VOLT_V23, Mfm.VOLT_V31, Mfm.AVERAGE_V_LL,
//        Mfm.I1, Mfm.I2, Mfm.I3, Mfm.AVERAGE_I,
//        Mfm.KW1, Mfm.KW2, Mfm.KW3,
//        Mfm.KVA1, Mfm.KVA2, Mfm.KVA3,
//        Mfm.KVAr1, Mfm.KVAr2, Mfm.KVAr3,
//        Mfm.TOTAL_KW, Mfm.TOTAL_KVA, Mfm.TOTAL_KVAR,
//        Mfm.PF1, Mfm.PF2, Mfm.PF3, Mfm.AVERAGE_PF,
//        Mfm.FREQUENCY,
//        Mfm.KWH, Mfm.KVAH, Mfm.KVARH);
}
bool MQTT_INIT(void)
{
    

    // Initialize random number generator
   // srand(time(NULL));

    // Generate random client ID
    generate_random_string(clientID, 19); // Generates a 19-character string
	
//    c_print("Device ID: %s\n", minifiedID);
//	  c_print("MQTT Topic: %s\n", MQTT_TOPIC);
    // Wait for network registration (CGREG OK)


        // Check for network registration
	
	          
            SIMTransmit_ONLY("AT+SMQTDISCON=0\r\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete
						SIMTransmit_ONLY("AT+SMQTTCONFIG\r\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete
	
						SIMTransmit_ONLY("AT+SMQTTCONNECT=0\r\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete


            SIMTransmit_ONLY("AT+CMQTTSTART\r\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete

            // Acquire a Client with random ID
            char command[100];
            sprintf(command, "AT+CMQTTACCQ=0,\"%s\"\r\n", clientID);
						//SIMTransmit("AT+CMQTTACCQ=0,\"client01\"\r\n"); // Acquire a Client
            SIMTransmit_ONLY(command);
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete
						
						memset(command,0x00,sizeof(command));

            // Connect to a MQTT Server
            

      // Construct the AT command dynamically
//					snprintf(command, sizeof(command),
//									 "AT+CMQTTCONNECT=0,\"%s\",60,1,\"%s\",\"%s\"\r\n",
//									 MQTT_BROKER_ADDRESS, MQTT_USERNAME, MQTT_PASSWORD);
           sprintf(command,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1,\"%s\",\"%s\"\r\n",MQTT_BROKER_ADDRESS,MQTT_PORT,MQTT_USERNAME,MQTT_PASSWORD);
//					 sprintf(command,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1\r\n",MQTT_BROKER_ADDRESS,MQTT_PORT);

					// Transmit the constructed command
//					if(sendATCommand(command,"OK",1000))return Mqtt_connectOk = true;
//					else return Mqtt_connectOk = false;
            memset(Rx_Sim_Buffer,0,sizeof(Rx_Sim_Buffer));
            SIMTransmit_ONLY(command);
						vTaskDelay(pdMS_TO_TICKS(1000)); 

            

            if (check_data_sim(Rx_Sim_Buffer, "ERROR"))
            {
                Mqtt_connectOk = false;
                try_connect_mqtt++;
                 
            }
            else
            {
                Mqtt_connectOk = true;
            }
     return Mqtt_connectOk;
    
    
}
bool MQTT_INIT_SSL(void)
{
    

    // Initialize random number generator
   // srand(time(NULL));

    // Generate random client ID
    generate_random_string(clientID, 19); // Generates a 19-character string
	
//    c_print("Device ID: %s\n", minifiedID);
//	  c_print("MQTT Topic: %s\n", MQTT_TOPIC);
    // Wait for network registration (CGREG OK)


        // Check for network registration



            SIMTransmit_ONLY("AT+CMQTTSTART\r\n");
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete

            // Acquire a Client with random ID
            char command[100];
            sprintf(command, "AT+CMQTTACCQ=0,\"%s\",1\r\n", clientID);
						//SIMTransmit("AT+CMQTTACCQ=0,\"client01\"\r\n"); // Acquire a Client
            SIMTransmit_ONLY(command);
            vTaskDelay(pdMS_TO_TICKS(500)); // Wait for reset to complete
						
						memset(command,'\0',sizeof(command));

            // Connect to a MQTT Server
            

      // Construct the AT command dynamically
//					snprintf(command, sizeof(command),
//									 "AT+CMQTTCONNECT=0,\"%s\",60,1,\"%s\",\"%s\"\r\n",
//									 MQTT_BROKER_ADDRESS, MQTT_USERNAME, MQTT_PASSWORD);
           sprintf(command,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1,%s,%s\r\n",MQTT_BROKER_ADDRESS,8883,MQTT_USERNAME,MQTT_PASSWORD);
//					 sprintf(command,"AT+CMQTTCONNECT=0,\"%s:%d\",60,1\r\n",MQTT_BROKER_ADDRESS,MQTT_PORT);

					// Transmit the constructed command
//					if(sendATCommand(command,"OK",1000))return Mqtt_connectOk = true;
//					else return Mqtt_connectOk = false;
            memset(Rx_Sim_Buffer,'\0',sizeof(Rx_Sim_Buffer));
            SIMTransmit_ONLY(command);
						vTaskDelay(pdMS_TO_TICKS(1000)); 

            

            if (check_data_sim(Rx_Sim_Buffer, "ERROR"))
            {
                Mqtt_connectOk = false;
                try_connect_mqtt++;
                 
            }
            else
            {
                Mqtt_connectOk = true;
            }
     return Mqtt_connectOk;
    
    
}