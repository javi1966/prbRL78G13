#ifndef __IR_H
#define __IR_H


volatile unsigned long gTau0Ch3Width;


unsigned char rx_index = 0;         /* Index of bit to receive */
unsigned char rx_fronthalf = 1;     /* If high, process first 32 cycles. If low, second 32 cycles */
extern unsigned char rx_packet[14];
extern unsigned char got_packet = 0;
unsigned int pulses_seen = 0;

extern unsigned char packet[14];


void initIR();

#endif