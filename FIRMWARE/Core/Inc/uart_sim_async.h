#ifndef __UART_SIM_SYNC__
#define __UART_SIM_SYNC__

#include "main.h"
#include <stdbool.h>

#define SIM_RX_BUFFER 500
static char AT[] = "AT \r\n";
static char ATE0[] = "ATE0 \r\n";
static char AT_DIS_FROMSERVER[] ="AT+CMQTTDISC=0,60 \r\n";
static char AT_Release_client_mqtt[] ="AT+CMQTTREL=0 \r\n";
static char AT_MQTT_STOP[]= "AT+CMQTTSTOP \r\n";
static char AT_MQTT_START[]="AT+CMQTTSTART \r\n";
static char AT_MQTT_ACQUIRE_A_CLIENT[] = "AT+CMQTTACCQ=0,\"HBSIM_CABIN_BINH_DINH\",0 \r\n";
static char AT_MQTT_CONNECT[] = "AT+CMQTTCONNECT=0,\"tcp://broker.hivemq.com\",60,1 \r\n";
static char CRLF[]							= "\r\n";
typedef struct {

	// RX meta
	char *rxBufferBase;		// pointer to the rx buffer
	int rxBufferSize;		// the size of the rx buffer
	int rxBufferIndex;		// index into the rx buffer
	int rxMsgLength;		// 0 = message not ready. > 0 = complete message ready and value is the message length
	char terminator;			// terminating character for the rx stream

	// TX meta
//	char *txBufferPtr;		// pointer to the transmit buffer
//	int txBufferLength;		// tx buff length
//	int txBufferCount;		// counts the characters as they are transmitted
//	int txInProgress;		// flag indicates tx in progress (1) or idle (0)
  UART_HandleTypeDef *husart;
	// uart in use
	USART_TypeDef *usart;	// pointer to the usart device
	

} asseControlData;


typedef struct {
char * topic;
char * mess;
} Rx_Subrice_Sim;

extern asseControlData simIO;
//extern Rx_Subrice_Sim Rx_mess_subrice;
extern void asseSend(asseControlData *c, char *txBuffer, int bufferLen);
extern void asseISR(asseControlData *c);
extern void asseInitialise(asseControlData *c, USART_TypeDef *usart,UART_HandleTypeDef *husart, char terminator, char *rxBuffer, int rxBuffSize);
extern void startUSART_SIMServices(void);
extern void ISRSIMUART(void);
extern void clear_asseISR(asseControlData *c);
uint8_t check_data_sim(uint8_t * data, char * data_check);
void handle_payload_rx_sim(Rx_Subrice_Sim *rx,char * rx_string);
void handle_sdt_rx_sim(char * rx_string, char * sdt_string);

void SIMTransmit(char *cmd);
void SIMTransmit_ONLY(char *cmd);
bool sendATCommand(char *cmd, char *expected_response, uint32_t time_delay);
bool checkSimConnection(void);
bool checkSignalQuality(void);
bool checkInternetConnection(void) ;
bool checkMqttConnection(void) ;
#endif


