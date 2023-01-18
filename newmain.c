/* 
 * File:   newmain.c
 * Author: student
 *
 * Created on 20. april 2021., 13.13
 */


#include <stdio.h>
#include <stdlib.h>
#include "adc.h"
#define PWM_NORMAL 17  //16
#define TMR2_period 10000 /*  Fosc = 10MHz, 1/Fosc = 0.1us !!!, 0.1us * 10 = 1us  */
#define TMR1_period 50000 //interapt na 5000ms, sa ovim moze da izmeri distancu do oko 75cm senzori
#define TMR3_period 10000 //interapt na 5000ms, sa ovim moze da izmeri distancu do oko 75cm senzori
#define TMR4_period 100 //ms
//#define PWM_BACK 2000


_FOSC(CSW_FSCM_OFF & XT_PLL4);//instruction takt je isti kao i kristal 10MHz
_FWDT(WDT_OFF);
unsigned int stoperica=0,stoperica1=0, brojac1=0, Distance1=0, brojac2=0, Distance2=0;
int flag=0, counter=0, counter2=0;
int i = 0;


void PWM_CFG(void)
{
    //OC1CON =OC_IDLE_CON & OC_TIMER2_SRC & OC_PWM_FAULT_PIN_DISABLE & T2_PS_1_256; //KONFIGURACIJA PWM
    PR2=55; //2499
    OC1RS=1800; //20
    OC1R=1000;
    OC1CONbits.OCM = 0b110; //0B110
    T2CONbits.TCKPS = 0b11;  //0B11
    T2CONbits.TON=1;
    OC1RS = PWM_NORMAL;
}


void __attribute__((__interrupt__)) _U1RXInterrupt(void) 
    {
    IFS0bits.U1RXIF = 0;
    //   tempRX=U1RXREG;
    } 

void __attribute__ ((__interrupt__)) _T2Interrupt(void) // pwm
{
	TMR2 =0;
   
	IFS0bits.T2IF = 0;       
}

void __attribute__ ((__interrupt__)) _T3Interrupt(void) // svakih 10us
{
	TMR3 =0;
    stoperica++;//brojac za funkciju delay_ms
	IFS0bits.T3IF = 0;       
}

void Init_T1(void) 
{
	TMR1 = 0;
	PR1 = TMR1_period;
	
	T1CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T2IP = 3 // T2 interrupt pririty (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T1IF = 0; // clear interrupt flag
	IEC0bits.T1IE = 1; // enable interrupt

	//T1CONbits.TON = 1; // T1 on 
}
void Init_T2(void) 
{
	TMR2 = 0;
	PR2 = TMR2_period;
	
	T2CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T2IP = 3 // T2 interrupt pririty (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T2IF = 0; // clear interrupt flag
	IEC0bits.T2IE = 1; // enable interrupt

	//T2CONbits.TON = 1; // T2 on 
}
void Init_T3(void) 
{
	TMR3 = 0;
	PR3 = TMR3_period;
	
	T3CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T2IP = 3 // T2 interrupt pririty (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS0bits.T3IF = 0; // clear interrupt flag
	IEC0bits.T3IE = 1; // enable interrupt

	T3CONbits.TON = 1; // T2 on 
}

void Init_T4(void) 
{
	TMR4 = 0;
	PR4 = TMR4_period;
	
	T4CONbits.TCS = 0; // 0 = Internal clock (FOSC/4)
	//IPC1bits.T2IP = 3 // T2 interrupt pririty (0-7)
	//SRbits.IPL = 3; // CPU interrupt priority is 3(11)
	IFS1bits.T4IF = 0; // clear interrupt flag
	IEC1bits.T4IE = 1; // enable interrupt

    T4CONbits.TON = 1; // T2 on 
}

void __attribute__ ((__interrupt__)) _T1Interrupt(void)
{
	TMR1 = 0;  
    
    IFS0bits.T1IF = 0;
    
       
}

void __attribute__ ((__interrupt__)) _T4Interrupt(void)
{
	TMR4 =0;
    stoperica1++;//brojac za funkciju delay_ms
	IFS1bits.T4IF = 0;  
}




void delay_us(unsigned int vreme){
        stoperica = 0;
		while(stoperica < vreme);
    }

void delay_ms(unsigned int vreme1){
        stoperica1 = 0;
		while(stoperica1 < vreme1);
    }

void WriteUART1(unsigned int data)
    {
	while (U1STAbits.TRMT==0);
    if(U1MODEbits.PDSEL == 3)
        U1TXREG = data;
    else
        U1TXREG = data & 0xFF;
    }

void RS232_putst(register const char *str)
{
    while((*str) != 0)
    {
        WriteUART1(*str);
        if(*str == 13) WriteUART1(10);
        if(*str == 10) WriteUART1(13);
        str++;
    }
}

void WriteUART1dec2string(unsigned int data)
    {
	unsigned char temp;

	temp=data/1000;
	WriteUART1(temp+'0');
	data=data-temp*1000;
	temp=data/100;
	WriteUART1(temp+'0');
	data=data-temp*100;
	temp=data/10;
	WriteUART1(temp+'0');
	data=data-temp*10;
	WriteUART1(data+'0');
    }

void initUART1(void)
{
U1BRG=0x0040;//baud rate 9600
U1MODEbits.ALTIO = 0;
IEC0bits.U1RXIE = 1;
U1STA&=0xfffc;
U1MODEbits.UARTEN=1;
U1STAbits.UTXEN=1;
}
void pinConfig()
{
   
    ADPCFGbits.PCFG9=1;
    TRISBbits.TRISB9=0;// input1
    ADPCFGbits.PCFG10=1;
    TRISBbits.TRISB10=0;// input2
    ADPCFGbits.PCFG11=1;
    TRISBbits.TRISB11=0;// input3
    ADPCFGbits.PCFG12=1;
    TRISBbits.TRISB12=0;// input4
    
    TRISDbits.TRISD0=0;
    TRISDbits.TRISD1=0;
}

void napred()
{
     RS232_putst("Napred");
     WriteUART1(10);
    WriteUART1(13);
    pinConfig();
    unsigned int r=PWM_NORMAL;

    LATBbits.LATB9=1;//in1 naprijed
    LATBbits.LATB10=0;//in2 nazad
    LATDbits.LATD1=0;
    
   

}

void idiLevo()
{   
     RS232_putst("Levo");
     WriteUART1(10);
    WriteUART1(13);
    pinConfig();
    
    LATBbits.LATB11=1;
    LATBbits.LATB12=0;
    LATDbits.LATD1=1;
    ///delay_us(50);
    
    LATBbits.LATB9=1;
    LATBbits.LATB10=0;
    unsigned int r = PWM_NORMAL;
    
   
     
}

void idiDesno()
{
    RS232_putst("Desno");
    WriteUART1(10);
    WriteUART1(13);
     pinConfig();
     LATBbits.LATB11=0;
     LATBbits.LATB12=1;
     LATDbits.LATD1=1;
     //delay_us(50);
     
     LATBbits.LATB9=1;
     LATBbits.LATB10=0;
   unsigned int r = PWM_NORMAL;
   
   
    
}
void nazadDesno()
{
     RS232_putst("Nazad Desno");
     WriteUART1(10);
    WriteUART1(13);
     pinConfig();
     LATBbits.LATB11=0;
     LATBbits.LATB12=1;
     LATDbits.LATD1=1;
    // delay_us(50);
     
     LATBbits.LATB9=0;
     LATBbits.LATB10=1;
   unsigned int r = PWM_NORMAL;
   
   
    
}

void nazadLevo()
{
     RS232_putst("Nazad Levo");
     WriteUART1(10);
    WriteUART1(13);
     pinConfig();
     LATBbits.LATB11=1;
     LATBbits.LATB12=0;
     LATDbits.LATD1=1;
   //  delay_us(50);
     
     LATBbits.LATB9=0;
     LATBbits.LATB10=1;
   unsigned int r = PWM_NORMAL;
    
    
}

void stop()
{
    RS232_putst("Stop");
    WriteUART1(10);
    WriteUART1(13);
    pinConfig();
    //unsigned int r=PWM_NORMAL;

    LATBbits.LATB9=0;//in1 naprijed
    LATBbits.LATB10=0;//in2 nazad
    LATDbits.LATD1=0;
    
    
}

void nazad()
{
    RS232_putst("Nazad");
    WriteUART1(10);
    WriteUART1(13);
    pinConfig();
    unsigned int r=PWM_NORMAL;
    
    LATBbits.LATB9=0;//in1 naprijed
    LATBbits.LATB10=1;//in2 nazad
    LATDbits.LATD1=0;
    
    
}

int main(int argc, char** argv) {
    initUART1();
    Init_T1();
    //Init_T2();
    Init_T3();
    Init_T4();
    PWM_CFG();
    RS232_putst("serijska 2");
    WriteUART1(10);
    WriteUART1(13);
     
    
    TRISDbits.TRISD9 = 1; //echo
    TRISDbits.TRISD3 = 0; //trig
 
    TRISDbits.TRISD8 = 1; //echo
    TRISDbits.TRISD2 = 0; //trig
    
    while(1){
    
     RS232_putst("while");
     WriteUART1(10);
     WriteUART1(13);
     TMR1=0;
     
       
     //konfiguracija trigera za prvi senzor
       // T3CONbits.TON = 1; // T2 on 
        LATDbits.LATD3 = 0;
        delay_ms(1);
        LATDbits.LATD3 = 1;
        delay_ms(1);
        LATDbits.LATD3 = 0;
       // T3CONbits.TON = 0; // T2 on
    
        counter=0;
    while(!PORTDbits.RD9);
    //T1CONbits.TON=1;
    // Turn ON Timer1
    while(PORTDbits.RD9==1){
    counter++;
    delay_ms(1);
    }
   // T1CONbits.TON=0;
    // Turn OFF Timer1 
    
   
    
    Distance1 = counter*0.17;// Distance =(velocity x Time)/2 . 5*0.34=1.7
    
    RS232_putst("Distance1 = ");
    WriteUART1dec2string(Distance1);
    
    WriteUART1(10);
    WriteUART1(13);
        
       
       
    //konfiguracija trigera za drugi senzor    
      //  T3CONbits.TON = 1; // T2 on 
        LATDbits.LATD2 = 0;
        delay_ms(1);
        LATDbits.LATD2 = 1;
        delay_ms(1);
        LATDbits.LATD2 = 0;
      //  T3CONbits.TON = 0; // T2 on
        
        TMR1=0;
        counter2= 0;
    while(!PORTDbits.RD8);
    //T1CONbits.TON=1;	// Turn ON Timer1
    while(PORTDbits.RD8==1){
        
    counter2++;
    delay_ms(1);
    
    };
    //T1CONbits.TON=0;			// Turn OFF Timer1 
    //brojac2=TMR1;
    
    
    Distance2 = counter2*0.17;// Distance =(velocity x Time)/2  
    RS232_putst("Distance2 = ");
    WriteUART1dec2string(Distance2);
      
    if(Distance1 < 30 || Distance2 > 300 || Distance1 > 300)
    {  
        
        //stop();
        //delay_us(20);
        idiLevo();
        delay_us(450);  //250
        nazadDesno();
        delay_us(450); //200
        
        stop();
        delay_us(20);
        
        idiLevo();
        delay_us(1000);  //400
       
    }
    
    else if(Distance2 > 10 && Distance2 <= 15 && Distance1 >= 30 )
    {
        napred();
        delay_us(20);
    }
    
    else if(Distance2 > 15 && Distance2 <= 300 && Distance1 >=30  )
    {   
      
        idiDesno();
        delay_us(450);  //250
        nazadLevo();
        delay_us(200);
        
        stop();
        delay_us(20);
        
        idiDesno();
        delay_us(500);
    }
    
    else if(Distance2 <= 10 && Distance1 >= 30 )
    {
      
        idiLevo();
        delay_us(400);
    }
    
   
     else
     {
        napred();
       delay_us(30);
     }  

  }

    return (EXIT_SUCCESS);
}

