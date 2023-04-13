// ------------------------------------------------------------------------------
// DATOS Y ENCABEZADO
//-------------------------------------------------------------------------------
    
// Archivo:                 postlab7.c
// Dispositivo:             PIC16F887
// Autor:                   GUSTABO CÓRDOVA 
// Programa:                Configurar módulo PWM 1 
// Creado:                  09 APR, 2023 
// Última modificación:     10 APR, 2023

// ------------------------------------------------------------------------------
// CONFIGURACIÓN DEL PIC
//-------------------------------------------------------------------------------

// CONFIG1
#pragma config FOSC = INTRC_NOCLKOUT// Oscillator Selection bits (INTOSCIO oscillator: I/O function on RA6/OSC2/CLKOUT pin, I/O function on RA7/OSC1/CLKIN)
#pragma config WDTE = OFF       // Watchdog Timer Enable bit (WDT disabled and can be enabled by SWDTEN bit of the WDTCON register)
#pragma config PWRTE = OFF      // Power-up Timer Enable bit (PWRT disabled)
#pragma config MCLRE = OFF      // RE3/MCLR pin function select bit (RE3/MCLR pin function is digital input, MCLR internally tied to VDD)
#pragma config CP = OFF         // Code Protection bit (Program memory code protection is disabled)
#pragma config CPD = OFF        // Data Code Protection bit (Data memory code protection is disabled)
#pragma config BOREN = OFF      // Brown Out Reset Selection bits (BOR disabled)
#pragma config IESO = OFF       // Internal External Switchover bit (Internal/External Switchover mode is disabled)
#pragma config FCMEN = ON      // Fail-Safe Clock Monitor Enabled bit (Fail-Safe Clock Monitor is disabled)
#pragma config LVP = ON        // L`ow Voltage Programming Enable bit (RB3 pin has digital I/O, HV on MCLR must be used for programming)
// CONFIG2
#pragma config BOR4V = BOR40V   // Brown-out Reset Selection bit (Brown-out Reset set to 4.0V)
#pragma config WRT = OFF        // Flash Program Memory Self Write Enable bits Write protection off)

#include <xc.h>
#include <stdint.h>

// ------------------------------------------------------------------------------
// CONSTANTES
//-------------------------------------------------------------------------------

#define _XTAL_FREQ 8000000UL        // UL se coloca como un seguro para tener valores positivos
#define _tmr0_value 246

// ------------------------------------------------------------------------------
// VARIABLES
//-------------------------------------------------------------------------------

uint8_t _tmr0_counter;      // variable para contar interrupciones del Timer0
uint8_t _led_pot;           // variable para guardar el valor del ADC

// ------------------------------------------------------------------------------
// PROTOTIPOS
//-------------------------------------------------------------------------------

void setup(void);           // prototipo de la función de inicialización

// ------------------------------------------------------------------------------
// INTERRUPCIONES
//-------------------------------------------------------------------------------

void __interrupt() isr(void) 
{
    if (T0IF) // si la interrupción es del Timer0
    {
        _tmr0_counter++;                // aumenta el contador de interrupciones del Timer0
        INTCONbits.T0IF = 0;            // borra la bandera de interrupción del Timer0
        TMR0 = _tmr0_value;             // reinicia el Timer0
        PIR1bits.TMR1IF = 0;            // borra la bandera de interrupción del Timer1
        if (_led_pot > _tmr0_counter)   // si el valor del ADC es mayor que el contador de interrupciones del Timer0
        {
            PORTCbits.RC3 = 1;          // enciende el LED conectado al pin RC3
        }
        else
        {
            PORTCbits.RC3 = 0;          // apaga el LED conectado al pin RC3
        }
    }
    
// ------------------------------------------------------------------------------
// ADC
//-------------------------------------------------------------------------------
    if(PIR1bits.ADIF)                               // si la interrupción es del ADC
    {
        if(ADCON0bits.CHS == 0)                     // si el canal seleccionado del ADC es el 0
        {
            CCPR1L = (ADRESH>>1)+40;                // configura el valor del registro CCPR1L del PWM para el SERVO conectado al pin RC2
            CCP1CONbits.DC1B1 = ADRESH & 0b01;      // configura los bits DC1B1 y DC1B0 del registro CCP1CON para el SERVO conectado al pin RC2
            CCP1CONbits.DC1B0 = (ADRESL>>7);
        }
        else if (ADCON0bits.CHS == 1)               // si el canal seleccionado del ADC es el 1
        {
            CCPR2L = (ADRESH>>1)+40;                // configura el valor del registro CCPR2L del PWM para el SERVO conectado al pin RC1
            CCP2CONbits.DC2B1 = ADRESH & 0b01;      // configura los bits DC2B1 y DC2B0 del registro CCP2CON para el SERVO conectado al pin RC1
            CCP2CONbits.DC2B0 = (ADRESL>>7);            
        }
        else                                        // si el canal seleccionado del ADC es el 2
        {
            _led_pot = ADRESH;                      // guarda el valor del ADC en la variable _led_pot
            PORTD = ADRESH;                         // muestra el valor del ADC en los pines del puerto C
        }
        PIR1bits.ADIF = 0;                          // borra la bandera de interrupción del ADC
        return;
    }
}

// ------------------------------------------------------------------------------
// MAIN
//-------------------------------------------------------------------------------

void main (void)
{
    setup();                // llama a la función de inicialización
    ADCON0bits.GO =1;       // inicia la conversión del ADC
    while(1) 
    {

// ------------------------------------------------------------------------------
// LOOP PRINCIPAL
//-------------------------------------------------------------------------------
// Verificación si el ADC está disponible para su uso
    if (ADCON0bits.GO == 0)
    {
        // Si el canal del ADC es 0
        if (ADCON0bits.CHS == 0)
        {
            ADCON0bits.CHS = 1; // Configura el canal del ADC en 1
        }
        // Si el canal del ADC es 1
        else if (ADCON0bits.CHS == 1)
        {
            ADCON0bits.CHS = 2; // Configura el canal del ADC en 2
        }
        // Si el canal del ADC no es ni 0 ni 1
        else
        {
            ADCON0bits.CHS = 0; // Configura el canal del ADC en 0
        }     
        __delay_us(50); 
        ADCON0bits.GO = 1; // Inicia la conversión del ADC
    }
    }
}

// Función que inicializa y configura los periféricos
void setup (void)
{
    ANSEL = 0b00000111;                     // Configura los pines analógicos/digitales
    ANSELH = 0;                             // Configura los pines analógicos/digitales
    
    TRISA = 0b00000111; 
    TRISC = 0; 
    //TRISD = 0;           
    
    // Configura el reloj
    OSCCONbits.IRCF = 0b110;                // Configura la velocidad del reloj interno
    OSCCONbits.SCS = 1;                     // Selecciona el reloj interno
    
    // Configura el temporizador TMR0
    OPTION_REGbits.T0CS = 0;                // Configura el TMR0 como temporizador interno
    OPTION_REGbits.T0SE = 0;                // Configura la transición del TMR0 en el flanco de subida
    OPTION_REGbits.PSA = 0;                 // Habilita el prescaler del TMR0
    OPTION_REGbits.PS2 = 1;                 // Configura el prescaler del TMR0
    OPTION_REGbits.PS1 = 0;                 // Configura el prescaler del TMR0
    OPTION_REGbits.PS0 = 1;                 // Configura el prescaler del TMR0
    TMR0 = _tmr0_value;                     // Carga un valor inicial en el TMR0
    
    INTCONbits.T0IF = 0;                    // Limpia la bandera de interrupción del TMR0

    // Configura el ADC
    ADCON1bits.ADFM = 0;                    // Configura el resultado del ADC como justificado a la izquierda
    ADCON1bits.VCFG0 = 0;                   // Configura el voltaje de referencia en VSS
    ADCON1bits.VCFG1 = 0;                   // Configura el voltaje de referencia en VDD
    ADCON0bits.ADCS = 0b10;                 // Configura el reloj del ADC
    ADCON0bits.CHS = 0;                     // Configura el canal del ADC
    ADCON0bits.ADON = 1;                    // Habilita el ADC
    __delay_us(50); 
    
    // Configurando el PWM
    // Configuración para CCP1
    TRISCbits.TRISC2 = 1;                   // Configurando el pin RC2 como entrada
    CCP1CONbits.P1M = 0;                    // Selección de modo single output para CCP1
    CCP1CONbits.CCP1M = 0b1100;             // Selección de modo PWM para CCP1

    CCPR1L = 0x0f;                          // Valor inicial para el registro CCPR1L, determina la frecuencia del PWM
    CCP1CONbits.DC1B = 0;                   // Bits menos significativos del valor de la señal PWM

    // Configuración para CCP2
    TRISCbits.TRISC1 = 1;                   // Configurando el pin RC1 como entrada
    CCP2CONbits.CCP2M = 0b1100;             // Selección de modo PWM para CCP2

    CCPR2L = 0x0f;                          // Valor inicial para el registro CCPR2L, determina la frecuencia del PWM       //////////////////////
    CCP2CONbits.DC2B1 = 0;                  // Bits menos significativos del valor de la señal PWM

    // Configurando el TMR2
    PR2 = 249;                              // Valor del registro PR2, determina el ciclo de trabajo del PWM
    PIR1bits.TMR2IF = 0;                    // Bandera de interrupción del Timer2 apagada
    T2CONbits.T2CKPS = 0b11;                // Preescalador para Timer2 a 1:16
    T2CONbits.TMR2ON = 1;                   // Encendiendo Timer2

    while (PIR1bits.TMR2IF == 0);           // Esperando a que Timer2 llegue al final del ciclo
    PIR1bits.TMR2IF = 0;                    // Apagando la bandera de interrupción del Timer2

    TRISCbits.TRISC2 = 0;                   // Configurando el pin RC2 como salida
    TRISCbits.TRISC1 = 0;                   // Configurando el pin RC1 como salida

    // Configuraciones de interrupción
    INTCONbits.GIE = 1;                     // Habilitando interrupciones globales
    INTCONbits.T0IE = 1;                    // Habilitando interrupciones del Timer0
    INTCONbits.T0IF = 0;                    // Apagando la bandera de interrupción del Timer0
    INTCONbits.PEIE = 1;                    // Habilitando interrupciones periféricas
    PIE1bits.ADIE = 1;                      // Habilitando interrupciones del ADC
    PIR1bits.ADIF = 0;                      // Apagando la bandera de interrupción del ADC

    __delay_us(50); 

    return;
}
