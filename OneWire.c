#include "OneWire.h"

uint8_t OneWire_check(void){
    cli();
    uint8_t state;
    ONE_WIRE_DDR |= (1<<ONE_WIRE_GPIO);
    _delay_us(485);
    ONE_WIRE_DDR &= ~(1<<ONE_WIRE_GPIO);
    _delay_us(65);
    state = (ONE_WIRE_PIN & (1<<ONE_WIRE_GPIO)) == 0;
    _delay_us(420);
    sei();
    return state;
}
void OneWire_sendBit(uint8_t bit){
    ONE_WIRE_DDR |= (1<<ONE_WIRE_GPIO);
    _delay_us(3);
    if(bit&1){ONE_WIRE_DDR &= ~(1<<ONE_WIRE_GPIO);}
    _delay_us(65);
    ONE_WIRE_DDR &= ~(1<<ONE_WIRE_GPIO);
    
}
void OneWire_sendByte(uint8_t byte){
    cli();
    for(int i = 0; i < 8; i++){
        OneWire_sendBit((byte & (1<<i)) != 0);
        _delay_us(2);   //задержка на востановление высокого уровня 
    }
    sei();
}
uint8_t OneWire_readBit(void){
    uint8_t b = 0;
    ONE_WIRE_DDR |= (1<<ONE_WIRE_GPIO);
    _delay_us(2);
    ONE_WIRE_DDR &= ~(1<<ONE_WIRE_GPIO);
    _delay_us(13);
    b = ((ONE_WIRE_PIN & (1<<ONE_WIRE_GPIO))>>ONE_WIRE_GPIO);
    _delay_us(45);
    return b;
}
uint8_t OneWire_readByte(void){
    cli();
    uint8_t byte = 0;
    for(int i = 0; i < 8; i++){
        byte|= OneWire_readBit()<<i;
    }
    sei();
    return byte;
}
uint8_t OneWire_searchROM(uint64_t addreses[], uint8_t family, uint8_t numOfDevices){
    for(uint8_t deviceNum = 0; deviceNum < numOfDevices; deviceNum++){
        uint8_t incomingBits = 0;
        uint64_t currAddress = 0; 
        if(OneWire_check()){
            OneWire_sendByte(0xF0);
            for(uint8_t i = 0; i <8; i++){
                incomingBits |= (OneWire_readBit()<<1);
                incomingBits |= (OneWire_readBit()<<0);
                switch (incomingBits){
                    case 0b00000000: //если конфликт
                        OneWire_sendBit( (family&(1<<i)) == (1<<i) );   //оставляем только тех, у кого совпадает бит с искомой family
                        break;
                    case 0b00000001: //у всех соединенных 0 в этой позиции
                        if( (family&(1<<i)) == 0 ){ //если совпадает с family
                            OneWire_sendBit(0); //оставляем
                        } else {    //если не совпадает
                            return 0; //выходим из функции
                        }
                        break;
                    case 0b00000010: //у всех соединенных 1 в этой позиции
                        if( (family&(1<<i)) != 0 ){ //если совпадает с family
                            OneWire_sendBit(1); //оставляем
                        } else {    //если не совпадает
                            return 0; //выходим из функции
                        }
                        break;
                    case 0b00000011: //на шине ничего нет
                        return 0; //выходим из функции
                        break;
                }
                incomingBits = 0;
            }   
            currAddress |= family;  //если программа дошла сюда, то family совпала
            for(uint8_t i = 8; i < 64; i++){    //продолжаем искать 
                incomingBits |= (OneWire_readBit()<<1);
                incomingBits |= (OneWire_readBit()<<0);
                switch (incomingBits){
                    case 0b00000000: //если конфликт                       
                        if (deviceNum != 0){    //если это не первый поиск
                            if ((addreses[deviceNum-1]&(1ULL<<i)) == 0){ //если у предыдущего бит был 0
                                currAddress |=  (1ULL<<i); //пишем 1
                                OneWire_sendBit(1); //оставляем на шине только тех, у кого 1
                            } else { //если у предыдущего бит был 1
                                currAddress &= ~(1ULL<<i); //пишем 0 
                                OneWire_sendBit(0); //оставляем на шине только тех, у кого 0
                            }
                        } else { //поиск первый 
                            currAddress &= ~(1ULL<<i); //пишем 0 
                            OneWire_sendBit(0); //оставляем на шине только тех, у кого 0
                        }
                        break;
                    case 0b00000001: //у всех соединенных 0 в этой позиции
                        currAddress &= ~(1ULL<<i); //пишем 0 
                        OneWire_sendBit(0); //оставляем на шине 
                        break;
                    case 0b00000010: //у всех соединенных 1 в этой позиции
                        currAddress |= (1ULL<<i); //пишем 1
                        OneWire_sendBit(1); //оставляем на шине 
                        break;
                    case 0b00000011: //на шине ничего нет
                        return 0;//выходим из функции
                        break;
                }
            incomingBits = 0;
            }
        }
        if(deviceNum == 0){
            addreses[deviceNum] = currAddress;
        } else if( (deviceNum>0) && (addreses[deviceNum-1])!=currAddress ){  //указано не правильное количество устройств
            addreses[deviceNum] = currAddress;  //записываем найденый ROM
        }
    }
    return 1;   //поиск прошел удачно
}
void OneWire_sendROM(uint64_t address){
    for(uint64_t i = 0; i < 64; i++){
        if( (address&(1ULL<<i) ) != 0){
            OneWire_sendBit(1);
        } else {
            OneWire_sendBit(0);
        }
    }
}

