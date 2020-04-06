/* 
 * File:   UART.h
 * Author: dima_ses
 *
 * Created on March 18, 2019, 7:03 PM
 */
volatile char rxBuff[30];   //буффер приема

#ifndef UART_H
#define	UART_H
#include "main.h"

void UART_Init(uint32_t baudrate);  //инициализация
void UART_Transmit_byte(uint8_t data); //отправка байта
void UART_Transmit_string(char data[]);   //отправка строки
void UART_Transmit_int(int32_t num);    //отправка целого числа
uint8_t UART_Receive(void); //прием
void UART_Clear_buff(void); //очистка буффера
uint8_t receive_compl(void);
#endif	/* UART_H */

