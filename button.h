/* 
 * File:   button.h
 * Author: dima_ses
 *
 * Created on September 20, 2019, 7:26 PM
 */

#ifndef BUTTON_H
#define DISCHARGE_PIN PB0
#define DISCHARGE_PIN_PORT PORTB
#define DISCHARGE_PIN_DDR DDRB

#define setDischargePinToIutput() DISCHARGE_PIN_DDR&=~(1<<DISCHARGE_PIN)
#define setDischargePinToOutput() DISCHARGE_PIN_DDR|=(1<<DISCHARGE_PIN)
#define dischargePinHigh() DISCHARGE_PIN_PORT|=(1<<DISCHARGE_PIN)
#define dischargePinLow() DISCHARGE_PIN_PORT&=~(1<<DISCHARGE_PIN)

#define enableComparator() ACSR&=~(1<<ACD)
#define disableComparator() ACSR|=(1<<ACD)

#define F_CPU 16000000UL
#include "main.h"
void Btn_init();    //инициализация 
uint8_t Read_btn(void); //возвращает состояние пина кнопки
uint8_t Btn_press(void);    //если кнопка была нажата
uint8_t Btn_release(void);  //если кнопки была отпущена
uint8_t Btn_hold(void); //если кнопка удерживается более 1с
uint8_t Btn_double_press(void);   //двойное нажатие
void Reset_btn_press_count(void);   //сброс счетчика нажатий, если не прочитан нужный тип
void Btn_looping(void); //функция для обработки нажатий должна выполнятся с периодом 10мс
#endif	/* BUTTON_H */

