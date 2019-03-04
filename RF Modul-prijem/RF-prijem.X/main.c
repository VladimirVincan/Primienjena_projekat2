/*********************************************************************************
Naziv programa: UART serijska RS232 komunikacija
Primena programa:  Program omogucava prijem karaktera na dsPIC30f4013 preko RF modula povezanog na
                serijski  port. Brzina komunikacije je 1200 
			    a, frekvencija o scilatora je 10MHz sa PLL-om 4 
Opis programa:  prima karaktere dobijene preko RF modula i ispisuje ih na terminal  
*********************************************************************************/
#include<p30fxxxx.h>
#include "pwm.h"
//#include "movement.h"
#include "timer2.h"

_FOSC(CSW_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
_FWDT(WDT_OFF);

unsigned char tempRX;
unsigned int broj1,broj2;

/***********************************************/
/*
 * PWM
 */
/***********************************************/

void __attribute__((__interrupt__)) _T2Interrupt(void)
{
   	TMR2 = 0;
    IFS0bits.T2IF = 0;
}

void mot1_init_pwm(void)
{
    PR1=2499;//odredjuje frekvenciju po formuli -- Set the Timer 2 period (max 65535)
    OC1RS=20;//postavimo pwm
    OC1R=1000;//inicijalni pwm pri paljenju samo
    OC1CON = 0x0006;
    T2CONbits.TON=1;//ukljucujemo timer koji koristi
    OC1RS=1249;//ovim postavljamo faktor ispune
}

void mot2_init_pwm(){
    PR2=2499;//odredjuje frekvenciju po formuli -- Set the Timer 3 period (max 65535)
    OC2RS=20;//postavimo pwm
    OC2R=1000;//inicijalni pwm pri paljenju samo
    OC2CON = 0x0006;
    T2CONbits.TON=1;//ukljucujemo timer koji koristi
    OC2RS=1249;//ovim postavljamo faktor ispune
}

void mot1_set_pwm(int percentage)
{
    OC1RS = percentage;//ovim postavljamo pwm
}

void mot2_set_pwm(int percentage)
{
    OC2RS = percentage;//ovim postavljamo pwm
}

/***********************************************/
/*
 * UART
 */
/***********************************************/

void initUART1(void)
{
    U1BRG=0x0207;//ovim odredjujemo baudrate 1200
    U1MODEbits.ALTIO=0;//biramo koje pinove koristimo za komunikaciju osnovne ili alternativne
    IEC0bits.U1RXIE=1;//omogucavamo rx1 interupt
    U1STA&=0xfffc;
    U1MODEbits.UARTEN=1;//ukljucujemo ovaj modul
    U1STAbits.UTXEN=1;//ukljucujemo predaju
}


void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
{
    IFS0bits.U1RXIF = 0;
    tempRX=U1RXREG;
} 

void WriteUART1(unsigned int data)
{
    while(!U1STAbits.TRMT);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

/***********************************************/
/*
 * MOVEMENT
 */
/***********************************************/

void init_pins()
{
    MOT_EN1_TRIS = 0;
    MOT_EN2_TRIS = 0;
    MOT_IN1_TRIS = 0;
    MOT_IN2_TRIS = 0;
    MOT_IN3_TRIS = 0;
    MOT_IN4_TRIS = 0;
}

void set_forward()
{
    mot1_set_pwm(PWM_MIN);
    MOT_IN1_LAT = 1;
    MOT_IN2_LAT = 0;
    mot1_set_pwm(PWM_MAX);
}

void set_backward()
{
    mot1_set_pwm(PWM_MIN);
    MOT_IN1_LAT = 0;
    MOT_IN2_LAT = 1;
    mot1_set_pwm(PWM_MAX);
}

void set_left()
{
    mot2_set_pwm(PWM_MIN);
    MOT_IN3_LAT = 1;
    MOT_IN4_LAT = 0;
    mot2_set_pwm(PWM_MAX);
}

void set_right()
{
    mot2_set_pwm(PWM_MIN);
    MOT_IN3_LAT = 0;
    MOT_IN4_LAT = 1;
    mot2_set_pwm(PWM_MAX);
}

void set_stop()
{
    mot1_set_pwm(PWM_MIN);
    mot2_set_pwm(PWM_MIN);
    MOT_IN1_LAT = 0;
    MOT_IN2_LAT = 0;
    MOT_IN3_LAT = 0;
    MOT_IN4_LAT = 0;
}

/***********************************************/
/*
 * PARSER
 */
/***********************************************/

#define MESSAGES_LEN 8
#define TRANSMIT_COUNT 3
char messages[MESSAGES_LEN];

void init_messages()
{
    int i;
    for (i=0;i<MESSAGES_LEN;++i)
        messages[i] = '0';
}

void parse(unsigned char c)
{
    int i;
    for (i=0;i<MESSAGES_LEN-1;++i) 
        messages[i] = messages[i+1];
    messages[MESSAGES_LEN-1]=c;
}

char get_movement_command()
{
    int f=0,b=0,L=0,r=0,s=0,x=0;
    int i;
    for (i=0;i<MESSAGES_LEN;++i)
        if (messages[i]=='f') ++f;
        else if (messages[i]=='b') ++b;
        else if (messages[i]=='L') ++L;
        else if (messages[i]=='r') ++r;
        else if (messages[i]=='s') ++s;
        else if (messages[i]=='x') ++x;
    
    if (f >= TRANSMIT_COUNT) return 'f';
    else if (b >= TRANSMIT_COUNT) return 'b';
    else if (L >= TRANSMIT_COUNT) return 'L';
    else if (r >= TRANSMIT_COUNT) return 'r';
    else if (s >= TRANSMIT_COUNT) return 's';
    else if (x >= TRANSMIT_COUNT) return 'x';
    else return '0';
}

/***********************************************/
/*
 * LEDS
 */
/***********************************************/

void init_leds()
{
    TRISAbits.TRISA11 = 0;
    TRISBbits.TRISB8 = 0;
    TRISFbits.TRISF6 = 0;
    ADPCFGbits.PCFG8 = 1;
}

void change_led1()
{
    LATAbits.LATA11 =~ LATAbits.LATA11;
}

void change_led2()
{
    LATBbits.LATB8 =~ LATBbits.LATB8;
}

void change_led3()
{
    LATFbits.LATF6 =~ LATFbits.LATF6;
}

/***********************************************/
/*
 * MAIN
 */
/***********************************************/

int main(void)
	{
	for(broj1=0;broj1<10000;broj1++);
    initUART1();
    WriteUART1('t');
    
    init_messages();
    init_leds();
    init_pins();
    mot1_init_pwm();
    mot2_init_pwm();
    mot1_set_pwm(0);
    mot2_set_pwm(0);
    
    /*TRISBbits.TRISB0 = 1;
    TRISBbits.TRISB1 = 1;
    ADPCFGbits.PCFG0 = 1;
    ADPCFGbits.PCFG1 = 1;
    TRISBbits.TRISB2 = 1;
    ADPCFGbits.PCFG2 = 1;
    TRISBbits.TRISB3 = 1;
    ADPCFGbits.PCFG3 = 1;
    TRISBbits.TRISB4 = 1;
    ADPCFGbits.PCFG4 = 1;*/
	while(1)
	{
        if(tempRX)
        {
            WriteUART1(tempRX);
            parse(tempRX);
            tempRX=0;
            change_led1();
            change_led2();
            change_led3();
        }
        switch (get_movement_command()){
        case 'f':
            set_forward();
            break;
        case 'b':
            set_backward();
            break;
        case 'L':
            set_left();
            break;
        case 'r':
            set_right();
            break;
        case 's':
            set_stop();
            break;
        case '0':
        default:
            break;
        }
        /*if (PORTBbits.RB0)
            set_forward();
        if (PORTBbits.RB1)
            set_backward();
        if (PORTBbits.RB2)
            set_stop();
        if (PORTBbits.RB3)
            
        if (PORTBbits.RB4)
            set_right();*/
        //for(broj1=0;broj1<3000;broj1++)
        for(broj2=0;broj2<1000;broj2++);
    }//od whilea
    return 0;
}//od maina






