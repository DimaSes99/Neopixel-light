#include <util/delay.h>

#include "button.h"
uint16_t timVal = 0;    //значение таймера (время зарядки сенсора)
double timValFiltered = 0; 

uint16_t btnPressCountCycles = 0;   //сколько циклов по 10 мс кнопка была нажата
uint8_t btnState = 0;   //текущее состояние кнопки
uint8_t prevBtnState = 0;   //предыдущее состояние кнопки
uint8_t btnPressCount = 0;     //счетчик нажатий
uint8_t readyToReadFlag = 1;    //флаг готовности чтения
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
    timValFiltered += (timVal - timValFiltered) * 0.05; //фильтрация значений 
    if(state == 0 && timValFiltered > 700){  //переключение с гистерезисом
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
        btnPressCount = 0; //считали нажатие и сбросили переменную
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
    if(btnPressCountCycles >= 100){    // если кнопка нажата больше 1 секунды
        return 1;
    } else {
        return 0;
    }
}
uint8_t Btn_double_press(void){
        if((readyToReadFlag == 1) && (btnPressCount == 2) ){
        btnPressCount = 0; //считали нажатие и сбросили переменную
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
    static uint8_t clickTimeoutTimer = 0; //таймер для отсчета времени между нажатиями
    prevBtnState = btnState;    //запомнить предыдущее состояние кнопки
    btnState = Read_btn();  //записать новое состояние кнопки
    if(btnState == 1 && prevBtnState == 1){ //если до этого кнопка быда нажата
        btnPressCountCycles++; //считаем сколько циклов по 0,002 кнопка нажата
    } 
    if(Btn_release() && btnPressCountCycles < 500){    //если кнопку отпустили до 1с после нажатия то это простое нажатие
        if (clickTimeoutTimer < 250){ //если не прошло 0.5с от предыдущего нажатия
            btnPressCount++;    //увеличить количество нажатий
            clickTimeoutTimer = 0;  //сброс тамера отсчета между нажатиями
            readyToReadFlag = 0;    //сброс флага готовности чтения
        }
        btnPressCountCycles = 0;   //кнопку отпустили, сбросили счетчик циклов
    }
    if(Btn_release()){btnPressCountCycles = 0;}   //кнопку отпустили, сбросили счетчик циклов
    if(clickTimeoutTimer >= 250){
        readyToReadFlag = 1;
        clickTimeoutTimer = 0;
    }
    if(readyToReadFlag == 0){clickTimeoutTimer++;}
}

ISR(TIMER1_COMPA_vect){
    /*Обработка нажатий*/
    static uint8_t div = 0;
    if(div < 7){   
        div++;
    } else {
        Btn_looping();
        div = 0;
    }
    /*Обработка сенсора*/
    setDischargePinToOutput();
    dischargePinLow();  //разрядить емкость
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