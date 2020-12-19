// Stop Go C Example (Bitbanding)
// Jesus Adrian Guerra

//-----------------------------------------------------------------------------
// Hardware Target
//-----------------------------------------------------------------------------

// Target Platform: EK-TM4C123GXL Evaluation Board
// Target uC:       TM4C123GH6PM
// System Clock:    40 MHz

// Hardware configuration:
// Red LED:
//   PF1 drives an NPN transistor that powers the red LED
// Green LED:
//   PF3 drives an NPN transistor that powers the green LED
// Pushbutton:
//   SW1 pulls pin PF4 low (internal pull-up is used)

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdbool.h>
#include "tm4c123gh6pm.h"

// Bitband aliases
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 1*4)))
#define YELLOW_LED   (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 2*4)))
#define BLUE_LED     (*((volatile uint32_t *)(0x42000000 + (0x400243FC-0x40000000)*32 + 3*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400073FC-0x40000000)*32 + 2*4)))
#define PUSH_BUTTON  (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 1*4)))
#define PUSH_BUTTON2 (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 2*4)))

// PortF masks
#define GREEN_LED_MASK 4
#define BLUE_LED_MASK 8
#define YELLOW_LED_MASK 4
#define RED_LED_MASK 2
#define PUSH_BUTTON_MASK 2
#define PUSH_BUTTON2_MASK 4


//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Blocking function that returns only when SW1 is pressed
void waitPbPress()
{
	while(PUSH_BUTTON);
}
void waitPb2Press()
{
    while(!PUSH_BUTTON2);
}

void waitMicrosecond(uint32_t us)
{
    __asm("WMS_LOOP0:   MOV  R1, #6");          // 1
    __asm("WMS_LOOP1:   SUB  R1, #1");          // 6
    __asm("             CBZ  R1, WMS_DONE1");   // 5+1*3
    __asm("             NOP");                  // 5
    __asm("             NOP");                  // 5
    __asm("             B    WMS_LOOP1");       // 5*2 (speculative, so P=1)
    __asm("WMS_DONE1:   SUB  R0, #1");          // 1
    __asm("             CBZ  R0, WMS_DONE0");   // 1
    __asm("             NOP");                  // 1
    __asm("             B    WMS_LOOP0");       // 1*2 (speculative, so P=1)
    __asm("WMS_DONE0:");                        // ---
                                                // 40 clocks/us + error
}

// Initialize Hardware
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, sysdivider of 5, creating system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R4 | SYSCTL_RCGCGPIO_R3 | SYSCTL_RCGCGPIO_R1;
    _delay_cycles(3);

    // Configure LED and pushbutton pins
    GPIO_PORTE_DIR_R |= YELLOW_LED_MASK | RED_LED_MASK | BLUE_LED_MASK;// bits 1 and 3 are outputs, other pins are inputs
    GPIO_PORTD_DIR_R |=GREEN_LED_MASK;
    GPIO_PORTB_DIR_R &= ~PUSH_BUTTON_MASK;// bit 4 is an input
    GPIO_PORTB_DIR_R &= ~PUSH_BUTTON2_MASK;
    GPIO_PORTD_DR2R_R |= GREEN_LED_MASK;// set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTE_DR2R_R |= YELLOW_LED_MASK | RED_LED_MASK | BLUE_LED_MASK;
    GPIO_PORTE_DEN_R |= YELLOW_LED_MASK | RED_LED_MASK | BLUE_LED_MASK;
    GPIO_PORTD_DEN_R |= GREEN_LED_MASK;
    GPIO_PORTB_DEN_R |= PUSH_BUTTON_MASK | PUSH_BUTTON2_MASK;
                                                         // enable LEDs and pushbuttons
    GPIO_PORTB_PUR_R |= PUSH_BUTTON_MASK ;   // enable internal pull-up for push button
    GPIO_PORTB_PDR_R |= PUSH_BUTTON2_MASK;
}

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

int main(void)
{
	// Initialize hardware
	initHw();

    // Turn off green LED, turn on red LED
    GREEN_LED = 1;
    RED_LED = 0;
    YELLOW_LED = 1;
    BLUE_LED = 0;

    RED_LED = 1;

    // Wait for PB press
    waitPb2Press();

    // Turn off red LED, turn on green LED
    RED_LED = 0;
    GREEN_LED = 0;

    waitMicrosecond(1000000);
    BLUE_LED = 1;

    waitPbPress();
    waitMicrosecond(500000);

    // Endless loop
    while(true)
    {
        waitMicrosecond(500000);
        YELLOW_LED = 1;
        waitMicrosecond(500000);
        YELLOW_LED = 0;
    }

}
