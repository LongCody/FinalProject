int readSensor() {
  *port_h |= 0b00001000;
	// digitalWrite(sensorPower, HIGH);	- Turn the sensor ON

	delay(10);							      // wait 10 milliseconds
	val = analogRead(sensorPin);	// Read the analog value form sensor
	
  *port_h &= 0b11110111;
  // digitalWrite(sensorPower, LOW); - Turn the sensor OFF
	return val;	// send current reading
}