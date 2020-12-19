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
void initHw()
{
    // Configure HW to work with 16 MHz XTAL, PLL enabled, system clock of 40 MHz
    SYSCTL_RCC_R = SYSCTL_RCC_XTAL_16MHZ | SYSCTL_RCC_OSCSRC_MAIN | SYSCTL_RCC_USESYSDIV | (4 << SYSCTL_RCC_SYSDIV_S);

    // Set GPIO ports to use APB (not needed since default configuration -- for clarity)
    // Note UART on port A must use APB
    SYSCTL_GPIOHBCTL_R = 0;

    // Enable clocks
    SYSCTL_RCGCGPIO_R = SYSCTL_RCGCGPIO_R5;
    _delay_cycles(3);

    // Configure LED pins
    GPIO_PORTF_DIR_R |= GREEN_LED_MASK | RED_LED_MASK;  // bits 1 and 3 are outputs
    GPIO_PORTF_DR2R_R |= GREEN_LED_MASK | RED_LED_MASK; // set drive strength to 2mA (not needed since default configuration -- for clarity)
    GPIO_PORTF_DEN_R |= GREEN_LED_MASK | RED_LED_MASK;  // enable LEDs
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
                    if((c >= 65 && c <=90) || (c >=97 && c <= 122))
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
                        if((d >= 33 && d <= 47) || (d >= 58 && c <= 64) || (d >= 91 && d <= 96) || d==NULL)
                        {
                            data->fieldPosition[count]=i;
                            data->fieldType[count]=a;
                            count++;
                            data->fieldCount= count;
                        }


                    }
                    else if((c >= 48 && c <= 57))
                    {
                        if((d >= 33 && d <= 47) || ( d>= 58 && c <= 64) || (d >= 91 && d <= 96) || d==NULL)
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
    if (fieldNumber<=sizeof(data->fieldPosition))
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



    //putsUart0(data.buffer);


    //parseFields(&data);
    //getFieldString(&data,1);
    //getFieldInteger(&data,1);
    //getFieldInteger(&data,2);
    //isCommand(&info, "set", 2);
    //isCommand(&info, "alert", 1);

int main(void)
{
    USER_DATA data;
    initHw();
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
    }
while (true);


}



