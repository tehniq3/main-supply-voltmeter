// base sketch from https://brainy-bits.com/tutorials/4-bits-7-segment-led-display-with-arduino/
// changed TM1637 library by niq_ro for degree, r & h letter
// main voltmeter on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/
// ver.0 - base schetch for see main voltage
// ver.1, change TM1637 pins, put 3 leds for state (under, ok, overvoltage)
// ver.2, put MENU, increase, decrease buttons

#include "TM1637.h" // use changed library from https://github.com/tehniq3/TM1637-display/tree/master/TM1637
//{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
//0~9,A,b,C,d,E,F,"-"," ",degree,r,h,n,t, 
//A -> 10, b -> 11, C -> 12, d -> 13, E -> 14, F -> 15,
// "-" -> 16, space -> 17, degree -> 18, r -> 19, h -> 20, n -> 21
// t -> 22, "u with _" -> 23, "u with -" -> 24


#define CLK 7  //Pins for TM1637       
#define DIO 6
TM1637 tm1637(CLK,DIO);

#define sub 10    // pin for undervoltage led
#define bun 9     // pin for ok voltage led
#define peste 8   // pin for overvoltage led
#define releu 11  // pin for relay
#define meniu 4   // pin for MENU button
#define minus 3   
#define plus 2

float R1 = 1.0;   // resistor from GNTD to pin A1
float R2 = 22.0;  // resisitor from A1 to measure voltage
float voltage, voltagetr, vtot;
int vs, vsm, vm, vu;
float trafo = 20;
float ud = 0.65;
float uref = 1.1;
float u1, u0;
float cor = 0.95;
//float cor = 1.05;

float under = 190.0;  // under voltage value
float over = 250.0;   // over voltage value
int intarziere = 10;  // value in second for delay after disconect relay because voltage is not ok
int intarziere2 = 1;  // value in second for delay after reading when voltage is ok
int nivel = 0;

void setup(){
 pinMode(A0, INPUT); 
 pinMode(meniu, INPUT); 
 pinMode(plus, INPUT); 
 pinMode(minus, INPUT); 
 pinMode(sub, OUTPUT);
 pinMode(bun, OUTPUT);
 pinMode(peste, OUTPUT);
 pinMode(releu, OUTPUT);

 digitalWrite(meniu, HIGH);  // put inputs in high state (when push is to ground -> low state)
 digitalWrite(plus, HIGH);
 digitalWrite(minus, HIGH);
 digitalWrite(sub, LOW);
 digitalWrite(bun, LOW);
 digitalWrite(peste, LOW); 
 digitalWrite(releu, LOW); 
  
analogReference(INTERNAL); // use AREF for reference voltage  
  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL); 
  //BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;

vtot = 0;
delay(2000);

}//end "setup()"
 
 
 
void loop(){     //Start of Program 

if (nivel == 0) {     // measurement state 
for (byte i=0; i < 10; i++){  
voltagetr = analogRead(A0);  // read voltage steps (0..1023)
vtot = vtot + voltagetr; 
delay(200);
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  i = 10;
  delay(500);
}    
}

voltagetr = vtot/10;
u1 = voltagetr*(R1+R2)*uref/R1;
u1 = u1/1024;
u0 = (u1 + ud)/1.41;
voltage = cor*u0*trafo;
vtot = 0;

// calculate number for each figure number
vs = voltage /100;
vsm = voltage - 100*vs;
vm = vsm /10;
vu = vsm - 10*vm;

    tm1637.display(3,16); // put -
 delay(50);
 
    tm1637.display(0,vs); 
    tm1637.display(1,vm);
    tm1637.display(2,vu); 
    tm1637.display(3,17); // put space 

if (digitalRead(meniu) == LOW) {
  nivel = 1;
  delay(500);
}    
}

if (nivel == 0) {     // measurement state 
if (voltage <= under)  // undervoltage
{
digitalWrite(sub, HIGH);
digitalWrite(bun, LOW);
digitalWrite(peste, LOW);
digitalWrite(releu, LOW);

for (byte j=0; j < intarziere; j++){  
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  j = intarziere;
  delay(500);
}    
delay(1000);
}
}
if (voltage >= over)  // overvoltage
{
digitalWrite(sub, LOW);
digitalWrite(bun, LOW);
digitalWrite(peste, HIGH);
digitalWrite(releu, LOW);
for (byte j=0; j < intarziere; j++){  
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  j = intarziere;
  delay(500);
}    
delay(1000);
}

}  
if ((voltage > under) && (voltage < over)) 
{ 
digitalWrite(sub, LOW);
digitalWrite(bun, HIGH);
digitalWrite(peste, LOW);
digitalWrite(releu, HIGH);
for (byte j=0; j < intarziere2; j++){  
if (digitalRead(meniu) == LOW) {
  nivel = 1;
  j = intarziere2;
  delay(500);
}    
delay(1000);
}

}
}    // end loop for measurement

if (nivel == 1) {    // for change the value
    tm1637.display(0,5);  // put 'S' (5) 
    tm1637.display(1,14); // put 'E
    tm1637.display(2,22); // put 't'
    tm1637.display(3,17); // put space 
delay(2000);
nivel = 2;
}

if (nivel == 2) {    // for change the undervoltage value
int unders = under / 100;
int underzu = under - 100*unders;
int underz = underzu/10;
int underu = underzu - 10*underz;

    tm1637.display(0,23);  // put 'u upper and _ bottom' 
    tm1637.display(1,unders); // put hundreds
    tm1637.display(2,underz); // put tens
    tm1637.display(3,underu); // put units 

if (digitalRead(meniu) == LOW) {
  nivel = 3;
  delay(500);
}    

if (digitalRead(plus) == LOW) {
  under = under + 1;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  under = under - 1;
  delay(250);
}    

}  // end loop for change undervoltage value   
   
if (nivel == 3) {    // for change the overrvoltage value
int overs = over / 100;
int overzu = over - 100*overs;
int overz = overzu/10;
int overu = overzu - 10*overz;

    tm1637.display(0,24);  // put 'u with -' 
    tm1637.display(1,overs); // put hundreds
    tm1637.display(2,overz); // put tens
    tm1637.display(3,overu); // put units 

if (digitalRead(meniu) == LOW) {
  nivel = 4;
  delay(500);
}    

if (digitalRead(plus) == LOW) {
  over = over + 1;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  over = over - 1;
  delay(250);
}    

}  // end loop for change overvoltage value   

if (nivel == 4) {    // for change delay time for nook voltage
int intarzierez = intarziere / 10;
int intarziereu = intarziere - 10*intarzierez;

    tm1637.display(0,22);  // put 't' 
    tm1637.display(1,17); // put space
    tm1637.display(2,intarzierez); // put tens
    tm1637.display(3,intarziereu); // put units 

if (digitalRead(meniu) == LOW) {
  nivel = 0;
  delay(500);
}    

if (digitalRead(plus) == LOW) {
  intarziere = intarziere + 1;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  intarziere = intarziere - 1;
  if (intarziere < 1) intarziere = 1;
  delay(250);
}    

}  // end loop for change delay value  for nook voltage
   
 
}// end loop() 
