#include <Settings.h>
#include <Servo.h>
int pin = BRAKE_OUT_PIN;
Servo STEER_SERVO;
void setup() {
  STEER_SERVO.attach(pin); 
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(pin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
//  Serial.println("here");

    Serial.println("ON");
    STEER_SERVO.writeMicroseconds(1515);
    delay(1500);
    Serial.println("OFF");
    STEER_SERVO.writeMicroseconds(1300);
    delay(1500);
    // 1300 full off
    // 1515 full on
}
