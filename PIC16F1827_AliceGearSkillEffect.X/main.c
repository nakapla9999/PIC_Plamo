/*
 * Summary: アリスギアのスキル風エフェクト
 * Author: nakapla
 * Created on 2023/02/16
 */

// PIC16F1827 Configuration Bit Settings

// CONFIG1
#pragma config FOSC = INTOSC    // Oscillator Selection (INTOSC oscillator: I/O function on CLKIN pin)
#pragma config WDTE = OFF       // Watchdog Timer Enable (WDT disabled)
#pragma config PWRTE = ON       // Power-up Timer Enable (PWRT enabled)
#pragma config MCLRE = OFF      // MCLR Pin Function Select (MCLR/VPP pin function is digital input)
#pragma config CP = OFF         // Flash Program Memory Code Protection (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Memory Code Protection (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown-out Reset Enable (Brown-out Reset disabled)
#pragma config CLKOUTEN = OFF   // Clock Out Enable (CLKOUT function is disabled. I/O or oscillator function on the CLKOUT pin)
#pragma config IESO = OFF       // Internal/External Switchover (Internal/External Switchover mode is disabled)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable (Fail-Safe Clock Monitor is disabled)

// CONFIG2
#pragma config WRT = OFF        // Flash Memory Self-Write Protection (Write protection off)
#pragma config PLLEN = OFF      // PLL Enable (4x PLL disabled)
#pragma config STVREN = ON      // Stack Overflow/Underflow Reset Enable (Stack Overflow or Underflow will cause a Reset)
#pragma config BORV = LO        // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (Vbor), low trip point selected.)
#pragma config LVP = OFF        // Low-Voltage Programming Enable (Low-voltage programming enabled)

#include <xc.h>
#define _XTAL_FREQ 8000000

void pwmLED(int duty,int interval);

void main(void) {
    
    OSCCON = 0b01110010;    // 内部クロック8MHz 4MHzにする場合:0b01101010
    ANSELA = 0b00000000;    // アナログは使用しない(すべてデジタルI/Oに割当)
    ANSELB = 0b00000000;
    
    TRISA = 0b00100000;     // RA5だけ入力専用、その他のピンは出力に割当
    TRISB = 0b00000000;
    
    PORTA = 0;              // 出力ピンの初期化(すべてLOWにする)
    PORTB = 0;
    
    // LED明滅制御のための変数
    int zentime1 = 150;
    int ontime1 = 1;
    int ima1 = 1;
    char fugou1 = 1;
    char exw = 1;
    
    while(1){
       
        // RA5ポートのタクトスイッチが押されるまでアイドリングする。
        while(RA5) {
            for(int i = 1; i < 10; i++){     // 徐々に明るく。
                pwmLED(i, 10);
            }
            for(int i = 10; i > 1; i--){     // 徐々に暗く。
                pwmLED(i, 10);
            } 
            __delay_ms(500);
        }

        // 発射前に消灯し0.5秒待つ。
        RA1 = 0;
        __delay_ms(500);
         
        // 急速に発光させる。
        zentime1 = 100;
        ima1 = 1;
        ontime1 = 1;
        fugou1 = 1;
        exw = 1;

        while(exw){
            if (ima1 > ontime1) {
                RA1 = 0;
            } else {
                RA1 = 1;
            }
            ima1++;
            if (ima1 > zentime1) {
                ima1 = 1;
                ontime1 = ontime1 + fugou1;
                // 最大光度の50%で抜ける。
                if (ontime1 > zentime1 * 0.5) {
                    exw = 0;
                }
            }
        }
        
        // 最大光度で発射!!
        RA1 = 1;
        __delay_ms(2);
        
        // 徐々にフェードアウトする。
        zentime1 = 300;
        ima1 = 1;
        ontime1 = 1;
        fugou1 = 1;
        exw = 1;
        
        while (exw){
            if (ima1 > ontime1) {
                RA1 = 1;
            } else {
               RA1 = 0; 
            }
            ima1++;
            if (ima1 > zentime1) {
                ima1 = 1;
                ontime1 = ontime1 + fugou1;
                if (ontime1 > zentime1) {
                    exw = 0;
                }
            }
        }
        
        // 1秒消灯し、アイドリングに戻る。
        RA1 = 0;
        __delay_ms(1000);        
    }
}

// PWMでLEDの明度をコントロールする。
void pwmLED(int duty,int interval){
    
    int i,j = 0;
    
    for(j = 1; j < interval; j++){
        for(i = 1; i < 1000; i++){
            if(i < duty){
                RA1 = 1;    
            }else{
                RA1 = 0;    
            }
        }
    }
}