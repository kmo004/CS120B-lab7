/*	Author: kmo004
 *  Partner(s) Name: Michael Wen
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0;

unsigned long _avr_timer_M = 1;
unsigned long _avr_timer_cntcurr = 0;

void TimerOn(){
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;
	SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00;
}

void TimerISR(){
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	}
}

void TimerSet(unsigned long M){
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}

enum STATES { START,PLUS, MINUS, RESET, WAIT} state;
unsigned char num = 0x07;


void tick(){
	switch(state){
		case START:
		state = WAIT;
		break;
		
		case WAIT:
		if ((~PINA & 0x03) == 0x03){
			state = RESET;
		}
		else if ((~PINA & 0x02) == 0x02){
			state = MINUS;
		}
		else if ((~PINA & 0x01) == 0x01){
			state = PLUS;
		}
		
		break;
		
		case PLUS:
		state = WAIT;
		break;
		
		case MINUS:
		state = WAIT;
		break;
		
		case RESET:
		state = WAIT;
		break;
		
		default:
			state = START;
			break;
		
		
	}
	switch(state){
		case START:
		break;
		
		case PLUS:
		if(num < 0x09){
			num = num + 0x01;
		}
		break;
		
		case MINUS:
		if(num > 0x00){
			num = num - 0x01;
		}
		break;
		
		case RESET:
			num = 0x00;
		break;
		
		case WAIT:
			break;

		
	}
}
int main(void) {
    /* Insert DDR and PORT initializations */
    DDRA = 0x00; PORTA = 0xFF;
	DDRC = 0xFF; PORTC = 0x00;
	DDRD = 0xFF; PORTD = 0x00;
	
	TimerSet(75);
	TimerOn();
	
	state = START;
	num = 0x07;
	LCD_init();
    /* Insert your solution below */
    while (1) {
		tick();
		LCD_Cursor(1);
		LCD_WriteData(num + '0');
		while(!TimerFlag);
		TimerFlag = 0;

    }
    return 1;
}
