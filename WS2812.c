#include "WS2812.h"
uint8_t brightness = 0;
uint8_t frame [NUM_LEDS][3];    //массив кадра
uint8_t oper = 1;   //переменная текущей операции
uint8_t operationComplFlag = 1; //флаг завершения операции
uint8_t smoothBrightnes = 0;    //переменная для плавного включения и выключения
uint8_t brightChangeFlag = 0;    //направление изменения яркости (0 -, 1 +) и текущее состояние изменения
// <editor-fold defaultstate="colapsed" desc="Пояснение к переменной">
//Чтобы не плодить переменные сделаем так:
//бит переменной brightChangeFlag отвечает за направление изменения яркости
//бит текущее состояние изменения (установлен - яркость изменяется)
#define BRIGHTNESS_DIRECTION 0
#define BRIGHTNES_STATE 1
// </editor-fold>
uint8_t mode = 0;

void Send_1(void){
    WS_PORT |= (1<<WS_PIN);
    asm(
    "rjmp .+0\n\t"        // 2      nop nop         (T =  4)
    "rjmp .+0\n\t"        // 2      nop nop         (T =  6)
    "rjmp .+0\n\t"        // 2      nop nop         (T =  8)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 10)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 12)
    "nop\n\t"               // 1      nop           (T = 13)
    );
    //_delay_us(0.8);
    WS_PORT &= ~(1<<WS_PIN);
    asm(
    "rjmp .+0\n\t"        // 2      nop nop         (T = 10)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 12)
    "nop\n\t"               // 1      nop           (T = 13)
    );
    //_delay_us(0.45);
}
void Send_0(void){
    WS_PORT |= (1<<WS_PIN);
    asm(
    "rjmp .+0\n\t"        // 2      nop nop         (T =  4)
    "rjmp .+0\n\t"        // 2      nop nop         (T =  6)
    );
    //_delay_us(0.4); 
    WS_PORT &= ~(1<<WS_PIN);
    asm(
    "rjmp .+0\n\t"        // 2      nop nop         (T = 10)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 12)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 14)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 16)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 18)
    "rjmp .+0\n\t"        // 2      nop nop         (T = 20) 
    );
    //_delay_us(0.85);
}
void Send_byte(uint8_t byte){
    cli();
    for(uint8_t i = 0; i < 8; i++){
        if( ((128>>i) & byte) == 0){
            Send_0();
        } else {
            Send_1();
        }
    }
}
void Send_res(void){
    WS_PORT &= ~(1<<WS_PIN);
    _delay_us(55);
    sei();
}
void Set_pixel_color(uint8_t r, uint8_t g, uint8_t b, uint8_t n){
    frame[n][0] = r;
    frame[n][1] = g;
    frame[n][2] = b;
}
void Show_frame(uint8_t b){
    WS_DDR |= (1<<WS_PIN);
    for(int i = 0; i < NUM_LEDS; i++){
        Send_byte( (uint16_t)(frame[i][1] * b / 255) );
        Send_byte( (uint16_t)(frame[i][0] * b / 255) );
        Send_byte( (uint16_t)(frame[i][2] * b / 255) );
    }
    Send_res();
}
void Clear_frame(void){
    for(int i = 0; i < NUM_LEDS; i++){
        frame[i][0] = 0;
        frame[i][1] = 0;
        frame[i][2] = 0;
    }
}
void Switch_off(){
    WS_DDR |= (1<<WS_PIN);
    for(int i = 0; i < NUM_LEDS; i++){
        Send_byte(0x00);
        Send_byte(0x00);
        Send_byte(0x00);
    }
    Send_res();
}
void Set_brightnes(uint8_t b){
        brightness = b;
}
void Incr_brightness(uint8_t inc,  uint8_t max){
    if((uint16_t) brightness + inc < max){
        brightness += inc;
    } else {
        brightness = max;
    }
}
void Decr_brightness(uint8_t dec, uint8_t min){
    if((int16_t) brightness - dec > min){
        brightness -= dec;
    } else {
        brightness = min;
    }    
}
uint8_t Read_brightnes(void){
    return brightness;
}

void WS_init(void){
    /*Инициализация таймера 0*/
    TCCR0A |= (1<<WGM01); //CTC mode
    TCCR0B |= (1<<CS00)|(1<<CS02); //clk/1024
    TIMSK0 |= (1<<OCIE0A);  //разрешить прерывание по совпадению
    OCR0A = 157;    //период около 10мс
    sei();
     Switch_off();
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
void Brightness_control_looping(void){
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
            Incr_brightness(1, MAX_BRIGHTNESS);
        } else {
            Decr_brightness(1, MIN_BRIGHTNESS);
        }
    }
}
void Set_mode(uint8_t m){
    mode = m; 
    switch (m){
        case SOLID_WHITE:
            Switch_off();  //выключить ленту, может помочь от зависших пикселей
            Fill_color(255, 255, 255);
            break;
        case FADING_RAINBOW:
            Switch_off();
            Fill_rainbow(64);
            break;
        case SOLID_COLOR_FADE:
            Switch_off();
            Fill_color(255, 0, 0);
            break;
    }
}
ISR(TIMER0_COMPA_vect){
    
    static uint8_t state = 0;
    if(Btn_press()){    //вкл/выкл по одинарному нажатию
        state ^= 1;
        Set_operation(state);
    }
    if(state){  //если включена
        if(Btn_double_press()){ //смена режима по двойному нажатию
            if(mode==2){
                mode = 0;
            } else {
                mode++;
            }
            Set_mode(mode);
        }
            switch (mode){  //анимация режимов
            case SOLID_WHITE:
                //тут не нужно ничего делать, для наглядности
                break;
            case FADING_RAINBOW:
                Fade_all(1);
                break;
            case SOLID_COLOR_FADE:
                Fade_all(1);
                break;
        }
        Brightness_control_looping();   //изменение яркости удержанием
        Show_frame(brightness); //показать кадр с учетом яркости
    }
    // <editor-fold defaultstate="collapsed" desc="Выполение операций">
    if(operationComplFlag == 0){    //если текущая операция не завершена
        static uint8_t tempBrightness = MAX_BRIGHTNESS; //временная яркость для запоминания уровня перед выключением
        static uint8_t rememberBrightnessFlag = 0;  //флаг записи временной яркости
        switch (oper){
            case 0: //плавно уменьшить яркость до 0
                if(rememberBrightnessFlag == 0){tempBrightness = brightness;} //перед уменьшением записали текущую яркость
                rememberBrightnessFlag = 1; //подняли флаг и больше не записываем
                Decr_brightness(BRIGHTNESS_STEP_AMOUNT, 0);
                if(brightness == 0){operationComplFlag = 1;} //поднять флаг завершения операции
                Show_frame(brightness);
                Switch_off();
                break;
            case 1: //плавно увеличить яркость до уровня Set_brightnesss()
                Incr_brightness(BRIGHTNESS_STEP_AMOUNT, MAX_BRIGHTNESS);
                if((uint16_t)brightness >= tempBrightness){
                    brightness = tempBrightness;
                    operationComplFlag = 1;//поднять флаг завершения операции
                    rememberBrightnessFlag = 0;//опустили флаг запоминания яроксти
                } 
                break;
        }
    }
    // </editor-fold>
    Reset_btn_press_count();//если больше 1 нажатия при выключеном состоянии, то сброс количества нажатий
}

