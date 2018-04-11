// main voltmeter on TM1637 LED display by niq_ro from http://www.tehnic.go.ro
// & http://arduinotehniq.com/
// ver.0 - base schetch for see main voltage
// ver.1, change TM1637 pins, put 3 leds for state (under, ok, overvoltage)
// ver.1a1 - eject TM1637 display, display on Serial monitor

#define sub 10    // pin for undervoltage led
#define bun 9     // pin for ok voltage led
#define peste 8   // pin for overvoltage led
#define releu 11  // pin for relay

float R1 = 1.0;   // resistor from GND to pin A1
float R2 = 22.0;  // resisitor from A1 to measure voltage
float voltage, voltagetr, vtot;
int vs, vsm, vm, vu;
float trafo = 20;
float ud = 0.65;
float uref = 1.1;
float u1, u0;
//float cor = 0.95;
float cor = 1.05;

float under = 190.0;  // under voltage value
float over = 250.0;   // over voltage value

int timplung = 10000; // time (ms) for over and undervoltage
int timpscurt = 2000; // time(ms) for ok voltage

void setup(){
 Serial.begin(9600);
  
 pinMode(A0, INPUT); 
 pinMode(sub, OUTPUT);
 pinMode(bun, OUTPUT);
 pinMode(peste, OUTPUT);
 pinMode(releu, OUTPUT);

 digitalWrite(sub, LOW);
 digitalWrite(bun, LOW);
 digitalWrite(peste, LOW); 
 digitalWrite(releu, LOW); 
  
analogReference(INTERNAL); // use AREF for reference voltage  
  
vtot = 0;
Serial.println();
Serial.println("-----------");
Serial.println("Under & overvoltage relay for AC main power supply");
Serial.println("original program by Nicu FLORICA (niq_ro)");
Serial.println("-----------");
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

Serial.println("-----------");
Serial.print("Main Voltage = ");
Serial.print(voltage);
Serial.print("V -> ");


if (voltage <= under)  // undervoltage
{
Serial.print(" undervoltage (");
Serial.print(voltage);
Serial.print(" < ");
Serial.print(under);
Serial.println("V");
digitalWrite(sub, HIGH);
digitalWrite(bun, LOW);
digitalWrite(peste, LOW);
digitalWrite(releu, LOW);
delay(timplung);
}  
if (voltage >= over)  // overvoltage
{
Serial.print(" overvoltage (");
Serial.print(voltage);
Serial.print(" > ");
Serial.print(over);
Serial.println("V");
digitalWrite(sub, LOW);
digitalWrite(bun, LOW);
digitalWrite(peste, HIGH);
digitalWrite(releu, LOW);
delay(timplung);
}  
if ((voltage > under) && (voltage < over)) 
{ 
Serial.println(" ok"); 
digitalWrite(sub, LOW);
digitalWrite(bun, HIGH);
digitalWrite(peste, LOW);
digitalWrite(releu, HIGH);
delay (timpscurt);
}
 
}// end loop() 
