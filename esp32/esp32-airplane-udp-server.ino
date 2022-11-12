#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>

#define POWER_LED 5
#define INIT_LED 16
#define PING_LED 18
#define ERR_LED 19

#define CTR1 39 // left x @todo
#define CTR2 36 // left y

#define CTR3 35 // right x
#define CTR4 34 // right y

#define CTR5 32 // right potenc
#define CTR6 33 // right potenc

typedef struct _udp_packet {
  int x1;
  int y1;
  int x2;
  int y2;
  int r1;
  int r2;
} UDP_PACKET;

const char* ssid = "esp32-udp";
const char* password = "password";

WiFiUDP Udp;
unsigned int udpPort = 4210;

IPAddress serverIp(192, 168, 4, 1);
IPAddress clientIp(192, 168, 4, 2);
IPAddress serverGateway(192, 168, 4, 1);
IPAddress serverSubnet(255, 255, 255, 0);

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println("Initializing");

  pinMode(POWER_LED, OUTPUT);
  pinMode(INIT_LED, OUTPUT);
  pinMode(PING_LED, OUTPUT);
  pinMode(ERR_LED, OUTPUT);

  pinMode(CTR1, INPUT);
  pinMode(CTR2, INPUT);
  pinMode(CTR3, INPUT);
  pinMode(CTR4, INPUT);
  pinMode(CTR5, INPUT);
  pinMode(CTR6, INPUT);

  digitalWrite(POWER_LED, HIGH);

  if(!WiFi.mode(WIFI_AP)) {
    Serial.println("WiFi.mode(WIFI_AP) failed");
  }

  if(!WiFi.softAPConfig(serverIp, serverGateway, serverSubnet)) {
    Serial.println("WiFi.softAPConfig failed");
  }

  if(!WiFi.softAP(ssid, password)) {
    Serial.println("WiFi.softAP(ssid, password) failed");
  }

  Serial.println("WIFI AP Initialized!");

  Udp.begin(udpPort);
  Serial.printf("UDP Initialized!");

  digitalWrite(INIT_LED, HIGH);
}

int param_x1 = 0;
int param_y1 = 0;
int param_r1 = 0;
int param_x2 = 0;
int param_y2 = 0;
int param_r2 = 0;

int param_x1_tmp = 0;
int param_y1_tmp = 0;
int param_r1_tmp = 0;
int param_x2_tmp = 0;
int param_y2_tmp = 0;
int param_r2_tmp = 0;

double ReadVoltage(byte pin){
  double reading = analogRead(pin); // Reference voltage is 3v3 so maximum reading is 3v3 = 4095 in range 0 to 4095
  if(reading < 1 || reading > 4095) return 0;
  // return -0.000000000009824 * pow(reading,3) + 0.000000016557283 * pow(reading,2) + 0.000854596860691 * reading + 0.065440348345433;
  return -0.000000000000016 * pow(reading,4) + 0.000000000118171 * pow(reading,3)- 0.000000301211691 * pow(reading,2)+ 0.001109019271794 * reading + 0.034143524634089;
}

double multipleAnalogRead(byte pin, int multiple = 64) {
  double total = 0;
  for(int i = 0; i < multiple; i++) {
    total += analogRead(pin);
  }

  return total/multiple;
}


void loop()
{
  if(Udp.beginPacket(clientIp, udpPort) == 0) {
    Serial.println("Udp.beginPacket error");
    digitalWrite(ERR_LED, HIGH);
    delay(500);
    digitalWrite(ERR_LED, LOW);
    return;
  }

  param_x1_tmp = map(multipleAnalogRead(CTR1), 0, 4095, 0, 40);
  param_y1_tmp = map(multipleAnalogRead(CTR2), 0, 4095, 0, 40);

  param_x2_tmp = map(multipleAnalogRead(CTR3), 0, 4095, 0, 40);
  param_y2_tmp = map(multipleAnalogRead(CTR4), 0, 4095, 0, 40);

  param_r1_tmp = map(multipleAnalogRead(CTR5), 0, 4095, 0, 100);
  param_r2_tmp = map(multipleAnalogRead(CTR6), 0, 4095, 0, 100);

  if(abs(param_x1 - param_x1_tmp) > 1) {
    param_x1 = param_x1_tmp;
  }

  if(abs(param_y1 - param_y1_tmp) > 1) {
    param_y1 = param_y1_tmp;
  }

  if(abs(param_r1 - param_r1_tmp) > 1) {
    param_r1 = param_r1_tmp;
  }

  if(abs(param_x2 - param_x2_tmp) > 1) {
    param_x2 = param_x2_tmp;
  }

  if(abs(param_y2 - param_y2_tmp) > 1) {
    param_y2 = param_y2_tmp;
  }

  if(abs(param_r2 - param_r2_tmp) > 1) {
    param_r2 = param_r2_tmp;
  }

  UDP_PACKET packet = {param_x1, param_y1, param_x2, param_y2, param_r1, param_r2};
  Serial.printf("x1:%d y1:%d x2:%d y2:%d r1:%d r2:%d\n", param_x1, param_y1, param_x2, param_y2, param_r1, param_r2);

  Udp.write((byte *)&packet, sizeof(UDP_PACKET));

  if(Udp.endPacket() == 0) {
    Serial.println("Udp.endPacket() error");
    digitalWrite(ERR_LED, HIGH);
    delay(500);
    digitalWrite(ERR_LED, LOW);
    return;
  }

  digitalWrite(PING_LED, HIGH);
  delay(10);
  digitalWrite(PING_LED, LOW);
  delay(10);
}
