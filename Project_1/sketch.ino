bool blink_state_yellow = LOW;
int last_button_state = HIGH;
int first_led_state = HIGH;
long prev_mils_yellow = 0;
int button_state = LOW;
bool blink_state = LOW;
int LED_state = LOW;
long prev_mils = 0;

enum Mode 
{
  BONUS_1, 
  MANDATORY,
  BONUS_2,
};

Mode current_mode = MANDATORY;

void setup() {
  pinMode(13, OUTPUT);
  pinMode(12, OUTPUT);
  pinMode(11, OUTPUT);
  pinMode(2, INPUT);
}

void mandatory(long current_mils)
{
  if(current_mils - prev_mils >= 500)
  {
    prev_mils = current_mils;
    blink_state = !blink_state;
    digitalWrite(12, blink_state);
    digitalWrite(11, blink_state);
  }
}

void bonus_1(long current_mils)
{
  if(current_mils - prev_mils >= 1000)
  {
    prev_mils = current_mils;
    blink_state = !blink_state;
    digitalWrite(12, blink_state);
    if(blink_state == LOW)
      digitalWrite(11, HIGH);
    else
      digitalWrite(11, LOW);
  }
}

void bonus_2(long current_mils)
{
  if(current_mils - prev_mils >= 250)
  {
    prev_mils = current_mils;
    blink_state = !blink_state;
    digitalWrite(12, blink_state);
  }
  if(current_mils - prev_mils_yellow >= 1000)
  {
    prev_mils_yellow = current_mils;
    blink_state_yellow = !blink_state_yellow;
    digitalWrite(11, blink_state_yellow);
  }
}

void switch_mode() 
{
  button_state = digitalRead(2);

  if(button_state != last_button_state || first_led_state == HIGH)
  {
    delay(50);

    if(button_state == HIGH || first_led_state == HIGH)  
    {
      LED_state = !LED_state;
      digitalWrite(13, LED_state);
      first_led_state = LOW;
      // current_mode = static_cast<Mode>((current_mode + 1) % 3);
    }
  }
  last_button_state = button_state;
}

void loop() 
{
  switch_mode();

  long current_mils = millis();

  if (LED_state == LOW) 
  {
    switch (current_mode) 
    {
      case MANDATORY:
        mandatory(current_mils); break;
      case BONUS_1:
        bonus_1(current_mils); break;
      case BONUS_2:
        bonus_2(current_mils); break;
    }
  }
  else
  {
    digitalWrite(11, LOW);
    digitalWrite(12, LOW);
  }
}
