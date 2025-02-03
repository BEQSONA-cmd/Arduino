#include <Servo.h>

Servo servo1;

int angle = 0;
int servoPin =  3;
int photo1Pin = A0;
int photo2Pin = A1;

void setup() 
{
    Serial.begin(9600);
    pinMode(servoPin, OUTPUT);
    servo1.attach(servoPin);
}

void loop() 
{
    int photo1 = analogRead(photo1Pin);
    int photo2 = analogRead(photo2Pin);

    int diff = photo1 - photo2;
    if (diff > 5)
        angle -= 2;
    if (diff < -5)
        angle += 2;

    if (angle > 179)
        angle = 179;

    if (angle < 40)
        angle = 40;

    servo1.write(angle);

    delay(10);
}

