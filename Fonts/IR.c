#include "ior5f100le.h"
#include "ior5f100le_ext.h"
#include "intrinsics.h"



void initIR()
{
  
        TMR03 = 0x0104;  //modo captura,con flanco bajada
	TOM0 &= ~0x0008;  //TI03 input
	TOL0 &= ~0x0008;
	TO0 &= ~ 0x0008;
	TOE0 &= ~0x0008;
	NFEN1 &= (unsigned char)~0x08;	// previene ruido canal 3
        PM3 |= 0x02U;                   //P31 input
        TMIF03 = 0U;	/* clear INTTM03 interrupt flag */
	TMMK03 = 0U;	/* enable INTTM03 interrupt */
	TS0 |= 0x0008;  //TRIGGER ON.
} 

//******************************************************************************************

#pragma vector = INTTM03_vect
__interrupt void MD_INTTM03(void)
{
        /* Allow interrupts to nest inside this one - preserve 25% duty cycle output */
         __enable_interrupt;
        
	if ((TSR03 & 0x0001) != 0U)	/* overflow occurs */
	{			
		gTau0Ch3Width = (unsigned long)TDR03 + 0x10000U;
	}
	else
	{
		gTau0Ch3Width = (unsigned long)TDR03;
	}
        
          if (rx_fronthalf) {
            
            /* If we've seen many pulses when we're expecting the first 32 cycles */
            if (pulses_seen > 28) {
                
                if ((pulses_seen > 56) || (rx_packet[rx_index - 1] == 0)) {
                    pulses_seen = 0;
                    
                    /* The current bit is zero if we have many pulses back to back (1 to 0 transition)
                     * or if we have a handful of pulses and the previous bit was zero (0 to 0 transition)
                     */
                    rx_packet[rx_index] = 0;
                    
                    /* Start looking for the next 32 cycles in this bit */
                    rx_fronthalf = 0;
                }
            }
        
        /* Are we receiving the second 32 cycles of an incoming pulse train? */
        } else {
            if (rx_packet[rx_index] == 1) {
                if (pulses_seen > 28) {
                    rx_index++;
                    
                    /* If the current bit was detected as a 1 already, then we can start looking for
                     * the front half of the next bit after we receive a bunch of high cycles
                     */
                    rx_fronthalf = 1;
                    
                    /* If we completed reception of the last bit, reset the bit counter for the 
                     * rx_packet and prepare to display the command byte on screen
                     */
                    if (rx_index > 13) {
                        rx_index = 0;
                        got_packet = 1;
                    }
                }
            }
        }
        
        /* Count pulses seen by this interrupt */
        if (pulses_seen < 100)
            pulses_seen++;
        
        /* Reset the interval timeout counter */
        //it_timeouts = 0;
        
        /* Restart the interval timer */
        TT0 |= 0x0008;
	/* Mask channel 3 interrupt */
	TMMK03 = 1U;	/* disable INTTM03 interrupt */
	TMIF03 = 0U;	/* clear INTTM03 interrupt flag */
        TMIF03 = 0U;	/* clear INTTM03 interrupt flag */
	TMMK03 = 0U;	/* enable INTTM03 interrupt */
	TS0 |= 0x0008;
        
} //fin INT       