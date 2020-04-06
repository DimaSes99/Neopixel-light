#include <util/delay.h>

#include "button.h"
uint16_t timVal = 0;    //�������� ������� (����� ������� �������)
double timValFiltered = 0; 

uint16_t btnPressCountCycles = 0;   //������� ������ �� 10 �� ������ ���� ������
uint8_t btnState = 0;   //������� ��������� ������
uint8_t prevBtnState = 0;   //���������� ��������� ������
uint8_t btnPressCount = 0;     //������� �������
uint8_t readyToReadFlag = 1;    //���� ���������� ������
void Btn_init(){
    TCCR1B |= (1<<WGM12)|(1<<CS10); // clk/1 CTC
    TIMSK1 |= (1<<OCIE1A);
    OCR1A = 4000;
    ACSR |= (1<<ACIE)|(1<<ACIS1);
    DDRD |= (1<<PD3);
    PORTD |= (1<<PD3);
    sei();
}
uint8_t Read_btn(void){
    static uint8_t state = 0;
    timValFiltered += (timVal - timValFiltered) * 0.05; //���������� �������� 
    if(state == 0 && timValFiltered > 700){  //������������ � ������������
        state = 1;
    }
    if(state == 1 && timValFiltered < 500){
        state = 0;
    }
    //UART_Transmit_int((uint16_t)timValFiltered);
    //UART_Transmit_byte(state + '0');
    //UART_Transmit_byte('\n');
    return state;
}
uint8_t Btn_press(void){
    if((readyToReadFlag == 1) && (btnPressCount == 1) ){
        btnPressCount = 0; //������� ������� � �������� ����������
        return 1;
    } else {
        return 0;
    }
}
uint8_t Btn_release(void){
    if(btnState == 0 && prevBtnState == 1){
        return 1;
    } else {
        return 0;
    }    
}
uint8_t Btn_hold(void){
    if(btnPressCountCycles >= 100){    // ���� ������ ������ ������ 1 �������
        return 1;
    } else {
        return 0;
    }
}
uint8_t Btn_double_press(void){
        if((readyToReadFlag == 1) && (btnPressCount == 2) ){
        btnPressCount = 0; //������� ������� � �������� ����������
        return 1;
    } else {
        return 0;
    }
}
void Reset_btn_press_count(void){
    if(readyToReadFlag == 1){
        btnPressCount = 0;
    }
}
void Btn_looping(void){
    static uint8_t clickTimeoutTimer = 0; //������ ��� ������� ������� ����� ���������
    prevBtnState = btnState;    //��������� ���������� ��������� ������
    btnState = Read_btn();  //�������� ����� ��������� ������
    if(btnState == 1 && prevBtnState == 1){ //���� �� ����� ������ ���� ������
        btnPressCountCycles++; //������� ������� ������ �� 0,002 ������ ������
    } 
    if(Btn_release() && btnPressCountCycles < 500){    //���� ������ ��������� �� 1� ����� ������� �� ��� ������� �������
        if (clickTimeoutTimer < 250){ //���� �� ������ 0.5� �� ����������� �������
            btnPressCount++;    //��������� ���������� �������
            clickTimeoutTimer = 0;  //����� ������ ������� ����� ���������
            readyToReadFlag = 0;    //����� ����� ���������� ������
        }
        btnPressCountCycles = 0;   //������ ���������, �������� ������� ������
    }
    if(Btn_release()){btnPressCountCycles = 0;}   //������ ���������, �������� ������� ������
    if(clickTimeoutTimer >= 250){
        readyToReadFlag = 1;
        clickTimeoutTimer = 0;
    }
    if(readyToReadFlag == 0){clickTimeoutTimer++;}
}

ISR(TIMER1_COMPA_vect){
    /*��������� �������*/
    static uint8_t div = 0;
    if(div < 7){   
        div++;
    } else {
        Btn_looping();
        div = 0;
    }
    /*��������� �������*/
    setDischargePinToOutput();
    dischargePinLow();  //��������� �������
    setDischargePinToIutput();
    //timValFiltered += (timVal - timValFiltered) * 0.001;
}
ISR(ANALOG_COMP_vect){
    static uint16_t sum = 0;
    static uint8_t cnt = 0;
    if(cnt < 2){
        cnt++;
        sum+=TCNT1;
    } else {
        timVal = sum/2;
        sum = 0;
        cnt = 0;
    }
}