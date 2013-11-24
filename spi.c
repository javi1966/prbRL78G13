#include <stdint.h>
#include "ior5f100le.h"
#include "ior5f100le_ext.h"
#include "intrinsics.h"
#include "spi.h"


#define NOP	__no_operation

volatile unsigned char   *gpCsi10RxAddress;   /* csi10 receive buffer address */
volatile unsigned short   gCsi10RxLen;      /* csi10 receive data length */
volatile unsigned short  gCsi10RxCnt;      /* csi10 receive data count */
volatile unsigned char   *gpCsi10TxAddress;   /* csi10 send buffer address */
volatile unsigned short   gCsi10TxLen;      /* csi10 send data length */
volatile unsigned short   gCsi10TxCnt;      /* csi10 send data count */
/* Start user code for global. Do not edit comment generated here */
volatile unsigned char  G_SPI_SendingData = 0;
volatile unsigned char  G_SPI_ReceivingData = 0;


//*******************************************************************
void spi10_Init(void)
{
  
   int i = 0;
   
	SAU0EN = 1U;	/* supply SAU0 clock */
	NOP();
	NOP();
	NOP();
	NOP();
	SPS0 =0x0002U;
        
        
         //RESET I/0
	//#warning RESET-IO must be inverted for actual HW
	P13 |= (1<<0); // Assert P130 (#RESET-IO)
        for (i=0;i<10000;i++);
        P13 &= ~(1<<0);  // Deassert P130 (#RESET-IO)
        for (i=0;i<10000;i++);
        
	ST0 |= 0x0004U;	/* disable CSI10 ,canal 2*/
	CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
	CSIIF10 = 0U;	/* clear INTCSI10 interrupt flag */
	/* Set INTCSI10 low priority */
	CSIPR110 = 0U;
	CSIPR010 = 0U;
	SIR02 =0x0007U;	    /* clear error flag */
	SMR02 = 0x0020U;    //Modo SPI
	SCR02 = 0xC007U;    //8 bits,Tx y RX
	SDR02 = 0xFE00U;    //divisor fclk
	SO0 |= 0x0400;	    /* CSI10 clock initial level */
	SO0 &= ~0x0004U;	/* CSI10 SO initial level */
	SOE0 |= 0x0004U;	/* enable CSI10 output */
	/* Set SI10 pin , P03*/
	PMC0 &= 0xF7U;
	PM0 |= 0x08U;
	/* Set SO10 pin , P02*/
	P0 |= 0x04U;
	PMC0 &= 0xFBU;
	PM0 &= 0xFBU;
	/* Set SCK10 pin ,P04*/
	P0 |= 0x10U;
	PM0 &= 0xEFU;
        
        //Start SPI
        
        CSIIF10 = 0U;	/* clear INTCSI10 interrupt flag */
	CSIMK10 = 0U;	/* enable INTCSI10 */
	SO0 |= 0x0400U;  	/* CSI10 clock initial level */
	SO0 &= ~0x0004U;	/* CSI10 SO initial level */
	SOE0 |= 0x0004U;	/* enable CSI10 output */
	SS0 |= 0x0004U;	/* enable */
        
}
        
 //**********************************************************************
uint8_t spi10_SendReceiveData(unsigned char *txbuf, unsigned short txnum, unsigned char *rxbuf)  
{
   unsigned char status = 0;
   
   G_SPI_SendingData = 1;
   G_SPI_ReceivingData = 1;
   
   if (txnum < 1U)
	{
		status = 0x85U;
	}
	else
	{
		gCsi10TxCnt = txnum;	/* send data count */
		gpCsi10TxAddress = txbuf;	/* send buffer pointer */
		gpCsi10RxAddress = rxbuf;	/* receive buffer pointer */
		CSIMK10 = 1U;	/* disable INTCSI10 interrupt */
		SIO10 = *gpCsi10TxAddress;	/* started by writing data to SDR[7:0] */
		gpCsi10TxAddress++;
		gCsi10TxCnt--;
		CSIMK10 = 0U;	/* enable INTCSI10 interrupt */
	}

	return (status);
  

}  
//****************************************************************************

#pragma vector = INTCSI10_vect
__interrupt void INTCSI10(void)
{
  
   unsigned char err_type;

   err_type = (unsigned char)(SSR02 & 0x0001U);
   SIR02 = (unsigned short)err_type;
   if (err_type == 1U)
   {
      spi10_ErrorCallback(err_type);   // overrun error occurs 
   }
   else 
   {
      if (gCsi10TxCnt > 0U)
      {
         *gpCsi10RxAddress = SIO10;
         gpCsi10RxAddress++;
         SIO10 = *gpCsi10TxAddress;
         gpCsi10TxAddress++;
         gCsi10TxCnt--;      
      }
      else 
      {
         if (gCsi10TxCnt == 0U)
         {
            *gpCsi10RxAddress = SIO10;
         }
         spi10_SendEndCallback();   //complete send 
         spi10_ReceiveEndCallback();   // complete receive 
      }
   }
  
}


//***************************************************************************
void spi10_ReceiveEndCallback(void)
{
   /* Start user code. Do not edit comment generated here */
        G_SPI_ReceivingData = 0;
   /* End user code. Do not edit comment generated here */
}
//*************************************************************************

void spi10_ErrorCallback(unsigned char err_type)
{
   /* Start user code. Do not edit comment generated here */
   /* End user code. Do not edit comment generated here */
}
//*************************************************************************
void spi10_SendEndCallback(void)
{
   /* Start user code. Do not edit comment generated here */
        G_SPI_SendingData = 0;
   /* End user code. Do not edit comment generated here */
}