#ifndef I2CLCD_H
#define I2CLCD_H
#include "stm32f10x.h" // Device header
#include "systick.h"
#include "ds1307.h"
#define LCD_ADDRESS 0x4E

void lcd_send_byte(char data)
{
	I2C_GenerateSTART(I2C2, ENABLE);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_MODE_SELECT))
	{
	};
	I2C_Send7bitAddress(I2C2, 0x4E, I2C_Direction_Transmitter);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED))
	{
	};
	I2C_SendData(I2C2, data);
	while (!I2C_CheckEvent(I2C2, I2C_EVENT_MASTER_BYTE_TRANSMITTED))
	{
	};
	I2C_GenerateSTOP(I2C2, ENABLE);
}

void lcd_send_data(char data)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (data & 0xf0);
	data_l = ((data << 4) & 0xf0);
	data_t[0] = data_u | 0x0D; // en=1, rs=0
	data_t[1] = data_u | 0x09; // en=0, rs=0
	data_t[2] = data_l | 0x0D; // en=1, rs=0
	data_t[3] = data_l | 0x09; // en=0, rs=0
	for (uint8_t i = 0; i < 4; ++i)
	{
		lcd_send_byte(data_t[i]);
	}
}

void lcd_send_cmd(char cmd)
{
	char data_u, data_l;
	uint8_t data_t[4];
	data_u = (cmd & 0xf0);
	data_l = ((cmd << 4) & 0xf0);
	data_t[0] = data_u | 0x0C; // en=1, rs=0
	data_t[1] = data_u | 0x08; // en=0, rs=0
	data_t[2] = data_l | 0x0C; // en=1, rs=0
	data_t[3] = data_l | 0x08; // en=0, rs=0
	for (uint8_t i = 0; i < 4; ++i)
	{
		lcd_send_byte(data_t[i]);
	}
}

void lcd_send_string(uint8_t *str)
{
	while (*str)
		lcd_send_data(*str++);
}

void lcd_clear_display(void)
{
	lcd_send_cmd(0x01); // clear display
	Delay_SysTick(50);
}

void lcd_init(void)
{
	lcd_send_cmd(0x33); /* set 4-bits interface */
	Delay_SysTick(50);
	lcd_send_cmd(0x32);
	Delay_SysTick(50);
	lcd_send_cmd(0x28); /* start to set LCD function */
	Delay_SysTick(50);
	lcd_send_cmd(0x06); /* set entry mode */
	Delay_SysTick(50);
	lcd_send_cmd(0x0C); /* set display to on */
	Delay_SysTick(50);
	lcd_send_cmd(0x02); /* move cursor to home and set data address to 0 */
	Delay_SysTick(50);
	lcd_send_cmd (0x80);
	Delay_SysTick(50);
}

// lcd go to X(first row), Y(second row) line
void lcd_GoToXY(char row, char col)
{
	char pos;

	if (row < 2)
	{
		pos = 0x80 | (row << 6);

		if (col < 16)
			pos = pos + col;

		lcd_send_cmd(pos);
	}
}

// lcd display rtc time function
void lcd_DisplayRtcTime(char hour, char min, char sec)
{
	lcd_GoToXY(0, 0);
	lcd_send_data(((hour >> 4) & 0x0f) + 0x30);
	lcd_send_data((hour & 0x0f) + 0x30);
	lcd_send_data(':');

	lcd_send_data(((min >> 4) & 0x0f) + 0x30);
	lcd_send_data((min & 0x0f) + 0x30);
	lcd_send_data(':');

	lcd_send_data(((sec >> 4) & 0x0f) + 0x30);
	lcd_send_data((sec & 0x0f) + 0x30);
}

// lcd display rtc date function
void lcd_DisplayRtcDate(char day, char month, char year)
{
	lcd_GoToXY(1, 0);
	lcd_send_data(((day >> 4) & 0x0f) + 0x30);
	lcd_send_data((day & 0x0f) + 0x30);
	lcd_send_data('/');

	lcd_send_data(((month >> 4) & 0x0f) + 0x30);
	lcd_send_data((month & 0x0f) + 0x30);
	lcd_send_data('/');

	lcd_send_data(((year >> 4) & 0x0f) + 0x30);
	lcd_send_data((year & 0x0f) + 0x30);
}

void lcd_DisplayRtc(TimeAndDate *TimeAndDatePtr)
{
	lcd_GoToXY(0, 0);
	lcd_send_data(((TimeAndDatePtr->hour >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->hour & 0x0f) + 0x30);
	lcd_send_data(':');

	lcd_send_data(((TimeAndDatePtr->minute >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->minute & 0x0f) + 0x30);
	lcd_send_data(':');

	lcd_send_data(((TimeAndDatePtr->second >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->second & 0x0f) + 0x30);

	lcd_GoToXY(1, 0);
	lcd_send_data(((TimeAndDatePtr->date >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->date & 0x0f) + 0x30);
	lcd_send_data('/');

	lcd_send_data(((TimeAndDatePtr->month >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->month & 0x0f) + 0x30);
	lcd_send_data('/');

	lcd_send_data(((0x20 >> 4) & 0x0f) + 0x30);
	lcd_send_data((0x20 & 0x0f) + 0x30);
	lcd_send_data(((TimeAndDatePtr->year >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->year & 0x0f) + 0x30);
}

void lcd_DisplayAlarm (TimeStructTypedef *TimeAndDatePtr)
{
	lcd_GoToXY(0, 0);
	
	lcd_send_string("ALARM: ");

	lcd_GoToXY(1, 0);
	lcd_send_data(((TimeAndDatePtr->hour >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->hour & 0x0f) + 0x30);
	lcd_send_data(':');

	lcd_send_data(((TimeAndDatePtr->minute >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->minute & 0x0f) + 0x30);
	lcd_send_data(':');

	lcd_send_data(((TimeAndDatePtr->second >> 4) & 0x0f) + 0x30);
	lcd_send_data((TimeAndDatePtr->second & 0x0f) + 0x30);
}

void lcd_change_cursor_position (uint8_t position)
{
	lcd_send_cmd(position);
	Delay_SysTick(50);
	lcd_send_cmd(0x0F);
	Delay_SysTick(50);	
}

#endif
