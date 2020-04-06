/* 
 * File:   WS2812_effects.h
 * Author: dima_ses
 *
 * Created on September 18, 2019, 9:14 PM
 */

#ifndef WS2812_EFFECTS_H
#define	WS2812_EFFECTS_H
#include "WS2812.h"
#include <avr/interrupt.h>
#include "button.h"
void WS_efcts_init(void);   //�������������
uint8_t Get_fade_stage(uint8_t n);  //���������� ������ fade �������
void Fade_pixel(uint8_t n, uint8_t amount); 
void Fade_all(uint8_t step);
void Fill_color(uint8_t r, uint8_t g, uint8_t b); //��������� ���� ���� ������
void Fill_rainbow(uint8_t step);    //��������� ���� ���� ������� � �����
void Set_operation(uint8_t setOper);    
#endif	/* WS2812_EFFECTS_H */

