#include "stm32f1xx_hal.h"

#define LCD_WIDTH 20
#define LCD_HEIGHT 4


void lcd_init (void);   // initialize lcd

void lcd_write_i2c(char saddr,uint8_t *buffer, uint8_t length);
	
void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (char *str);  // send string to the lcd

void lcd_clear (void);  

void setCursor(int a, int b);
void updateLcdContent(int row, const char *newContent);

