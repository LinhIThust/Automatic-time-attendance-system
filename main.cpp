
#include <stdio.h>
#include <string.h>


#include "RFID.h"
void uart_init(){
	UBRRH = 0x00;
	UBRRL = 0x19;//set baurate 2400
	//UBRRL = 0x67;//set baurate 2400 crytal 4m
	
	/* Enable receiver and transmitter */
	UCSRB = (1<<RXEN)|(1<<TXEN);
	/* Set frame format: 8data, 2stop bit */
	UCSRC =	(1<<URSEL)|(1 << UCSZ1) | (1 << UCSZ0); 
}
void USART_Transmit( unsigned char data ){
	/* Wait for empty transmit buffer */
	while ( !( UCSRA & (1<<UDRE)) );
	/* Put data into buffer, sends the data */
	UDR = data;
}
unsigned char USART_Receive( void ){
	/* Wait for data to be received */
	while ( !(UCSRA & (1<<RXC)) );
	/* Get and return received data from buffer */
	return UDR;
}
void SPI_MasterInit(void)
{
	DDRB |= (1<<SCK_PIN)|(1<<MOSI_PIN)|(1<<SS);
	SPCR |=	(1<<SPE)|(1<<MSTR)|(1<<SPR0);
	sbi(PORTB,SS);
}

void _SendString(char str[])
{
	int i =0;
	
	while (str[i] != 0x00)
	{
		USART_Transmit(str[i]);
		i++;
	}
}

MFRC522 rfid(2,6);

int main(void)
{
	SPI_MasterInit();
	uart_init();
	DDRA = 0xff;
	DDRC = 0xff;
	rfid.begin();
	_SendString("START");
	uint8_t status;
	uint8_t data[MAX_LEN];
	while(1)
	{
		
		memset( data, '\0', sizeof(char)*MAX_LEN );
		status = rfid.requestTag(MF1_REQIDL, data);
		//sendz(status);
		if (status == MI_OK) {
			status = rfid.antiCollision(data);
			for(unsigned char i =0;i<MAX_LEN;i++)
			{
				USART_Transmit(data[i]);
				
			}
			sbi(PORTC,5);
			rfid.selectTag(data);
			// Stop the tag and get ready for reading a new tag.
			rfid.haltTag();
			cbi(PORTC,5);
		}
	}
}

