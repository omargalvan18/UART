#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>
#include <util/delay.h>
#include <stdio.h>

#define DATA_REGISTER_EMPTY (1<<UDRE0)	//define el bit que indica que el registro está vacío
#define RX_COMPLETE (1<<RXC0)			//define el bit que indica una recepción completa
#define FRAMING_ERROR (1<<FE0)			//define el bit de error
#define PARITY_ERROR (1<<UPE0)			//define el bit de paridad
#define DATA_OVERRUN (1<<DOR0)			//define el bit de data


#define RX_BUFFER_SIZE0 8				//define el tamaño del bufer
char rx_buffer0[RX_BUFFER_SIZE0];       //Buffer de Recepción
unsigned char rx_wr_index0=0;			//Indice del Buffer de Recepción
int adcData = 0;						//Guarda el valor de la lectura ADC



void Ports_setup(void);					//prototipos de las funciones a usar
void USART_setup(void);
void ADC_setup(void);
void Timer_setup(void);



void Ports_setup(void)					//configuración de puertos como entrada y salida
{
	DDRB = 0x00;
	DDRC = 0x00;
	DDRD = 0x02;
	
	PORTB = 0x00;
	PORTC = 0x00;
	PORTD = 0x00;
}

void USART_setup(void)				//configuración de la UART a 9600 bauds, 1 bit de stop y sin paridad
{
	UCSR0A |= (0<<RXC0)|(0<<TXC0)|(0<<UDRE0)|(0<<FE0)|(0<<DOR0)|(0<<UPE0)|(0<<U2X0)|(0<<MPCM0);
	UCSR0B |= (0<<RXCIE0)|(0<<TXCIE0)|(0<<UDRIE0)|(0<<RXEN0)|(1<<TXEN0)|(0<<UCSZ02)|(0<<TXB80);
	UCSR0C |= (0<<UMSEL01)|(0<<UMSEL00)|(0<<UPM01)|(0<<UPM00)|(0<<USBS0)|(1<<UCSZ01)|(1<<UCSZ00)|(0<<UCPOL0);
	UBRR0 = 103;					//para 16MHz
}

void USART_send(unsigned char ch)	//funcion que envía	el caractér 
{
	
	while(!(UCSR0A&(1<<UDRE0)));	//mientras esté vacío el buffer
	UDR0 = ch;						//envía datos
}

void ADC_setup(void)				//configuración del ADC, canal 0, 128, Vref = VCC
{
	ADMUX |= (0<<REFS1)|(1<<REFS0)|(0<<ADLAR)|(0<<MUX3)|(0<<MUX2)|(0<<MUX1)|(0<<MUX0);
	ADCSRA |= (1<<ADEN)|(0<<ADSC)|(0<<ADATE)|(0<<ADIF)|(0<<ADIE)|(1<<ADPS2)|(1<<ADPS1)|(1<<ADPS0);
	ADCSRB |= (0<<ACME)|(0<<ADTS2)|(0<<ADTS1)|(0<<ADTS0);
	DIDR0 |= (0<<ADC5D)|(0<<ADC4D)|(0<<ADC3D)|(0<<ADC2D)|(0<<ADC1D)|(1<<ADC0D);
}


int main(void)
{
	Ports_setup();				//inicialización de las funciones
	USART_setup();
	ADC_setup();
	
	unsigned char info[]="muy bien!";		//mensaje 1
	unsigned char info2[]="muy mal!!";		//mensaje 2
	
	while(1)
	{
		ADCSRA |= (1<<ADSC);
		while((ADCSRA&(1<<ADSC))==0);
		ADCSRA |= (1<<ADIF);
		adcData = ADC;				//realiza la conversión y guarda el valor 
		
		 int digit0 = adcData%10;
		 int digit1 = (adcData/10)%10;
		 int digit2 = (adcData/100)%10;
		 int digit3 = (adcData/1000)%10;
		 
		 char data[] = {digit3+0x30,digit2+0x30,digit1+0x30,digit0+0x30};
		 
		 for(int i=0;i<4;i++)    //envía los caracteres
		 {
			 USART_send(data[i]);
		 }
		 USART_send(0x0A);    //código ASCII para salto de línea
		 USART_send(0x0D);    //código ASCII para retorno de carro
		 
	}
}

