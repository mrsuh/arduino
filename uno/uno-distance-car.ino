#include <Servo.h>
#include <Wire.h>
#include <VL53L0X.h>

int servoPin = 11;
int ledPin = 13;

int sonicEchoPin = 10;
int sonicTrigPin = 9;

int enA = 5;
int enIn1 = 2;
int enIn2 = 3;

int enB = 6;
int enIn3 = 4;
int enIn4 = 7;

VL53L0X sensor;
Servo servo;

// the setup routine runs once when you press reset:
void setup() {
  // initialize the digital pin as an output.
  pinMode(ledPin, OUTPUT);
  servo.attach(servoPin);
  Serial.begin(9600);
  pinMode(sonicTrigPin, OUTPUT);
  pinMode(sonicEchoPin, INPUT);

  pinMode(enA, OUTPUT);
  pinMode(enB, OUTPUT);
  pinMode(enIn1, OUTPUT);
  pinMode(enIn2, OUTPUT);
  pinMode(enIn3, OUTPUT);
  pinMode(enIn4, OUTPUT);

  Wire.begin();
  sensor.init();
  sensor.setTimeout(500);
  sensor.setMeasurementTimingBudget(200000);

  delay(4000);
}

int compare(const void* a, const void* b) {
  return *((int*)a) - *((int*)b);
}

int getDistance() {
  int duration, cm;
  digitalWrite(sonicTrigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(sonicTrigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(sonicTrigPin, LOW);
  duration = pulseIn(sonicEchoPin, HIGH, 500);
  cm = duration / 58;
  return cm;
}

int getLaserDistance() {
  return ceil(sensor.readRangeSingleMillimeters() / 10);
}

int getMathDistance() {

  int count = 10;
  int measures[count];

  for (int i = 0; i < count; i++) {
    measures[i] = getDistance();
  }

  qsort(measures, count, sizeof(int), compare);

  return measures[count - 2];
}

void engineForward(int delaySec) {
  digitalWrite(enIn1, LOW);
  digitalWrite(enIn2, HIGH);
  digitalWrite(enIn3, LOW);
  digitalWrite(enIn4, HIGH);

  analogWrite(enA, 180);
  analogWrite(enB, 180);

  int distance = 0;
  if (delaySec == 0) {
    int delayCurrent = 0;
    int delayMax = 3000;
    while (true) {
      delay(200);
      distance = getLaserDistance();
      if (distance < 40) {
        break;
      }
      delayCurrent += 200;
      if (delayCurrent > delayMax) {
        break;
      }
    }
  } else {
    delay(delaySec);
  }

  digitalWrite(enIn1, LOW);
  digitalWrite(enIn2, LOW);
  digitalWrite(enIn3, LOW);
  digitalWrite(enIn4, LOW);
}

void engineRotateRight(int delaySec) {
  digitalWrite(enIn1, LOW);
  digitalWrite(enIn2, HIGH);

  digitalWrite(enIn3, HIGH);
  digitalWrite(enIn4, LOW);

  analogWrite(enA, 200);
  analogWrite(enB, 200);

  delay(delaySec);

  digitalWrite(enIn1, LOW);
  digitalWrite(enIn2, LOW);
  digitalWrite(enIn3, LOW);
  digitalWrite(enIn4, LOW);
}

void engineRotateLeft(int delaySec) {
  digitalWrite(enIn1, HIGH);
  digitalWrite(enIn2, LOW);

  digitalWrite(enIn3, LOW);
  digitalWrite(enIn4, HIGH);

  analogWrite(enA, 200);
  analogWrite(enB, 200);

  delay(delaySec);

  digitalWrite(enIn1, LOW);
  digitalWrite(enIn2, LOW);
  digitalWrite(enIn3, LOW);
  digitalWrite(enIn4, LOW);
}

int getHighestDistancePostition() {
  int highestPosition = 0;
  int highestDistance = 0;
  for (int pos = 0; pos <= 180; pos += 45) {
    servo.write(pos);
    delay(500);
    int distance = getLaserDistance();
    char buffer[100];
    sprintf(buffer, "Servo position: %d, measure: %d cm\n", pos, distance);
    Serial.print(buffer);
    if (distance > highestDistance) {
      highestDistance = distance;
      highestPosition = pos;
    }
    delay(200);
  }

  servo.write(90);
  delay(200);

  if (highestDistance < 15) {
    return -1;
  }

  return highestPosition;
}

int getSmartDistance() {
  int laserDistance = getLaserDistance();
  int sonicDistance = getMathDistance();

  if(sonicDistance == 0) {
    return laserDistance;
  }

  if (laserDistance < sonicDistance) {
    return laserDistance;
  }

  return sonicDistance;
}

void rotate() {
  int ratoteDuration45 = 170;
  int highestPosition = getHighestDistancePostition();
  switch (highestPosition) {
    case 0:
      engineRotateRight(ratoteDuration45 * 2);
      break;
    case 45:
      engineRotateRight(ratoteDuration45);
      break;
    case 90:
      break;
    case 135:
      engineRotateLeft(ratoteDuration45);
      break;
    case 180:
      engineRotateLeft(ratoteDuration45 * 2);
      break;
    case -1:
      engineRotateLeft(ratoteDuration45 * 4);
      break;
  }
}

int engineRotateSmart(int left) {

  int currentDistance = getSmartDistance();
  int currentDelay = 0;
  int maxDelay = 3000;
  while (true) {
    if (left == 1) {
      engineRotateLeft(200);
    } else {
      engineRotateRight(200);
    }
    currentDelay+=200;

    int distance = getSmartDistance();
    if (distance > 150) {
      return 0;
    }

    if (currentDelay > maxDelay) {
      return -1;
    }
  }
  return 0;
}

// the loop routine runs over and over again forever:
void loop() {
  int success = engineRotateSmart(1);
  if (success < 0) {
    delay(500);
    engineRotateSmart(0);
  }
  delay(500);
  engineForward(0);
  delay(500);
}
