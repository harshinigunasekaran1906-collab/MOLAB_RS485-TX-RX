#include <xc.h>
#include <string.h>

/* ================= CONFIG BITS ================= */
#pragma config FOSC = INTRC_NOCLKOUT
#pragma config WDTE = OFF
#pragma config PWRTE = OFF
#pragma config MCLRE = ON
#pragma config CP = OFF
#pragma config CPD = OFF
#pragma config BOREN = OFF
#pragma config IESO = OFF
#pragma config FCMEN = OFF
#pragma config LVP = OFF

#define _XTAL_FREQ 8000000

/* ================= RS485 ================= */
#define RS485_EN RB0     // DE & RE? tied together

/* ================= UART FUNCTIONS ================= */
void UART_Init(void)
{
    TRISC6 = 0;    // TX
    TRISC7 = 1;    // RX

    SPBRG = 51;    // 9600 baud @ 8 MHz
    BRGH = 1;
    SYNC = 0;
    SPEN = 1;
    TXEN = 1;
    CREN = 1;
}

void UART_WriteChar(char data)
{
    while(!TXIF);     // wait until TX buffer free
    TXREG = data;
}

void UART_WriteString(const char *str)
{
    while(*str)
        UART_WriteChar(*str++);
}

/* ================= RS485 SEND ================= */
void RS485_SendString(const char *str)
{
    RS485_EN = 1;         // TX mode
    __delay_ms(2);

    UART_WriteString(str);

    while(!TRMT);        // wait for transmission complete
    __delay_ms(2);
    RS485_EN = 0;         // back to RX mode
}

/* ================= MAIN ================= */
void main(void)
{
    OSCCON = 0x70;   // 8 MHz internal clock
    ANSEL = 0;
    ANSELH = 0;

    TRISB0 = 0;
    RS485_EN = 0;    // RX mode

    UART_Init();

    char buffer[10];
    unsigned char index = 0;

    while(1)
    {
        if(!RCIF)
            continue;        // wait for character

        char c = RCREG;

        /* ---- ENTER pressed (\r or \n) ---- */
        if(c == 0x0D || c == 0x0A )
        {
            buffer[index] = '\0';   // end string

            if(strcmp(buffer, "hi") == 0)
            {
                RS485_SendString("hello\r\n");
            }
            else{
                RS485_SendString("invaild\r\n");
                }

            index = 0;   // reset buffer for next command
        }
        else
        {
            if(index < sizeof(buffer) - 1)
            {
                buffer[index++] = c;   // store character
            }
              
            }
        }
    }

