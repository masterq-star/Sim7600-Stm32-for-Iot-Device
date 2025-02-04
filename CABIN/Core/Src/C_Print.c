#include "C_Print.h"
//#include "usbd_cdc_if.h"

#define TIMEOUT_MS 100  // Define timeout duration in milliseconds

void c_print(const char *format, ...) {
//    char buffer[CDC_BUFFER_SIZE];  // Buffer to hold the formatted string
//    va_list args;
//    //uint32_t start_time;

//    va_start(args, format);                           // Start processing variadic arguments
//    vsnprintf(buffer, sizeof(buffer), format, args);  // Format the string
//    va_end(args);                                     // End processing variadic arguments

//    // Timeout mechanism
//    //start_time = HAL_GetTick();  // Record the start time
//    while (CDC_Transmit_FS((uint8_t *)buffer, strlen(buffer)) != USBD_OK) {
////        if ((HAL_GetTick() - start_time) >= TIMEOUT_MS) {
////            // Timeout occurred
////            //printf("USB Transmit Timeout\n");
////            return;
////        }
//    }

//    // Successfully transmitted
//    //printf("Data transmitted successfully: %s\n", buffer);
}