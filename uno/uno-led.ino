#define LED1 16
#define LED2 17
#define LED3 5
#define LED4 18

#define CTR1 33 // left y
#define CTR2 25 // left x
#define CTR3 26 // left potenc
#define CTR4 27 // right x
#define CTR5 14 // right y
#define CTR6 12 // right potenc

void blink(int pin, int delayMSec) {
  digitalWrite(pin, HIGH);
  delay(delayMSec);             
  digitalWrite(pin, LOW);  
}


void setup() {
  Serial.begin(115200);
  
  pinMode(LED1, OUTPUT);
  pinMode(LED2, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);

  pinMode(CTR1, INPUT);
  pinMode(CTR2, INPUT);
  pinMode(CTR3, INPUT);
  pinMode(CTR4, INPUT);
  pinMode(CTR5, INPUT);
  pinMode(CTR6, INPUT);
  
}

void loop() {
  int ctr1 = analogRead(CTR1);
  int ctr2 = analogRead(CTR2);
  int ctr3 = analogRead(CTR3);
  int ctr4 = analogRead(CTR4);
  int ctr5 = analogRead(CTR5);
  int ctr6 = analogRead(CTR6);
  
  Serial.printf("ctr1:%d ctr2:%d ctr3:%d ctr4:%d ctr5:%d ctr6:%d\n", ctr1, ctr2, ctr3, ctr4, ctr5, ctr6);

  int delayMsec = map(ctr3, 0, 4100, 0, 1000);

  blink(LED1, delayMsec);  
  blink(LED2, delayMsec);
  blink(LED3, delayMsec);
  blink(LED4, delayMsec);
}
