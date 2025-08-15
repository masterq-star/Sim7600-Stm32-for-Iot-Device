#include "uart_sim_async.h"
#include "string.h"
#include "stdio.h"
#include "cmsis_os.h"
#include "C_Print.h"
char * RxMqtt0 ;
char *RxMqtt1;
char * RxMqtt2 ;
char *RxMqtt3;
char *RxMqtt4;
 char * pch;
 char * gsdt;
char *array[7];
uint8_t i=0 ;

uint8_t check = 0;
char * check_string;
char response[500];
extern UART_HandleTypeDef huart1;
extern uint8_t Rx_Sim_Buffer[500];
extern char buffer[100];


char sim_RxBuffer[SIM_RX_BUFFER] = {0};
asseControlData simIO;
//Rx_Subrice_Sim Rx_mess_subrice;

void handle_payload_rx_sim(Rx_Subrice_Sim *rx,char * rx_string)
{
 //char str[] ="- This, a sample string.";
 RxMqtt0 = strtok (rx_string,"\r\n");
 RxMqtt1=strtok(NULL, "\r\n");
	RxMqtt2=strtok(NULL, "\r\n");
	RxMqtt3=strtok(NULL, "\r\n");
	RxMqtt4=strtok(NULL, "\r\n");


//	sscanf(rx_string, "+%s+%s\r\n%s+%s\r\n%s", RxMqtt0, RxMqtt1, RxMqtt2,RxMqtt3,RxMqtt4);
//	strncpy(rx->topic,RxMqtt2,strlen(RxMqtt2));
//	strncpy(rx->mess,RxMqtt4,strlen(RxMqtt4));
	 rx->topic = RxMqtt2;
	  rx->mess = RxMqtt4;


}
void handle_sdt_rx_sim(char * rx_string, char * sdt_string)
{
 //char str[] ="- This, a sample string.";
	pch = strtok (rx_string,":");
	gsdt=strtok(NULL, ":");
	strcpy(sdt_string, gsdt);
}
void asseSend(asseControlData *c, char *txBuffer, int bufferLen){
			// block here if there's a transmission in progress
		//	uint32_t start = HAL_GetTick();
		//	while (c->txInProgress != 0){
		//		if ((HAL_GetTick() - start) > asseTimeout){
		//			return;
		//		}
		//	}
		//	c->txInProgress = 1;
		//	c->txBufferPtr = txBuffer;
		//	c->txBufferLength = bufferLen;
		//	c->txBufferCount = 1;
			// send the first character in the buffer to start transmission
	    
			while(HAL_UART_Transmit(c->husart, (unsigned char *)txBuffer, bufferLen , 20)== HAL_BUSY);
			c->usart->CR1 |= USART_CR1_RXNEIE;
		//	c->txInProgress = 0;
			//c->usart->DR = *c->txBufferPtr;
		}
void asseISR(asseControlData *c){

				// test for receive (RXNE flag set)
				if ((c->usart->SR & 0x20) != 0){
					// get the data
					char d = c->usart->DR;
					if ((d >= 32)) {
						// store the character, if room in the buffer
						if (c->rxBufferIndex < c->rxBufferSize) {
							*(c->rxBufferBase + c->rxBufferIndex++) = d;
						}
						// test for terminator
//						if (d == c->terminator) {
//							// terminator found, so set message ready flag to the length of the message
//							c->rxMsgLength = c->rxBufferIndex;
//							// and reset the index
//							c->rxBufferIndex = 0;
//						}
					}
					else{
							if(d == 0x0D || d == 0x0A){
									c->rxMsgLength = c->rxBufferIndex;
								//memset(c->rxBufferBase + c->rxBufferIndex,'a',500-c->rxBufferIndex);
								//for(int i = c->rxMsgLength ; i < 500 ; i++)
								//i = c->rxBufferIndex;
								//*(c->rxBufferBase )=0x01;
								//*(c->rxBufferBase + c->rxBufferIndex++) = 0x55;
									// and reset the index
									c->rxBufferIndex = 0;
							
							}
						
						
					}
				}
					// end if Rx
			//	// test for tx complete (Bit 6 TC: Transmission complete)
			//	if ((c->usart->SR & 0x40) != 0){
			//		
			//		// end transmission when null character encountered
			//		if (*c->txBufferPtr == 0){
			//			// end of transmission, set the TCCF flag in ICR to clear TC
			//			c->usart->SR |= 0x40;
			//			c->txInProgress = 0;
			//		}
			//		else {
			//			// if reached the end of the buffer, end the transmission
			//			if (c->txBufferCount++ >= c->txBufferLength){
			//				c->usart->SR |= 0x40;
			//				c->txInProgress = 0;
			//			}
			//			else{
			//				// send the next char
			//				c->usart->DR = *c->txBufferPtr++;
			//			}
			//		}
			//	} // end if host tx
			}

/*
 * Initialises the serial IO channel
 *
 */
void asseInitialise(asseControlData *c, USART_TypeDef *usart, UART_HandleTypeDef *husart, char terminator, char *rxBuffer, int rxBuffSize){

					// save the recieve buffer address & size
					c->rxBufferBase = rxBuffer;
					c->rxBufferSize = rxBuffSize;

					// save reference to the selected usart
					c->usart = usart;
					c->husart = husart;

					// set the rx terminating character
					c->terminator = terminator;


					// clear UE in CR1 to enable write to configuration registers
					c->usart->CR1 = 0;

					// set bit 12 in CR3 - disable Rx overrun interrupts
					//c->usart->CR3 = 0x1000;
					// set CR1:
					// Bit 6 TCIE: Transmission complete interrupt enable
					// Bit 5 RXNEIE: RX Not Empty interrupt enable
					// Bit 3 TE: Transmitter enable
					// Bit 2 RE: Receiver enable
					// Bit 13 UE: USART enable
					
					
					c->usart->CR1 |= USART_CR1_RXNEIE; // RE=1.. Enable the Receiver
					c->usart->CR1 |= (1<<2); // RE=1.. Enable the Receiver
					c->usart->CR1 |= (1<<3);  // TE=1.. Enable Transmitter
					c->usart->CR1 |= (1<<13);
					
						
}
void clear_asseISR(asseControlData *c){
 
 for(int i = 0 ; i < 500 ; i++) *(c->rxBufferBase +i)=0x01;
	c->rxMsgLength = 0;

}
void startUSART_SIMServices(void){
	 asseInitialise(&simIO, USART1,&huart1, 0x0d, sim_RxBuffer, SIM_RX_BUFFER);
}
inline void ISRSIMUART(void){
	asseISR(&simIO);
}
uint8_t check_data_sim(uint8_t * data, char * data_check)
{
if(strstr((char *)data,data_check)) // Use 0,5 For Roaming
      {
       return 1 ;
      }
else return 0;
}
void SIMTransmit(char *cmd)
{
  memset(buffer,0,sizeof(buffer));
  HAL_UART_Transmit(&huart1,(uint8_t *)cmd,strlen(cmd),1000);
	c_print("TRANSMIT: %s\n", cmd);
  HAL_UART_Receive (&huart1, (uint8_t *)buffer, 100, 100);
	c_print("RECEIVE: %s\n", cmd);
}
void SIMTransmit_ONLY (char *cmd)
{
memset(Rx_Sim_Buffer,0,sizeof(Rx_Sim_Buffer));
	c_print("TRANSMIT: %s\n", cmd);
HAL_UART_Transmit(&huart1,(uint8_t *)cmd,strlen(cmd),1000);

//HAL_UART_Receive(&huart1, Rx_Sim_Buffer, sizeof(Rx_Sim_Buffer), 1000);
}

// Function to send AT command and receive response
bool sendATCommand(char *cmd, char *expected_response, uint32_t time_delay) {
    memset(Rx_Sim_Buffer, 0, sizeof(Rx_Sim_Buffer));  // Clear the buffer
    HAL_UART_Transmit(&huart1, (uint8_t *)cmd, strlen(cmd), 1000);
	  c_print("TRANSMIT: %s\n", cmd);
   
	  vTaskDelay(time_delay / portTICK_PERIOD_MS);
    // Add a small delay to allow the SIM module to respond
	 // Receive the response
    HAL_UART_Receive(&huart1, Rx_Sim_Buffer, sizeof(Rx_Sim_Buffer), 1000);
	 c_print("RECEIVE: %s\n", Rx_Sim_Buffer);
    //vTaskDelay(pdMS_TO_TICKS(time_delay));
    
//	  HAL_GPIO_TogglePin(LED4_GPIO_Port,LED4_Pin);
   

    // Null-terminate the received buffer
    //if(strstr((char *)Rx_Sim_Buffer, "ERROR") != NULL) return false; // IF HAVE AN ERROR IN CHAR BUFFER ==> RETURN FALSE
    // Check if the expected response is in the received data
    return (strstr((char *)Rx_Sim_Buffer, expected_response) != NULL);
}
// Function to check SIM connection
bool checkSimConnection(void) {
    vTaskDelay(pdMS_TO_TICKS(300));
    return sendATCommand("AT\r\n", "OK",100);
}

bool checkSignalQuality(void)
{
	  vTaskDelay(pdMS_TO_TICKS(300));
    if (sendATCommand("AT+CSQ\r\n", "OK", 100))
    {
        // AT+CSQ response format: +CSQ: <rssi>,<ber>
        // Example: +CSQ: 15,99 or +CSQ: 99,99 (invalid signal)
        char *csqResponse = strstr((char *)Rx_Sim_Buffer, "+CSQ: ");
        if (csqResponse != NULL)
        {
            int rssi, ber;
            if (sscanf(csqResponse, "+CSQ: %d,%d", &rssi, &ber) == 2)
            {
                // Check for invalid signal indicator
                if (rssi == 99 && ber == 99)
                {
                    return false;
                }
                // Process valid RSSI and BER values
                
								return true;
            }
        }
    }
    return false;
}

bool checkInternetConnection(void) {
    return sendATCommand("AT+CGATT?\r\n", "OK",100);
}

bool checkMqttConnection(void) {
    // Replace with appropriate AT command to check MQTT connection
    return sendATCommand("AT+CMQTTCONNECT?\r\n", "OK",100);
}
