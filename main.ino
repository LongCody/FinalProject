#include <Stepper.h>

#include <dht.h>
#include <LiquidCrystal.h>
#include <Stepper.h>
#define sensorPower 6
#define sensorPin A0
#define ENABLE 33
#define DIRA 37
#define DIRB 35
#define DHT11_PIN 7
#define WATER_LEVEL_THRESHOLD 100
#define TEMPERATURE_THRESHOLD 20


#define WRITE_HIGH_PB(pin_num)  *port_a |= (0x01 << pin_num);
#define WRITE_LOW_PB(pin_num)  *port_a &= ~(0x01 << pin_num);

//Assigning pins and ports and sorts and stuff
volatile unsigned char* ddr_h  = (unsigned char*) 0x101;  // sensor power output
volatile unsigned char* port_h  = (unsigned char*) 0x102; // sensor power low/high

volatile unsigned char* ddr_c  = (unsigned char*) 0x27;   // ENABLE, DIRA, DIRB
volatile unsigned char* port_c  = (unsigned char*) 0x28;  // low/high
volatile unsigned char* pin_c = (unsigned char*) 0x26; //pin stuff

volatile unsigned char* ddr_g  = (unsigned char*) 0x33;   // STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_4
volatile unsigned char* port_g  = (unsigned char*) 0x34;  // low/high

volatile unsigned char* ddr_d  = (unsigned char*) 0x2A;   // STEPPER_PIN_2, STEPPER_PIN_3, STEPPER_PIN_4
volatile unsigned char* port_d = (unsigned char*) 0x2B;  // low/high

volatile unsigned char* ddr_l  = (unsigned char*) 0x10A;  // STEPPER_PIN_3, STEPPER_PIN_4
volatile unsigned char* port_l  = (unsigned char*) 0x10B; // low/high

volatile unsigned char* ddr_a  = (unsigned char*) 0x21;  // LEDS PA0-PA3
volatile unsigned char* port_a  = (unsigned char*) 0x22;
volatile unsigned char* pin_a = (unsigned char*) 0x20;

// Value for storing water level
int val = 0;
int i;
int step_number = 0;
int stepPerRev = 2048;
int motSpeed = 10;
int dt = 500;
boolean fan_on = false;
Stepper myStep(stepPerRev, 38,40,39,41);
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);
dht DHT;

#define DHT11_PIN 7

//FLags for the different states
enum states {
   disabled = 0, // yellow LED
   idle = 1,     // green LED
   running = 2,  // blue LED
   error = 3     // red LED
};

enum states stat = disabled;

void setup(){
  lcd.begin(16, 2);

  ///// SENSOR POWER /////
  *ddr_h |= 0b00001000;
  // set pin to low
  *port_h &= 0b11110111;

  *port_a |= 0b00100000;



  ///// ENABLE /////
  *ddr_c |= 0b00010000;

  ///// DIRA /////
  *ddr_c |= 0b00000001;

  ///// DIRB /////
  *ddr_c |= 0b00000100;
  
  ///// STEPPER_PIN_2, 3, and 4 /////
  *ddr_g |= 0b00000111;

  ///// STEPPER_PIN_1 /////
  *ddr_d |= 0b10000000;

  ///// STEPPER_PIN_3 /////
  *ddr_l |= 0b10000000;

  ///// STEPPER_PIN_4 /////
  *ddr_l |= 0b01000000;
	
  ///// LED PA0-3 set /////
  *ddr_a |= 0b00001111;

  *ddr_a &= 0b11101111;

  step_number = 0;

  myStep.setSpeed(motSpeed);
  

	Serial.begin(9600);
}

void loop(){
  

switch(stat) {
    case disabled:
      Serial.println("Disabled State");
      disabled_state();
      break;
    case idle:
      Serial.println("Idle State");
      idle_state();
      break;
    case error:
      Serial.println("Error State");
      error_state();
      break;
    case running:
      Serial.println("Running State");
      running_state();
      break;
    default:
      break;
  }

  //myStep.step(-stepPerRev/2);
    Serial.print( "T = " );
    Serial.print( DHT.temperature, 1 );
    Serial.print( " deg. C, H = " );
    Serial.print( DHT.humidity, 1 );
    Serial.println( "%" );

  delay(1000);

  //get the reading from the function below and print it
	int level = readSensor();
	
	Serial.print("Water level: ");
	Serial.println(level);
	
	delay(1000);
}


int readSensor() {
  *port_h |= 0b00001000;
	// digitalWrite(sensorPower, HIGH);	- Turn the sensor ON

	delay(10);							      // wait 10 milliseconds
	val = analogRead(sensorPin);	// Read the analog value form sensor
	
  *port_h &= 0b11110111;
  // digitalWrite(sensorPower, LOW); - Turn the sensor OFF
	return val;	// send current reading
}


void disabled_state(){
  *port_a |= 0b00000010;
  *port_a &= 0b11110010;
  *port_c &= 0b11111110;
  *port_c |= 0b00000100;
  int chk = DHT.read11(DHT11_PIN);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
  while(step_number>0){
    myStep.step(-1);
    step_number--;
  }
  while((*pin_a & 0b00100000) == 0){}
  stat = idle;
}

void idle_state(){
  // turn on green LED
  // turn off other LED's and the fan
  *port_a |= 0b00000100;
  *port_a &= 0b11110100;
  *port_c &= 0b11111110;
  *port_c |= 0b00000100;
  int level = readSensor();
  int chk = DHT.read11(DHT11_PIN);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
  if(level < WATER_LEVEL_THRESHOLD) stat = error;
  else if( DHT.temperature > TEMPERATURE_THRESHOLD) stat = running;
  while(step_number>512){
    myStep.step(-1);
    step_number--;
  }
  while(step_number<512){
    myStep.step(1);
    step_number++;
  }
}

void running_state(){
  *port_a |= 0b00000001;
  *port_a &= 0b11110001;
  int level = readSensor();
  if(level < WATER_LEVEL_THRESHOLD) stat = error;
  else if(DHT.temperature > TEMPERATURE_THRESHOLD){
delay(1000);
int chk = DHT.read11(DHT11_PIN);
  lcd.setCursor(0,0); 
  lcd.print("Temp: ");
  lcd.print(DHT.temperature);
  lcd.print((char)223);
  lcd.print("C");
  lcd.setCursor(0,1);
  lcd.print("Humidity: ");
  lcd.print(DHT.humidity);
  lcd.print("%");
     for (i=255;i>100;i--) {
  *port_c |= 0b00000001;
  // digitalWrite(DIRA,HIGH); //one way

  *port_c &= 0b11111011;
  // digitalWrite(DIRB,LOW);*/


  analogWrite(ENABLE,i); //enable on
  delay(40);
  }
return running_state();    
  }
  else{
    stat = idle;
  }

  while(step_number<1024){
    myStep.step(1);
    step_number++;
  }
  while(step_number>1024){
    myStep.step(-1);
    step_number--;
  }
   for (i=255;i>100;i--) {
  *port_c |= 0b00000001;
  // digitalWrite(DIRA,HIGH); //one way

  *port_c &= 0b11111011;
  // digitalWrite(DIRB,LOW);*/


  analogWrite(ENABLE,i); //enable on
  delay(5);
  }
}

void error_state(){
  *port_a |= 0b00001000;
  *port_a &= 0b11111000;
  *port_c &= 0b11111110;
  *port_c |= 0b00000100;
   int chk = DHT.read11(DHT11_PIN);
  lcd.clear();
  lcd.print("Low Water");
  int level = readSensor();
  while(level < WATER_LEVEL_THRESHOLD){
    delay(1000);
    level = readSensor();
    lcd.setCursor(0,1);
    lcd.print("Level: ");
    lcd.setCursor(7, 1);
    lcd.print(level);
  }
  stat = idle;
  while(step_number<1536){
    myStep.step(1);
    step_number++;
  }
}