/* 
 * File:   main.c
 * Author: dima_ses
 *
 * Created on September 17, 2019, 6:34 PM
 */

#include "main.h"
/*
 * 
 */
int main() {
    UART_Init(8);   //230400
    DS_init(RESOLUTION_10BIT, 80, 10);
    WS_init();
    Btn_init();
    Set_mode(FADING_RAINBOW);
    while (1) {
        uint16_t temperature = DS_simpleRead()/10000;
        if( (temperature > 75) && (Read_brightnes() > 127)){
            Set_brightnes(127);
        }  
        UART_Transmit_int(temperature);
        UART_Transmit_byte('\n');
        _delay_ms(1000);
    }
    return 0;
}

