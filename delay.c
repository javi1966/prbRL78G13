#include "delay.h"

#define GetInstructionClock() (12000000U)

void delay_10us(int  us)
{
	volatile unsigned long _dcnt;
  

	_dcnt = us *((unsigned long)(0.00001/(1.0/GetInstructionClock())/50));
    //    _dcnt = us  *   ((unsigned long) 12 *(0.00001/( 1.0/GetInstructionClock())));
	//while(_dcnt--){}
  
    while (--_dcnt)
    {
      
    }  
	
	
}

//*****************************************************************
void delay_ms(int ms)
{
    unsigned char i;
    while(ms--)
    {
        i=4;
        while(i--)
        {
            delay_10us(25);
        }
    }
}