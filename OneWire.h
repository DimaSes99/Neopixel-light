/* 
 * File:   OneWire.h
 * Author: sesdi
 *
 * Created on 19 января 2020 г., 18:56
 */

#ifndef ONEWIRE_H
#define	ONEWIRE_H
//---------------------------------------------------//
#define ONE_WIRE_GPIO PC1
#define ONE_WIRE_DDR DDRC
#define ONE_WIRE_PIN PINC
#define ONE_WIRE_PORT PORTC
//---------------------------------------------------//
#define DEVICES_NUM 2   //количество устройств на шине

#include "main.h"

uint8_t OneWire_check(void);                    //проверка наличия устройств на шине (или сброс)
void OneWire_sendBit(uint8_t bit);              //отправка бита
void OneWire_sendByte(uint8_t byte);            //отправка байта
uint8_t OneWire_readBit(void);                  //чтение бита
uint8_t OneWire_readByte(void);                 //чтение байта
uint8_t OneWire_searchROM(uint64_t addreses[], uint8_t family, uint8_t numOfDevices);    //поиск всех устройств по их семейству
void OneWire_sendROM(uint64_t address);         //отправка адреса
#endif	/* ONEWIRE_H */

