// Serial Example
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
// UART Interface:
//   U0TX (PA1) and U0RX (PA0) are connected to the 2nd controller
//   The USB on the 2nd controller enumerates to an ICDI interface and a virtual COM port
//   Configured to 115,200 baud, 8N1

//-----------------------------------------------------------------------------
// Device includes, defines, and assembler directives
//-----------------------------------------------------------------------------

#include <stdint.h>
#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "tm4c123gh6pm.h"
#include "uart0.h"

// Bitband aliases
#define RED_LED      (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 1*4)))
#define GREEN_LED    (*((volatile uint32_t *)(0x42000000 + (0x400253FC-0x40000000)*32 + 3*4)))

// PortF masks
#define GREEN_LED_MASK 8
#define RED_LED_MASK 2
#define PCSEVEN 128
#define DEINT 1
#define DEI (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 0*4)))



#define MAX_CHARS 80
#define MAX_FIELDS 5
typedef struct _USER_DATA {
    char buffer[MAX_CHARS+1];
    uint8_t fieldCount;
    uint8_t fieldPosition[MAX_FIELDS];
    char fieldType[MAX_FIELDS];
}
USER_DATA;
//-----------------------------------------------------------------------------
// Subroutines
//-----------------------------------------------------------------------------

// Initialize Hardware

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
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    // Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R = 0;

    //#define PCSEVEN 128
   // #define UART_TX_MASK 268435456
    //GPIO_PORTC_DIR_R &= ~UART_RX_MASK;
       //GPIO_PORTC_DEN_R &= ~UART_TX_MASK | UART_RX_MASK;
       //GPIO_PORTC_AFSEL_R |= UART_TX_MASK | UART_RX_MASK;
       //SYSCTL_GPIOAMSEL_R = 1;
    SYSCTL_RCGCACMP_R = 1;
    SYSCTL_RCGCTIMER_R |= SYSCTL_RCGCTIMER_R1;
    SYSCTL_RCGCGPIO_R |= SYSCTL_RCGCGPIO_R2 | SYSCTL_RCGCGPIO_R1| SYSCTL_RCGCGPIO_R4;
    _delay_cycles(3);
    GPIO_PORTC_DIR_R &= ~PCSEVEN;
    GPIO_PORTC_DEN_R &= ~PCSEVEN;
    COMP_ACREFCTL_R = 0x020F;
    COMP_ACCTL0_R = 0x040C;
    waitMicrosecond(10);

    //#define DEINT 1
    //#define DEI (*((volatile uint32_t *)(0x42000000 + (0x400053FC-0x40000000)*32 + 0*4)))
    GPIO_PORTB_DIR_R |= DEINT;
    GPIO_PORTB_DEN_R |= DEINT;
    //while(1){
    DEI=1;
    //}
    //while(1){
    DEI=0;
    //}

    //SYSCTL_RCGCGPIO_R4;
    //_delay_cycles(3);
    GPIO_PORTE_DIR_R &= ~1;
    GPIO_PORTE_DEN_R &= ~1;

    GPIO_PORTE_DIR_R &= ~2;
    GPIO_PORTE_DEN_R &= ~2;

    GPIO_PORTE_DIR_R &= ~4;
    GPIO_PORTE_DEN_R &= ~4;

    GPIO_PORTE_AMSEL_R|= 1| 2| 4;











    // Enable clocks
    //SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R5;
    //_delay_cycles(3);

    // Configure LED pins
    //GPIO_PORTF_DIR_R |= GREEN_LED_MASK | RED_LED_MASK;  // bits 1 and 3 are outputs
    //GPIO_PORTF_DR2R_R |= GREEN_LED_MASK | RED_LED_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
    //GPIO_PORTF_DEN_R |= GREEN_LED_MASK | RED_LED_MASK;  // enable LEDs
}


//volume part
    uint32_t getVolume()
    {
        TIMER1_CTL_R &= ~TIMER_CTL_TAEN;
        TIMER1_CFG_R = TIMER_CFG_32_BIT_TIMER;
        TIMER1_TAMR_R =TIMER1_TAMR_R =TIMER_TAMR_TACDIR | 0x2;//edit for later
        TIMER1_TAILR_R = 40000000;
    DEI=1;
    while(COMP_ACSTAT0_R!=2)
        {
        //DEI=1;
        //COMP_ACSTAT0_R;
        }
    DEI=0;
    TIMER1_TAV_R =0x0;
    TIMER1_CTL_R |= TIMER_CTL_TAEN;

    while(COMP_ACSTAT0_R!=0)
    {
        //DEI=0;
        //COMP_ACSTAT0_R;
    }
    TIMER1_CTL_R &= ~TIMER_CTL_TAEN;
    //return TIMER1_TAV_R;
    int y=TIMER1_TAV_R;

    float x= ((.008491848*y)-2.512975543);
    //return x;
    float v= 3.976*x;
    //return v;
    float v2= (v/61.024)*1000;
    return v2;
    }

    float getLightPercentage(){
        float p;//uint16_t
        float pp;
        setAdc0Ss3Mux(3);
        //setAdc0Ss3Log2AverageCount(2);
        p= readAdc0Ss3();
        pp=(p/7);
        return pp;

    }

    float getMoisturePercentage(){
        float mp;//uint16_t
        float mpp;
        setAdc0Ss3Mux(2);
        //setAdc0Ss3Log2AverageCount(2);
        mp= readAdc0Ss3();
        mpp=((mp-3062)/(-14.74));
        return mpp;
    }

    double getBatteryVoltage(){
        double ba;//uint16_t
        double bat;
        setAdc0Ss3Mux(1);
        //setAdc0Ss3Log2AverageCount(2);
        ba= readAdc0Ss3();
        bat=((ba-287)/(890));//935.5
        return bat;
    }

//-----------------------------------------------------------------------------
// Main
//-----------------------------------------------------------------------------

getsUart0(USER_DATA* data)
{
char c;
int count=0;
while(1)
{
    c=getcUart0();
    if ((c==(8)||(c==127))&&(count>0))
        {
        count--;
        continue;
        }
        if (c==(10)||c==(13))
        {
        data->buffer[count]=0;
        break;
        }
        if (c>=32)
        {
         data->buffer[count++]=c;
         if (count==MAX_CHARS)
                 {
                    data->buffer[count]=0;
                    break;
                 }
         else
         {
             continue;
         }
        }
        else
        {
            continue;
        }
        }

}
parseFields(USER_DATA* data)
{
    //int ascii=c;

       char a='a';
       char n='n';
       int i=0;
       int count=0;
       for(i=0; data->buffer[i]!=NULL; i++)
       {
          char c= data->buffer[i];
          char d= data->buffer[i-1];
          if(i==0)
                {
                    if(c >= 65 && c <=90)
                     {

                         data->fieldPosition[count]=i;
                         data->fieldType[count]=a;
                         count++;
                         data->fieldCount= count;
                     }
                    if(c >=97 && c <= 122)
                     {

                         data->fieldPosition[count]=i;
                         data->fieldType[count]=a;
                         count++;
                         data->fieldCount= count;
                     }
                    else if((c >= 48 && c <= 57))
                    {
                        data->fieldPosition[count]=i;
                        data->fieldType[count]=n;
                        count++;
                        data->fieldCount= count;
                    }
                    else
                    {
                        data->buffer[i]=NULL;
                    }

                }
                else
                {
                    if((c >= 65 && c <=90) || (c >=97 && c <= 122))
                    {
                        if(d >= 33 && d <= 47)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=a;
                            count++;
                            data->fieldCount= count;
                        }
                        if(d >= 58 && c <= 64)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=a;
                            count++;
                            data->fieldCount= count;
                        }
                        if(d >= 91 && d <= 96)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=a;
                            count++;
                            data->fieldCount= count;
                        }
                        if(d==NULL)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=a;
                            count++;
                            data->fieldCount= count;
                        }


                    }
                    else if((c >= 48 && c <= 57))
                    {
                        if(d >= 33 && d <= 47)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=n;
                            count++;
                            data->fieldCount= count;
                        }
                        if(d >= 58 && c <= 64)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=n;
                            count++;
                            data->fieldCount= count;
                        }
                        if(d >= 91 && d <= 96)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=n;
                            count++;
                            data->fieldCount= count;
                        }
                        if(d==NULL)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=n;
                            count++;
                            data->fieldCount= count;
                        }


                    }
                    else
                    {
                       data->buffer[i]=NULL;
                    }
               }
        }
}
char* getFieldString(USER_DATA* data, uint8_t fieldNumber)
{
    int i=data->fieldPosition[fieldNumber-1];
    int n=0;
    char m[5];
    if (fieldNumber<sizeof(data->buffer)/sizeof(data->buffer[0]))
    {
        while(data->buffer[i]!=NULL)
        {
            m[n]=data->buffer[i];
            i++;
            n++;
        }
    return m;
    }
    else
    {
        return NULL;
    }
}

int32_t getFieldInteger(USER_DATA* data, uint8_t fieldNumber)
{
    int l=data->fieldPosition[fieldNumber];
       int k=0;
       char q[5];
       if (fieldNumber<=sizeof(data->fieldPosition))
       {
           while(data->buffer[l]!=NULL)
           {
               q[k]=data->buffer[l];
               l++;
               k++;
           }
           int O=atoi(q);//function for converting string to integer from https://www.ibm.com/support/knowledgecenter/ssw_ibm_i_72/rtref/itoi.htm
       return O;
       }
       else
       {
           return NULL;
       }
}
bool isCommand(USER_DATA* data, const char strCommand[], uint8_t minArguments)
{
        int i=data->fieldPosition[0];
        int n=0;
        char m[5];
        bool a= false;
        int w=sizeof(strCommand);
        int x;
        if ((data->fieldCount-1)>=minArguments)
        {
            while(data->buffer[i]!=NULL)
            {
                m[n]=data->buffer[i];
                i++;
                n++;
            }
            for (x = 0; x < w-1; x++)
            {
                if (m[x] != strCommand[x])
                {
                    a=false;
                    break;
                }
                else
                {
                    a=true;
                }
            }
        }
        else
        {
            a=false;
        }
        return a;
}





int main(void)
{
    USER_DATA data;
    initHw();
    initAdc0Ss3();
    getVolume();
    getLightPercentage();//change position later
    getMoisturePercentage();
    getBatteryVoltage();
    //int f=getVolume();
    //char f2[255];
    //sprintf(f2, "Volume in ml is %d",f);
    initUart0();

    while(1)
    {

    getsUart0 (&data);
    putsUart0(data.buffer);
    putsUart0("\n\r");
    bool valid=false;

    // Parse fields
     parseFields(&data);
     // Echo back the parsed field information (type and fields)
     //#ifdef DEBUG
     uint8_t i;
    for (i = 0; i < data.fieldCount; i++)
     { putcUart0(data.fieldType[i]);
     putsUart0("\t");
     putsUart0(&data.buffer[data.fieldPosition[i]]);
     putsUart0("\n");
     putsUart0("\r");
     }
     //#endif
     // Command evaluationbool valid = false;
     // set add, data  add and data are integers
     if (isCommand(&data, "set", 2))
     {
     int32_t add = getFieldInteger(&data, 1);
     int32_t dat = getFieldInteger(&data, 2);
     valid = true;
     // do something with this information
     }
     if (isCommand(&data, "status", 0))
         {
         int f=getVolume();
         int ppp=getLightPercentage();
         int mppp=getMoisturePercentage();
         int battery=getBatteryVoltage();

            char f2[255];
            char ppp2[255];
            char mppp2[255];
            char battery2[255];
            sprintf(f2, "Volume in ml is %d",f);
         putsUart0(f2);
         putsUart0("\n");
         putsUart0("\r");
         sprintf(ppp2, "Light percentage is %d",ppp);
         putsUart0(ppp2);
         putsUart0("\n");
         putsUart0("\r");
         sprintf(mppp2, "Moisture percentage is %d",mppp);
         putsUart0(mppp2);
         putsUart0("\n");
         putsUart0("\r");
         sprintf(battery2, "Battery voltage is %d",battery);
         putsUart0(battery2);

         valid = true;
        putsUart0("\n");
         putsUart0("\r");
         }
     // alert ON|OFF  alert ON or alert OFF are the expected commands
     if (isCommand(&data, "alert", 1))
     {
    char* str = getFieldString(&data, 1);
     valid = true;
     // process the string with your custom strcmp instruction, then do something
     }
     // Process other commands here
     // Look for error
     if (!valid)
     putsUart0("Invalid command\n");
     putsUart0("\n");
     putsUart0("\r");
    }

}



