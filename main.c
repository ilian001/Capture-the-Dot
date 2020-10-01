/*
*		ilian001@ucr.edu
*		FINAL PROJECT GAME
 */ 

#include <avr/io.h>
#include <stdio.h>
#include <stdlib.h>
#define F_CPU 1000000
#include <util/delay.h>
#define E   5
#define RS  6

char PORT[8] = {8,4,2,1,16,32,64,128};
unsigned char pointColumn[8] = {1,2,4,8,16,32,64,128};	
unsigned char row = 0;
unsigned char column = 1;
int HORIZONTAL=511;//neutral x
int VERTICAl = 511;//neutral y
int HORIZONTALMOV = 0;
int VERTICAlMOV = 0;
unsigned char cnt = 0;
long point_timer = 1000;
unsigned char i = 0;
unsigned char j = 0;
unsigned char pointTotal = 0;
static int w[8] = {0b11111110,0b11111111,0b00000011,0b11111111,0b11111111,0b00000011,0b11111111,0b11111110};

enum MoveStates {move_init,move_ingame}MoveState;
void Movement() {
	unsigned char button = ~PINA & 0x04;
	switch(MoveState) {					//transition
		case move_init:
			if(button) {
				MoveState = move_ingame;
				_delay_ms(200);
			}
		break;
		case move_ingame:
			if(button) {
				MoveState = move_init;
				_delay_ms(200);
			}
		break;	
	}
	switch(MoveState) {					//actions
		case move_init:
		row = 0;
		column = 1;
		break;
		case move_ingame:
		//JOYSTICK----------------------------------------------------------------------
		switch (ADMUX) {
			case 0x40: {
				ADCSRA |=(1<<ADSC);//start ADC conversion
				while ( !(ADCSRA & (1<<ADIF)));//wait till ADC conversion
				HORIZONTALMOV = ADC;//moving value
				ADC=0;//reset ADC register
				ADMUX=0x41;//changing channel
				break;
			}

			case 0x41: {
				ADCSRA |=(1<<ADSC);// start ADC conversion
				while ( !(ADCSRA & (1<<ADIF)));// wait till ADC conversion
				VERTICAlMOV = ADC;// moving value
				ADC=0;// reset ADC register
				ADMUX=0x40;// changing channel
				break;
			}
		}
	
		if (HORIZONTALMOV<HORIZONTAL-50) {
			if (column < 128) column = column << 1;
			_delay_ms(300);
		}

		if (HORIZONTALMOV>(HORIZONTAL+50)) {
			if (column > 1) column = column >> 1;
			_delay_ms(300);
		}
	
		if (VERTICAlMOV<VERTICAl-50) {
			if(row < 7) row++;
			_delay_ms(300);
		}

		if (VERTICAlMOV>VERTICAl+50) {
			if(row > 0) row--;
			_delay_ms(300);
		}
		//END JOYSTICK------------------------------------------------------------------
		break;
	}
}

enum PointStates{point_init,point_wait,point_change}PointState;
void Points() {
	unsigned char button = ~PINA & 0x04;
	
	switch(PointState) {				//transition
		case point_init:
			if(button) {
				PointState = point_wait;
				_delay_ms(200);
			}
		break;
		case point_wait:
			if(button) {
				PointState = point_init;
				_delay_ms(200);
			}
			if(cnt > point_timer) {			//time up
				PointState = point_change;
			}
			else if (i==row && PORT[j] == column) {							//point capture
				PointState = point_change;
			}
		break;
		case point_change:
		break;
	}
	switch(PointState) {				//action
		case point_init :
		i = 7;
		j = 7;
		pointTotal = 0;
		point_timer = 1000;
		cnt = 0;
		break;
		case point_wait :
		cnt++;
		break;
		case point_change:
		if(i==row && PORT[j] == column) {
			pointTotal++;
			point_timer -= 100;
		}
		cnt = 0;
		//i--;
		j = rand() % 7;
		PointState = point_wait;
		break;
	}
};	

int main(void)
{
	DDRA = 0x00, PORTA = 0xFF;		//input joystick
	DDRB = 0xFF, PORTB = 0x00;		//output
	DDRC = 0xFF, PORTC = 0x00;		//output	
	DDRD = 0xFF, PORTD = 0x00;		//output
	
	ADMUX |=(1<<REFS0);
	ADCSRA |=(1<<ADEN) |(1<ADPS2)|(1<ADPS1) |(1<<ADPS0);

	
	
	MoveState = move_init;
	PointState = point_init;
	row = 0;
	column = 1;
	
    while (1) 
    {
		Movement();
		PORTC = PORT[row];
		PORTD = ~column;
		_delay_us(5000);
		PORTC= 0;
		Points();
		PORTC = PORT[i];
		PORTD = ~pointColumn[j];
		_delay_us(5000);
		
		if(pointTotal >= 5) {
			MoveState = move_init;
			PointState = point_init;
			for(int a = 0; a < 1000; a++) {	//0b11111110,0b11111111,0b00000011,0b11111111,0b11111111,0b00000011,0b11111111,0b11111110
				for(int b = 0; b < 8; b++) {
					PORTC = pointColumn[b];
					PORTD = ~w[b];
					_delay_us(5000);
				}
				PORTC = 0;
				
				
			}
			_delay_ms(5000);
		}
		
    }
}

