/*
 * lcd.h
 *
 *  Created on: 2022. 4. 10.
 *      Author: hyung
 */

#ifndef INC_LCD_H_
#define INC_LCD_H_
#include "main.h"
#include "i2c.h"
#include "usart.h"

//HAL_StatusTypeDef Res;

void I2CScan();
HAL_StatusTypeDef LCDSendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags);
void LCD_SendData(uint8_t lcd_addr, uint8_t data);
void LCD_SendCommand(uint8_t lcd_addr, uint8_t cmd);
void LCD_SendString(uint8_t lcd_addr, char * str);
void LCDInit(uint8_t lcd_addr);
void LCDPrint(int i, char *str);
void init();
void loop();

#endif /* INC_LCD_H_ */
