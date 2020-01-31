/*
 * main.c
 */

#include <stdio.h>          //C:\ti\ccsv6\tools\compiler\ti-cgt-arm_15.12.1.LTS\include
#include <tm4c123gh6pm.h>   //C:\ti\TivaWare_C_Series-2.1.4.178\inc
#include <stdint.h>         //C:\ti\ccsv6\tools\compiler\ti-cgt-arm_15.12.1.LTS\include
#include "../BSP.h"         //C:\Users\Leandro\workspace_v6_1_3
//Aula 8 (04/12/19)
#define TARGET_IS_BLIZZARD_RB1  //Habilita as funções em ROM do TM4C123x
//#define TARGET_IS_TM4C123_RB1
//#define UART_BUFFERED
#include <stdbool.h>        //C:\ti\ccsv6\tools\compiler\ti-cgt-arm_15.12.1.LTS\include
#include "utils/uartstdio.h"//Prototypes for the UART console functions (vide notas a seguir)
#include "driverlib/pin_map.h"//Mapping of peripherals to pins for all parts
#include "inc/hw_memmap.h"  //Macros defining the memory map of the device
#include "driverlib/gpio.h" //Defines and Macros for GPIO API
#include "driverlib/sysctl.h"//Prototypes for the system control driver
#include "driverlib/uart.h" //Defines and Macros for the UART
#include "driverlib/rom.h"  //Macros to facilitate calling functions in the ROM

//Protótipos de funções
void UART0_Inicializa(void);
void configuraUART(void);
unsigned char UART0_RxChar(void);
void UART0_TxChar(unsigned char data);
void SysTick_Init(void);
void SysTick_Wait(unsigned long delay);
void SysTick_Wait10ms(unsigned long delay);
void SysTick_Init_Int(uint32_t ms);

//Função principal
int main(void)
{
    //Variáveis locais
    uint16_t X, Y, produto = 2, i;
    uint8_t Select;
    int32_t coin = 0;
    int32_t valores[4] = {5, 4, 4, 3};
    //configuraUART();
    BSP_LCD_Init();
    BSP_Joystick_Init();
    BSP_Button1_Init();
    BSP_Button2_Init();
    SysTick_Init();
    BSP_RGB_D_Init(0, 0, 0);

    BSP_LCD_FillScreen(LCD_GREY);
    BSP_LCD_DrawString2(2, 2, " Veding Machine ", LCD_BLACK, LCD_YELLOW);
    BSP_LCD_DrawString2(2, 4, "  (18/12/2019)  ", LCD_BLACK, LCD_YELLOW);
    SysTick_Wait10ms(200);

    while(1)
    {
        coin = 0;
        BSP_LCD_FillScreen(LCD_YELLOW);
        BSP_LCD_DrawString2(0, 0, "Saldo: R$", LCD_BLACK, LCD_YELLOW);
        BSP_LCD_DrawString2(10, 0, "   0,00", LCD_YELLOW, LCD_BLACK);

        BSP_LCD_DrawString2(0, 2, "Produtos:", LCD_BLACK, LCD_YELLOW);
        BSP_LCD_DrawString2(1, 3, "Coca    R$5,00", LCD_BLACK, LCD_YELLOW);
        BSP_LCD_DrawString2(1, 4, "Fanta   R$4,00", LCD_BLACK, LCD_YELLOW);
        BSP_LCD_DrawString2(1, 5, "Guarana R$4,00", LCD_BLACK, LCD_YELLOW);
        BSP_LCD_DrawString2(1, 6, "Coroti  R$3,00", LCD_BLACK, LCD_YELLOW);

        do
        {
            if(BSP_Button1_Input()==0)
            {
                coin++;
                if(coin>1000) coin = 1000;

                BSP_LCD_SetCursor(10, 0); BSP_LCD_OutUDec4(coin, LCD_YELLOW);
                SysTick_Wait10ms(10);
            }

            if(BSP_Button2_Input()==0)
            {
                coin--;
                if(coin<0) coin = 0;

                BSP_LCD_SetCursor(10, 0); BSP_LCD_OutUDec4(coin, LCD_YELLOW);
                SysTick_Wait10ms(10);
            }

            BSP_Joystick_Input(&X, &Y, &Select);
        }
        while(Y > 200);

        while(Y < 400) BSP_Joystick_Input(&X, &Y, &Select); //Espera voltar para posição central

        produto = 3;
        BSP_LCD_DrawString2(0, produto, ">", LCD_BLACK, LCD_YELLOW);

        while(Select > 0)
        {
            BSP_Joystick_Input(&X, &Y, &Select);
            if(Y < 200)
            {
                produto++;
                if(produto > 6) produto = 3;

                if(produto == 3)
                    BSP_LCD_DrawString2(0, 6, " ", LCD_BLACK, LCD_YELLOW);  //Apaga anterior
                else
                    BSP_LCD_DrawString2(0, produto-1, " ", LCD_BLACK, LCD_YELLOW);  //Apaga anterior

                BSP_LCD_DrawString2(0, produto, ">", LCD_BLACK, LCD_YELLOW);    //Seleciona atual

                while(Y < 400) BSP_Joystick_Input(&X, &Y, &Select); //Espera voltar para posição central
            }
        }

        if(coin >= valores[produto-3])
        {
            coin = coin - valores[produto-3];

            BSP_LCD_DrawString2(0, 8, "Venda efetuada!", LCD_YELLOW, LCD_BLACK);
            BSP_RGB_D_Set(0, 1, 0);
        }
        else
        {
            BSP_LCD_DrawString2(0, 8, "Saldo insuficiente", LCD_YELLOW, LCD_BLACK);
            BSP_RGB_D_Set(1, 0, 0);
        }

        SysTick_Wait10ms(100);

        BSP_LCD_DrawString2(0, 10, "Troco: R$     ,00", LCD_YELLOW, LCD_BLACK);
        BSP_LCD_SetCursor(10, 10); BSP_LCD_OutUDec4(coin, LCD_YELLOW);

        for(i=0; i<coin; i++)
        {
            BSP_RGB_D_Toggle(0, 0, 1);
            SysTick_Wait10ms(30);
            BSP_RGB_D_Toggle(0, 0, 1);
            SysTick_Wait10ms(30);
        }

        BSP_RGB_D_Set(0, 0, 0);
        //Fim
    }


    return 0;
}

//Funções auxiliares

// Inicializa a UART com baud rate de 115.200 (assumindo 16 MHz de clock da UART)
void UART0_Inicializa(void)
{
    SYSCTL_RCGCUART_R |= 0x01;      // ativa UART0
    SYSCTL_RCGCGPIO_R |= 0x01;      // ativa port A
    UART0_CTL_R &= ~0x0001;         // desabilita UART
    UART0_IBRD_R = 8;               // IBRD = int(16,000,000 / (16 * 115.200)) = int(8,6805)
    UART0_FBRD_R = 44;              // FBRD = int(0,6805 * 64 + 0.5) = 44
    UART0_LCRH_R = 0x0070;          // 8 bits de dados, sem paridade, um stop bit, FIFOs hab.
    UART0_CTL_R |=  0x0301;         // habilita UART
    GPIO_PORTA_AMSEL_R &= ~0x03;    // desabilita analog  function em PA1-0
    GPIO_PORTA_AFSEL_R |= 0x03;     // habilita alternative function em PA1-0
    GPIO_PORTA_DEN_R |= 0x03;       // habilita digital I/O em PA1-0
    GPIO_PORTA_PCTL_R = (GPIO_PORTA_PCTL_R & 0xFFFFFF00) + 0x00000011; // conf. PA1-0 como UART
}
//Função para configuração da UART0 usando TivaWare
void configuraUART(void)
{
    /*
    //Usando funções em FLASH

    //
    // Enable the GPIO Peripheral used by the UART.
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    GPIOPinConfigure(GPIO_PA0_U0RX);
    GPIOPinConfigure(GPIO_PA1_U0TX);
    GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);

    /*///*
    //Usando funções em ROM

    //
    // Enable the GPIO Peripheral used by the UART.
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);

    //
    // Enable UART0
    //
    ROM_SysCtlPeripheralEnable(SYSCTL_PERIPH_UART0);

    //
    // Configure GPIO Pins for UART mode.
    //
    ROM_GPIOPinConfigure(GPIO_PA0_U0RX);
    ROM_GPIOPinConfigure(GPIO_PA1_U0TX);
    ROM_GPIOPinTypeUART(GPIO_PORTA_BASE, GPIO_PIN_0 | GPIO_PIN_1);

    //
    // Use the internal 16MHz oscillator as the UART clock source.
    //
    ROM_UARTClockSourceSet(UART0_BASE, UART_CLOCK_PIOSC);

    //
    // Initialize the UART for console I/O.
    //
    UARTStdioConfig(0, 115200, 16000000);
    //*/
}
// Aguarda um dado na porta serial
// Saída: código ASCII
unsigned char UART0_RxChar(void)
{
    while((UART0_FR_R & 0x0010) != 0);      // aguarde enquanto RXFE = 1
    return ((unsigned char)(UART0_DR_R & 0xFF));
}
// Envia um dado pela porta serial
// Entrada: caractere de 8 bits ASCII
void UART0_TxChar(unsigned char data)
{
    while((UART0_FR_R & 0x0020) != 0);      // aguarde enquanto TXFF = 1
    UART0_DR_R = data;
}
// Inicialização do SysTick com busy wait running usando o clock do barramento
void SysTick_Init(void)
{
    NVIC_ST_CTRL_R = 0;             // 1) Desabilita SysTick durante o setup
    NVIC_ST_RELOAD_R = 0x00FFFFFF;  // 2) Valor máximo de recarga
    NVIC_ST_CURRENT_R  = 0;         // 3) Escreve qualquer valor para limpar o atual
    NVIC_ST_CTRL_R = 0x00000005;    // 4) Habilita SysTick com clock do sistema
}
// Delay de tempo usando modo busy wait
// O parâmetro de delay é a unidade de clock do sistema
//(unidade de xx ns para clock de 16 MHz)
void SysTick_Wait(unsigned long delay)
{
     volatile unsigned long elapsedTime;
     unsigned long startTime = NVIC_ST_CURRENT_R;
     do
     {
         elapsedTime = (startTime - NVIC_ST_CURRENT_R) & 0x00FFFFFF;
      }
      while(elapsedTime <= delay);
}
// Supondo clock do sistema de 16 MHz
void SysTick_Wait10ms(unsigned long delay)
{
     unsigned long i;
     for(i=0; i<delay; i++)
     {
         SysTick_Wait(160000);  // Espera 10ms
     }
}
// Inicialização do SysTick com busy wait running usando o clock do barramento
void SysTick_Init_Int(uint32_t ms)
{
    uint32_t pulsos = 16000*ms - 1;
    NVIC_ST_CTRL_R = 0;             // 1) Desabilita SysTick durante o setup
    NVIC_ST_RELOAD_R = pulsos;      // 2) Valor máximo de recarga
    NVIC_ST_CURRENT_R  = 0;         // 3) Escreve qualquer valor para limpar o atual
    NVIC_ST_CTRL_R = 0x00000007;    // 4) Habilita SysTick com clock do sistema
}
//Tratamento da interrupção do SysTick
void SysTickHandler(void)
{
    static uint32_t ms = 0;

    ms++;

    //Teste SysTick
    if(ms == 100)
    {
        UART0_TxChar('#');
    }
    if(ms == 200)
    {
        UART0_TxChar('_');
        ms = 0;
    }
}
