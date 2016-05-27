/* Tugas Besar EL3016 Sistem Mikroprosesor
 * CNC Braille Printer
 * Author : M. Nibrosul Umam (13213045)
 *          Muhammad Isnain Hartanto (13213046)
 *          Bagus Prabangkoro (13213125)
 * Institut Teknologi Bandung
 * 2016
 */
 
#include <Stepper.h>
#include <Servo.h>
#include <math.h>
//#include <String.h>

#define DegToRad(x)	x*3.14/180.0

#define A 0.002  //variable rev/mm
#define B 100 //veriable step/rev
#define zServoAngle0    37// sudut servo agar spidol siap tulis
#define zServoAngle1    10 // sudut servo agar spidol tidak siap tulis

Stepper stepperX(B, 2, 3, 4, 5);      //variable stepper X dengan step/rev sebesar 100
Stepper stepperY(B, 6, 7, 8, 9);      //variable stepper Y dengan step/rev sebesar 100
Stepper stepperZ(B, 10, 11, 12, 13);   //variable stepper Z dengan step/rev sebesar 100
Servo zServo;

int dtitik=10;
int dhuruf=10;

char printb = 'u';

String inputString = "";
boolean stringComplete = false;

void setup() {
  // put your setup code here, to run once:
  pinMode(2, OUTPUT);   //pin 1 Stepper X
  pinMode(3, OUTPUT);   //pin 2 Stepper X
  pinMode(4, OUTPUT);   //pin 3 Stepper X
  pinMode(5, OUTPUT);   //pin 4 Stepper X
  pinMode(6, OUTPUT);   //pin 1 Stepper Y
  pinMode(7, OUTPUT);   //pin 2 Stepper Y
  pinMode(8, OUTPUT);   //pin 3 Stepper Y
  pinMode(9, OUTPUT);   //pin 4 Stepper Y
  pinMode(10, OUTPUT);   //pin 1 Stepper Z
  pinMode(11, OUTPUT);   //pin 2 Stepper Z
  pinMode(12, OUTPUT);   //pin 3 Stepper Z
  pinMode(13, OUTPUT);   //pin 4 Stepper Z
  zServo.attach(2);
//  angkat
  zServo.write(zServoAngle1);
  delay(1000);
  
  inputString.reserve(200);
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:  
  if (stringComplete) {
    Serial.println("OK");
    inputString.trim();
    const char sym[2] = ";";
    int i = inputString.indexOf(';');
    int j = inputString.indexOf(';', i+1);
    String Order, cmd1, cmd2;

    Order = inputString.substring(0, i);
    cmd1 = inputString.substring(i+1, j);
    cmd2 = inputString.substring(j+1, inputString.length());
    
    if(Order == "P"){
      CreateLingkaran(cmd1.toInt(), cmd2.toInt());
    }else if(Order == "R"){
      CreateKotak(cmd1.toInt(), cmd2.toInt());
    }else if(Order == "I"){
      CreateInfinite(cmd1.toInt(), cmd2.toInt());
    }
    
//    if(inputString == "Lingkaran"){
//      CreateLingkaran();
//    }else if(inputString == "Kotak\n"){
//      CreateKotak();
//    }else if(inputString == "Infinite\n"){
//      CreateInfinite();
//    }
    // clear the string:
    inputString = "\0";
    stringComplete = false;
  }
}

void CreateLingkaran(int nsisi, int sisi){
  //  turun
  zServo.write(zServoAngle0);
  delay(1000);
  Serial.println("Printing Lingkaran ...");
  printbPolygon(nsisi, sisi);
  printbPolygon(nsisi, sisi);
  printbPolygon(nsisi, sisi);
  //  angkat
  delay(100);
  Serial.println("Done.");
  zServo.write(zServoAngle1);
  delay(1000); 
}

void CreateKotak(int w, int l){
  //  turun
  zServo.write(zServoAngle0);
  delay(1000);
  Serial.println("Printing Kotak ...");
  printbRect(w, l);
  printbRect(w, l);
  printbRect(w, l);
  Serial.println("Done.");
  //  angkat
  delay(100);
  zServo.write(zServoAngle1);
  delay(1000); 
}

void CreateInfinite(int w, int l){
  //  turun
  zServo.write(zServoAngle0);
  delay(1000);
  Serial.println("Printing Infinite ...");
  printbInfinite(w);
  printbInfinite(w);
  printbInfinite(w);
  Serial.println("Done.");
  //  angkat
  delay(100);
  zServo.write(zServoAngle1);
  delay(1000); 
}

void serialEvent() {
  while (Serial.available()) {
    // get the new byte:
    char inChar = (char)Serial.read();
    // add it to the inputString:
    inputString += inChar;
    // if the incoming character is a newline, set a flag
    // so the main loop can do something about it:
    if (inChar == '\n') {
      stringComplete = true;
    }
  }
}


void ymove(float mm) {
  stepperY.setSpeed(30);
  stepperY.step((int)B*A*mm);    //StepperX melakukan step sebesar B*A*mm
}

void zmove(float mm) {
  stepperZ.setSpeed(30);
  stepperZ.step((int)B*A*mm);    //StepperY melakukan step sebesar B*A*mm
}

void xmove(char x) {
  if(x == 'u'){
    zServo.write(zServoAngle1);
    delay(1000);
  }else if(x == 'd'){
    zServo.write(zServoAngle0);
    delay(1000);
  }
}

void printbSquare(int x) {
  ymove(-x);
  delay(100);
  zmove(-x);
  delay(100);
  ymove(x);
  delay(100);
  zmove(x);
  delay(100);
}

void printbRect(int x, int y) {
  ymove(-x);
  delay(100);
  zmove(-y);
  delay(100);
  ymove(x);
  delay(100);
  zmove(y);
  delay(100);
}

void printbInfinite(int x){
  int divider = 5;
  zmove(x);
  delay(100);
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

void printbLine(int lgt1, double teta1){
  int divider = 30;
  for(int i = 0; i < lgt1/divider; i++){
    ymove(round(cos(DegToRad(teta1))*divider));
    zmove(round(sin(DegToRad(teta1))*divider));
  }
}

void printbPolygon(int n, int lgt){
  for(int j = 0; j < n; j++){
    printbLine(lgt, (double)j*360.0/n);
  }
}

