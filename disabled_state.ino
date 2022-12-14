enum states {
   disabled = 0, // yellow LED
   idle = 1,     // green LED
   running = 2,  // blue LED
   error = 3     // red LED
};

enum states stat = disabled;

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