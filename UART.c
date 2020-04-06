#include "UART.h"

volatile uint8_t rxFlag = 0;

ISR(USART_RX_vect){    //��������� ���������� �� �����
    static uint8_t rxIndex = 0; //��� ���������� �������
    rxFlag = 0; //����� �� �������, ������� ��� 
    if (rxIndex == 0) UART_Clear_buff();    //�������� ������ ����� ������� ����� ������
    char incomingChar = UART_Receive();
    if (incomingChar != 0x0a){  
        rxBuff[rxIndex] = incomingChar;
        rxIndex++;
    } else {    //���� ��������� ����� ������(LF line end)
        rxIndex = 0;
        rxFlag = 1;//����� �������
    }
}

void UART_Init(uint32_t baudrate){
 UBRR0H = (unsigned char) (baudrate>>8);     //������ �������� � �������
 UBRR0L = (unsigned char) (baudrate);
 
 UCSR0A |= (1<<U2X0); //�������� �������� ������
 UCSR0B = (1<<RXEN0)|(1<<TXEN0); //��������� ����� � �������� 
 UCSR0B |= (1<<RXCIE0); //��������� ���������� �� ���������� ������
 UCSR0C = (1<<USBS0)|(1<<UCSZ00)|(1<<UCSZ01); //8 ���, 2 ���� ����, ��� �������� ��������
}

void UART_Transmit_byte(uint8_t data){//�������� �����
    while(!(UCSR0A & (1<<UDRE0)));//��������� ����� �������� ������
    UDR0 = data; //��������� ������
}

void UART_Transmit_string(char data[]){//�������� ������
    uint8_t i = 0;
    while(data[i] != 0x00){
        UART_Transmit_byte(data[i]);
        i++;
    }
}

void UART_Transmit_int(int32_t num){//�������� ������ �����
    char numString [11]; //������ ��� �������� �����
    if (num == 0){
        UART_Transmit_byte('0');
        return;
    }
    if (num < 0){   //��� ������������� �������� �������� ���� -
        UART_Transmit_byte('-');
        num *= -1;  //�������� ����� �� -1 � ������ ��� ������
    }
    for(uint8_t i = 0; i < sizeof(numString) - 1; i++){ //��������� ������
        numString[i] = 0;
    }
    int32_t div = 1;
    uint8_t counter = 10;
    while (num / div != 0 ){    //��������� �� ����� ����� ������������ ������
        numString[counter] = ((num / div) % 10) + '0';
        div *= 10;
        counter--;
    }
    uint8_t notZero = 0;
    while(numString[notZero] == 0){ //����� ������ �� 0
        notZero++;
    }
    for(uint8_t i = 0; i < sizeof(numString) - notZero; i++){   //�������� ��� �� ���� � ������ ������
        numString[i] = numString[i + notZero];
        numString[i + notZero] = 0;
    }
    UART_Transmit_string(numString);
}

uint8_t UART_Receive(void){//����� ����� ������
    while(!(UCSR0A &(1<<RXC0)));
    return UDR0;
}

void UART_Clear_buff(){//������� ������� ������
    for(int i = 0; i < sizeof(rxBuff) - 1; i++){
        rxBuff[i] = 0;  //��������� ������
    }
}

uint8_t receive_compl(void){//�������� ��������� ������
    return rxFlag;  //���� ����� ������� � ������ ������ �� ���������, �� ���� 1
}