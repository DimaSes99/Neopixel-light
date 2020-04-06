/* 
 * File:   ds18b20.h
 * Author: sesdi
 *
 * Created on 20 января 2020 г., 21:54
 */

#ifndef DS18B20_H
#define	DS18B20_H
//---------------------------------------------------//
#define RESOLUTION_9BIT 0x1F
#define RESOLUTION_10BIT 0x3F
#define RESOLUTION_11BIT 0x5F
#define RESOLUTION_12BIT 0x7F
//---------------------------------------------------//
#define SEARCH_ROM 0xF0
#define RAEAD_ROM 0x33
#define MATCH_ROM 0x55
#define SKIP_ROM 0xCC
#define ALARM_SEARCH 0xEC
//---------------------------------------------------//
#define CONVERT_T 0x44
#define WRITE_SCRATCHPAD 0x4E
#define READ_SCRATCHPAD 0xBE
#define COPY_SCRATCHPAD 0x48
#define RECALL_E2 0xB8
#define READ_POWER_SUPPLY 0xB4
#define FAMILY 0x28
//---------------------------------------------------//
#define STRONG_PULLUP_ON    ONE_WIRE_PORT|=(1<<ONE_WIRE_GPIO);ONE_WIRE_DDR|=(1<<ONE_WIRE_GPIO)
#define STRONG_PULLUP_OFF   ONE_WIRE_PORT&=~(1<<ONE_WIRE_GPIO);ONE_WIRE_DDR&=~(1<<ONE_WIRE_GPIO)
#include "main.h"
uint64_t addreses[DEVICES_NUM]; //массив для хранения адресов

uint8_t DS_init(uint8_t resol, int8_t tAlarmH, int8_t tAlarmL); //инициализация
void DS_writeScratchpad(int8_t Th, int8_t Tl, uint8_t resolution);  
void DS_readScratchpad(uint8_t scratchpad[], uint64_t address);
int32_t DS_2bytesToTemperature(uint8_t LSB, uint8_t MSB);   //превращает 2 байта в температуру в десятитысячных градуса
int32_t DS_readTemperature(uint64_t address);   //читает температуру с определенного адреса
void DS_readAllSensors(int32_t results[]);  //читает температуру со всех датчиков, записывает в массив
int32_t DS_simpleRead(void);    //простое чтение температуры с одного датчика
#endif	/* DS18B20_H */

