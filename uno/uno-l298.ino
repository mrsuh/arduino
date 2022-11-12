
#define PIN_ENA 2 // Вывод управления скоростью вращения мотора №1
#define PIN_IN1 3 // Вывод управления направлением вращения мотора №1
#define PIN_IN2 4 // Вывод управления направлением вращения мотора №1

void setup() {

  Serial.begin(9600);
  
  pinMode(PIN_ENA, OUTPUT);

  pinMode(PIN_IN1, OUTPUT);

  pinMode(PIN_IN2, OUTPUT);

  digitalWrite(PIN_IN1, LOW);

  digitalWrite(PIN_IN2, LOW);

  Serial.println("READY!");

}

long int  value = 0;

void loop() {

if (Serial.available()) {
     value = Serial.parseInt();

     if(value == 0) {
      digitalWrite(PIN_IN1, LOW);
      return;
     }

     digitalWrite(PIN_IN1, HIGH);
     
     Serial.print(value);
     Serial.print("\n");
     //analogWrite(PIN_ENA, value);
  }
}
