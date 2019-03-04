/*********************************************************************************
Naziv programa: UART serijska RS232 komunikacija
Primena programa:  Program omogucava slanje karaktera na dsPIC30f4013 preko RF modula povezanog na
                serijski  port. Brzina komunikacije je 1200 
			    a, frekvencija o scilatora je 10MHz sa PLL-om 4 
Opis programa:  salje karaktere preko RF modula i ispisuje ih na terminal  
*********************************************************************************/
#include<p30fxxxx.h>
_FOSC(CSW_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal
_FWDT(WDT_OFF);

unsigned char tempRX;
unsigned int broj1,broj2;

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

/*********************************************************************
* Ime funkcije      : WriteUART1                                     *
* Opis              : Funkcija upisuje podatak u registar 			 *
* 					  za slanje podataka U1TXREG,   				 *
* Parametri         : unsigned int data-podatak koji treba poslati   *
* Povratna vrednost : Nema                                           *
*********************************************************************/

void WriteUART1(unsigned int data)
{
	  while(!U1STAbits.TRMT);

    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
}

void RS232_putst(register const char *str)
{
  while((*str)!=0)
  {
    WriteUART1(*str);
    if (*str==13) WriteUART1(10);
    if (*str==10) WriteUART1(13);
    str++;
  }
}

int main(void)
	{
	for(broj1=0;broj1<10000;broj1++);
    initUART1();
    WriteUART1('t');
    
    ADPCFG = 0x1e00;
    TRISB = 0x1e00;
    TRISD = 0x0003;
    
	while(1)
	{
        //RS232_putst("00000000");
        //RS232_putst("sssssssss");
        if (PORTBbits.RB9 == 1)
            RS232_putst("tttttttt"); // turbo
        else if (PORTBbits.RB10 == 1)
            RS232_putst("ffffffff"); // forward
        else if (PORTBbits.RB11 == 1)
            RS232_putst("rrrrrrrr"); // right
        else if (PORTBbits.RB12 == 1)
            RS232_putst("ssssssss"); // stop
        else if (PORTDbits.RD0 == 1)
            RS232_putst("LLLLLLL"); // left
        else if (PORTDbits.RD1 == 1)
            RS232_putst("bbbbbbbb"); // back
        else 
            RS232_putst("xxxxxxxx"); // idle
        RS232_putst("0123456789");
        
        for(broj1=0;broj1<200;broj1++)
        for(broj2=0;broj2<1000;broj2++);
    }//od whilea
    return 0;
}//od maina