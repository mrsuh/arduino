#include <WiFi.h>
#include <WiFiUdp.h>

#define PIN_LED_INITIALIZED 5
#define PIN_LED_WIFI_CONNECTED 18
#define PIN_LED_ERROR 19

#define PIN_LEFT_X 39
#define PIN_LEFT_Y 36
#define PIN_LEFT_R 32

#define PIN_RIGHT_X 35
#define PIN_RIGHT_Y 34
#define PIN_RIGHT_R 33

typedef struct _udp_packet {
    int x1 = 0;
    int y1 = 0;
    int x2 = 0;
    int y2 = 0;
    int r1 = 0;
    int r2 = 0;
} UDP_PACKET;

const char *networkSsid = "esp32-udp";
const char *networkPassword = "password";

WiFiUDP udp;
UDP_PACKET udpPacket;
boolean connected = false;

IPAddress serverIp(192, 168, 4, 1);
unsigned int udpPort = 4210;

double multipleAnalogRead(byte pin, int multiple = 64) {
    double total = 0;
    for (int i = 0; i < multiple; i++) {
        total += analogRead(pin);
    }

    return total / multiple;
}

int needToChange(int prev, int current) {
    return abs(prev - current) > 1;
}

int readJoystickValue(int pin, int previousValue) {
    int value = (int)map(multipleAnalogRead(pin), 0, 4095, 0, 100);

    // middle
    if(value > 40 && value < 60) {
        return 50;
    }

    // left
    if(value < 10) {
        return 0;
    }

    // right
    if(value > 90) {
        return 100;
    }

    if(abs(previousValue - value) > 1) {
        return value;
    }

    return previousValue;
}

int readPotentiometerValue(int pin, int previousValue) {
    int value = (int)map(multipleAnalogRead(pin), 0, 4095, 0, 100);

    // left
    if(value < 10) {
        return 0;
    }

    // right
    if(value > 90) {
        return 100;
    }

    if(abs(previousValue - value) > 10) {
        return value;
    }

    return previousValue;
}

void connectToWiFi(const char *ssid, const char *pwd) {
    Serial.println("Connecting to WiFi network: " + String(ssid));

    // delete old config
    WiFi.disconnect(true);

    WiFi.onEvent(WiFiEvent);

    WiFi.begin(ssid, pwd);

    Serial.println("Waiting for WIFI connection...");
}

// https://github.com/espressif/arduino-esp32/blob/master/libraries/WiFi/examples/WiFiClientEvents/WiFiClientEvents.ino
void WiFiEvent(WiFiEvent_t event) {
    Serial.println(event);
    switch (event) {
        case ARDUINO_EVENT_WIFI_STA_GOT_IP:
            Serial.print("WiFi connected! IP address: ");
            Serial.println(WiFi.localIP());
            //initializes the UDP state
            //This initializes the transfer buffer
            udp.begin(WiFi.localIP(), udpPort);
            connected = true;
            switchWifiConnectedLed(true);
            break;
        case ARDUINO_EVENT_WIFI_STA_DISCONNECTED:
            Serial.println("WiFi lost connection");
            connected = false;
            switchWifiConnectedLed(false);
            WiFi.reconnect();
            break;
        default:
            break;
    }
}

void switchWifiConnectedLed(bool enabled) {
    if(enabled) {
        digitalWrite(PIN_LED_WIFI_CONNECTED, HIGH);
    } else {
        digitalWrite(PIN_LED_WIFI_CONNECTED, LOW);
    }
}

void switchInitializedLed(bool enabled) {
    if(enabled) {
        digitalWrite(PIN_LED_INITIALIZED, HIGH);
    } else {
        digitalWrite(PIN_LED_INITIALIZED, LOW);
    }
}

void switchErrorLed(bool enabled) {
    if(enabled) {
        digitalWrite(PIN_LED_ERROR, HIGH);
    } else {
        digitalWrite(PIN_LED_ERROR, LOW);
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("\nInitializing");

    pinMode(PIN_LED_INITIALIZED, OUTPUT);
    pinMode(PIN_LED_WIFI_CONNECTED, OUTPUT);
    pinMode(PIN_LED_ERROR, OUTPUT);

    pinMode(PIN_LEFT_X, INPUT);
    pinMode(PIN_LEFT_Y, INPUT);
    pinMode(PIN_LEFT_R, INPUT);
    pinMode(PIN_RIGHT_X, INPUT);
    pinMode(PIN_RIGHT_Y, INPUT);
    pinMode(PIN_RIGHT_R, INPUT);

    switchInitializedLed(true);
    switchWifiConnectedLed(false);
    switchErrorLed(false);

    connectToWiFi(networkSsid, networkPassword);
}

void loop() {
    if (!connected) {
        return;
    }

    if (udp.beginPacket(serverIp, udpPort) == 0) {
        Serial.println("Udp.beginPacket failed");
        switchErrorLed(true);
        return;
    }

    udpPacket.x1 = readJoystickValue(PIN_LEFT_X, udpPacket.x1);
    udpPacket.y1 = readJoystickValue(PIN_LEFT_Y, udpPacket.y1);
    udpPacket.r1 = readPotentiometerValue(PIN_LEFT_R, udpPacket.r1);

    udpPacket.x2 = readJoystickValue(PIN_RIGHT_X, udpPacket.x2);
    udpPacket.y2 = readJoystickValue(PIN_RIGHT_Y, udpPacket.y2);
    udpPacket.r2 = readPotentiometerValue(PIN_RIGHT_R, udpPacket.r2);

    if(udp.write((byte * ) & udpPacket, sizeof(UDP_PACKET)) == 0) {
        Serial.println("Udp.write() error");
        switchErrorLed(true);
        return;
    }

    if (udp.endPacket() == 0) {
        Serial.println("Udp.endPacket() error");
        switchErrorLed(true);
        return;
    }

    switchErrorLed(false);

    delay(10);
}
