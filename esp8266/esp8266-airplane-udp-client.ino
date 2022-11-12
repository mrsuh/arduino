#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>

#define PIN_DRIVER 5
#define PIN_SERVO1 14
#define PIN_SERVO2 12
#define PIN_SERVO3 13
#define PIN_SERVO4 15

#define PIN_LED_INITIALIZED 16
#define PIN_LED_ACTION 2
#define PIN_LED_ERROR 2

#define DRIVER_MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define DRIVER_MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs

typedef struct _udp_packet {
    int x1;
    int y1;
    int x2;
    int y2;
    int r1;
    int r2;
} UDP_PACKET;

const char *networkSsid = "esp32-udp";
const char *networkPassword = "password";

UDP_PACKET packet;
WiFiUDP Udp;
unsigned int udpPort = 4210;

IPAddress serverIp(192, 168, 4, 1);
IPAddress serverGateway(192, 168, 4, 1);
IPAddress serverSubnet(255, 255, 255, 0);

Servo driver;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void switchInitializedLed(bool enabled) {
    if (enabled) {
        digitalWrite(PIN_LED_INITIALIZED, HIGH);
    } else {
        digitalWrite(PIN_LED_INITIALIZED, LOW);
    }
}

void switchActionLed(bool enabled) {
    if (enabled) {
        digitalWrite(PIN_LED_ACTION, HIGH);
    } else {
        digitalWrite(PIN_LED_ACTION, LOW);
    }
}

void switchErrorLed(bool enabled) {
    if (enabled) {
        digitalWrite(PIN_LED_ERROR, HIGH);
    } else {
        digitalWrite(PIN_LED_ERROR, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nInitializing");

    if (!WiFi.mode(WIFI_AP)) {
        Serial.println("WiFi.mode(WIFI_AP) failed");
        return;
    }

    if (!WiFi.softAPConfig(serverIp, serverGateway, serverSubnet)) {
        Serial.println("WiFi.softAPConfig failed");
        return;
    }

    if (!WiFi.softAP(networkSsid, networkPassword)) {
        Serial.println("WiFi.softAP(ssid, password) failed");
        return;
    }

    Serial.println("WIFI AP Initialized!");

    if (Udp.begin(udpPort) != 1) {
        Serial.println("Udp.begin(udpPort) failed");
        return;
    }

    pinMode(PIN_LED_INITIALIZED, OUTPUT);
    pinMode(PIN_LED_ACTION, OUTPUT);
    pinMode(PIN_LED_ERROR, OUTPUT);

    switchInitializedLed(false);
    switchActionLed(false);
    switchErrorLed(false);

    driver.attach(PIN_DRIVER, DRIVER_MIN_PULSE_LENGTH, DRIVER_MAX_PULSE_LENGTH);
    driver.writeMicroseconds(DRIVER_MIN_PULSE_LENGTH);

    servo1.attach(PIN_SERVO1);
    servo2.attach(PIN_SERVO2);
    servo3.attach(PIN_SERVO3);
    servo4.attach(PIN_SERVO4);

    Serial.printf("Initialized!");
}

int lastDriverChangeValue = 0;

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize <= 0) {

        return;
    }

    Udp.read((byte * ) & packet, sizeof(UDP_PACKET));
    Serial.printf(
            "x1:%d y1:%d x2:%d y2:%d r1:%d r2:%d\n",
            packet.x1,
            packet.y1,
            packet.x2,
            packet.y2,
            packet.r1,
            packet.r2
    );

    servo1.write(map(packet.x1, 0, 100, 0, 180));
    servo2.write(map(packet.y1, 0, 100, 0, 180));
    servo3.write(map(packet.x2, 0, 100, 0, 180));
    servo4.write(map(packet.y2, 0, 100, 0, 180));
    if (millis() - lastDriverChangeValue > 200) {
        driver.writeMicroseconds(map(packet.r1, 0, 100, DRIVER_MIN_PULSE_LENGTH, DRIVER_MAX_PULSE_LENGTH));
        lastDriverChangeValue = millis();
    }
}
#include <WiFi.h>
#include <WiFiUdp.h>
#include <WiFiAP.h>

#define PIN_DRIVER 5
#define PIN_SERVO1 14
#define PIN_SERVO2 12
#define PIN_SERVO3 13
#define PIN_SERVO4 15

#define PIN_LED_INITIALIZED 16
#define PIN_LED_ACTION 2
#define PIN_LED_ERROR 2

#define DRIVER_MIN_PULSE_LENGTH 1000 // Minimum pulse length in µs
#define DRIVER_MAX_PULSE_LENGTH 2000 // Maximum pulse length in µs

typedef struct _udp_packet {
    int x1;
    int y1;
    int x2;
    int y2;
    int r1;
    int r2;
} UDP_PACKET;

const char *networkSsid = "esp32-udp";
const char *networkPassword = "password";

UDP_PACKET packet;
WiFiUDP Udp;
unsigned int udpPort = 4210;

IPAddress serverIp(192, 168, 4, 1);
IPAddress serverGateway(192, 168, 4, 1);
IPAddress serverSubnet(255, 255, 255, 0);

Servo driver;
Servo servo1;
Servo servo2;
Servo servo3;
Servo servo4;

void switchInitializedLed(bool enabled) {
    if (enabled) {
        digitalWrite(PIN_LED_INITIALIZED, HIGH);
    } else {
        digitalWrite(PIN_LED_INITIALIZED, LOW);
    }
}

void switchActionLed(bool enabled) {
    if (enabled) {
        digitalWrite(PIN_LED_ACTION, HIGH);
    } else {
        digitalWrite(PIN_LED_ACTION, LOW);
    }
}

void switchErrorLed(bool enabled) {
    if (enabled) {
        digitalWrite(PIN_LED_ERROR, HIGH);
    } else {
        digitalWrite(PIN_LED_ERROR, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nInitializing");

    if (!WiFi.mode(WIFI_AP)) {
        Serial.println("WiFi.mode(WIFI_AP) failed");
        return;
    }

    if (!WiFi.softAPConfig(serverIp, serverGateway, serverSubnet)) {
        Serial.println("WiFi.softAPConfig failed");
        return;
    }

    if (!WiFi.softAP(networkSsid, networkPassword)) {
        Serial.println("WiFi.softAP(ssid, password) failed");
        return;
    }

    Serial.println("WIFI AP Initialized!");

    if (Udp.begin(udpPort) != 1) {
        Serial.println("Udp.begin(udpPort) failed");
        return;
    }

    pinMode(PIN_LED_INITIALIZED, OUTPUT);
    pinMode(PIN_LED_ACTION, OUTPUT);
    pinMode(PIN_LED_ERROR, OUTPUT);

    switchInitializedLed(false);
    switchActionLed(false);
    switchErrorLed(false);

    driver.attach(PIN_DRIVER, DRIVER_MIN_PULSE_LENGTH, DRIVER_MAX_PULSE_LENGTH);
    driver.writeMicroseconds(DRIVER_MIN_PULSE_LENGTH);

    servo1.attach(PIN_SERVO1);
    servo2.attach(PIN_SERVO2);
    servo3.attach(PIN_SERVO3);
    servo4.attach(PIN_SERVO4);

    Serial.printf("Initialized!");
}

int lastDriverChangeValue = 0;

void loop() {
    int packetSize = Udp.parsePacket();
    if (packetSize <= 0) {

        return;
    }

    Udp.read((byte * ) & packet, sizeof(UDP_PACKET));
    Serial.printf(
            "x1:%d y1:%d x2:%d y2:%d r1:%d r2:%d\n",
            packet.x1,
            packet.y1,
            packet.x2,
            packet.y2,
            packet.r1,
            packet.r2
    );

    servo1.write(map(packet.x1, 0, 100, 0, 180));
    servo2.write(map(packet.y1, 0, 100, 0, 180));
    servo3.write(map(packet.x2, 0, 100, 0, 180));
    servo4.write(map(packet.y2, 0, 100, 0, 180));
    if (millis() - lastDriverChangeValue > 200) {
        driver.writeMicroseconds(map(packet.r1, 0, 100, DRIVER_MIN_PULSE_LENGTH, DRIVER_MAX_PULSE_LENGTH));
        lastDriverChangeValue = millis();
    }
}
