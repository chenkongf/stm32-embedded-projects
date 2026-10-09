#ifndef __SPI_H
#define __SPI_H

#define 	SPI_CLOCK		LL_APB2_GRP1_PERIPH_GPIOA

#define	  MISO 				LL_GPIO_PIN_0
#define		MISO_PORT		GPIOA

#define 	MOSI 				LL_GPIO_PIN_1
#define 	MOSI_PORT		GPIOA

#define 	SCK  				LL_GPIO_PIN_2
#define 	SCK_PORT		GPIOA

#define 	NSS  				LL_GPIO_PIN_3
#define   NSS_PORT		GPIOA

#define 	RST  				LL_GPIO_PIN_4
#define		RST_PORT		GPIOA

#define 	SCK_H 			SCK_PORT->BSRR			= SCK
#define 	SCK_L 			SCK_PORT->BRR 			= SCK
#define 	MOSI_H 			MOSI_PORT->BSRR 		= MOSI
#define 	MOSI_L 			MOSI_PORT->BRR 			= MOSI
#define 	NSS_H  			NSS_PORT->BSRR 			= NSS
#define 	NSS_L  			NSS_PORT->BRR 			= NSS
#define 	RST_H  			RST_PORT->BSRR 			= RST
#define 	RST_L  			RST_PORT->BRR 			= RST
#define 	READ_MISO 	LL_GPIO_IsInputPinSet(MISO_PORT, MISO)


#define MI_OK                          (char)0
#define MI_NOTAGERR                    (char)(-1)
#define MI_ERR                         (char)(-2)

void spi_init(void);
unsigned char ReadRawRC(unsigned char Address);
void WriteRawRC(unsigned char Address, unsigned char value);


#endif


