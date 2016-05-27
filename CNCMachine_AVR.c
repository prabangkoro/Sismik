// Bismillah
/* Tugas Besar EL3016 Sistem Mikroprosesor
 * CNC Braille Printer
 * Author : M. Nibrosul Umam (13213045)
 *          Muhammad Isnain Hartanto (13213046)
 *          Bagus Prabangkoro (13213125)
 * Institut Teknologi Bandung
 * 2016
 */
#define F_CPU 7372800UL
#include <avr/interrupt.h>
#include <avr/io.h>
#include <util/delay.h>
#include <math.h>


#define ServoPin 		PD2
#define StepperXPin1	PB2
#define StepperXPin2	PB3
#define StepperXPin3	PB4
#define StepperXPin4	PB5
#define StepperYPin1	PD6
#define StepperYPin2	PD7
#define StepperYPin3	PB0
#define StepperYPin4	PB1

#define A 				0.002  //variable rev/mm
#define B 				100 //veriable step/rev
#define zServoAngle0    95 // sudut servo agar spidol siap tulis
#define zServoAngle1    65 // sudut servo agar spidol tidak siap tulis

#define DegToRad(x)		x*3.14/180.0
#define	RadToDeg(x)		x*180.0/3.14

#define true 1
#define false 0

char StepFWD[]	= {0x09, 0x05, 0x06, 0x0a};
char StepBWD[]	= {0x0a, 0x06, 0x05, 0x09};
	
float ms;

char inputString[100];
int stringComplete;
// prototype


// pwm interrupt
// set on Capture
// ICR1
ISR(TIM1_CAPT_vect) {
	// process the timer1 overflow here
	PORTD |= (1<<ServoPin);
}

// clear on commpare match
ISR(TIM1_COMPA_vect) {
	// process the timer1 overflow here
	PORTD &= ~(1<<ServoPin);
}

void ServoPWNInit(){
	// output not-connected
	TCCR1A |= (1<<WGM11);
	// Prescaller 64; MODE 14 Fast PWM
	TCCR1B |= (1<<WGM13)|(1<<WGM12)|(1<<CS11)|(1<<CS10);
	// set clock FPWM = 50Hz
	ICR1 = 4999;
}

void ServoPWMGenerator(float PulseWidth){
	int count;
	count = (int) ms*1000 / 4;
	OCR1A = count - 1;
}

void ServoSetAngle(int degree){
	//float ms;
	// convert degree into pulse width
	ms = 0.388 + 0.009733*degree;
	ServoPWMGenerator(ms);
}

void StepperXInit(){
	PORTD |= (1<<StepperXPin1)|(1<<StepperXPin2)|(1<<StepperXPin3)|(1<<StepperXPin4);
}

void StepperYInit(){
	PORTB |= (1<<StepperYPin1)|(1<<StepperYPin2);
	PORTD |= (1<<StepperYPin3)|(1<<StepperYPin4);
}

void StepXGenerator(char move){
	/*
	PORTB2 = (move>>0)&0x01;
	PORTB3 = (move>>1)&0x01;
	PORTB4 = (move>>2)&0x01;
	PORTB5 = (move>>3)&0x01;
	PORT */
	
	PORTB	= 0x02 && (move>>0);
	PORTB	= 0X03 && (move>>1);
	PORTB	= 0x04 && (move>>2);
	PORTB	= 0x05 && (move>>3);
	

	_delay_ms(2);
}

void StepYGenerator(char move){
	/*
	PORTD6 = (move>>0)&0x01;
	PORTD7 = (move>>1)&0x01;
	PORTB0 = (move>>2)&0x01;
	PORTB1 = (move>>3)&0x01;*/
	PORTD  = 0X06 && (move>>0);
	PORTD  = 0X07 && (move>>1);
	PORTB  = 0X00 && (move>>2);
	PORTB  = 0X01 && (move>>3);
	_delay_ms(2);
}

void StepX(int steps){
	if(steps > 0){
		// move forward
		for(char i = 0; i < steps; i++){
			StepXGenerator(StepFWD[i%4]);
		}
		}else if(steps < 0){
		// move backward
		for(char i = 0; i < -steps; i++){
			StepXGenerator(StepBWD[i%4]);
		}
	}
}

void StepY(int steps){
	if(steps > 0){
		// move forward
		for(char i = 0; i < steps; i++){
			StepYGenerator(StepFWD[i%4]);
		}
		}else if(steps < 0){
		// move backward
		for(char i = 0; i < -steps; i++){
			StepYGenerator(StepBWD[i%4]);
		}
	}
}

void ymove(float mm) {
	StepY((int)B*A*mm);    //StepperX melakukan step sebesar B*A*mm
}

void xmove(float mm) {
	StepX((int)B*A*mm);    //StepperY melakukan step sebesar B*A*mm
}

void zmove(char x) {
	if( x == 'u'){			// write 'u' to move pen upward
		ServoSetAngle(zServoAngle1);
		_delay_ms(1000);
		}else if( x == 'd'){	// write 'd' to move pen downward
		ServoSetAngle(zServoAngle0);
		_delay_ms(1000);
	}
}

void printbSquare(int x) {
	ymove(-x);
	_delay_ms(100);
	xmove(-x);
	_delay_ms(100);
	ymove(x);
	_delay_ms(100);
	xmove(x);
	_delay_ms(100);
}

void printbTriangle(){
	for(int i=0;i<30;i++){
		ymove(-10);
		zmove(-10);
	}
	for(int i=0;i<30;i++){
		ymove(-10);
		zmove(10);
	}
	for(int i=0;i<30;i++){
		ymove(10);
	}
}

void printbInfinite(int x){
	int divider = 5;
	zmove(x);
	_delay_ms(100);
	for(int i = 0; i < x/divider; i++){
		ymove(-divider);
		zmove(-divider);
	}
	zmove(x);
	for(int i = 0; i < x/divider; i++){
		ymove(divider);
		zmove(-divider);
	}
}

void printbLine(int lgt1, float teta1){
	int divider = 10;
	for(int i = 0; i < lgt1/divider; i++){
		ymove(round(cos(DegToRad(teta1))*divider));
		zmove(round(sin(DegToRad(teta1))*divider));
	}
}

void printbPolygon(int n, int lgt){
	for(int i = 0; i < n; i++){
		printbLine(lgt, (float)i*360.0/n);
	}
}
void usartReadChar(){
	while(!(UCSR0A &(1<< RXC0))){}
	inputString[0] = UDR0;
}
void usartWriteChar(char *inputdata){
	while(!(UCSR0A &(1<< UDRE0))){}
	
	UDR0 = *inputdata;
}

void CreateLingkaran(){
	ServoSetAngle(zServoAngle0);
	_delay_ms(1000);
	usartWriteChar("Printing Lingkara ...");
	int sisi = 100;
	int nsisi = 30;
	printbPolygon(nsisi,sisi);
	printbPolygon(nsisi,sisi);
	printbPolygon(nsisi,sisi);
	_delay_ms(100);
	usartWriteChar("Done.");
	ServoSetAngle(zServoAngle1);
	_delay_ms(1000);
}

void CreateKotak(){
	ServoSetAngle(zServoAngle0);
	_delay_ms(1000);
	usartWriteChar("Printing Kotak ...");
	printbSquare(500);
	printbSquare(500);
	printbSquare(500);
	usartWriteChar("Done.");
	_delay_ms(100);
	ServoSetAngle(zServoAngle1);
	_delay_ms(1000);
}

void CreateInfinite(){
	ServoSetAngle(zServoAngle0);
	_delay_ms(1000);
	usartWriteChar("Printing Infinite ...");
	printbInfinite(300);
	printbInfinite(300);
	printbInfinite(300);
	usartWriteChar("Done.");
	_delay_ms(100);
	ServoSetAngle(zServoAngle1);
	_delay_ms(1000);
}

int main(void){
	UCSR0A =0x00;
	UCSR0B =0x98;
	UCSR0C =0x86;
	UBRR0L =47;

	sei();
	ServoPWNInit();
	StepperXInit();
	StepperYInit();
	stringComplete = false;
	//serialinit();
	while(1){
		//ServoSetAngle(20);
		if(stringComplete==false){
			usartWriteChar("ok");
			usartReadChar();
			if(inputString == "Lingkaran\n"){
				CreateLingkaran();
			}else if(inputString == "Kotak\n"){
				CreateKotak();
			}else if(inputString == "Infinite\n"){
				CreateInfinite();
			}
		}
//		inputString="";
		stringComplete =false;
		
		
	}
	return 0;
}

