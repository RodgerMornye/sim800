/*
 * i2c-lcd.h
 *
 *  Created on: 9 мар. 2024 г.
 *      Author: Sutor
 */

#ifndef MAIN_I2C_LCD_H_
#define MAIN_I2C_LCD_H_


void lcd_init (void);   // initialize lcd

void lcd_send_cmd (char cmd);  // send command to the lcd

void lcd_send_data (char data);  // send data to the lcd

void lcd_send_string (int x, int y, char *str);  // send string to the lcd

void lcd_send_float (int x, int y, float f);  // send float to the lcd

void lcd_send_int (int x, int y, int i); // send int to the lcd

void lcd_put_cur(int row, int col);  // put cursor at the entered position row (0 or 1), col (0-15);

void lcd_clear (void);


#endif /* MAIN_I2C_LCD_H_ */
