// ------------------------------------------------------------------------------
// DATOS Y ENCABEZADO
//-------------------------------------------------------------------------------
    
// Archivo:                 prelab7.c
// Dispositivo:             PIC16F887
// Autor:                   GUSTABO CÓRDOVA 
// Programa:                Configurar módulo PWM 1 
// Creado:                  09 APR, 2023 
// Última modificación:     10 APR, 2023

// ------------------------------------------------------------------------------
// CONFIGURACIÓN DEL PIC
//-------------------------------------------------------------------------------

// CONFIG1
#pragma config FOSC     =   INTRC_NOCLKOUT  // Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE     =   OFF             // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE    =   OFF             // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE    =   OFF             // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP       =   OFF             // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD      =   OFF             // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN    =   OFF             // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO     =   OFF             // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN    =   OFF             // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP      =   OFF             // Low Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)

// CONFIG2
#pragma config BOR4V    =   BOR40V          // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT      =   OFF             // Flash Program Memory Self Write Enable bits (Write protection off)

// ------------------------------------------------------------------------------
// VARIABLES
//-------------------------------------------------------------------------------

#include <xc.h>
#include <stdint.h>
 
#define _XTAL_FREQ 8000000UL        // UL se coloca como un seguro para tener valores positivos

void main() {
    TRISCbits.TRISC2 = 0;           // Configura el pin RC2 como salida
    CCP1CONbits.CCP1M = 0b1100;     // Configura el módulo CCP1 como PWM
    CCPR1L = 0x7F;                  // Configura el valor del ciclo de trabajo en 50%
    PR2 = 255;                      // Configura el valor del período en 255
    T2CONbits.T2CKPS = 0b11;        // Configura el preescalador del temporizador en 1:64
    T2CONbits.TMR2ON = 1;           // Enciende el temporizador 2
    
    while(1) {
                                    // Coloca el servo en 90 grados
        CCPR1L = 0x9D;              // Configura el valor del ciclo de trabajo en 62.5%
        __delay_ms(500);            // Espera 500 ms
    }
}

