#include "ModbusMaster.h"
uint8_t TxData[8];





void sendData (uint8_t *data)
{
	HAL_GPIO_WritePin(TX_EN_GPIO_Port, TX_EN_Pin, GPIO_PIN_SET);
	HAL_UART_Transmit(&huart3, data, 8, 1000);
	HAL_GPIO_WritePin(TX_EN_GPIO_Port,TX_EN_Pin , GPIO_PIN_RESET);
}

void poll_request_read_res(uint8_t Slaveid , uint8_t Functioncode , uint16_t Starting_Address , uint16_t Number_Address)
{
  TxData[0] = Slaveid;  // slave address
  TxData[1] = Functioncode;  // Function code for Read Holding Registers

  TxData[2] = Starting_Address >>8;
  TxData[3] = Starting_Address & 0xFF;
  //The Register address will be 00000000 00000100 = 4 + 40001 = 40005

  TxData[4] = Number_Address >> 8;
  TxData[5] = Number_Address & 0xFF;
  // no of registers to read will be 00000000 00000101 = 5 Registers = 10 Bytes

  uint16_t crc = crc16(TxData, 6);
  TxData[6] = crc&0xFF;   // CRC LOW
  TxData[7] = (crc>>8)&0xFF;  // CRC HIGH

  sendData(TxData);
}
float f_2uint_float(uint16_t uint1, uint16_t uint2) {    // reconstruct the float from 2 unsigned integers

    uint32_t combined = ((uint32_t)uint1 << 16) | uint2;
    float result;
    // Use a union to safely reinterpret the bits of the uint32_t as a float
    union {
        uint32_t i;
        float f;
    } u;
    u.i = combined;
    result = u.f;
    return result;

}
void Import_data_MFM(MFMDATA * MFM_STRUCT , uint16_t * array_data){
					MFM_STRUCT->VOLT_V1N =  f_2uint_float(array_data[0],array_data[1]);
					MFM_STRUCT->VOLT_V2N =  f_2uint_float(array_data[2],array_data[3]);
					MFM_STRUCT->VOLT_V3N =  f_2uint_float(array_data[4],array_data[5]);
						MFM_STRUCT->AVERAGE_V_LN =  f_2uint_float(array_data[6],array_data[7]);
					MFM_STRUCT->VOLT_V12 =  f_2uint_float(array_data[8],array_data[9]);
					MFM_STRUCT->VOLT_V23 =  f_2uint_float(array_data[10],array_data[11]);
					MFM_STRUCT->VOLT_V31 =  f_2uint_float(array_data[12],array_data[13]);
						MFM_STRUCT->AVERAGE_V_LL =  f_2uint_float(array_data[14],array_data[15]);
					MFM_STRUCT->I1 =  f_2uint_float(array_data[16],array_data[17]);
					MFM_STRUCT->I2=  f_2uint_float(array_data[18],array_data[19]);
					MFM_STRUCT->I3 =  f_2uint_float(array_data[20],array_data[21]);
						MFM_STRUCT->AVERAGE_I =  f_2uint_float(array_data[22],array_data[23]);
					MFM_STRUCT->KW1 =  f_2uint_float(array_data[24],array_data[25]);
					MFM_STRUCT->KW2=  f_2uint_float(array_data[26],array_data[27]);
					MFM_STRUCT->KW3 =  f_2uint_float(array_data[28],array_data[29]);	
						MFM_STRUCT->KVA1 =  f_2uint_float(array_data[30],array_data[31]);
					MFM_STRUCT->KVA2=  f_2uint_float(array_data[32],array_data[33]);
					MFM_STRUCT->KVA3 =  f_2uint_float(array_data[34],array_data[35]);	
							MFM_STRUCT->KVAr1 =  f_2uint_float(array_data[36],array_data[37]);
					MFM_STRUCT->KVAr2=  f_2uint_float(array_data[38],array_data[39]);
					MFM_STRUCT->KVAr3 =  f_2uint_float(array_data[40],array_data[41]);	
							MFM_STRUCT->TOTAL_KW =  f_2uint_float(array_data[42],array_data[43]);
					MFM_STRUCT->TOTAL_KVA=  f_2uint_float(array_data[44],array_data[45]);
					MFM_STRUCT->TOTAL_KVAR =  f_2uint_float(array_data[46],array_data[47]);
							MFM_STRUCT->PF1 =  f_2uint_float(array_data[48],array_data[49]);
					MFM_STRUCT->PF2=  f_2uint_float(array_data[50],array_data[51]);
					MFM_STRUCT->PF3 =  f_2uint_float(array_data[52],array_data[53]);
							MFM_STRUCT->AVERAGE_PF =  f_2uint_float(array_data[54],array_data[55]);
					MFM_STRUCT->FREQUENCY=  f_2uint_float(array_data[56],array_data[57]);
					MFM_STRUCT->KWH =  f_2uint_float(array_data[58],array_data[59]);
							MFM_STRUCT->KVAH =  f_2uint_float(array_data[60],array_data[61]);
					MFM_STRUCT->KVARH=  f_2uint_float(array_data[62],array_data[63]);
}
void Import_data_ARRAY(uint16_t * array, uint8_t * data_import){

	for(int i = 0 ; i < 64 ; i ++)
	{
	 array[i] = data_import[i*2 + 3] << 8 | data_import[i*2 + 4];
	}

}
