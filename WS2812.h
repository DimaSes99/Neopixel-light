/* 
 * File:   WS2812.h
 * Author: dima_ses
 *
 * Created on September 17, 2019, 6:34 PM
 */
#ifndef WS2812_H
#define	WS2812_H

#define F_CPU 16000000UL
#include "main.h"

#define WS_PORT PORTC
#define WS_DDR DDRC
#define WS_PIN PC0
#define NUM_LEDS 10

#define MIN_BRIGHTNESS 30
#define MAX_BRIGHTNESS 255
#define BRIGHTNESS_STEP_AMOUNT 10    //��� ��������� ������� ��� ���������/����������
/*������ (mode)*/
#define SOLID_WHITE 0
#define FADING_RAINBOW 1
#define SOLID_COLOR_FADE 2
/*_____________*/
void Send_1(void);
void Send_0(void);
void Send_byte(uint8_t);
void Send_res(void);
void Set_pixel_color(uint8_t r, uint8_t g, uint8_t b, uint8_t n);   //������������� ���� ������� � �����
void Show_frame(uint8_t b);  //���������� ������ ����� �� �����
void Clear_frame(void); //������� ����
void Switch_off(void);  //��������� ����� �� ���������� ������ � �����
void Set_brightnes(uint8_t b);  //������������� �������
void Incr_brightnes(uint8_t inc, uint8_t max);   //����������� ������� 
void Decr_brightnes(uint8_t dec, uint8_t min);   //��������� �������
uint8_t Read_brightnes(void);   //���������� ������� �������� �������

void WS_init(void);   //�������������
uint8_t Get_fade_stage(uint8_t n);  //���������� ������ fade �������
void Fade_pixel(uint8_t n, uint8_t amount); 
void Fade_all(uint8_t step);
void Fill_color(uint8_t r, uint8_t g, uint8_t b); //��������� ���� ���� ������
void Fill_rainbow(uint8_t step);    //��������� ���� ���� ������� � �����
void Set_operation(uint8_t setOper);    //������� ��������� � ���������� 
void Brightness_control_looping(void);  //��������� ������� ����������
void Set_mode(uint8_t m);   //���������� ����� ������

#endif	/* WS2812_H */

