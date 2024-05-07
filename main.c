#include "stm32f10x.h" // Device header
#include "systick.h"
#include "ds1307.h"
#include "i2c_lcd.h"

/* Hello*/
#define NORMAL_MODE 0
#define ADJUST_MODE 1
#define ALARM_MODE 2

#define ALARM_ON 1
#define ALARM_OFF 0

uint8_t BUTTON_ADJUST_MODE = 0;
uint8_t LAST_BUTTON_ADJUST_MODE = 0;

uint8_t BUTTON_CURSOR_STATE = 0;
uint8_t LAST_BUTTON_CURSOR_STATE = 0;

uint8_t BUTTON_ADJUST_VALUE = 0;
uint8_t LAST_BUTTON_ADJUST_VALUE = 0;

uint8_t BUTTON_SET_TIME = 0;
uint8_t LAST_BUTTON_SET_TIME = 0;

uint8_t MODE = NORMAL_MODE;

uint8_t IsAlarm;

TimeStructTypedef ReadTime;
Date ReadDate;
TimeAndDate Ds1307Read;
TimeAndDate Temp;
TimeStructTypedef AlarmTime;

void IsTimeToAlarm(void);
void GPIOInit(void);
void NVIC_Configuration(void);
void TIM_Configuration(void);

int main(void)
{
	InitSysTick();
	GPIOInit();
	NVIC_Configuration();
	TIM_Configuration();
	DS1307_Init();
	// DS1307_Write(2, 56, 0, 1, 3, 12, 23);
	AlarmTime = DS1307_ReadAlarmTime(&IsAlarm);
	DS1307_Read();
	lcd_init();
	uint8_t CurrentPosition;

	while (1)
	{
		LAST_BUTTON_ADJUST_MODE = BUTTON_ADJUST_MODE;
		BUTTON_ADJUST_MODE = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_14);

		LAST_BUTTON_CURSOR_STATE = BUTTON_CURSOR_STATE;
		BUTTON_CURSOR_STATE = GPIO_ReadInputDataBit(GPIOC, GPIO_Pin_15);

		LAST_BUTTON_ADJUST_VALUE = BUTTON_ADJUST_VALUE;
		BUTTON_ADJUST_VALUE = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_8);

		LAST_BUTTON_SET_TIME = BUTTON_SET_TIME;
		BUTTON_SET_TIME = GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9);

		if (LAST_BUTTON_ADJUST_MODE == 0 && BUTTON_ADJUST_MODE == 1)
		{
			if (MODE == NORMAL_MODE)
			{
				MODE = ADJUST_MODE;
				CurrentPosition = 0;
				lcd_clear_display();
				Temp = DS1307_Read();
				lcd_DisplayRtc(&Ds1307Read);

				LcdSendCmd(0x81);
				Delay_SysTick(50);
				LcdSendCmd(0x0F);
				Delay_SysTick(50);
			}
			else if (MODE == ADJUST_MODE)
			{
				MODE = ALARM_MODE;
				CurrentPosition = 0;
				lcd_clear_display();
				lcd_DisplayAlarm(&AlarmTime);
				LcdSendCmd(0x0C);
				Delay_SysTick(50);
				LcdSendCmd(0x87);
				Delay_SysTick(50);
				if (IsAlarm)
				{
					LcdSendString("ON");
				}
				else
				{
					LcdSendString("OFF");
				}

				LcdSendCmd(0x87);
				Delay_SysTick(50);
				LcdSendCmd(0x0F);
				Delay_SysTick(50);
			}
			else if (MODE == ALARM_MODE)
			{
				MODE = NORMAL_MODE;
				lcd_clear_display();
				LcdSendCmd(0x0C);
				Delay_SysTick(50);
			}
		}
		//
		if (MODE == NORMAL_MODE)
		{
			Ds1307Read = DS1307_Read();
			lcd_DisplayRtc(&Ds1307Read);
			if (IsAlarm)
			{
				IsTimeToAlarm();
			}
			if (LAST_BUTTON_SET_TIME == 0 && BUTTON_SET_TIME == 1)
			{
				if (!GPIO_ReadOutputDataBit(GPIOC, GPIO_Pin_13))
				{
					GPIO_SetBits(GPIOC, GPIO_Pin_13);
				}
			}
		}
		else if (MODE == ADJUST_MODE)
		{

			// if (BUTTON_SET_TIME == 1 && LAST_BUTTON_SET_TIME == 0)
			if (BUTTON_SET_TIME == 1)
			{
				while (GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_9))
				{
				}
				DS1307_Write_Struct(&Temp);
				lcd_clear_display();
				LcdSendCmd(0x0C);
				Delay_SysTick(50);
				MODE = NORMAL_MODE;
			}

			if (BUTTON_CURSOR_STATE == 1 && LAST_BUTTON_CURSOR_STATE == 0)
			{
				(CurrentPosition >= 5) ? (CurrentPosition = 0) : (CurrentPosition++);

				switch (CurrentPosition)
				{
				case (1):
					LcdChangeCursorPosition(0x84);
					break;
				case (2):
					LcdChangeCursorPosition(0x87);
					break;
				case (3):
					LcdChangeCursorPosition(0xC1);
					break;
				case (4):
					LcdChangeCursorPosition(0xC4);
					break;
				case (5):
					LcdChangeCursorPosition(0xC9);
					break;
				default:
					LcdSendCmd(0x81);
					LcdChangeCursorPosition(0x81);
					break;
				}
			}

			if (BUTTON_ADJUST_VALUE && !LAST_BUTTON_ADJUST_VALUE)
			{
				switch (CurrentPosition)
				{
				case (1):
				{
					uint8_t binMinute = BcdToBin(Temp.minute);
					(binMinute >= 59) ? (binMinute = 0) : (binMinute++);
					Temp.minute = BinToBcd(binMinute);
					LcdSendCmd(0x83);
					Delay_SysTick(50);
					LcdSendData(((Temp.minute >> 4) & 0x0f) + 0x30);
					LcdSendData((Temp.minute & 0x0f) + 0x30);
					break;
				}
				case (2):
				{
					uint8_t binSecond = BcdToBin(Temp.second);
					(binSecond >= 59) ? (binSecond = 0) : (binSecond++);
					Temp.second = BinToBcd(binSecond);
					LcdSendCmd(0x86);
					Delay_SysTick(50);
					LcdSendData(((Temp.second >> 4) & 0x0f) + 0x30);
					LcdSendData((Temp.second & 0x0f) + 0x30);
					break;
				}
				case (3):
				{
					uint8_t binDate = BcdToBin(Temp.date);
					uint8_t binMonth = BcdToBin(Temp.month);
					uint8_t binYear = BcdToBin(Temp.year);
					uint8_t maxDay;

					if (binMonth == 2)
					{
						(binYear % 4 == 0) ? (maxDay = 29) : (maxDay = 28);
					}
					else if (binMonth == 1 || binMonth == 3 ||
							 binMonth == 5 || binMonth == 7 ||
							 binMonth == 8 || binMonth == 10 ||
							 binMonth == 12)
					{
						maxDay = 31;
					}
					else if (binMonth == 4 || binMonth == 6 ||
							 binMonth == 9 || binMonth == 11)
					{
						maxDay = 30;
					}

					(binDate >= maxDay) ? (binDate = 1) : (binDate++);

					Temp.date = BinToBcd(binDate);
					LcdSendCmd(0xC0);
					Delay_SysTick(50);
					LcdSendData(((Temp.date >> 4) & 0x0f) + 0x30);
					LcdSendData((Temp.date & 0x0f) + 0x30);
					break;
				}
				case (4):
				{
					uint8_t binMonth = BcdToBin(Temp.month);
					(binMonth >= 12) ? (binMonth = 1) : (binMonth++);
					Temp.month = BinToBcd(binMonth);
					LcdSendCmd(0xC3);
					Delay_SysTick(50);
					LcdSendData(((Temp.month >> 4) & 0x0f) + 0x30);
					LcdSendData((Temp.month & 0x0f) + 0x30);
					break;
				}
				case (5):
				{
					uint8_t binYear = BcdToBin(Temp.year);
					(binYear >= 99) ? (binYear = 0) : (binYear++);
					Temp.year = BinToBcd(binYear);
					LcdSendCmd(0xC8);
					Delay_SysTick(50);
					LcdSendData(((Temp.year >> 4) & 0x0f) + 0x30);
					LcdSendData((Temp.year & 0x0f) + 0x30);
					break;
				}
				default:
				{
					uint8_t binHour = BcdToBin(Temp.hour);
					(binHour >= 23) ? (binHour = 0) : (binHour++);
					Temp.hour = BinToBcd(binHour);
					LcdSendCmd(0x80);
					Delay_SysTick(50);
					LcdSendData(((Temp.hour >> 4) & 0x0f) + 0x30);
					LcdSendData((Temp.hour & 0x0f) + 0x30);
					break;
				}
				}
			}
		}
		else if (MODE == ALARM_MODE)
		{
			if (BUTTON_CURSOR_STATE == 1 && LAST_BUTTON_CURSOR_STATE == 0)
			{
				(CurrentPosition >= 3) ? (CurrentPosition = 0) : (CurrentPosition++);
				switch (CurrentPosition)
				{
				case (1):
					LcdChangeCursorPosition(0xC1);
					break;
				case (2):
					LcdChangeCursorPosition(0xC4);
					break;
				case (3):
					LcdChangeCursorPosition(0xC7);
					break;
				default:
					LcdChangeCursorPosition(0x87);
					break;
				}
			}

			if (BUTTON_ADJUST_VALUE && !LAST_BUTTON_ADJUST_VALUE)
			{
				switch (CurrentPosition)
				{
				case (1):
				{
					uint8_t binHour = BcdToBin(AlarmTime.hour);
					(binHour >= 23) ? (binHour = 0) : (binHour++);
					AlarmTime.hour = BinToBcd(binHour);
					LcdSendCmd(0xC0);
					Delay_SysTick(50);
					LcdSendData(((AlarmTime.hour >> 4) & 0x0f) + 0x30);
					LcdSendData((AlarmTime.hour & 0x0f) + 0x30);
					break;
				}
				case (2):
				{
					uint8_t binMinute = BcdToBin(AlarmTime.minute);
					(binMinute >= 59) ? (binMinute = 0) : (binMinute++);
					AlarmTime.minute = BinToBcd(binMinute);
					LcdSendCmd(0xC3);
					Delay_SysTick(50);
					LcdSendData(((AlarmTime.minute >> 4) & 0x0f) + 0x30);
					LcdSendData((AlarmTime.minute & 0x0f) + 0x30);
					break;
				}
				case (3):
				{
					uint8_t binSecond = BcdToBin(AlarmTime.second);
					(binSecond >= 59) ? (binSecond = 0) : (binSecond++);
					AlarmTime.second = BinToBcd(binSecond);
					LcdSendCmd(0xC6);
					Delay_SysTick(50);
					LcdSendData(((AlarmTime.second >> 4) & 0x0f) + 0x30);
					LcdSendData((AlarmTime.second & 0x0f) + 0x30);
					break;
				}
				default:
				{
					LcdSendCmd(0x87);
					Delay_SysTick(50);
					if (IsAlarm)
					{
						IsAlarm = ALARM_OFF;
						LcdSendString("OFF");
					}
					else
					{
						IsAlarm = ALARM_ON;
						LcdSendCmd(0x89);
						Delay_SysTick(50);
						LcdSendString(" ");
						LcdSendCmd(0x87);
						Delay_SysTick(50);
						LcdSendString("ON");
					}
					LcdSendCmd(0x87);
					Delay_SysTick(50);
					LcdSendCmd(0x0F);
					Delay_SysTick(50);
				}
				}
			}

			if (BUTTON_SET_TIME == 1 && LAST_BUTTON_SET_TIME == 0)
			{
				DS1307_Set_Alarm(&AlarmTime, IsAlarm);
			}
			//
		}
	}
}

void IsTimeToAlarm(void)
{
	if ((Ds1307Read.hour == AlarmTime.hour) && (Ds1307Read.minute == AlarmTime.minute) && (Ds1307Read.second == AlarmTime.second))
	{
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		TIM_Cmd(TIM2, ENABLE);
	}
}
void GPIOInit(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C2, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);

	//
	GPIO_SetBits(GPIOB, GPIO_Pin_10 | GPIO_Pin_11);
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11;
	GPIO_Init(GPIOB, &GPIO_InitStruct);

	GPIO_InitTypeDef GPIOC_InitStruct;
	GPIOC_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIOC_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOC_InitStruct.GPIO_Pin = GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init(GPIOC, &GPIOC_InitStruct);

	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	GPIOC_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIOC_InitStruct.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init(GPIOC, &GPIOC_InitStruct);

	GPIO_InitTypeDef GPIOB_InitStruct;
	GPIOB_InitStruct.GPIO_Mode = GPIO_Mode_IPD;
	GPIOB_InitStruct.GPIO_Speed = GPIO_Speed_50MHz;
	GPIOB_InitStruct.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIOB_InitStruct);

	I2C_InitTypeDef I2C2_InitStruct;
	I2C2_InitStruct.I2C_Ack = I2C_Ack_Disable;
	I2C2_InitStruct.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C2_InitStruct.I2C_ClockSpeed = 100000;
	I2C2_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C2_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C2_InitStruct.I2C_OwnAddress1 = 0;
	I2C_Init(I2C2, &I2C2_InitStruct);
	I2C_Cmd(I2C2, ENABLE);
}

void NVIC_Configuration(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

	NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

	NVIC_Init(&NVIC_InitStructure);
}

void TIM_Configuration(void)
{
	TIM_TimeBaseInitTypeDef TIM_TimeBaseInitStruct;
	TIM_TimeBaseInitStruct.TIM_CounterMode = TIM_CounterMode_Up;
	TIM_TimeBaseInitStruct.TIM_Period = 33333;
	TIM_TimeBaseInitStruct.TIM_Prescaler = 7200 * 9 - 1;
	;
	TIM_TimeBaseInit(TIM2, &TIM_TimeBaseInitStruct);

	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_ITConfig(TIM2, TIM_IT_Update, ENABLE);
	TIM_Cmd(TIM2, DISABLE);
}

void TIM2_IRQHandler(void)
{
	GPIO_SetBits(GPIOC, GPIO_Pin_13);
	TIM_ClearFlag(TIM2, TIM_FLAG_Update);
	TIM_Cmd(TIM2, DISABLE);
}