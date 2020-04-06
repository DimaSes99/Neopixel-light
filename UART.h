/* 
 * File:   UART.h
 * Author: dima_ses
 *
 * Created on March 18, 2019, 7:03 PM
 */
volatile char rxBuff[30];   //������ ������

#ifndef UART_H
#define	UART_H
#include "main.h"

void UART_Init(uint32_t baudrate);  //�������������
void UART_Transmit_byte(uint8_t data); //�������� �����
void UART_Transmit_string(char data[]);   //�������� ������
void UART_Transmit_int(int32_t num);    //�������� ������ �����
uint8_t UART_Receive(void); //�����
void UART_Clear_buff(void); //������� �������
uint8_t receive_compl(void);
#endif	/* UART_H */

