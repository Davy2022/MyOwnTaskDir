/*
 * lcd.c
 *
 *  Created on: 2022. 4. 10.
 *      Author: hyung
 */
#include "main.h"
#include "lcd.h"
#include "i2c.h"
#include "usart.h"

#include <string.h>
#include <stdio.h>

#define LCD_ADDR 0x27 << 1
#define PIN_RS 1<< 0
#define PIN_EN 1<<2
#define BACKLIGHT 1<<3

HAL_StatusTypeDef Res;

void I2CScan()
{
	for(uint16_t i = 0; i<128; i++)
	{
		Res = HAL_I2C_IsDeviceReady(&hi2c1, i << 1, 1, 10);
		if (Res == HAL_OK)
		{
			char msg[64];
			snprintf(msg, sizeof(msg), "0X%02X", i);
			HAL_UART_Transmit(&huart3, (uint8_t*)msg, strlen(msg), HAL_MAX_DELAY);
		}
		else
			HAL_UART_Transmit(&huart3, (uint8_t*)".", 1, HAL_MAX_DELAY);
	}
	HAL_UART_Transmit(&huart3, (uint8_t*)".\r\n", 1, HAL_MAX_DELAY);
}

HAL_StatusTypeDef LCDSendInternal(uint8_t lcd_addr, uint8_t data, uint8_t flags)
{
	for(;;)
	{
		Res = HAL_I2C_IsDeviceReady(&hi2c1, lcd_addr, 1, HAL_MAX_DELAY);
		if(Res == HAL_OK) break;
	}

	uint8_t up = data & 0xF0;
	uint8_t lo = (data << 4) & 0xF0;

	uint8_t data_arr[4];
	data_arr[0] = up|flags|BACKLIGHT|PIN_EN;
	data_arr[1] = up|flags|BACKLIGHT;
	data_arr[2] = lo|flags|BACKLIGHT|PIN_EN;
	data_arr[3] = lo|flags|BACKLIGHT;

	Res = HAL_I2C_Master_Transmit(&hi2c1, lcd_addr, data_arr, sizeof(data_arr), HAL_MAX_DELAY);
	HAL_Delay(5);
	return Res;
}

void LCD_SendData(uint8_t lcd_addr, uint8_t data)
{
	LCDSendInternal(lcd_addr, data, PIN_RS);
}

void LCD_SendCommand(uint8_t lcd_addr, uint8_t cmd)
{
	LCDSendInternal(lcd_addr, cmd, 0);
}

void LCD_SendString(uint8_t lcd_addr, char * str)
{
	while(*str)
	{
		LCD_SendData(lcd_addr, (uint8_t)(*str));
		str++;
	}
}

void LCDInit(uint8_t lcd_addr)
{
	LCD_SendCommand(lcd_addr, 0b00110000);
	LCD_SendCommand(lcd_addr, 0b00000010);
	LCD_SendCommand(lcd_addr, 0b00001100);
	LCD_SendCommand(lcd_addr, 0b00000001);
}

void LCDPrint(int i, char *str)
{
	if (i == 0)
	{
		LCD_SendCommand(LCD_ADDR, 0b10000000);
		LCD_SendString(LCD_ADDR, str);
	}
	else
	{
		LCD_SendCommand(LCD_ADDR, 0b11000000);
		LCD_SendString(LCD_ADDR, str);
	}
}

void init()
{
	I2CScan();
	LCDInit(LCD_ADDR);
}

void loop()
{
	HAL_Delay(100);
}
