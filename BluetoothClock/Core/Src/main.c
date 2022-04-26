/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
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
#include "adc.h"
#include "i2c.h"
#include "rtc.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "lcd.h"
#include <string.h>
#include <stdio.h>
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */
typedef struct{
	uint16_t music_scale;
	uint16_t note_duration;
}MelTypeDef;
/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define UP_KEY_MIN  0
#define UP_KEY_MAX 100

#define DOWN_KEY_MIN 800
#define DOWN_KEY_MAX 900

#define RIGHT_KEY_MIN 2900
#define RIGHT_KEY_MAX 3100

#define LEFT_KEY_MIN 1900
#define LEFT_KEY_MAX 2000

#define LONG_CLICK_MIN 1500
#define LONG_CLICK_MAX 5000

#define DOUBLE_CLICK_MIN 50
#define DOUBLE_CLICK_MAX 150

#define C	262
#define D	294
#define E	330
#define F	349
#define G	392
#define A	440
#define B	494
#define C1	523
#define D1	587
#define E1	659

#define MEL_NUM 55

enum CLOCK_MODE{
	NORMAL_STATE = 0,
	TIME_SETTING = 1,
	ALARM_TIME_SETTING = 2,
	MUSIC_SELECT = 3
};

enum CLOCK_BUTTON{
	NONE = 0,
	UP = 1,
	DOWN = 2,
	RIGHT = 3,
	LEFT = 4,
	SEL = 5
};

/*
MelTypeDef JackRabbit[MEL_NUM] = {
		(G, 2), (E, 1), (E, 1), (G, 1), (E, 1), (C, 2), // 6
        (D, 2), (E, 1), (D, 1), (C, 1), (E, 1), (G, 2), // 6
		(C1, 1), (G, 1), (C1, 1), (G, 1), (C1, 1), (G, 1), (E, 2), //7
		(G, 2), (D, 1), (F, 1), (E, 1), (D, 1), (C, 2)
};*/
MelTypeDef btype;
MelTypeDef ctype;

struct clock_state{
	enum CLOCK_MODE mode;
	enum CLOCK_BUTTON button;
	int8_t music_num;
};

struct clock_state current_state;

typedef struct {
	int8_t ampm;
	int8_t hours;
	int8_t minutes;
	int8_t seconds;
}TimeTypeDef;

TimeTypeDef ctime;  // current time
TimeTypeDef stime; // setting time
TimeTypeDef atime;  // alarm time

typedef struct {
  int8_t music_num;
  char music_title[16];
}MusicTypeDef;

MusicTypeDef alarm_music[] =
{
		{0,"Jack_Rabbit"},
		{1,"Carol_Jazz"},
		{2,"Big_Mac_Song"},
};

/* Base address of the Flash sectors Bank 1 */
#define ADDR_FLASH_SECTOR_0     ((uint32_t)0x08000000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_1     ((uint32_t)0x08004000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_2     ((uint32_t)0x08008000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_3     ((uint32_t)0x0800C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_4     ((uint32_t)0x08010000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_5     ((uint32_t)0x08020000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_6     ((uint32_t)0x08040000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_7     ((uint32_t)0x08060000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_8     ((uint32_t)0x08080000) /* Base @ of Sector 8, 128 Kbytes */
#define ADDR_FLASH_SECTOR_9     ((uint32_t)0x080A0000) /* Base @ of Sector 9, 128 Kbytes */
#define ADDR_FLASH_SECTOR_10    ((uint32_t)0x080C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_11    ((uint32_t)0x080E0000) /* Base @ of Sector 11, 128 Kbytes */

/* Base address of the Flash sectors Bank 2 */
#define ADDR_FLASH_SECTOR_12     ((uint32_t)0x08100000) /* Base @ of Sector 0, 16 Kbytes */
#define ADDR_FLASH_SECTOR_13     ((uint32_t)0x08104000) /* Base @ of Sector 1, 16 Kbytes */
#define ADDR_FLASH_SECTOR_14     ((uint32_t)0x08108000) /* Base @ of Sector 2, 16 Kbytes */
#define ADDR_FLASH_SECTOR_15     ((uint32_t)0x0810C000) /* Base @ of Sector 3, 16 Kbytes */
#define ADDR_FLASH_SECTOR_16     ((uint32_t)0x08110000) /* Base @ of Sector 4, 64 Kbytes */
#define ADDR_FLASH_SECTOR_17     ((uint32_t)0x08120000) /* Base @ of Sector 5, 128 Kbytes */
#define ADDR_FLASH_SECTOR_18     ((uint32_t)0x08140000) /* Base @ of Sector 6, 128 Kbytes */
#define ADDR_FLASH_SECTOR_19     ((uint32_t)0x08160000) /* Base @ of Sector 7, 128 Kbytes */
#define ADDR_FLASH_SECTOR_20     ((uint32_t)0x08180000) /* Base @ of Sector 8, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_21     ((uint32_t)0x081A0000) /* Base @ of Sector 9, 128 Kbytes  */
#define ADDR_FLASH_SECTOR_22     ((uint32_t)0x081C0000) /* Base @ of Sector 10, 128 Kbytes */
#define ADDR_FLASH_SECTOR_23     ((uint32_t)0x081E0000) /* Base @ of Sector 11, 128 Kbytes */

//#define MAGIC_NUM 0xdeadbeef

typedef struct {
  //uint32_t magic_num;
  TimeTypeDef setting_time;
  TimeTypeDef alarm_time;
  int8_t alarm_music_num;
}NVitemTypeDef;

typedef struct{
	int32_t time;
	GPIO_PinState level;
}ClickInfoDef;

ClickInfoDef click[3];

//#define nv_items  ((NVitemTypeDef *) ADDR_FLASH_SECTOR_3)

NVitemTypeDef default_nvitem =
{
  //MAGIC_NUM,
  {0, 0,0,0},
  {0, 0,0,0},
  0
};
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
//ADC_HandleTypeDef hadc1;
//RTC_HandleTypeDef hrtc;
//TIM_HandleTypeDef htim2;
//UART_HandleTypeDef huart6;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */
void time_display(void);
void music_select(char str[18]);
void adc_rcv(void);
void clock_set(TimeTypeDef * time);
void blink(TimeTypeDef * time, char str[18]);
void time_set(TimeTypeDef * time, char str[18]);
void flash_read(void);
HAL_StatusTypeDef flash_write1(uint32_t start, uint32_t end, TimeTypeDef * time);
HAL_StatusTypeDef flash_write2(uint32_t start, uint32_t end, uint16_t Melody[]);
static uint32_t GetSector(uint32_t Address);
static uint32_t GetSectorSize(uint32_t Sector);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
uint8_t alFlag = 0, muFlag = 0;

HAL_StatusTypeDef flash_write1(uint32_t start, uint32_t end, TimeTypeDef * time)
{
	uint32_t FirstSector,NbOfSectors,SECTORError;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef error= HAL_OK;
    //uint8_t *ptr;
    //__IO uint32_t MemoryProgramStatus;

	HAL_FLASH_Unlock();
	FirstSector = GetSector(start);//ADDR_FLASH_SECTOR_3;
	NbOfSectors = GetSector(end) - FirstSector + 1;

	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

	error = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
	if(error != HAL_OK) return error;

	//if (current_state.mode == TIME_SETTING)
	//{
	//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start, time->ampm);
	//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+4, time->hours);
	//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+8, time->minutes);
	//	HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+12, time->seconds);
	//}

	else if (current_state.mode == ALARM_TIME_SETTING)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_BYTE, start+16, time->ampm);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+20, time->hours);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+24, time->minutes);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+28, time->seconds);
	}

	else
	{
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start, time->ampm);
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+4, time->hours);
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+8, time->minutes);
		//HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+12, time->seconds);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start, default_nvitem.setting_time.ampm);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+4, default_nvitem.setting_time.hours);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+8, default_nvitem.setting_time.minutes);
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start+12, default_nvitem.setting_time.seconds);
	}

	HAL_FLASH_Lock();

	return 0;
}

HAL_StatusTypeDef flash_write2(uint32_t start, uint32_t end, uint16_t Melody[])
{
	uint32_t FirstSector,NbOfSectors,SECTORError;
	FLASH_EraseInitTypeDef EraseInitStruct;
	HAL_StatusTypeDef error= HAL_OK;

	HAL_FLASH_Unlock();
	FirstSector = GetSector(start);
	/* Get the number of sector to erase from 1st sector*/
	NbOfSectors =  1;
	/* Fill EraseInit structure*/
	EraseInitStruct.TypeErase     = FLASH_TYPEERASE_SECTORS;
	EraseInitStruct.VoltageRange  = FLASH_VOLTAGE_RANGE_3;
	EraseInitStruct.Sector        = FirstSector;
	EraseInitStruct.NbSectors     = NbOfSectors;

	error = HAL_FLASHEx_Erase(&EraseInitStruct, &SECTORError);
	if(error != HAL_OK) return error;

	for(int i = 0 ; Melody[i] != 0 ; i++)
	{
		HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, start, Melody[i]);
		start += 4;
	}

	HAL_FLASH_Lock();

	return 0;
}

static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  if((Address < ADDR_FLASH_SECTOR_1) && (Address >= ADDR_FLASH_SECTOR_0))
  {
    sector = FLASH_SECTOR_0;
  }
  else if((Address < ADDR_FLASH_SECTOR_2) && (Address >= ADDR_FLASH_SECTOR_1))
  {
    sector = FLASH_SECTOR_1;
  }
  else if((Address < ADDR_FLASH_SECTOR_3) && (Address >= ADDR_FLASH_SECTOR_2))
  {
    sector = FLASH_SECTOR_2;
  }
  else if((Address < ADDR_FLASH_SECTOR_4) && (Address >= ADDR_FLASH_SECTOR_3))
  {
    sector = FLASH_SECTOR_3;
  }
  else if((Address < ADDR_FLASH_SECTOR_5) && (Address >= ADDR_FLASH_SECTOR_4))
  {
    sector = FLASH_SECTOR_4;
  }
  else if((Address < ADDR_FLASH_SECTOR_6) && (Address >= ADDR_FLASH_SECTOR_5))
  {
    sector = FLASH_SECTOR_5;
  }
  else if((Address < ADDR_FLASH_SECTOR_7) && (Address >= ADDR_FLASH_SECTOR_6))
  {
    sector = FLASH_SECTOR_6;
  }
  else if((Address < ADDR_FLASH_SECTOR_8) && (Address >= ADDR_FLASH_SECTOR_7))
  {
    sector = FLASH_SECTOR_7;
  }
  else if((Address < ADDR_FLASH_SECTOR_9) && (Address >= ADDR_FLASH_SECTOR_8))
  {
    sector = FLASH_SECTOR_8;
  }
  else if((Address < ADDR_FLASH_SECTOR_10) && (Address >= ADDR_FLASH_SECTOR_9))
  {
    sector = FLASH_SECTOR_9;
  }
  else if((Address < ADDR_FLASH_SECTOR_11) && (Address >= ADDR_FLASH_SECTOR_10))
  {
    sector = FLASH_SECTOR_10;
  }
  else if((Address < ADDR_FLASH_SECTOR_12) && (Address >= ADDR_FLASH_SECTOR_11))
  {
    sector = FLASH_SECTOR_11;
  }
  else if((Address < ADDR_FLASH_SECTOR_13) && (Address >= ADDR_FLASH_SECTOR_12))
  {
    sector = FLASH_SECTOR_12;
  }
  else if((Address < ADDR_FLASH_SECTOR_14) && (Address >= ADDR_FLASH_SECTOR_13))
  {
    sector = FLASH_SECTOR_13;
  }
  else if((Address < ADDR_FLASH_SECTOR_15) && (Address >= ADDR_FLASH_SECTOR_14))
  {
    sector = FLASH_SECTOR_14;
  }
  else if((Address < ADDR_FLASH_SECTOR_16) && (Address >= ADDR_FLASH_SECTOR_15))
  {
    sector = FLASH_SECTOR_15;
  }
  else if((Address < ADDR_FLASH_SECTOR_17) && (Address >= ADDR_FLASH_SECTOR_16))
  {
    sector = FLASH_SECTOR_16;
  }
  else if((Address < ADDR_FLASH_SECTOR_18) && (Address >= ADDR_FLASH_SECTOR_17))
  {
    sector = FLASH_SECTOR_17;
  }
  else if((Address < ADDR_FLASH_SECTOR_19) && (Address >= ADDR_FLASH_SECTOR_18))
  {
    sector = FLASH_SECTOR_18;
  }
  else if((Address < ADDR_FLASH_SECTOR_20) && (Address >= ADDR_FLASH_SECTOR_19))
  {
    sector = FLASH_SECTOR_19;
  }
  else if((Address < ADDR_FLASH_SECTOR_21) && (Address >= ADDR_FLASH_SECTOR_20))
  {
    sector = FLASH_SECTOR_20;
  }
  else if((Address < ADDR_FLASH_SECTOR_22) && (Address >= ADDR_FLASH_SECTOR_21))
  {
    sector = FLASH_SECTOR_21;
  }
  else if((Address < ADDR_FLASH_SECTOR_23) && (Address >= ADDR_FLASH_SECTOR_22))
  {
    sector = FLASH_SECTOR_22;
  }
  else /* (Address < FLASH_END_ADDR) && (Address >= ADDR_FLASH_SECTOR_23) */
  {
    sector = FLASH_SECTOR_23;
  }
  return sector;
}

/**
  * @brief  Gets sector Size
  * @param  None
  * @retval The size of a given sector
  */
static uint32_t GetSectorSize(uint32_t Sector)
{
  uint32_t sectorsize = 0x00;
  if((Sector == FLASH_SECTOR_0) || (Sector == FLASH_SECTOR_1) || (Sector == FLASH_SECTOR_2) ||\
     (Sector == FLASH_SECTOR_3) || (Sector == FLASH_SECTOR_12) || (Sector == FLASH_SECTOR_13) ||\
     (Sector == FLASH_SECTOR_14) || (Sector == FLASH_SECTOR_15))
  {
    sectorsize = 16 * 1024;
  }
  else if((Sector == FLASH_SECTOR_4) || (Sector == FLASH_SECTOR_16))
  {
    sectorsize = 64 * 1024;
  }
  else
  {
    sectorsize = 128 * 1024;
  }
  return sectorsize;
}

volatile int timer_count,second,minute,hour,old_count;
char line[2][18], time_str[16];
uint32_t adc_value;
uint8_t btn_value;
volatile static uint8_t pos;
char uart_buf[30];

void clock_set(TimeTypeDef * time)
{
	if(time->seconds>=60)
	{
		time->minutes++;
		time->seconds = 0;
	}

	else if(time->minutes>=60)
	{
		time->hours++;
		time->minutes = 0;
	}

	else if(time->hours==13)
	{
		time->hours = 1;
		if (time->ampm == 0) time->ampm++;
		else if (time->ampm == 1) time->ampm--;
	}
}

void blink(TimeTypeDef * time, char str[18])
{
	static int blink_flag;
	static volatile int blink_count;
	uint8_t ampm = time->ampm;
	uint8_t hours = time->hours;
	uint8_t minutes = time->minutes;
	uint8_t seconds = time->seconds;

	if (timer_count - blink_count > 50)
	{
		if (blink_flag == 0) blink_flag++;
		else if (blink_flag == 1) blink_flag--;
		blink_count = timer_count;
	}

	if (blink_flag == 0)
	{
		if (ampm == 0) sprintf(str,"AM %02d:%02d:%02d", hours,minutes,seconds);
		else if (ampm == 1) sprintf(str,"PM %02d:%02d:%02d", hours,minutes,seconds);
	}

	else if (blink_flag == 1)
	{
		if (pos == 0) sprintf(str,"__ %02d:%02d:%02d", hours,minutes,seconds);
		else if (pos == 1)
		{
			if (ampm == 0) sprintf(str,"AM __:%02d:%02d", minutes,seconds);
			else if (ampm == 1) sprintf(str,"PM __:%02d:%02d", minutes,seconds);
		}
		else if (pos == 2)
		{
			if (ampm == 0) sprintf(str,"AM %02d:__:%02d", hours,seconds);
			else if (ampm == 0) sprintf(str,"PM %02d:__:%02d", hours,seconds);
		}
		else if (pos == 3)
		{
			if (ampm == 0) sprintf(str,"AM %02d:%02d:__", hours,minutes);
			else if (ampm == 0) sprintf(str,"PM %02d:%02d:__", hours,minutes);
		}
	}

	if (current_state.mode == ALARM_TIME_SETTING)
		strcat(str," AL");
	else strcat(str,"   ");
}

void time_set(TimeTypeDef * time, char str[18])
{
	if(pos == 0)
	{
		switch(current_state.button)
		{
		case RIGHT:
			pos++;
			break;
		case UP:
			if(time->ampm == 0)
				time->ampm++;
			else if (time->ampm == 1)
				time->ampm--;
			break;
		case DOWN:
			if(time->ampm == 0)
				time->ampm++;
			else if (time->ampm == 1)
				time->ampm--;
			break;
		default:
			break;
		}
	}

	else if (pos == 1)
	{
		switch(current_state.button)
		{
		case RIGHT:
			pos++;
			break;
		case LEFT:
			pos--;
			break;
		case UP:
			time->hours++;
			if(time->hours>12)
				time->hours = 1;
			break;
		case DOWN:
			time->hours--;
			if(time->hours<0)
				time->hours = 12;
			break;
		default:
			break;
		}
	}

	else if (pos == 2)
	{
		switch(current_state.button)
		{
		case RIGHT:
			pos++;
			break;
		case LEFT:
			pos--;
			break;
		case UP:
			time->minutes++;
			if(time->minutes>=60)
				time->minutes = 0;
			break;
		case DOWN:
			time->minutes--;
			if(time->minutes<0)
				time->minutes = 59;
			break;
		default:
			break;
		}
	}

	else if(pos == 3)
	{
		switch(current_state.button)
		{
		case RIGHT:
			pos = 0;

			if (current_state.mode == TIME_SETTING)
			{
				default_nvitem.setting_time.ampm = time->ampm;
				default_nvitem.setting_time.hours = time->hours;
				default_nvitem.setting_time.minutes = time->minutes;
				default_nvitem.setting_time.seconds = time->seconds;
				flash_write1(ADDR_FLASH_SECTOR_13, ADDR_FLASH_SECTOR_14, time);
			}
			else if(current_state.mode == ALARM_TIME_SETTING)
			{
				default_nvitem.alarm_time.ampm = time->ampm;
				default_nvitem.alarm_time.hours = time->hours;
				default_nvitem.alarm_time.minutes = time->minutes;
				default_nvitem.alarm_time.seconds = time->seconds;
				flash_write1(ADDR_FLASH_SECTOR_3, ADDR_FLASH_SECTOR_4, time);
			}
			current_state.mode = NORMAL_STATE;
			break;
		case LEFT:
			pos--;
			break;
		case UP:
			time->seconds++;
			if(time->seconds>=60)
				time->seconds = 0;
			break;
		case DOWN:
			time->seconds--;
			if(time->seconds<0)
				time->seconds = 59;
			break;
		default:
			break;
		}
	}

	blink(time, str);
}

void flash_read(void)
{
	current_state.mode = NORMAL_STATE;
	current_state.button = NONE;
	current_state.music_num = 0;

	//memcpy(&default_nvitem,nv_items,sizeof(NVitemTypeDef));
	stime.ampm = *((uint32_t*)0x08104000);
	stime.hours = *((uint32_t*)0x08104004);
	stime.minutes = *((uint32_t*)0x08104008);
	stime.seconds = *((uint32_t*)0x0810400C);

	ctime.ampm = stime.ampm;
	ctime.hours = stime.hours;
	ctime.minutes = stime.minutes;
	ctime.seconds = stime.seconds;

	atime.ampm = *((uint32_t*)0x0800C000);
	atime.hours = *((uint32_t*)0x0800C004);
	//atime.hours = *((uint32_t*)0x0810400K);
	atime.minutes = *((uint32_t*)0x0800C008);
	atime.seconds = *((uint32_t*)0x0800C00C);
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if(htim->Instance==TIM2)
	{
		if((timer_count%100)==0)
		{
			ctime.seconds++;
			clock_set(&ctime);
		}
		timer_count++;
		if (timer_count % 10 == 0 && timer_count != 0)
			old_count = timer_count;
	}
}

void adc_rcv(void)
{
	HAL_ADC_Start(&hadc1);
	if(HAL_ADC_PollForConversion(&hadc1,20) == HAL_OK) adc_value = HAL_ADC_GetValue(&hadc1);

	if (adc_value <= UP_KEY_MAX || btn_value == 1)
	{
		current_state.button = UP;
		btn_value = 0;
	}
	else if ((adc_value>= DOWN_KEY_MIN && adc_value<=DOWN_KEY_MAX) || btn_value == 2)
	{
		current_state.button = DOWN;
		btn_value = 0;
	}
	else if ((adc_value>= LEFT_KEY_MIN && adc_value<=LEFT_KEY_MAX) || btn_value == 4)
	{
		current_state.button = LEFT;
		btn_value = 0;
	}
	else if ((adc_value>= RIGHT_KEY_MIN && adc_value<=RIGHT_KEY_MAX) || btn_value == 3)
	{
		current_state.button = RIGHT;
		btn_value = 0;
	}
	else
	{
		current_state.button = NONE;
	}
}

void time_display(void)
{
	memset(line,0,sizeof(line));

	if(current_state.mode == NORMAL_STATE)
	{
		sprintf(line[0],"Korea Polytech");
		if (ctime.ampm == 0) sprintf(line[1],"AM %02d:%02d:%02d", ctime.hours, ctime.minutes, ctime.seconds);
		else if (ctime.ampm == 1) sprintf(line[1],"PM %02d:%02d:%02d", ctime.hours, ctime.minutes, ctime.seconds);
		if (timer_count - old_count == 10)
			flash_write1(ADDR_FLASH_SECTOR_13, ADDR_FLASH_SECTOR_14, &ctime);
	}
	else if(current_state.mode == TIME_SETTING)
	{
		sprintf(line[0],"Time Setting  ");
		time_set(&stime, line[1]);
		ctime.ampm = default_nvitem.setting_time.ampm;
		ctime.hours = default_nvitem.setting_time.hours;
		ctime.minutes = default_nvitem.setting_time.minutes;
		ctime.seconds = default_nvitem.setting_time.seconds;
	}
	else if(current_state.mode == ALARM_TIME_SETTING)
	{
		sprintf(line[0],"Alarm Setting ");
		time_set(&atime, line[1]);
	}
	else if (current_state.mode == MUSIC_SELECT)
	{
		sprintf(line[0],"Music Setting ");
		music_select(line[1]);
	}

	LCDPrint(0, line[0]);
	LCDPrint(1, line[1]);
}

uint32_t last_time,current_time,time_interval, last_time_interval;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  if (huart->Instance == USART6)
  {
	  HAL_UART_Receive_IT(&huart6, &btn_value, 1);
  }
}

void music_select(char str[18])
{
  int pos2, count;

  pos2 = current_state.music_num;
  count = sizeof(alarm_music)/sizeof(alarm_music[0]);  //total music count

  switch(current_state.button)
  {
    case UP:
    	pos2++;
    	if(pos2==count)
    	{
          pos2 =0;
    	}
    	break;
    case DOWN:
    	pos2--;
    	if(pos2 < 0)
    	{
          pos2 =count-1;
    	}
    	break;
    default:
    	break;
  }
  current_state.music_num =  pos;

  memset(str,0,sizeof(str));
  if(pos2 == 0)
  {
    sprintf(str, "1. Jack Rabbit <<");
    if(muFlag == 1)
    {
    	sprintf(str, "Mel1 Selected!\r\n");
    	//Memory1(ADDR_FLASH_SECTOR_14,ADDR_FLASH_SECTOR_15,melody1);
    	//Memory1(ADDR_FLASH_SECTOR_15,ADDR_FLASH_SECTOR_16,noteDurations1);
    }
  }
  else if(pos == 1)
  {
	  sprintf(str, "2. Carol Jazz <<");
	  if(muFlag == 1)
	  {
		  sprintf(str, "Mel2 Selected!  ");
		  //Memory1(ADDR_FLASH_SECTOR_14,ADDR_FLASH_SECTOR_15,melody1);
		  //Memory1(ADDR_FLASH_SECTOR_15,ADDR_FLASH_SECTOR_16,noteDurations1);
	  }
  }
  else if(pos == 2)
  {
	  sprintf(str, "3. Big Mac Song <<");
  	  if(muFlag == 1)
  	  {
  		  sprintf(str, "Mel3 Selected!  ");
  		  //Memory1(ADDR_FLASH_SECTOR_14,ADDR_FLASH_SECTOR_15,melody1);
  		  //Memory1(ADDR_FLASH_SECTOR_15,ADDR_FLASH_SECTOR_16,noteDurations1);
  	  }
  }
}

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	GPIO_PinState pin;
	int i;

	if (GPIO_Pin == GPIO_PIN_11)
	{
		current_time = HAL_GetTick();
		time_interval = current_time - last_time;
		last_time = current_time;

		pin = HAL_GPIO_ReadPin(GPIOD, GPIO_PIN_11);
		//pin = HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13);

		if (time_interval <= 2)
		{
		}

		else
		{
			click[2].time = click[1].time;
			click[2].level = click[1].level;

			click[1].time = click[0].time;
			click[1].level = click[0].level;

			click[0].time = time_interval;
			click[0].level = pin;

			if(click[2].level ==GPIO_PIN_RESET && click[1].level == GPIO_PIN_SET &&  click[0].level ==GPIO_PIN_RESET)
			{
				for(i=0;i<3;i++)
				{
					if(click[i].time>= DOUBLE_CLICK_MIN && click[i].time <= DOUBLE_CLICK_MAX)
						continue;
					else break;
				}
				if(i==3)
				{
					if(current_state.mode == NORMAL_STATE)
					{
						current_state.mode = MUSIC_SELECT;
						btn_value = SEL;
					}
					else if(current_state.mode == MUSIC_SELECT)
					{
						btn_value = SEL;
						if(muFlag == 0) muFlag = 1;
					}
					else if(current_state.mode == ALARM_TIME_SETTING)
					{
						if(alFlag == 0)	alFlag = 1;
						else if(alFlag == 1) alFlag = 0;
					}
				}
			}

			if(click[0].level == GPIO_PIN_RESET && click[0].time >=LONG_CLICK_MIN && click[0].time <=LONG_CLICK_MAX)
			{
				if(current_state.mode == NORMAL_STATE)
				{
					btn_value = SEL;
					alFlag = 1;
					current_state.mode = ALARM_TIME_SETTING;
				}
				else if(current_state.mode == TIME_SETTING)
				{
					current_state.mode = NORMAL_STATE;
				}
			}

			else if(click[0].level == GPIO_PIN_RESET && click[0].time < LONG_CLICK_MIN && click[0].time > DOUBLE_CLICK_MAX)
			{
				if(current_state.mode == NORMAL_STATE)
				{
					current_state.mode = TIME_SETTING;
					btn_value = SEL;
				}
			}
		}
	}
}
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
  MX_USART3_UART_Init();
  MX_ADC1_Init();
  MX_RTC_Init();
  MX_USART6_UART_Init();
  MX_I2C1_Init();
  MX_TIM2_Init();
  MX_TIM3_Init();
  MX_TIM4_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  init();
  HAL_TIM_Base_Init(&htim2);
  HAL_TIM_Base_Start_IT(&htim2);
  HAL_UART_Receive_IT(&huart6, &btn_value, 1);

  flash_read();
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  adc_rcv();
	  time_display();
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

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI|RCC_OSCILLATORTYPE_LSE;
  RCC_OscInitStruct.LSEState = RCC_LSE_ON;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLM = 8;
  RCC_OscInitStruct.PLL.PLLN = 180;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 7;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_5) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI15_10_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI15_10_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI15_10_IRQn);
  /* TIM2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM2_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM2_IRQn);
  /* TIM4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(TIM4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(TIM4_IRQn);
  /* USART6_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART6_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART6_IRQn);
  /* USART3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(USART3_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(USART3_IRQn);
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

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
