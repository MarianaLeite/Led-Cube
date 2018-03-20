// Alunas: Camila Rodrigues      201511950277
//         Mariana Leite         201511950099
// Orientadores: Renato Zanetti e Tálita Sono

#include <p24FJ256GA106.h>
#include "user.h"

//Indicacao do clock utilizado pelo uC para o compilador
// Verificar o uso do define _BOOTLOADER na user.h
#ifndef _BOOTLOADER  
// CONFIG2
#pragma config POSCMOD = XT   // XT Oscillator mode selected
#pragma config OSCIOFNC = ON  // OSC2/CLKO/RC15 as as port I/O (RC15)
#pragma config FCKSM = CSDCMD // Clock Switching and Monitor disabled
#pragma config FNOSC = PRI    // Primary Oscillator (XT, HS, EC)
#pragma config IESO = ON      // Int Ext Switch Over Mode enabled

// CONFIG1
#pragma config WDTPS = PS32768 // Watchdog Timer Postscaler (1:32,768)
#pragma config FWPSA = PR128   // WDT Prescaler (1:128)
#pragma config WINDIS = ON     // Watchdog Timer Window Mode disabled
#pragma config FWDTEN = OFF    // Watchdog Timer disabled
#pragma config ICS = PGx2      // Emulator/debugger uses EMUC2/EMUD2
#pragma config GWRP = OFF      // Writes to program memory allowed
#pragma config GCP = OFF       // Code protection is disabled
#pragma config JTAGEN = OFF    // JTAG port is disabled

#endif

// Divisão da quantidade de bits (1023) pelo número de LEDS (9) que corresponde
// à faixa de conversão mínima para variação no modo proporcional
#define CONVERSAO_BASE 114

#define POT (BUFFER1_POT+BUFFER2_POT)/2

#define TEMP_RN TEMPERATURA_RN((BUFFER1_SENSOR1+BUFFER2_SENSOR1)/2)

#define TEMP_AMB TEMPERATURA_AMB((BUFFER1_SENSOR2+BUFFER2_SENSOR2)/2)

int TEMPERATURA_RN (int VALOR_CONVERSAO_RN);

int TEMPERATURA_AMB (int VALOR_CONVERSAO_AMB);

int MODULO (int NUMERO);

int BUFFER1_SENSOR1, BUFFER2_SENSOR1, BUFFER1_SENSOR2, BUFFER2_SENSOR2, BUFFER1_POT, BUFFER2_POT;
int VOUT_RN, TEMP1, VOUT_AMB, TEMP2;

 // Funcao prinicipal
int main(void) {
    
    // Entradas e saídas
    AD1PCFG = 0x1FFF;
    TRISB = 0xE000;
    TRISD = 0;
    TRISEbits.TRISE6 = 1;
    TRISEbits.TRISE7 = 1;
    AD1CON2bits.VCFG = 0;       // Vdd | Vss
    AD1CHSbits.CH0NA = 0;       // Vr- como referencia negativa do MUX A
    
    // Dados ADC
    AD1CON1bits.FORM = 0;       // Dados no formato inteiro
    AD1CON2bits.BUFM = 1;       // 8 word buffer
    AD1CON2bits.SMPI = 5;       // Preenche 6 buffers
    
    // Operação ADC
    AD1CON1bits.SSRC = 1;       // Conversão disparada pela INT0
    AD1CON1bits.ASAM = 1;       // Amostragem automática
    AD1CON2bits.CSCNA = 1;      // Habilita scanning
    AD1CSSL = 0xE000;           // AN13, AN14 e AN15 serão escaneados
    AD1CON2bits.ALTS = 0;       // Usa apenas MUX A
   
    // Interrupção
    INTCON1bits.NSTDIS = 0;     // Aninhamento habilitado
    INTCON2bits.ALTIVT = 0;     // Usa tabela do vetor padrão
    INTCON2bits.INT0EP = 0;     // Gera interrupção na borda de subida
    IPC3bits.AD1IP = 7;         // Prioridade máxima na interrupção no ADC
    IFS0bits.AD1IF = 0;
    IFS0bits.INT0IF = 0;
    
    // Enable
    AD1CON1bits.ADON = 1;
    IEC0bits.AD1IE = 1;
    
    while (1) // Loop principal
    {
        // Modo proporcional
        if (PORTEbits.RE6 == 0 && PORTEbits.RE7 == 0) {
            if (POT == 0) { 
                LATB = 0; 
                LATD = 0;
            }
            else if (POT != 0 && POT <= CONVERSAO_BASE) { 
                LATB = 0x0001;
                LATD = 0x0006;
            }
            else if (POT > CONVERSAO_BASE && POT <= 2*CONVERSAO_BASE) { 
                LATB = 0x0001;
                LATD = 0x0004;
            }
            else if (POT > 2*CONVERSAO_BASE && POT <= 3*CONVERSAO_BASE) { 
                LATB = 0x0001;
                LATD = 0x0000;
            }
            else if (POT > 3*CONVERSAO_BASE && POT <= 4*CONVERSAO_BASE) {
                LATB = 0x0003;
                LATD = 0x0030;
            }
            else if (POT > 4*CONVERSAO_BASE && POT <= 5*CONVERSAO_BASE) {
                LATB = 0x0003;
                LATD = 0x0020;
            }
            else if (POT > 5*CONVERSAO_BASE && POT <= 6*CONVERSAO_BASE) {
                LATB = 0x0003;
                LATD = 0x0000;
            }
            else if (POT > 6*CONVERSAO_BASE && POT <= 7*CONVERSAO_BASE) {
                LATB = 0x0007;
                LATD = 0x0180;
            }
            else if (POT > 7*CONVERSAO_BASE && POT <= 8*CONVERSAO_BASE) {
                LATB = 0x0007;
                LATD = 0x0100;
            }
            else if (POT > 8*CONVERSAO_BASE && POT <= 9*CONVERSAO_BASE) {
                LATB = 0x0007;
                LATD = 0x0000;
            }
        }
        // Modo binário
        else if (PORTEbits.RE6 == 1 && PORTEbits.RE7 == 0) {
            LATB = 0x0007;
            LATD = POT^0xFFFF;
        }
        // Modo monitor simples
        else if (PORTEbits.RE6 == 0 && PORTEbits.RE7 == 1) {
            LATD = 0;
            if ((TEMP_RN >= 365) && (TEMP_RN <= 370)) {
                LATB = 0x0038;
            }
            else if ((TEMP_RN >= 350 && TEMP_RN < 365) || (TEMP_RN > 370 && TEMP_RN <= 380)){
                __delay_ms(100);
                LATB = 0x01C0;
                __delay_ms(100);
                LATB = 0;
            }
            else if ((TEMP_RN  < 350) || (TEMP_RN > 380)) {
                __delay_ms(100);
                LATB = 0x0007;
                __delay_ms(100);
                LATB = 0;
            }
        }
        // Modo monitor completo
        else if (PORTEbits.RE6 == 1 && PORTEbits.RE7 == 1) {
            LATD = 0; 
            if ((TEMP_RN >= 365) && (TEMP_RN <= 370)) {
                LATB = 0x0038;
                if (MODULO(TEMP_AMB-TEMP_RN) > 180 || MODULO(TEMP_AMB-TEMP_RN) < 80){
                    LATB = 0xFFFF;
                    __delay_ms(100);
                    LATB = 0;
                    __delay_ms(100);
                }
            }
            else if ((TEMP_RN >= 350 && TEMP_RN < 365) || (TEMP_RN > 370 && TEMP_RN <= 380)){
                __delay_ms(100);
                LATB = 0x01C0;
                __delay_ms(100);
                LATB = 0;
            }
            else if ((TEMP_RN  < 350) || (TEMP_RN > 380)) {
                __delay_ms(100);
                LATB = 0x0007;
                __delay_ms(100);
                LATB = 0;
            }
        }
    }

    return -1;
}

void __attribute__((interrupt,no_auto_psv)) _ADC1Interrupt(void){
    if(IFS0bits.AD1IF){   
        if (AD1CON2bits.BUFS == 1) {
            BUFFER1_SENSOR1 = ADC1BUF0;
            BUFFER1_SENSOR2 = ADC1BUF1;
            BUFFER1_POT = ADC1BUF2;
            BUFFER2_SENSOR1 = ADC1BUF3;
            BUFFER2_SENSOR2 = ADC1BUF4;
            BUFFER2_POT = ADC1BUF5;
        }
        else {
            BUFFER1_SENSOR1 = ADC1BUF8;
            BUFFER1_SENSOR2 = ADC1BUF9;
            BUFFER1_POT = ADC1BUFA;
            BUFFER2_SENSOR1 = ADC1BUFB;
            BUFFER2_SENSOR2 = ADC1BUFC;
            BUFFER2_POT = ADC1BUFD;
        }
        IFS0bits.AD1IF = 0;
    }
}

int TEMPERATURA_RN (int VALOR_CONVERSAO_RN) {
    // Calcula a tensão no pino e multiplica por 100 a fim de contemplar duas casas decimais
    VOUT_RN = VALOR_CONVERSAO_RN*(3.3/1023.0)*100;
    // Calcula a temperatura de acordo com a equação do datasheet (multiplicada por 100) 
    // e multiplica por 10 a fim de contemplar 1 cada decimal
    TEMP1 = (VOUT_RN-50)*10;
    return TEMP1;
}

int TEMPERATURA_AMB (int VALOR_CONVERSAO_AMB) {
    // Calcula a tensão no pino e multiplica por 100 a fim de contemplar duas casas decimais
    VOUT_AMB = VALOR_CONVERSAO_AMB*(3.3/1023.0)*100;
    // Calcula a temperatura de acordo com a equação do datasheet (multiplicada por 100) 
    // e multiplica por 10 a fim de contemplar 1 cada decimal
    TEMP2 = (VOUT_AMB-50)*10;
    return TEMP2;
}

int MODULO (int NUMERO) {
    if (NUMERO < 0) {
        NUMERO *= -1;
    }
    return NUMERO;
}