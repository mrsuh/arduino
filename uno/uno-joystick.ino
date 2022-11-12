void setup()
{
  Serial.begin(115200);
  pinMode(25, INPUT);
  pinMode(32, INPUT);
  pinMode(33, INPUT);
  pinMode(35, INPUT);
}

void loop()
{
  int x1 = analogRead(25);
  int y1 = analogRead(33);
  int x2 = analogRead(32);
  int y2 = analogRead(35);
  Serial.printf("x1:%d y1:%d | x2:%d y2:%d\n", x1, y1, x2, y2);

  delay(200);
}
