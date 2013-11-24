#ifndef _SPI_H
#define _SPI_H

void spi10_Init(void);
uint8_t spi10_SendReceiveData(unsigned char *txbuf, unsigned short txnum, unsigned char *rxbuf); 
void spi10_ErrorCallback(unsigned char err_type);
__interrupt void INTCSI10(void);
void spi10_ReceiveEndCallback(void);
void spi10_SendEndCallback(void);

#endif