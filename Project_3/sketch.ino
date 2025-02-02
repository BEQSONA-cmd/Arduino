  int light[2];
  light[0] = analogRead(A0);
  light[1] = analogRead(A1);
  // light[2] = analogRead(A2);

  for(int i = 0; i < 2; i++)
  {
    Serial.print("Light number: ");
    Serial.print(i);
    Serial.print("(");
    Serial.print(light[i]);
    Serial.print(")\n");
  }
  delay(1000);
