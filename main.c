/**************************************************
 *
 * This is a template file.
 *
 * Copyright 2011 IAR Systems AB.
 *
* $Revision: 205 $
 *
 **************************************************/
//***************************************************************************

#include "ior5f100le.h"
#include "ior5f100le_ext.h"
#include "intrinsics.h"
#include "delay.h"
#include "lcd.h"
#include "IR.h"
#include "RTC.h"



// ****** defines's ********************

#define DI     __disable_interrupt
#define EI     __enable_interrupt
#define HALT    __halt
#define NOP     __no_operation
#define STOP    __stop

#define LED_VERDE1_ON       P5 &= ~(1<<2); 
#define LED_VERDE1_OFF      P5 |= (1<<2); 
#define LED_VERDE3_ON       P5 &= ~(1<<3);
#define LED_VERDE3_OFF      P5 |= (1<<3); 



#define GetSystemClock()  (12000000ul)
#define GetInstructionClock() GetSystemClock()





//*********** pragma's *****************

/* Start user code for global. Do not edit comment generated here */
#pragma location = "OPTBYTE"
__root const unsigned char opbyte0 = 0xEFU;
#pragma location = "OPTBYTE"
__root const unsigned char opbyte1 = 0xFFU;
#pragma location = "OPTBYTE"
__root const unsigned char opbyte2 = 0xE8U;
#pragma location = "OPTBYTE"
__root const unsigned char opbyte3 = 0x84U;

/* Set security ID */
#pragma location = "SECUID"
__root const unsigned char secuid[10] =
    {0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU, 0xFFU};
//****************************************************************************
// Variables Globales
int DT=0;
unsigned char flag = 1;




//*******************************************************************************
//Cambia Duty Cycle
void CambiaDT_CH45(unsigned char ratio)
{
	unsigned long reg = 0U;
	
	if (ratio > 100U)
	{
		ratio = 100U;
	}
	reg = TDR04;
	reg = (reg + 1) * ratio / 100U;
	TDR05 = (unsigned short)reg;
}

//*****************************************************************************

void  main( void )
{
      int valor=0;
      unsigned char buffer_reloj[]="00:00:00";
      
     
      unsigned char comando;
      unsigned char temp_stabset, temp_stabwait;
      
      int i;
   
    DI();  // configura reloj a 12Mhz
   
    PIOR = 0x00U;

   

    /* Set fMX    12mhZ@0.0833333 US*/
    
    CMC =  0x53U;
    OSTS = 0x07U;  //estabilizacion 2 ^ 18/fX
    MSTOP = 0U;    //X1 oscillator operating
    
    temp_stabset = 0xFFU;
    do
    {
        temp_stabwait = OSTC;
        temp_stabwait &= temp_stabset;
    }
    while (temp_stabwait != temp_stabset);
    /* Set fMAIN */
    MCM0 = 1U;  //Selects the high-speed system clock (fMX) as the main system clock (fMAIN)
    /* Set fSUB */
    XTSTOP = 0U;  //XT1 operating
    // Software wait 5us or more 
    for( i=0U; i<=100U; i++ )
    {
       NOP();
    }
    // fSUB
    OSMC = 0x00;   //reloj para perifericos y RTC e IT 
    /* Set fCLK */
    CSS = 0U;    //Main system clock (fMAIN)
    /* Set fIH */
    HIOSTOP = 1U;  //High-speed on-chip oscillator stopped
   
    EI();
      
      
      
   
       //Configura Puertos
       PM5 = 0x00U; //salidas
       P5 = 0xFF; 
       
       P0  = 0x00;
       PM0 = 0x20;
       
       
        LCDInit(); 
        initRTC();
        initIR();
  
  
       // Configura TIMER INTERVAL ******************************

        RTCEN = 1U;	/* supply RTC clock */
        ITMC =  0x0000U;   //cuenta 30,5176
        ITMK = 1U;	// disable INTIT interrupt 
	ITIF = 0U;	//clear INTIT interrupt flag 
	// Set INTIT low priority 
	ITPR1 = 1U;
	ITPR0 = 1U;
	ITMC = 0x0CCEU;  //Tosc 1/32768 30.5us * 3278(0xCCE) = 100ms
        //Start IT  cada 100ms
        ITIF = 0U;	// clear INTIT interrupt flag 
	ITMK = 0U;	// enable INTIT interrupt 
	ITMC |= 0x8000U;	// enable IT operation 
        
        //*****************************
        
      
        
        
        //********** CONFIGURA PWM SALIDA P05 *************************
        TAU0EN = 1U;	
        TPS0 = 0x0000U;
        TT0 = 0x0AFFU;
        TMMK04 = 0U;	/* disable INTTM04 interrupt */
	TMIF04 = 0U;	/* clear INTTM04 interrupt flag */
	/* Mask channel 5 interrupt */
	TMMK05 = 0U;	/* disable INTTM05 interrupt */
	TMIF05 = 0U;	/* clear INTTM05 interrupt flag */
        
        //Configura el master
        TMR04 = 0x0801U;   //master PWM
        TDR04 = 0x0C7FU;   //Periodo
        TOM0 &= ~ 0x0010U; //TO04
        TOL0 &= ~ 0x0010U; //TO04 active high
        TO0  &= ~ 0x0010U; //TO04
        TOE0 &= ~ 0x0010U; //TO04
         //Configura el slave
        TMR05 = 0x0409U;
        TDR05 = 0x0640U; //duty cycle 50%
        TOM0 |= 0x0020U; //TO05
        TOL0 &= ~ 0x0020U; //TO05 active high
        TO0  &= ~ 0x0020U; //TO05
        TOE0 |= 0x0020U; //TO05
        
       // P05
        P0 &= 0xDFU;
	PM0 &= 0xDFU;
        
        
        
        //Start PWM
        TMIF04 = 0U;	/* clear INTTM04 interrupt flag */
	TMMK04 = 1U;	/* enable INTTM04 interrupt */
	TMIF05 = 0U;	/* clear INTTM05 interrupt flag */
	TMMK05 = 1U;	/* enable INTTM05 interrupt */
	TOE0 |= 0x0020U;
	TS0  |= 0x0030U;
        
  
          
         // Draw text on screen 
          LCDFont(FONT_SMALL);
          LCDString("Hola",0,0);
         //LCDFont(FONT_LARGE);
        // LCDDrawBlock( 0, (LCD_H/2)-4, LCD_W-8, (LCD_H/2)-4); // x-axis
        // LCDDrawBlock( (LCD_W/2)-4, 0, (LCD_W/2)-4, LCD_H-9); // y-axis
         
         
         delay_ms(1500);
         
         
         
         
         
  //*************************************************************************
      while(1)
      {
        // sprintf((char const*)buffer,"Valor = %03d",valor++);
         //LCDStringLine(buffer,LCDCenter(7),3);
        //LCDPrintf(1,2,"Valor = %03d",valor++);
      //  CambiaDT_CH45(DT);
      //  LED_VERDE3_ON 
      //  delay_ms(500);
      //  LED_VERDE3_OFF 
        
      
        
            buffer_reloj[0] = (HOUR  >> 4) + 0x30; 
            buffer_reloj[1] = (HOUR & 0x0F) + 0x30;
            
            
            buffer_reloj[4] = (MIN & 0x0F) + 0x30;
            buffer_reloj[3] = (MIN >> 4) + 0x30;

            buffer_reloj[6] = (SEC >> 4) + 0x30;
            buffer_reloj[7] = (SEC & 0x0F) + 0x30;
            
            LCDString((const char*)buffer_reloj,1,10);
        
         if (got_packet) {
                
                 comando=decodeIR();
              
                
         /*      mask = 0x01;
               for ( i=2; i<18; i++) {
           
                 if (payload[i] == 1){
                   
                     addr = addr | mask;
                  }   
                mask <<= 1;
                }

               mask = 0x01;
               for (i=19; i<=34; i++)
               {
                 if (payload[i] == 1)
                  cmd = cmd | mask;
               mask <<= 1;
               }*/
               
               startIR();

                
                // Print the command to the LCD 
                LCDPrintf(2, LCDCenter(4), "0x%x ", comando);
                
               
               
            }
        
        
        //delay_ms(500);
       
      }  
  
  
  
 
}
//***********************************************************************
#pragma vector = INTTM04_vect
__interrupt void TAUO_Canal4_INT(void)
{
  
  static unsigned char count = 0;
  
  if(++count > 100)
  {
    P1=0;
  
  }
  TMIF04 = 0U;
}  


//***********************************************************************

#pragma vector = INTIT_vect
__interrupt void MD_INTIT(void)
{
  static int value=0;
  static int valor=0;
  static int valor1=0;
  
  
  if(++value > 10)  //100ms * 10 = 1 segundo
  {
    P5 ^= 0x30;  //P55 & P54
    
    if(flag)
    {  
      DT += 5;
      if(DT > 99)
      {  
       DT=99; 
       flag = 0;
      } 
    } 
     else
     {
       DT -= 5;
      if(DT < 0)
      {  
       DT=0; 
       flag = 1;
      }  
       
     } 
       
    value=0;
    ITIF = 0U;	
  }  
  
  if(++valor > 100)
  {
    LED_VERDE1_ON //P52
    valor=0;
    ITIF = 0U;	
  }  
  
  if(++valor1 > 150)
  {
    LED_VERDE1_OFF //P52
   
    valor1=0;
    ITIF = 0U;	
  }  
  
}
