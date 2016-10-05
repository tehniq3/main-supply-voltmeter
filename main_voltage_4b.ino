// base sketch from https://brainy-bits.com/tutorials/4-bits-7-segment-led-display-with-arduino/
// changed TM1637 library by niq_ro for degree, r & h letter
// main voltmeter on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/
// ver.0 - base schetch for see main voltage
// ver.1, change TM1637 pins, put 3 leds for state (under, ok, overvoltage)
// ver.2, put MENU, increase, decrease buttons
// ver.3, store value in EEPROM memory
// ver.4, put trafo with dual diode (full bridge)
// ver.4b, put test sequence...

#include "TM1637.h" // use changed library from https://github.com/tehniq3/TM1637-display/tree/master/TM1637
//{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
//0~9,A,b,C,d,E,F,"-"," ",degree,r,h,n,t, 
//A -> 10, b -> 11, C -> 12, d -> 13, E -> 14, F -> 15,
// "-" -> 16, space -> 17, degree -> 18, r -> 19, h -> 20, n -> 21
// t -> 22, "u with _" -> 23, "u with -" -> 24

#include <EEPROM.h> // http://tronixstuff.com/2011/03/16/tutorial-your-arduinos-inbuilt-eeprom/

#define CLK 9  //Pins for TM1637       
#define DIO 10
TM1637 tm1637(CLK,DIO);

#define sub 5    // pin for undervoltage led
#define bun 6     // pin for ok voltage led
#define peste 7   // pin for overvoltage led
#define releu 8  // pin for relay
#define meniu 2   // pin for MENU button
#define minus 3   
#define plus 4

float R1 = 1.0;   // resistor from GNTD to pin A0
float R2 = 33.0;  // resisitor from A1 to measure voltage
float voltage, voltagetr, vtot;
int vs, vsm, vm, vu;
float trafo = 19;
float ud = 1.3;   // are 2 diodes (full bridge)
float uref = 1.1;
float u1, u0;
float cor = 0.971;
//float cor = 1.05;

float under;  // under voltage value
float over;   // over voltage value
int intarziere;  // value in second for delay after disconect relay because voltage is not ok
int intarziere2 = 1;  // value in second for delay after reading when voltage is ok
int nivel = 0;
int underzu, overzu;
int under1 = 190; 
int under2 = 207;
int over1 = 240;
int over2 = 255;

// uncomment block first time, for write datas in EEPROM memory
/*
// under voltage is 190V ->
EEPROM.write(101,1);
EEPROM.write(102,9);
EEPROM.write(103,0);
// overvoltage is 250V ->
EEPROM.write(104,2);
EEPROM.write(105,5);
EEPROM.write(106,0);
// delay time for reconnect relay -> 9 seconds
EEPROM.write(107,0);
EEPROM.write(108,9);
*/

int unders = EEPROM.read(101);
int underz = EEPROM.read(102);
int underu = EEPROM.read(103);
int overs = EEPROM.read(104);
int overz = EEPROM.read(105);
int overu = EEPROM.read(106);
int intarzierez = EEPROM.read(107);
int intarziereu = EEPROM.read(108);


void setup(){
  // initialize serial:
  Serial.begin(9600); 
  // message on serial monitor
  Serial.println("--------");
  Serial.println("Under & overvoltage protection relay for main power supply ver.4.0");
  Serial.println("--------------");
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


under = 100*unders + 10*underz + underu;
if (under < under1) under = under1;
if (under > under2) under = under2;
over = 100*overs + 10*overz + overu;
if (over < over1) over = over1;
if (over > over2) over = over2;
intarziere = 10*intarzierez + intarziereu;
if (intarziere < 1) intarziere = 1;
if (intarziere > 20) intarziere = 20;

Serial.print("Umin = ");
Serial.print(under);
Serial.print("V Umax = ");
Serial.print(over);
Serial.print("V delay-off = ");
Serial.print(intarziere);
Serial.println("s");

// test sequence
    tm1637.display(0,22); // put 't'
    tm1637.display(1,14); // put 'E'
    tm1637.display(2,5);  // put 'S' (5) 
    tm1637.display(3,22); // put 't'    
digitalWrite(sub, HIGH);
digitalWrite(bun, LOW);
digitalWrite(peste, LOW);
delay(2000);
digitalWrite(sub, LOW);
digitalWrite(bun, HIGH);
digitalWrite(peste, LOW);
delay(2000);
digitalWrite(sub, LOW);
digitalWrite(bun, LOW);
digitalWrite(peste, HIGH);
delay(2000);
digitalWrite(sub, LOW);
digitalWrite(bun, LOW);
digitalWrite(peste, LOW);
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

 Serial.print("U = ");
 Serial.print(voltage);
 Serial.println("V ~");

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
delay(500);
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
unders = under / 100;
underzu = under - 100*unders;
underz = underzu/10;
underu = underzu - 10*underz;

    tm1637.display(0,23);  // put 'u upper and _ bottom' 
    tm1637.display(1,unders); // put hundreds
    tm1637.display(2,underz); // put tens
    tm1637.display(3,underu); // put units 

if (digitalRead(meniu) == LOW) {
  nivel = 3;
EEPROM.write(101,unders);
EEPROM.write(102,underz);
EEPROM.write(103,underu);
  delay(500);
}    

if (digitalRead(plus) == LOW) {
  under = under + 1;
  if (under > under2) under = under2;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  under = under - 1;
  if (under < under1) under = under1;
  delay(250);
}    

}  // end loop for change undervoltage value   
   
if (nivel == 3) {    // for change the overrvoltage value
overs = over / 100;
overzu = over - 100*overs;
overz = overzu/10;
overu = overzu - 10*overz;

    tm1637.display(0,24);  // put 'u with -' 
    tm1637.display(1,overs); // put hundreds
    tm1637.display(2,overz); // put tens
    tm1637.display(3,overu); // put units 

if (digitalRead(meniu) == LOW) {
  nivel = 4;
EEPROM.write(104,overs);
EEPROM.write(105,overz);
EEPROM.write(106,overu);
  delay(500);
}    

if (digitalRead(plus) == LOW) {
  over = over + 1;
  if (over > over2) over = over2;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  over = over - 1;
  if (over < over1) over = over1;
  delay(250);
}    

}  // end loop for change overvoltage value   

if (nivel == 4) {    // for change delay time for nook voltage
intarzierez = intarziere / 10;
intarziereu = intarziere - 10*intarzierez;

    tm1637.display(0,22);  // put 't' 
    tm1637.display(1,17); // put space
    tm1637.display(2,intarzierez); // put tens
    tm1637.display(3,intarziereu); // put units 

if (digitalRead(meniu) == LOW) {
  nivel = 0;
EEPROM.write(107,intarzierez);
EEPROM.write(108,intarziereu);
  delay(500);
}    

if (digitalRead(plus) == LOW) {
  intarziere = intarziere + 1;
  if (intarziere > 20) intarziere = 20;
  delay(250);
}    

if (digitalRead(minus) == LOW) {
  intarziere = intarziere - 1;
  if (intarziere < 1) intarziere = 1;
  delay(250);
}    

}  // end loop for change delay value  for nook voltage
   
 
}// end loop() 
