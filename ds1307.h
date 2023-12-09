#ifndef DS1307_H
#define DS1307_H
#include "stm32f10x.h" // Device header

#define DS1307_ADDRESS 0xD0
#define TIME_ADDRESS   0x00
#define DATE_ADDRESS   0x04

typedef struct DS1307_Data
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
	uint8_t day;
	uint8_t date;
	uint8_t month;
	uint8_t year;
} TimeAndDate;

typedef struct DS1307_Time
{
	uint8_t second;
	uint8_t minute;
	uint8_t hour;
} TimeStructTypedef;

typedef struct DS1307_Date
{
	uint8_t date;
	uint8_t month;
	uint8_t year;
} Date;

uint8_t bcd_to_bin(uint8_t bcd)
{
	return (bcd >> 4) * 10 + (bcd & 0x0f);
}

uint8_t bin_to_bcd(uint8_t bin)
{
	return ((bin / 10) << 4) | (bin % 10);
}

void DS1307_Init()
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)) {}
	//
	I2C_Send7bitAddress(I2C2, DS1307_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {}
	//
	I2C_SendData(I2C2, 0x07);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING)) {}
	//
	I2C_SendData(I2C2, 0x00);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED)) {}
	//
	I2C_GenerateSTOP(I2C2, ENABLE);
}

void DS1307_Write_Struct( TimeAndDate * TimeAndDatePtr)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};

	I2C_Send7bitAddress(I2C2, DS1307_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	};
	//
	I2C_SendData(I2C2, TIME_ADDRESS);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	//
	I2C_SendData(I2C2, TimeAndDatePtr->second & 0x7F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, TimeAndDatePtr->minute & 0x7F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, TimeAndDatePtr->hour & 0x3F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, TimeAndDatePtr->day & 0x07);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, TimeAndDatePtr->date & 0x3F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, TimeAndDatePtr->month & 0x1F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, TimeAndDatePtr->year & 0xFF);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	};
	I2C_GenerateSTOP(I2C2, ENABLE);
}

void DS1307_Write(uint8_t hour, uint8_t minute, uint8_t second,
				  uint8_t day, uint8_t date, uint8_t month, uint8_t year)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};

	I2C_Send7bitAddress(I2C2, DS1307_ADDRESS, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	};
	//
	I2C_SendData(I2C2, TIME_ADDRESS);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	//
	I2C_SendData(I2C2, bin_to_bcd(second) & 0x7F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, bin_to_bcd(minute) & 0x7F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, bin_to_bcd(hour) & 0x3F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, bin_to_bcd(day) & 0x07);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, bin_to_bcd(date) & 0x3F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, bin_to_bcd(month) & 0x1F);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING))
	{
	};
	I2C_SendData(I2C2, bin_to_bcd(year) & 0xFF);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	};
	I2C_GenerateSTOP(I2C2, ENABLE);
}

TimeStructTypedef DS1307_Read_Time(void)
{
	TimeStructTypedef MyTime = {0};
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};
	//
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	};
	//
	I2C_SendData(I2C2, TIME_ADDRESS);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	};
	// I2C_GenerateSTOP( I2C2, ENABLE);
	//
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};
	/*
		byte ke cuoi thu 3 se khong doc ngay ma se ma se cho thanh ghi btf (data tranfer finish)
	*/
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
	};
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_BTF))
	{
	};
	// 3
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
	};
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	MyTime.second = I2C_ReceiveData(I2C2);
	I2C_GenerateSTOP(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
	};
	MyTime.minute = I2C_ReceiveData(I2C2);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
	};
	MyTime.hour = I2C_ReceiveData(I2C2);
	return MyTime;
	//
}

Date DS1307_Read_Date(void)
{
	Date TempDate = {0};
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};
	//
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	};
	//
	I2C_SendData(I2C2, DATE_ADDRESS);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	};
	// I2C_GenerateSTOP( I2C2, ENABLE);
	//
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};
	/*
		byte ke cuoi thu 3 se khong doc ngay ma se ma se cho thanh ghi btf (data tranfer finish)
	*/
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED))
	{
	};
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_BTF))
	{
	};
	// 3
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
	};
	I2C_AcknowledgeConfig(I2C2, DISABLE);
	TempDate.date = I2C_ReceiveData(I2C2);
	I2C_GenerateSTOP(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
	};
	TempDate.month = I2C_ReceiveData(I2C2);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED))
	{
	};
	TempDate.year = I2C_ReceiveData(I2C2);
	return TempDate;
	//
}

TimeAndDate DS1307_Read(void)
{
	TimeAndDate TempTimeAndDate;
	I2C_AcknowledgeConfig(I2C2, ENABLE);
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};
	//
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	};
	//
	I2C_SendData(I2C2, TIME_ADDRESS);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	};
	//
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT)){}
	/*
		byte ke cuoi thu 3 se khong doc ngay ma se ma se cho thanh ghi btf (data tranfer finish)
	*/
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)) {}

	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
	TempTimeAndDate.second = I2C_ReceiveData(I2C2);

	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
	TempTimeAndDate.minute = I2C_ReceiveData(I2C2);

	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
	TempTimeAndDate.hour = I2C_ReceiveData(I2C2);

	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
	TempTimeAndDate.day = I2C_ReceiveData(I2C2);

	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_BTF)) {}

	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
		
	I2C_AcknowledgeConfig(I2C2, DISABLE);
		
	TempTimeAndDate.date = I2C_ReceiveData(I2C2);
	
	I2C_GenerateSTOP(I2C2, ENABLE);

	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
	
	TempTimeAndDate.month = I2C_ReceiveData(I2C2);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED)) {}
	TempTimeAndDate.year = I2C_ReceiveData(I2C2);
	return TempTimeAndDate;
}

void DS1307_Set_Alarm( TimeStructTypedef * TimeStructTypedefPtr, uint8_t IsAlarm)
{

	
	I2C_GenerateSTART( I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT )) {}
	//	
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED  )) {}
	//
	I2C_SendData(I2C2, 0x08);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {}	
	//
	I2C_SendData(I2C2, TimeStructTypedefPtr->second | (IsAlarm << 7));
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {}	
	//	
	I2C_SendData(I2C2, TimeStructTypedefPtr->minute );
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTING )) {}
	//	
	I2C_SendData(I2C2, TimeStructTypedefPtr->hour);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED )) {}
	//		
	I2C_GenerateSTOP( I2C2, ENABLE);
}

TimeStructTypedef DS1307_Read_Alarm_Time(uint8_t * IsAlarm)
{
	TimeStructTypedef AlarmTime;
	
	I2C_AcknowledgeConfig (I2C2, ENABLE);
	/* Master generrate start signal */
	I2C_GenerateSTART( I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT )) {}
		
	/* Master send slave address and action write */
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED)) {}
		
	/* Point to the 0x08 memory address */	
	I2C_SendData(I2C2, 0x08);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED )) {}

	/* Master regenerrate start signal */
	I2C_GenerateSTART( I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT )) {};
	/*
		byte ke cuoi thu 3 se khong doc ngay ma se ma se cho thanh ghi btf (data tranfer finish)
	*/
	/* Master send slave address and action read */		
	I2C_Send7bitAddress(I2C2, 0xD0, I2C_Direction_Receiver);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED  )) {};
	/* wait data to buffer */
	while (!I2C_GetFlagStatus(I2C2, I2C_FLAG_BTF)) {}
		
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED  )) {}
		
	I2C_AcknowledgeConfig (I2C2, DISABLE);

	AlarmTime.second = I2C_ReceiveData(I2C2);

		
	I2C_GenerateSTOP( I2C2, ENABLE);
		
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED  )) {}
	AlarmTime.minute = I2C_ReceiveData(I2C2);
		
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_RECEIVED  )) {}
	AlarmTime.hour = I2C_ReceiveData(I2C2);
	//
	*IsAlarm =  ( AlarmTime.second >> 7 );	
	AlarmTime.second = AlarmTime.second & ~(1 << 7);
	return AlarmTime;
}

#endif
