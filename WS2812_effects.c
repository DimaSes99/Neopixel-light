#include "WS2812_effects.h"
#define BRIGHTNESS_STEP_AMOUNT 10    //

uint8_t frame [NUM_LEDS][3];
uint8_t oper = 1;   //переменная текущей операции
uint8_t operationComplFlag = 1; //флаг завершения операции
uint8_t smoothBrightnes = 0;    //переменная для плавного включения и выключения
uint8_t brightChangeFlag = 0;    //направление изменения яркости (0 -, 1 +)
/*Чтобы не плодить переменные сделаем так:
 0 бит переменной brightChangeFlag отвечает за направление изменения яркости
 1 бит текущее состояние изменения (установлен - яркость изменяется)
 */

#define BRIGHTNESS_DIRECTION 0
#define BRIGHTNES_STATE 1
void WS_efcts_init(void){
    /*Инициализация таймера 0*/
    TCCR0A |= (1<<WGM01); //CTC mode
    TCCR0B |= (1<<CS00)|(1<<CS02); //clk/1024
    TIMSK0 |= (1<<OCIE0A);  //разрешить прерывание по совпадению
    OCR0A = 157;    //период около 10мс
    sei();
}
uint8_t Get_fade_stage(uint8_t n){
    /*
     Fade stages:
     stage 0:   255, 0, 0   ->  255, 255, 0
     stage 1:   255, 255, 0 ->  0, 255, 0
     stage 2:   0, 255, 0   ->  0, 255, 255
     stage 3:   0, 255, 255 ->  0, 0, 255
     stage 4:   0, 0, 255   ->  255, 0, 255
     stage 5:   255, 0, 255 ->  255, 0, 0
     */
    if( (frame[n][0]==255) && (frame[n][1]!=255) && (frame[n][2]==0) ) {return 0;}
    if( (frame[n][0]!=0) && (frame[n][1]==255) && (frame[n][2]==0) ) {return 1;}
    if( (frame[n][0]==0) && (frame[n][1]==255) && (frame[n][2]!=255) ) {return 2;}
    if( (frame[n][0]==0) && (frame[n][1]!=0) && (frame[n][2]==255) ) {return 3;}
    if( (frame[n][0]!=255) && (frame[n][1]==0) && (frame[n][2]==255) ) {return 4;}
    if( (frame[n][0]==255) && (frame[n][1]==0) && (frame[n][2]!=0) ) {return 5;}
    return 6;   //ни один из всех (0, 0, 0)
}
void Fade_pixel(uint8_t n, uint8_t step){
    uint8_t fadeStage = Get_fade_stage(n);
    switch (fadeStage){
        case 0:
            if( (int16_t)(frame[n][1]+step <= 255) ){
                frame[n][1] += step;
            } else {
                frame[n][0] -= (step - (255 -frame[n][1]));   //излишек перекинуть в следующую ступень
                frame[n][1] = 255;  //окончить текущую ступень
            }
            break;
        case 1:
            if( (int16_t)(frame[n][0]-step >= 0) ){
                frame[n][0] -= step;
            } else {
                frame[n][2] += (step - frame[n][0]); 
                frame[n][0]=0;
            }
            break;
        case 2:
            if( (int16_t)(frame[n][2]+step <= 255) ){
                frame[n][2] += step;
            } else {
                frame[n][1] -= (step - (255 -frame[n][2]));
                frame[n][2]=255;
            }
            break;
        case 3:
            if( (int16_t)(frame[n][1]-step >= 0) ){
                frame[n][1] -= step;
            } else {
                frame[n][0] += (step - frame[n][1]);
                frame[n][1]=0;
            }
            break;
        case 4:
            if( (int16_t)(frame[n][0]+step <= 255) ){
                frame[n][0] += step;
            } else {
                frame[n][2] -= (step - (255 -frame[n][0]));
                frame[n][0]=255;
            }
            break;
        case 5:
            if( (int16_t)(frame[n][2]-step >= 0) ){
                frame[n][2] -= step;
            } else {
                frame[n][1] += (step - frame[n][2]);
                frame[n][2]=0;
            }
            break;
        case 6:
            frame[n][0] = 255;
            break;
    }
}
void Fade_all(uint8_t step){
    for(int i = 0; i < NUM_LEDS; i++){
        Fade_pixel(i, step);
    }
}
void Fill_color(uint8_t r, uint8_t g, uint8_t b){
    for(int i = 0; i < NUM_LEDS; i++){
        Set_pixel_color(r, g, b, i);
    }
}
void Fill_rainbow(uint8_t step){
    Set_pixel_color(255, 0, 0, 0);  //первый красный
    for(int i = 0; i < NUM_LEDS - 1; i++){
        frame[i+1][0] = frame[i][0];
        frame[i+1][1] = frame[i][1];
        frame[i+1][2] = frame[i][2];
        uint8_t fadeStage = Get_fade_stage(i);
            switch (fadeStage){
        case 0:
            if( (int16_t)(frame[i+1][1]+step <= 255) ){
                frame[i+1][1] += step;
            } else {
                frame[i+1][0] -= (step - (255 -frame[i+1][1]));   //излишек перекинуть в следующую ступень
                frame[i+1][1] = 255;  //окончить текущую ступень
            }
            break;
        case 1:
            if( (int16_t)(frame[i+1][0]-step >= 0) ){
                frame[i+1][0] -= step;
            } else {
                frame[i+1][2] += (step - frame[i+1][0]); 
                frame[i+1][0]=0;
            }
            break;
        case 2:
            if( (int16_t)(frame[i+1][2]+step <= 255) ){
                frame[i+1][2] += step;
            } else {
                frame[i+1][1] -= (step - (255 -frame[i+1][2]));
                frame[i+1][2]=255;
            }
            break;
        case 3:
            if( (int16_t)(frame[i+1][1]-step >= 0) ){
                frame[i+1][1] -= step;
            } else {
                frame[i+1][0] += (step - frame[i+1][1]);
                frame[i+1][1]=0;
            }
            break;
        case 4:
            if( (int16_t)(frame[i+1][0]+step <= 255) ){
                frame[i+1][0] += step;
            } else {
                frame[i+1][2] -= (step - (255 -frame[i+1][0]));
                frame[i+1][0]=255;
            }
            break;
        case 5:
            if( (int16_t)(frame[i+1][2]-step >= 0) ){
                frame[i+1][2] -= step;
            } else {
                frame[i+1][1] += (step - frame[i+1][2]);
                frame[i+1][2]=0;
            }
            break;
        case 6:
            frame[i+1][0] = 255;
            break;
    }
    }
}
void Set_operation(uint8_t setOper){
    oper = setOper;
    operationComplFlag = 0; //опустить флаг завершения операции
}
ISR(TIMER0_COMPA_vect){
    static uint8_t state = 0;
    if(Btn_press()){
        state ^= 1;
        Set_operation(state);
    }
    if(state){  //если включена
        /*Управление яркостью*/
/*___________________________________________________________________________________________*/
        if( !Btn_hold() && ((brightChangeFlag & (1<<BRIGHTNES_STATE))==(1<<BRIGHTNES_STATE)) ){    //срабатывает сразу после отпускания кнопки после удержания
            brightChangeFlag ^= (1<<BRIGHTNESS_DIRECTION);   //меняет направление изменения яркости
        }
        if(Btn_hold()){
            brightChangeFlag |= (1<<BRIGHTNES_STATE);    //установить флаг изменения яркости
        } else {
            brightChangeFlag &= ~(1<<BRIGHTNES_STATE);   //снять флаг изменения яркости
        }
        if((brightChangeFlag & (1<<BRIGHTNES_STATE))==(1<<BRIGHTNES_STATE)){ //если стоит флаг изменения яркости
            if((brightChangeFlag & (1<<BRIGHTNESS_DIRECTION))==(1<<BRIGHTNESS_DIRECTION)){   //если установлен бит направления изменения
                Incr_brightnes(1);
            } else {
                Decr_brightnes(1);
            }
        }
/*___________________________________________________________________________________________*/
    }
    // <editor-fold defaultstate="collapsed" desc="Выполение операций">
    if(operationComplFlag == 0){    //если текущая операция не завершена
        switch (oper){
            case 0: //плавно уменьшить яркость до 0
                if((int16_t)smoothBrightnes - BRIGHTNESS_STEP_AMOUNT > 0){
                    smoothBrightnes-=BRIGHTNESS_STEP_AMOUNT;
                } else {
                    smoothBrightnes = 0;
                    operationComplFlag = 1; //поднять флаг завершения операции
                }
                break;
            case 1: //плавно увеличить яркость до уровня Set_brightness()
                if((int16_t)smoothBrightnes +BRIGHTNESS_STEP_AMOUNT < Read_brightnes()){
                    smoothBrightnes+=BRIGHTNESS_STEP_AMOUNT;
                } else {
                    smoothBrightnes = Read_brightnes();
                    operationComplFlag = 1; //поднять флаг завершения операции
                }
                break;
        }
    }
    // </editor-fold>
    Fade_all(1);
    Show_frame(smoothBrightnes);
}