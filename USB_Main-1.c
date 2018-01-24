/* 
 * File:   USB_Main.c
 * Author: Bob
 *
 * Created on April 4, 2017, 3:51 PM
 */

#include <stdio.h>
#include <stdlib.h>
// PIC18F4423 Configuration Bit Settings

// 'C' source line config statements

#include <xc.h>

// #pragma config statements should precede project file includes.
// Use project enums instead of #define for ON and OFF.

// CONFIG1H
#pragma config OSC = INTIO67     // Oscillator Selection bits (Internal oscillator block, CLKO function on RA6, port function on RA7)
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOREN = OFF  // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown Out Reset Voltage bits (Minimum setting)

// CONFIG2H
#pragma config WDT = ON        // Watchdog Timer Enable bit (WDT enabled)
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = PORTC   // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = ON      // PORTB A/D Enable bit (PORTB<4:0> pins are configured as analog input channels on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = OFF       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = OFF      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF         // Single-Supply ICSP Enable bit (Single-Supply ICSP enabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) not protected from table reads executed in other blocks

#define _XTAL_FREQ          8000000
#define sensor_1            LATBbits.LATB4    //PORTBbits.RB4
#define sensor_2            LATBbits.LATB5    //PORTBbits.RB5
#define sensor_3            LATCbits.LATC6    //PORTCbits.RC6
#define sensor_4            LATCbits.LATC7    //PORTCbits.RC7

#define clock   			LATCbits.LATC3    //PORTCbits.RC3
#define dataOut             LATCbits.LATC5    //PORTCbits.RC5
#define sdata_in            PORTCbits.RC4

// FIFO status, controls and data bus
#define rx_empty			PORTBbits.RB0
#define tx_full				PORTBbits.RB1

#define read_cycle_init     LATEbits.LATE0 = 0, LATAbits.LATA4 = 0    //PORTEbits.RE0 = 0, PORTAbits.RA4 = 0
#define read_cycle_term     LATEbits.LATE0 = 1   //PORTEbits.RE0 = 1

#define write_cycle_init	LATAbits.LATA4 = 1    //PORTAbits.RA4 = 1
#define write_cycle_term    LATAbits.LATA4 = 0    //PORTAbits.RA4 = 0

#define RLY1                PORTAbits.RA5 
#define RLY2                PORTAbits.RA6
#define RLY3                PORTCbits.RC2
#define KY1                 LATAbits.LATA5
#define KY2                 LATAbits.LATA6
#define KY3                 LATCbits.LATC2




// Bicolor LED states
#define RedLED                  LATEbits.LATE1 = 0; LATEbits.LATE2 = 1    //PORTEbits.RE1 = 0, PORTEbits.RE2 = 1
#define AmberLED                LATEbits.LATE1 = 1; LATEbits.LATE2 = 1    //PORTEbits.RE1 = 1, PORTEbits.RE2 = 1
#define GreenLED                LATEbits.LATE1 = 1; LATEbits.LATE2 = 0    //PORTEbits.RE1 = 1, PORTEbits.RE2 = 0
#define OffLED                  LATEbits.LATE1 = 0; LATEbits.LATE2 = 0    //PORTEbits.RE1 = 0, PORTEbits.RE2 = 0

// humid sensor 
#define humEnable1              LATBbits.LATB4 = 1; LATBbits.LATB5 = 0; LATCbits.LATC6 = 0; LATCbits.LATC7 = 0    //PORTBbits.RB4 = 1, PORTBbits.RB5 = 0, PORTCbits.RC6 = 0, PORTCbits.RC7 = 0
#define humEnable2              LATBbits.LATB4 = 0; LATBbits.LATB5 = 1; LATCbits.LATC6 = 0; LATCbits.LATC7 = 0    //PORTBbits.RB4 = 0, PORTBbits.RB5 = 1, PORTCbits.RC6 = 0, PORTCbits.RC7 = 0
#define humEnable3              LATBbits.LATB4 = 0; LATBbits.LATB5 = 0; LATCbits.LATC6 = 1; LATCbits.LATC7 = 0    //PORTBbits.RB4 = 0, PORTBbits.RB5 = 0, PORTCbits.RC6 = 1, PORTCbits.RC7 = 0
#define humEnable4              LATBbits.LATB4 = 0; LATBbits.LATB5 = 0; LATCbits.LATC6 = 0; LATCbits.LATC7 = 1    //PORTBbits.RB4 = 0, PORTBbits.RB5 = 0, PORTCbits.RC6 = 0, PORTCbits.RC7 = 1
#define humDisabled             LATBbits.LATB4 = 0; LATBbits.LATB5 = 0; LATCbits.LATC6 = 0; LATCbits.LATC7 = 0    //PORTBbits.RB4 = 0, PORTBbits.RB5 = 0, PORTCbits.RC6 = 0, PORTCbits.RC7 = 0

// Test Points
#define TP1                     LATBbits.LATB6    //PORTBbits.RB6
#define TP2                     LATBbits.LATB7    //PORTBbits.RB7

#define BUFFER_SIZE			4

/*Prototypes    */
void sclockPulse();
unsigned int input_D();
void push_fifo (unsigned char sendbyte);
void sendTemps();
void sendHumids();
void sendAux();
int pull_fifo();
unsigned int getADC(unsigned char chSelect);
void anlgSelect(int a);
void getAuxs();
void getTemps();
void anlgSelect(int a);
void getHumids();
void humSelect(int a);
void cmdHum();
void cmdTemp();
unsigned char RLYStatus();
/* END Prototypes    */ 

/*Global Variables*/
unsigned char bitchkA;
unsigned char bitchkC;

int rx_buff[BUFFER_SIZE];
int cmd_length = 0; 

unsigned int buf_idx = 0;

short int rtcc_flag = 0;
short int cmd_flag = 0;
short int Err1 = 0;			// reserved error flag for usb comm problem
short int Err2 = 0;			// reserved error flag for command execution error
short int Err3 = 0;			// reserved error flag for humidity sensor protocol
int y = 0;
short int count = 0;

unsigned char tempArray[32];
unsigned char humidArray[16];
unsigned char auxArray[16];
unsigned char RLYStat;
int tempResult = 0;

/*
 * Functions
 */


unsigned int input_D()
{
    read_cycle_init;
    TRISD = 0xff;
    read_cycle_term;
    return PORTD;
}

void push_fifo (unsigned char sendbyte)
{
    write_cycle_init;
    while(tx_full);
    PORTD = sendbyte;
    write_cycle_term;
    return;
}

void sendTemps()
{
    push_fifo (0x40);
    push_fifo(0x10);
    for(unsigned char i = 0; i < 32; i++)
    {
        push_fifo(tempArray[i]);
    }
    push_fifo (0xff);
    push_fifo(0x00);
}

void sendHumids()
{
    push_fifo (0x40);
    push_fifo(0x20);
    for(unsigned char i = 0; i < 16; i++)
    {
        push_fifo(humidArray[i]);
    }
    push_fifo (0xff);
    push_fifo(0x00);
}

void sendAux()
{
    push_fifo (0x40);
    push_fifo(0x30);
    for(unsigned char i = 0; i < 16; i++)
    {
        push_fifo(auxArray[i]);
    }
    push_fifo (0xff);
    push_fifo(0x00);
}

void sendRLY()
{
    push_fifo (0x40);
    push_fifo(0x50);
    push_fifo(RLYStat);
    push_fifo(bitchkA);
    push_fifo(bitchkC);
    push_fifo (0xff);
    push_fifo(0x00);
}

int pull_fifo()
{
	int length;
	while (!rx_empty)
	{	
		rx_buff[buf_idx] = input_D();
		if (rx_buff[buf_idx] == 0xF5)
		{
			length = buf_idx + 1;	
			buf_idx = 0; 
			cmd_flag = 1; 
		}
		else if (buf_idx > (BUFFER_SIZE - 1))
		{	
			Err1 = 1;  
			length = 0;
            break;
		}			// shouldn't get this far without termination character, set err1
		else
		{	buf_idx++;  } 
	} 
	return length;	
}

 unsigned int getADC(unsigned char chSelect)
 {
    unsigned int result = 0;
    if (chSelect == 0){ADCON0 = 0x01; }
    if (chSelect == 1){ADCON0 = 0x05; }
    __delay_us(5);
    ADCON0bits.GO = 1;
    while (ADCON0bits.GO);
    result = ADRESH;
    result = result << 8;
    result = result + ADRESL;
    return result;
 }

 void getTemps()
{
    unsigned int T;
    unsigned int z = 0;
    PORTAbits.RA7 = 0;  
    __delay_ms(2);
    for (unsigned char i = 0; i < 16; i++)
    {
        anlgSelect(i);
        z = (unsigned)(i * 2);
        __delay_ms(4);
        T = getADC(0);
        __delay_ms(1);
        tempArray[z] = (T & 0xff00) >> 8;
        tempArray[z + 1] = T & 0x00ff;
    }
    PORTAbits.RA7 = 1;  
 }

   void getAuxs()
{
    unsigned int A;
    unsigned int z = 0;
    for (unsigned char i = 0; i < 8; i++)
    {
        anlgSelect(i);
        z = (unsigned)(i * 2);
        AmberLED;
        __delay_ms(5);        
        A = getADC(1);
        auxArray[z] = (A & 0xff00) >> 8;
        auxArray[z + 1] = A & 0x00ff;
    }    
}
void anlgSelect(int a)
{
    switch(a)
    {
        case 0:     LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 1:     LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 2:     LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 3:     LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break; 
        case 4:     LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 5:     LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 6:     LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 7:     LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 0;    //PORTCbits.RC1 = 0;
                    break;
        case 8:     LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 9:     LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 10:    LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 11:    LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 0;    //PORTCbits.RC0 = 0;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 12:    LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 13:    LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 0;    //PORTBbits.RB2 = 0;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 14:    LATBbits.LATB2 = 0;    //PORTBbits.RB3 = 0;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;
        case 15:    LATBbits.LATB2 = 1;    //PORTBbits.RB3 = 1;
                    LATBbits.LATB3 = 1;    //PORTBbits.RB2 = 1;
                    LATCbits.LATC0 = 1;    //PORTCbits.RC0 = 1;
                    LATCbits.LATC1 = 1;    //PORTCbits.RC1 = 1;
                    break;                    
    }
}

  //////////////////////////////////////////////////////////
 /// Read Humidity Sensors and Store H and T into Array ///
//////////////////////////////////////////////////////////

void humSelect(int a)
{
    switch (a)
    {
        case 0:     humEnable1;
        break;
        case 1:     humEnable2;
        break;        
        case 2:     humEnable3;
        break;       
        case 3:     humEnable4;
        break;
        default:    humDisabled;
    }
}

unsigned int bitSet(unsigned int item, unsigned int place)
{
    item |= (unsigned)(1) << place;
    return item;
}

unsigned int bitClear(unsigned int item, unsigned int place)
{
    item &= (unsigned)(~((1) << place));
    return item;
}

void sclockPulse()
{
    LATCbits.LATC3 = 0;    //PORTCbits.RC3 = 0;
    //__delay_us(4);
    LATCbits.LATC3 = 1;    //PORTCbits.RC3 = 1;
    //__delay_us(0);
}

void cmdHum()
{
        clock = 0;
        dataOut = 0;
        __delay_us(1);
        clock = 1;
        __delay_us(1);
        clock = 0;
        __delay_us(1);
        dataOut = 1;            // ready to start command sequence
        __delay_us(10);
        clock = 1;
        dataOut = 0;            // set up for command
        __delay_us(2);
        clock = 0;      // 1
        clock = 1;
        __delay_us(2);
        clock = 0;      // 2
        clock = 1;
        __delay_us(2);
        clock = 0;      // 3
        clock = 1;
        __delay_us(2);
        clock = 0;      // 4
        clock = 1;
        __delay_us(2);
        clock = 0;      // 5
        clock = 1;
        dataOut = 1;
        __delay_us(2);
        clock = 0;  // 6
        clock = 1;
        __delay_us(2);
          dataOut = 0;
        __delay_us(2);
        clock = 0;  // 7
        clock = 1;
        __delay_us(2);
        dataOut = 1;
        __delay_us(2);
        clock = 0;  //8
        clock = 1;    
}

void cmdTemp()
{
        clock = 0;
        dataOut = 0;
        __delay_us(1);
        clock = 1;
        __delay_us(1);
        clock = 0;
        __delay_us(1);
        dataOut = 1;            // ready to start command sequence
        __delay_us(10);
        clock = 1;
        dataOut = 0;            // set up for command
        __delay_us(2);
        clock = 0;      // 1
        clock = 1;
        __delay_us(2);
        clock = 0;      // 2
        clock = 1;
        __delay_us(2);
        clock = 0;      // 3
        clock = 1;
        __delay_us(2);
        clock = 0;      // 4
        clock = 1;
        __delay_us(2);
        clock = 0;      // 5
        clock = 1;
        __delay_us(2);
        clock = 0;  // 6
        clock = 1;
        __delay_us(2);
          dataOut = 1;
        __delay_us(2);
        clock = 0;  // 7
        clock = 1;
        __delay_us(2);
        clock = 0;  //8
        clock = 1;    
}

void getHumids()
{

	GreenLED;					// TEST TEST
	short int sensor_inop;
	unsigned int sensor_data = 0;
	for (int i = 0; i < 4; i++)
	{	
        humSelect(i);   // selects which sensor will be measured.
		sensor_inop = 0;				// initialize inop flag, which gets set to confirm an absent sensor
		Err3 = 0;
        //clock = 0;
		dataOut = 1;
        for (int s = 0; s < 17; s++)
        {
            sclockPulse();
        }
        cmdHum();
		// check for ACK  low after short delay
		__delay_us(2);
        if (sdata_in)
		{	
			sensor_inop = 1;  				// set flag if sensor not responding
			Err3 = 1;
		}
		else
		{
            clock = 0; 
            __delay_us(2);
            clock = 1;
            __delay_us(2);
        }						// clock ACK away
        __delay_us(2);
		//wait for measurement to complete
		while (sdata_in && !Err3);      // wait here for sensor to pull data line, if no error (or missing device) flagged
        
		// loop for first (msB) byte
		for (unsigned char j = 8; j > 0; j--)						
		{
			clock = 0;
			if (sdata_in)
			{	sensor_data = bitSet(sensor_data,(unsigned char)((j - 1 ) + 8));  }
			else
			{	sensor_data = bitClear(sensor_data,(unsigned char)((j - 1) + 8));  }
			clock = 1;
		}	
        
            dataOut = 0;        // ack
            sclockPulse();      // ack
            dataOut = 1;        // ack
	
		// loop for second (lsB) byte
		for (unsigned char j = 8; j > 0; j--)						
		{
			clock = 0;
			if (sdata_in)
			{	sensor_data = bitSet(sensor_data,(unsigned char)((j - 1)));  }
			else
			{	sensor_data = bitClear(sensor_data,(unsigned char)((j - 1)));  }
			clock = 1;
		}
            sclockPulse();      // ack
			// error flagging
			if (Err3)
			{ 
                sensor_data = 0xf4f4; 
                humidArray[(unsigned char)(i * 4)] = (unsigned char)((sensor_data & 0xff00) >> 8);
                humidArray[(unsigned char)((i * 4) + 1)] = (unsigned char)((sensor_data & 0x00ff));                
            } 			
            else
            {
                humidArray[(unsigned char)(i * 4)] = ((sensor_data & 0xff00) >> 8);
                humidArray[(unsigned char)((i * 4) + 1)] = ((sensor_data & 0x00ff));                
            }  

			Err3 = 0;   // set error to no error for temp reading
		// send command - read temperature
            cmdTemp();
		// check for ACK  low
        if (sdata_in)
		{	
			sensor_inop = 1;  				// set flag if sensor not responding
			Err3 = 1;
		}
		else
		{
            clock = 0; 
            __delay_us(2);
            clock = 1;
            __delay_us(2);
        }					
		// then delay for ACK to go away
		__delay_us (2);
		//wait for measurement to complete
		while (sdata_in && !Err3  ); // wait here for sensor to pull data line, if no error and responding
		// loop for first (msB) byte
		for (int j = 8; j > 0; j--)						
		{
			clock = 0;
			if (sdata_in)
			{	sensor_data = bitSet(sensor_data,(unsigned char)((j - 1 ) + 8));  }
			else
			{	sensor_data = bitClear(sensor_data,(unsigned char)((j - 1) + 8));  }
			clock = 1;
		}	
            dataOut = 0;        // ack
            sclockPulse();      // ack
            dataOut = 1;        // ack
		// loop for second (lsB) byte
		for (int j = 8; j > 0; j--)						
		{
			clock = 0;
			if (sdata_in)
			{	sensor_data = bitSet(sensor_data,(unsigned char)((j - 1)));  }
			else
			{	sensor_data = bitClear(sensor_data,(unsigned char)((j - 1)));  }
			clock = 1;
		}
            sclockPulse();      // ack
		// error recovery
		if (Err3)
        {
            sensor_data = 0xf4f4; 
          	humidArray[(unsigned char)((i * 4) + 2)] = (sensor_data & 0xff00) >> 8;
            humidArray[(unsigned char)((i * 4) + 3)] = (sensor_data & 0x00ff);              
        }
        else
        {
          	humidArray[(unsigned char)((i * 4) + 2)] = (sensor_data & 0xff00) >> 8;
            humidArray[(unsigned char)((i * 4) + 3)] = (sensor_data & 0x00ff);  
        }
	}
    humDisabled;
	return;
}

unsigned char RLYStatus()
{
    char x = 0; 
    if (KY1)
    {
        x = 0x01;
    }
    if (KY2)
    {
        x = (unsigned)( x | 0x02);
    }
    if (KY3)
    {
        x = (unsigned)( x | 0x04);
    }  
    return x;
}



void main(void) 
{
    TRISA = 0b00001111;
	TRISB = 0b00000011;			
	TRISC = 0b00010000;
    TRISD = 0b00000000;
	TRISE = 0b11101000;   

    RLY1 = 0;
    RLY2 = 0;
    RLY3 = 0;
    
	// setup_oscillator( OSC_8MHZ );
    OSCCON = 0x73;
    
   // Things that are turned off
   //setup_spi(0);
   T2CON = 0x00;
   T3CON = 0x00;
   T1CON = 0x00;
   	

   CCP1CON = 0x00;
   CCP2CON = 0x00;
   SSPCON1 = 0x00;
   ADCON0 = 0x00;
   ADCON1 = 0x1D;           // sets up A/D for inputs on AN1&2, VREF+ on AN
   ADCON2 = 0xa1;           // sets up A/D for FOSC/8 and TAD = 4
   //INTCON = 0XA0;
   INTCON = 0X00;           // no interrupts
    read_cycle_term;
    write_cycle_term;
    LATD = 0x00;
    PORTD = 0x00;
    __delay_ms(100);
    //Start of main loop
    

    while(1)
    {
    y = 0;
    RedLED;
    __delay_ms(50);
    GreenLED;
    __delay_ms(50);
    getTemps();
    getAuxs();
    y = pull_fifo();
    if (y == 0)
        {
            //break;
        }
    if (y == 3)
        {
        if (rx_buff[1] == 0x10)
            {
                TRISD = 0x00;
                sendTemps();
                continue;  
            }
            if (rx_buff[1] == 0x20)
            {
                LATAbits.LATA7 = 1;    //PORTAbits.RA7 = 1;
                __delay_ms(10);
                getHumids();
                LATAbits.LATA7 = 0;    //PORTAbits.RA7 = 0;
                TRISD = 0x00;
                sendHumids();
                TRISD = 0xFF;
                continue;  
            }
            if (rx_buff[1] == 0x30)
            {
                TRISD = 0x00;
                sendAux();
                TRISD = 0xFF;
                continue;  
            }
        }
    if (y == 4)
    {
            bitchkA = PORTA;
            bitchkC = PORTC;
            if (rx_buff[1] == 0x50)
            {
                if (rx_buff[2] == 0x01)
                {
                    if ((bitchkA & 0x20) != 0)
                    {
                        KY1 = 0;
                    }
                    else
                    {
                        KY1 = 1;
                    }
                }
                if (rx_buff[2] == 0x02)
                {
                    if ((bitchkA & 0x40) != 0)
                    {
                        KY2 = 0;
                    }
                    else
                    {
                        KY2 = 1;
                    }
                }
                if (rx_buff[2] == 0x03)
                {
                    if ((bitchkC & 0x04) != 0)
                    {
                        KY3 = 0;
                    }
                    else
                    {
                        KY3 = 1;
                    }
                }
                RLYStat = RLYStatus();
                TRISD = 0x00;
                sendRLY();
                TRISD = 0xFF;
                continue;  
                CLRWDT();
            }
        }
    }
}



