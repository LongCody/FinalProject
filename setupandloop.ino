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
