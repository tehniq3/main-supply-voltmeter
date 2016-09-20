// base sketch from https://brainy-bits.com/tutorials/4-bits-7-segment-led-display-with-arduino/
// changed TM1637 library by niq_ro for degree, r & h letter
// main voltmeter on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/
// ver.0 - base schetch for see main voltage
// ver.1, change TM1637 pins, put 3 leds for state (under, ok, overvoltage)


#include "TM1637.h"
//{0,1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16,17,18,19};
//0~9,A,b,C,d,E,F,"-"," ",degree,r,h
#define CLK 7  //Pins for TM1637       
#define DIO 6
TM1637 tm1637(CLK,DIO);

#define sub 10    // pin for undervoltage led
#define bun 9     // pin for ok voltage led
#define peste 8   // pin for overvoltage led
#define releu 11  // pin for relay

float R1 = 1.0;   // resistor from GNTD to pin A1
float R2 = 22.0;  // resisitor from A1 to measure voltage
float voltage, voltagetr, vtot;
int vs, vsm, vm, vu;
float trafo = 20;
float ud = 0.65;
float uref = 1.1;
float u1, u0;
float cor = 0.97;

float under = 190.0;  // under voltage value
float over = 250.0;   // over voltage value

void setup(){
 pinMode(sub, OUTPUT);
 pinMode(bun, OUTPUT);
 pinMode(peste, OUTPUT);
 pinMode(releu, OUTPUT);

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
 
for (byte i=0; i < 10; i++){  
voltagetr = analogRead(A0);  // read voltage steps (0..1023)
vtot = vtot + voltagetr; 
delay(200);
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


if (voltage <= under)  // undervoltage
{
digitalWrite(sub, HIGH);
digitalWrite(bun, LOW);
digitalWrite(peste, LOW);
digitalWrite(releu, LOW);
delay(10000);
}  
if (voltage >= over)  // overvoltage
{
digitalWrite(sub, LOW);
digitalWrite(bun, LOW);
digitalWrite(peste, HIGH);
digitalWrite(releu, LOW);
delay(10000);
}  
if ((voltage > under) && (voltage < over)) 
{ 
digitalWrite(sub, LOW);
digitalWrite(bun, HIGH);
digitalWrite(peste, LOW);
digitalWrite(releu, HIGH);
delay (2000);
}

   
   
 
}// end loop() 
