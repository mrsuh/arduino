#include <AccelStepper.h>

const int stepsPerRevolution = 2048;  // change this to fit the number of steps per revolution

// ULN2003 Motor Driver Pins
#define IN1 5
#define IN2 4
#define IN3 14
#define IN4 12

// initialize the stepper library
AccelStepper stepper(AccelStepper::HALF4WIRE, IN1, IN3, IN2, IN4);

void setup() {
  // initialize the serial port
  Serial.begin(115200);  
  stepper.setMaxSpeed(100);
  stepper.setAcceleration(50);
}

int value = 0;
void loop() {
  if (Serial.available()) {
     value = Serial.parseInt();
     if(value > 0) {
      Serial.println(value);
      stepper.move(value);
     }
  }
  
  if (stepper.distanceToGo() > 0) {
    stepper.run();
  } 
}
