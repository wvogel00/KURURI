/*
 * File:   main.c
 * Author: eagle
 *
 * Created on 2016/07/16, 22:25
 */


#include <xc.h>
#define _XTAL_FREQ 8000000    //  delay用に必要(クロック8MHzを指定)
#define HIGH 1
#define LOW 0
#define DeltaT 3.5  //3.5msが、ロータリーエンコーダのパルス間隔
#define LED1 PORTAbits.RA0
#define LED2 PORTAbits.RA2
#define RELAY1 PORTAbits.RA1
#define RELAY2 PORTAbits.RA4
#define RELAY3 PORTCbits.RC5
#define RELAY4 PORTCbits.RC4
#define RELAY5 PORTCbits.RC3
#define IPHONE_MIC  PORTAbits.RA5
#define TO_IPHONE 0
#define TO_SPEAKER 1
// コンフィギュレーション１の設定
#pragma config FOSC     = INTOSC   // 内部ｸﾛｯｸ使用する(INTOSC)
#pragma config WDTE     = OFF      // ｳｵｯﾁﾄﾞｯｸﾞﾀｲﾏｰ無し(OFF)
#pragma config PWRTE    = ON       // 電源ONから64ms後にﾌﾟﾛｸﾞﾗﾑを開始する(ON)
#pragma config MCLRE    = OFF      // 外部ﾘｾｯﾄ信号は使用せずにﾃﾞｼﾞﾀﾙ入力(RA3)ﾋﾟﾝとする(OFF)
#pragma config CP       = OFF      // ﾌﾟﾛｸﾞﾗﾑﾒﾓﾘｰを保護しない(OFF)
#pragma config BOREN    = ON       // 電源電圧降下常時監視機能ON(ON)
#pragma config CLKOUTEN = OFF      // CLKOUTﾋﾟﾝをRA4ﾋﾟﾝで使用する(OFF)
#pragma config IESO     = OFF      // 外部・内部ｸﾛｯｸの切替えでの起動はなし(OFF)
#pragma config FCMEN    = OFF      // 外部ｸﾛｯｸ監視しない(OFF)
// コンフィギュレーション２の設定
#pragma config WRT      = OFF      // Flashﾒﾓﾘｰを保護しない(OFF)
#pragma config PPS1WAY  = OFF      // ロック解除シーケンスを実行すれば何度でもPPSLOCKをセット/クリアできる
#pragma config ZCDDIS   = ON       // ゼロクロス検出回路は無効とする(ON)
#pragma config PLLEN    = OFF      // 動作クロックを32MHzでは動作させない(OFF)
#pragma config STVREN   = ON       // スタックがオーバフローやアンダーフローしたらリセットをする(ON)
#pragma config BORV     = HI       // 電源電圧降下常時監視電圧(2.5V)設定(HI)
#pragma config LPBOR    = OFF      // 低消費電力ブラウンアウトリセット(LPBOR_OFF)
#pragma config LVP      = OFF      // 低電圧プログラミング機能使用しない(OFF)

char cw;    //時計回り判定フラグ 1=CW, 0=CCW
char signalTo = TO_SPEAKER;

void BLINKS(int t)
{
    if(t > 0)
    {
        for(; t > 0; t--)
        {
        LED2 = LOW;
        LED1 = HIGH;
        __delay_ms(200);
        LED1 = LOW;
        LED2 = HIGH;
        __delay_ms(200); 
        }
    }
    else
    {
        while(1)
        {
            LED2 = LOW;
            LED1 = HIGH;
            __delay_ms(200);
            LED1 = LOW;
            LED2 = HIGH;
            __delay_ms(200);       
        }   
    }
}

void interrupt isr()
{
    char counter0 = 0,counter1 = 0;
    GIE = 0;
    
    //iphone-speaker スイッチ割り込み
    if(INTCONbits.IOCIF && IOCCFbits.IOCCF1)
    {
        //フラグクリア
        IOCCFbits.IOCCF1 = 0;
        INTCONbits.IOCIF == 0;
        signalTo = signalTo == TO_SPEAKER ? TO_IPHONE : TO_SPEAKER;
    }
    
    if(INTCONbits.IOCIF && IOCCFbits.IOCCF0)   //RC0が立ち上がり
    {
        //フラグクリア
        IOCCFbits.IOCCF0 = 0;
        IOCCFbits.IOCCF1 = 0;
        INTCONbits.IOCIF == 0;
        
        //チャタリング対策
        for(int i = 0; i < 5; i++)
        {
            counter0 += PORTCbits.RC0;
            counter1 += PORTCbits.RC1;
            __delay_us(200);
        }
        if(3 <= counter0 && counter1 <= 3)
        {
            for(int i = 0; i < 10; i++)
            {
                IPHONE_MIC = HIGH;
                __delay_us(20);
                IPHONE_MIC = LOW;
                __delay_us(20);
                LED2 = LOW;
                __delay_us(20);
                LED1 = HIGH;
            }
        }
        else if (3 <= counter0 && 3 <= counter1)
        {
            for(int i = 0; i < 2; i++)
            {
                IPHONE_MIC = HIGH;
                __delay_us(120);
                IPHONE_MIC = LOW;
                __delay_us(80);
                LED1 = LOW;
                __delay_us(20);
                LED2 = HIGH;
            }
        }
    }
    GIE = 1;
}



void initialize()
{
    //ピンの各種設定
    OSCCON = 0b01110010;
    ANSELA = 0x00;  //AN0-3 as digital I/O
    ANSELC = 0x00;  //RC are not analog I/O.
    TRISA = 0x00;   //all RA as OUTPUT
    TRISC = 0x03;   // RC0,RC1 as input. others are output.
    PORTA = 0x00;   //initial out is LOW
    PORTC = 0x00;
    
    BLINKS(3);
    LED2 = LOW;
    
    //ピン割り込み
    IOCCP = 0x01;   //RC0,1を立ち上がり割り込み入力
    INTCON = 0x00;  //割り込み初期化
    PIE1 = 0;       //割り込み初期化
    PIE2 = 0;       //割り込み初期化
    GIE = 1;        //割り込み一括許可
    
    IOCCF = 0x00;   //IO edge検出割り込み初期化
    IOCIE = 1;      //IO edge検出割り込み初期化
    PEIE = 1;       //IO edge検出割り込み初期化
}

void SwitchRelay(int id)
{
    __delay_ms(1000);
    const int overlap = 500;
    switch(id)
    {
        case 0 : RELAY1 = HIGH; __delay_ms(200); RELAY5 = LOW; break;
        case 1 : RELAY2 = HIGH; __delay_ms(200); RELAY1 = LOW; break;
        case 2 : RELAY3 = HIGH; __delay_ms(200); RELAY2 = LOW; break;
        case 3 : RELAY4 = HIGH; __delay_ms(200); RELAY3 = LOW; break;
        case 4 : RELAY5 = HIGH; __delay_ms(200); RELAY4 = LOW; break;
    }
}

void main(void) {
    initialize();
    int mode = 1;
    double counter = 0;
    
    if(mode == 0)
        BLINKS(0);
    else
        while(1)
        {
            for(int i = 0; i < 5; i++)
            {
                SwitchRelay(i);
            }
        }
    
    return;
}
