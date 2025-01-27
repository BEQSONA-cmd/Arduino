#include <LiquidCrystal.h>

int brightness = 256;
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void bonus()
{
  char receivedChar = Serial.read();
  if (receivedChar == 'u' || receivedChar == 'd') 
  {
    if(receivedChar == 'u')
      brightness += 5;
    else
      brightness -= 5;
    Serial.print("You typed: ");
    Serial.println(receivedChar);
  }
}

void mandatory()
{
  int sensorValue = analogRead(A0);
  float volt = sensorValue * (5.0 / 1023.0);
  brightness = sensorValue / 4;

  Serial.println(brightness);
}

void setup() {
  lcd.begin(16, 2);
  Serial.begin(9600);
}


void loop() 
{
  // bonus();
  mandatory();

  if(brightness > 255)
    brightness = 255;
  else if(brightness < 0)
    brightness = 0;
  analogWrite(10, brightness);

  lcd.clear();
  lcd.print("Brightness: ");
  lcd.print(brightness);

  
  int squares = (brightness + 1) / 16;
  while(squares--)
  {
    lcd.setCursor(squares, 1);
    lcd.write(byte(255));
  }
  delay(10);
}
