#include "UART.h"

volatile uint8_t rxFlag = 0;

ISR(USART_RX_vect){    //обработка прерывания на прием
    static uint8_t rxIndex = 0; //для заполнения массива
    rxFlag = 0; //прием не окончен, ожтдаем еще 
    if (rxIndex == 0) UART_Clear_buff();    //очистить буффер перед приемом новых данных
    char incomingChar = UART_Receive();
    if (incomingChar != 0x0a){  
        rxBuff[rxIndex] = incomingChar;
        rxIndex++;
    } else {    //если обнаружен конец строки(LF line end)
        rxIndex = 0;
        rxFlag = 1;//прием окончен
    }
}

void UART_Init(uint32_t baudrate){
 UBRR0H = (unsigned char) (baudrate>>8);     //запись значения в регистр
 UBRR0L = (unsigned char) (baudrate);
 
 UCSR0A |= (1<<U2X0); //удвоение скорости обмена
 UCSR0B = (1<<RXEN0)|(1<<TXEN0); //разрешить прием и передачу 
 UCSR0B |= (1<<RXCIE0); //разрешить прерывание по завершению приема
 UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01); //8 бит, 2 стоп бита, без контроля четности
}

void UART_Transmit_byte(uint8_t data){//отправка байта
    while(!(UCSR0A & (1<<UDRE0)));//подождать конец передачи данных
    UDR0 = data; //отправить данные
}

void UART_Transmit_string(char data[]){//отправка строки
    uint8_t i = 0;
    while(data[i] != 0x00){
        UART_Transmit_byte(data[i]);
        i++;
    }
}

void UART_Transmit_int(int32_t num){//отправка целого числа
    char numString [11]; //строка для перелачи числа
    if (num == 0){
        UART_Transmit_byte('0');
        return;
    }
    if (num < 0){   //при отрицательном значении отправит знак -
        UART_Transmit_byte('-');
        num *= -1;  //умножить число на -1 и дальше как обычно
    }
    for(uint8_t i = 0; i < sizeof(numString) - 1; i++){ //заполнить нулями
        numString[i] = 0;
    }
    int32_t div = 1;
    uint8_t counter = 10;
    while (num / div != 0 ){    //раскласть на цифры число произвольной длинны
        numString[counter] = ((num / div) % 10) + '0';
        div *= 10;
        counter--;
    }
    uint8_t notZero = 0;
    while(numString[notZero] == 0){ //найти первый не 0
        notZero++;
    }
    for(uint8_t i = 0; i < sizeof(numString) - notZero; i++){   //сдвинуть все не нули в начало строки
        numString[i] = numString[i + notZero];
        numString[i + notZero] = 0;
    }
    UART_Transmit_string(numString);
}

uint8_t UART_Receive(void){//прием байта данных
    while(!(UCSR0A &(1<<RXC0)));
    return UDR0;
}

void UART_Clear_buff(){//очистка буффера приема
    for(int i = 0; i < sizeof(rxBuff) - 1; i++){
        rxBuff[i] = 0;  //заполнить нулями
    }
}

uint8_t receive_compl(void){//проверка окончания приема
    return rxFlag;  //если прием окончен и больше ничего не ожидается, то возв 1
}