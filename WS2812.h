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
#define BRIGHTNESS_STEP_AMOUNT 10    //шаг изменения яркости при включении/выключении
/*Режимы (mode)*/
#define SOLID_WHITE 0
#define FADING_RAINBOW 1
#define SOLID_COLOR_FADE 2
/*_____________*/
void Send_1(void);
void Send_0(void);
void Send_byte(uint8_t);
void Send_res(void);
void Set_pixel_color(uint8_t r, uint8_t g, uint8_t b, uint8_t n);   //устанавливает цвет пикселя в кадре
void Show_frame(uint8_t b);  //отправляет данные кадра на ленту
void Clear_frame(void); //очищает кадр
void Switch_off(void);  //выключает ленту не затрагивая данные в кадре
void Set_brightnes(uint8_t b);  //устанавливает яркость
void Incr_brightnes(uint8_t inc, uint8_t max);   //увеличивает яркость 
void Decr_brightnes(uint8_t dec, uint8_t min);   //уменьшает яркость
uint8_t Read_brightnes(void);   //возвращает текущее значение яркости

void WS_init(void);   //инициализация
uint8_t Get_fade_stage(uint8_t n);  //возвращает стадию fade пикселя
void Fade_pixel(uint8_t n, uint8_t amount); 
void Fade_all(uint8_t step);
void Fill_color(uint8_t r, uint8_t g, uint8_t b); //заполнить весь кадр цветом
void Fill_rainbow(uint8_t step);    //заполнить весь кадр радугой с шагом
void Set_operation(uint8_t setOper);    //плавное включение и выключение 
void Brightness_control_looping(void);  //изменение яркости удержанием
void Set_mode(uint8_t m);   //установить режим работы

#endif	/* WS2812_H */

