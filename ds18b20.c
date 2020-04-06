#include "ds18b20.h"
uint8_t powerSupplyType = 0; //тип питания 0 - паразитное, 1 - обычное
uint8_t resolution = RESOLUTION_12BIT;

uint8_t DS_init(uint8_t resol, int8_t tAlarmH, int8_t tAlarmL){
    resolution = resol;
    if(OneWire_check()){
        OneWire_sendByte(SKIP_ROM); //skip ROM
        DS_writeScratchpad(tAlarmH, tAlarmL, resol);   //записать настройки
        OneWire_check();
        OneWire_sendByte(SKIP_ROM); //skip ROM
        OneWire_sendByte(READ_POWER_SUPPLY); //read power supply
        powerSupplyType = OneWire_readBit();    //если хоть один parasit power, то применяется для всех
        if(OneWire_searchROM(addreses, FAMILY, DEVICES_NUM)!=1){    //поиск всех адресов
             return 0;   //Ошибка, нет устройств на шине
        }
        OneWire_check();    //сброс
        return 1; //Ошибок нет
    } else {
        return 0;   //Ошибка, нет устройств на шине
    }
}
void DS_writeScratchpad(int8_t Th, int8_t Tl, uint8_t resolution){
    OneWire_sendByte(WRITE_SCRATCHPAD); //write Scratchpad
    OneWire_sendByte(Th);
    OneWire_sendByte(Tl);
    OneWire_sendByte(resolution);
    OneWire_sendByte(COPY_SCRATCHPAD); //копирует содержимое Scratchpad в EEPROM
    if(powerSupplyType == 1){
        _delay_ms(10);
    } else {
        STRONG_PULLUP_ON;
        _delay_ms(10);
        STRONG_PULLUP_OFF;
    }
}
void DS_readScratchpad(uint8_t scratchpad[], uint64_t address){
    OneWire_sendByte(MATCH_ROM); //match ROM
    OneWire_sendROM(address);//отправить ROM
    OneWire_sendByte(READ_SCRATCHPAD); //read Scratchpad
    for(uint8_t i = 0; i < 9; i++){ //прочесть 9 байт 
        scratchpad[i] = OneWire_readByte();
    }
}
int32_t DS_2bytesToTemperature(uint8_t MSB, uint8_t LSB){
    int32_t temperature;
    if( (MSB&0b11111000) == 0){
        temperature = (int32_t)((MSB<<4)|(LSB>>4)) * 10000;   //целая часть
        switch(resolution){  //в зависимости от разрешения, дробная часть
            case RESOLUTION_9BIT:
                temperature+= (int32_t)(LSB&0b00001111) * 5000;
                break;
            case RESOLUTION_10BIT:
                temperature+= (int32_t)(LSB&0b00001111) * 2500;
                break;
            case RESOLUTION_11BIT:
                temperature+= (int32_t)(LSB&0b00001111) * 1250;
                break;
            case RESOLUTION_12BIT:
                temperature+= (int32_t)(LSB&0b00001111) * 625;
                break;
        }
        return temperature;
    } else {
        MSB = ~MSB;
        LSB = ~LSB;
        temperature = (int32_t)((MSB<<4)|(LSB>>4)) * 10000;   //целая часть
        switch(resolution){  //в зависимости от разрешения, дробная часть
            case RESOLUTION_9BIT:
                temperature+= (int32_t)((LSB&0b00001111)+1) * 5000;
                break;
            case RESOLUTION_10BIT:
                temperature+= (int32_t)((LSB&0b00001111)+1) * 2500;
                break;
            case RESOLUTION_11BIT:
                temperature+= (int32_t)((LSB&0b00001111)+1) * 1250;
                break;
            case RESOLUTION_12BIT:
                temperature+= (int32_t)((LSB&0b00001111)+1) * 625;
                break;
        }
        return -temperature;
    }
}
int32_t DS_readTemperature(uint64_t address){
    int32_t temperature;
    if (OneWire_check()){
        OneWire_sendByte(MATCH_ROM); //match ROM
        OneWire_sendROM(address);//отправить ROM
        OneWire_sendByte(CONVERT_T); //начать преобразование
        switch (powerSupplyType){
            case 0: //при паразитном питании
                STRONG_PULLUP_ON;
                    switch(resolution){  //ждем в зависимости от разрешения
                case RESOLUTION_9BIT:
                    _delay_ms(94);
                    break;
                case RESOLUTION_10BIT:
                    _delay_ms(188);
                    break;
                case RESOLUTION_11BIT:
                    _delay_ms(375);
                    break;
                case RESOLUTION_12BIT:
                    _delay_ms(750);
                    break;
            }
                STRONG_PULLUP_OFF;
                break;
            case 1: //при обычном питании
                while(OneWire_readBit() != 1){} //дождаться окончания преобразования
                break;  
        }
        OneWire_check();
        OneWire_sendByte(MATCH_ROM); //match ROM
        OneWire_sendROM(address);//отправить ROM
        OneWire_sendByte(READ_SCRATCHPAD); //read Scratchp
        temperature = DS_2bytesToTemperature(OneWire_readByte(), OneWire_readByte());
        OneWire_check();
    } else {    //ошибка
        return 0xFFFFFFFF;
    }
    return temperature;
}
void DS_readAllSensors(int32_t results[]){
    if(OneWire_check()){
      OneWire_sendByte(SKIP_ROM);
      OneWire_sendByte(CONVERT_T);
              switch (powerSupplyType){
            case 0: //при паразитном питании
                STRONG_PULLUP_ON;
                    switch(resolution){  //ждем в зависимости от разрешения
                case RESOLUTION_9BIT:
                    _delay_ms(94);
                    break;
                case RESOLUTION_10BIT:
                    _delay_ms(188);
                    break;
                case RESOLUTION_11BIT:
                    _delay_ms(375);
                    break;
                case RESOLUTION_12BIT:
                    _delay_ms(750);
                    break;
            }
                STRONG_PULLUP_OFF;
                break;
            case 1: //при обычном питании
                while(OneWire_readBit() != 1){} //дождаться окончания преобразования
                break;  
        }
         for(uint8_t i = 0; i < DEVICES_NUM; i++){
            OneWire_check();
            OneWire_sendByte(MATCH_ROM); //match ROM
            OneWire_sendROM(addreses[i]);//отправить ROM
            OneWire_sendByte(READ_SCRATCHPAD); //read Scratchp
            results[i] = DS_2bytesToTemperature(OneWire_readByte(), OneWire_readByte());
         }     
    } else {
         for(uint8_t i = 0; i < DEVICES_NUM; i++){
             results[i] = 0xFFFFFFFF;   //ошибка
         }
    }
}
int32_t DS_simpleRead(void){
    OneWire_check();
    OneWire_sendByte(SKIP_ROM);
    OneWire_sendByte(CONVERT_T);
    switch(resolution){  //ждем в зависимости от разрешения
        case RESOLUTION_9BIT:
            _delay_ms(94);
            break;
        case RESOLUTION_10BIT:
            _delay_ms(188);
            break;
        case RESOLUTION_11BIT:
            _delay_ms(375);
            break;
        case RESOLUTION_12BIT:
            _delay_ms(750);
            break;
    }
    OneWire_check();
    OneWire_sendByte(SKIP_ROM);
    OneWire_sendByte(READ_SCRATCHPAD);
    return DS_2bytesToTemperature(OneWire_readByte(), OneWire_readByte());
}