#ifndef __MOSBUS_MASTER__
#define __MOSBUS_MASTER__
#include "main.h"

#include "modbus_crc.h"


typedef struct{
  float VOLT_V1N;
	float VOLT_V2N;
	float VOLT_V3N;
	float AVERAGE_V_LN;
	float VOLT_V12;
	float VOLT_V23;
	float VOLT_V31;
	float AVERAGE_V_LL;
	float I1;
	float I2;
	float I3;
	float AVERAGE_I;
	float KW1;
	float KW2;
	float KW3;
	float KVA1;
	float KVA2;
	float KVA3;
	float KVAr1;
	float KVAr2;
	float KVAr3;
	float TOTAL_KW;
	float TOTAL_KVA;
	float TOTAL_KVAR;
	float PF1;
	float PF2;
	float PF3;
	float AVERAGE_PF;
	float FREQUENCY;
	float KWH;
	float KVAH;
	float KVARH;
}MFMDATA;
void sendData (uint8_t *data);
void poll_request_read_res(uint8_t Slaveid , uint8_t Functioncode , uint16_t Starting_Address , uint16_t Number_Address);
float f_2uint_float(uint16_t uint1, uint16_t uint2);
void Import_data_MFM(MFMDATA * MFM_STRUCT , uint16_t * array_data);
void Import_data_ARRAY(uint16_t * array, uint8_t * data_import);
#endif
