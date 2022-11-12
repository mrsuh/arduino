#define PIN_IN1 16
#define PIN_IN2 5
#define PIN_IN3 4
#define PIN_IN4 0

#define PIN_IN5 14
#define PIN_IN6 12
#define PIN_IN7 13
#define PIN_IN8 15

#define PIN_ECHO 9
#define PIN_TRIG 10


void setup() {

  Serial.begin(115200);
  
  pinMode(PIN_IN1, OUTPUT);
  pinMode(PIN_IN2, OUTPUT);
  pinMode(PIN_IN3, OUTPUT);
  pinMode(PIN_IN4, OUTPUT);
  pinMode(PIN_IN5, OUTPUT);
  pinMode(PIN_IN6, OUTPUT);
  pinMode(PIN_IN7, OUTPUT);
  pinMode(PIN_IN8, OUTPUT);

  digitalWrite(PIN_IN1, LOW);
  digitalWrite(PIN_IN2, LOW);
  digitalWrite(PIN_IN3, LOW);
  digitalWrite(PIN_IN4, LOW); 
  digitalWrite(PIN_IN5, LOW);
  digitalWrite(PIN_IN6, LOW);
  digitalWrite(PIN_IN7, LOW);
  digitalWrite(PIN_IN8, LOW); 

  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  
  Serial.println("READY!\n");
}

long int  value = 0;
long duration, cm;

void loop() {

  // Сначала генерируем короткий импульс длительностью 2-5 микросекунд.

  digitalWrite(PIN_TRIG, LOW);
  delayMicroseconds(5);
  digitalWrite(PIN_TRIG, HIGH);

  // Выставив высокий уровень сигнала, ждем около 10 микросекунд. В этот момент датчик будет посылать сигналы с частотой 40 КГц.
  delayMicroseconds(10);
  digitalWrite(PIN_TRIG, LOW);

  //  Время задержки акустического сигнала на эхолокаторе.
  duration = pulseIn(PIN_ECHO, HIGH);

  // Теперь осталось преобразовать время в расстояние
  cm = (duration / 2) / 29.1;

  Serial.print("Расстояние до объекта: ");
  Serial.print(cm);
  Serial.println(" см.");

if (Serial.available()) {
     value = Serial.parseInt();

     if(value < 0) {
      Serial.printf("OFF\n");
      digitalWrite(PIN_IN1, LOW);
      digitalWrite(PIN_IN2, LOW);
      digitalWrite(PIN_IN3, LOW);
      digitalWrite(PIN_IN4, LOW);
      digitalWrite(PIN_IN5, LOW);
      digitalWrite(PIN_IN6, LOW);
      digitalWrite(PIN_IN7, LOW);
      digitalWrite(PIN_IN8, LOW);
      
     }

     if(value > 0) {
      Serial.printf("ON\n");
      digitalWrite(PIN_IN1, HIGH);
      digitalWrite(PIN_IN2, LOW);
      digitalWrite(PIN_IN3, HIGH);
      digitalWrite(PIN_IN4, LOW);
      digitalWrite(PIN_IN5, HIGH);
      digitalWrite(PIN_IN6, LOW);
      digitalWrite(PIN_IN7, HIGH);
      digitalWrite(PIN_IN8, LOW);
      
     }

     Serial.printf("Sending %d\n", value);
 }
 delay(250);
}
