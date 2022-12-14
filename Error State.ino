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