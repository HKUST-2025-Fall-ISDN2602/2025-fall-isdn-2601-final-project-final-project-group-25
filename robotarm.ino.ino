#include <Servo.h>

Servo servo;

void setup() {
  // put your setup code here, to run once:
servo.attach(PWMpin); // attach the servo (a variable of type Servo) to a PWM pin on ESP8266
servo.write(angle); // servo is set to a position angle (a value to write to the servo, from 0 to 180)
}

void loop() {
  // put your main code here, to run repeatedly:

}
