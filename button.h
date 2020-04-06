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
void Btn_init();    //������������� 
uint8_t Read_btn(void); //���������� ��������� ���� ������
uint8_t Btn_press(void);    //���� ������ ���� ������
uint8_t Btn_release(void);  //���� ������ ���� ��������
uint8_t Btn_hold(void); //���� ������ ������������ ����� 1�
uint8_t Btn_double_press(void);   //������� �������
void Reset_btn_press_count(void);   //����� �������� �������, ���� �� �������� ������ ���
void Btn_looping(void); //������� ��� ��������� ������� ������ ���������� � �������� 10��
#endif	/* BUTTON_H */

