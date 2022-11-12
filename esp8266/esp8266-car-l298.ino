#include <Arduino.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>

#define moveLedPin 14
#define powerLedPin 12
#define connectionLedPin 13
#define helmPin 2
#define driverIn1Pin 16
#define driverIn2Pin 5

bool connected = false;
Servo helm;
StaticJsonDocument<200> doc;
AsyncWebServer server(80);
AsyncWebSocket ws("/ws");

const char *ssid = "boat-2020";
const char *password = "password";

int x = 0;
int y = 0;
bool moveInitialized = false;
unsigned int moveInterval = 100;
unsigned int stopDelay = 200;
unsigned int lastMoveTime = 0;
unsigned int currentTime = 0;

void onWsEvent(AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data,
               size_t len) {
    if (type == WS_EVT_CONNECT) {
        connected = true;
        connectionLedEnabled(true);
        Serial.printf("Client connected [%s][%u]\n", server->url(), client->id());
    } else if (type == WS_EVT_DISCONNECT) {
        connected = false;
        connectionLedEnabled(false);
        Serial.printf("Client disconnected [%s][%u]\n", server->url(), client->id());
    } else if (type == WS_EVT_ERROR) {
        Serial.printf("ws[%s][%u] error(%u): %s\n", server->url(), client->id(), *((uint16_t *) arg), (char *) data);
    } else if (type == WS_EVT_DATA) {
        AwsFrameInfo *info = (AwsFrameInfo *) arg;
        if (info->final && info->index == 0 && info->len == len) {
            data[len] = 0;

            DeserializationError error = deserializeJson(doc, (char *) data);

            if (error) {
                Serial.print("deserializeJson() failed: ");
                Serial.println(error.c_str());
                return;
            }

            lastMoveTime = millis();
            moveLedEnabled(true);
            moveInitialized = false;

            moveInterval = doc["interval"];
            stopDelay = doc["stopDelay"];
            x = doc["x"];
            y = doc["y"];
        }
    }
}

const char indexhtml[]
PROGMEM = R"rawliteral(
<!DOCTYPE html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="viewport" content="width=device-width, initial-scale=1 maximum-scale=1 user-scalable=no">
    <title>Boat</title>
    <style>
        html {
            margin: 0;
            padding: 0;
            height: 100%;
        }

        body {
            margin: 0;
            padding: 0;
            height: 100%;
            background-color: rgba(65, 105, 225, 0);
        }

        #info.connected {
            background-color: rgb(65, 105, 225);
            color: white;
        }

        #info.disconnected {
            background-color: rgb(220, 20, 60);
            color: white;
        }

    </style>
</head>

<body>
<div id="info" class="disconnected">
    <span id="status">Disconnected</span>
    <span id="coords">(0/0)</span>
</div>
<div id="debug"></div>
</body>

<script>
    let ws            = null;
    let urlParams     = new URLSearchParams(window.location.search);
    let interval      = urlParams.has('interval') ? parseInt(urlParams.get('interval')) : 100;
    let stopDelay     = urlParams.has('stopDelay') ? parseInt(urlParams.get('stopDelay')) : 200;
    let isDebugActive = urlParams.has('debug') ? parseInt(urlParams.get('debug')) === 1 : false;

    let active   = false;
    let percentX = 0;
    let percentY = 0;

    let debug = function(message) {
        if (!isDebugActive) {
            return;
        }

        let debugElement = document.getElementById('debug');
        let block        = document.createElement('div');
        block.innerText  = Date.now() + ' ' + message;
        debugElement.appendChild(block);

        while (debugElement.children.length > 10) {
            debugElement.removeChild(debugElement.firstChild);
        }
    };

    if (isDebugActive) {
        debug('Debug: true');
        debug('Interval: ' + interval + 'ms');
        debug('StopDelay: ' + stopDelay + 'ms');
    }

    let connect = function() {
        ws           = new WebSocket('ws://192.168.4.1/ws');
        ws.onopen    = function(e) {
            showStatus(true);
            debug('WS Connected');
        };
        ws.onclose   = function(e) {
            showStatus(false);
            debug('WS Closed' + JSON.stringify(e));
        };
        ws.onmessage = function(e) {
            debug('WS Message' + e.data);
        };
        ws.onerror   = function(e) {
            showStatus(false);
            debug('WS Error' + JSON.stringify(e));
        };
    };

    let showStatus = function(connected) {
        let infoElement   = document.getElementById('info');
        let statusElement = document.getElementById('status');
        if (connected) {
            statusElement.innerText = 'Connected';
            infoElement.className   = 'connected';
        } else {
            statusElement.innerText = 'Disconnected';
            infoElement.className   = 'disconnected';
        }
    };

    let showCoords = function({x, y}) {
        let coordsElement       = document.getElementById('coords');
        coordsElement.innerText = '(' + x + '/' + y + ')';
    };

    let move = function({x, y}) {
        if (ws && ws.readyState === WebSocket.OPEN) {
            let time = Date.now();
            debug('WS Send ' + JSON.stringify({x: x, y: y}));
            ws.send(JSON.stringify({x: x, y: y, interval: interval, stopDelay: stopDelay, time: time}));
        }
    };

    let mathCoords = function(e) {
        let touchobj = e.targetTouches[e.targetTouches.length - 1];

        let touchX = touchobj.clientX;
        let touchY = touchobj.clientY;

        let windowWidthHalf  = window.innerWidth / 2;
        let windowHeightHalf = window.innerHeight / 2;

        let x = touchX > windowWidthHalf ? touchX - windowWidthHalf : (windowWidthHalf - touchX) * -1;
        let y = touchY > windowHeightHalf ? (touchY - windowHeightHalf) * -1 : windowHeightHalf - touchY;

        percentX = parseInt(x * 100 / windowWidthHalf);
        percentY = parseInt(y * 100 / windowHeightHalf);
    };

    let drawBackground = function({x, y}) {

        let absX     = Math.abs(x);
        let absY     = Math.abs(y);
        let maxValue = absX > absY ? absX : absY;

        let bodyElement = document.getElementsByTagName('body')[0];

        bodyElement.style.backgroundColor = 'rgba(65, 105, 225,' + (maxValue / 100).toFixed(2) + ')';

        if (maxValue === 0) {
            bodyElement.style.transition = 'background-color 300ms linear';
        } else {
            bodyElement.style.transition = '';
        }
    };

    window.addEventListener('touchforcechange', function(e) {
        e.preventDefault();
        e.stopPropagation();
    }, {passive: false});

    window.addEventListener('touchstart', function(e) {
        active = true;
        e.preventDefault();
        e.stopPropagation();
        mathCoords(e);
        showCoords({x: percentX, y: percentY});
        drawBackground({x: percentX, y: percentY});
    }, {passive: false});

    window.addEventListener('touchmove', function(e) {
        e.preventDefault();
        e.stopPropagation();
        active = true;
        mathCoords(e);
        showCoords({x: percentX, y: percentY});
        drawBackground({x: percentX, y: percentY});
    }, {passive: false});

    window.addEventListener('touchend', function(e) {
        e.preventDefault();
        e.stopPropagation();
        active   = false;
        percentX = 0;
        percentY = 0;
    }, {passive: false});

    window.onerror = function(error, url, line) {
        debug(JSON.stringify({error: error, url: url, line: line}));
    };

    connect();
    setInterval(function() {

        if (!active) {
            showCoords({x: percentX, y: percentY});
            drawBackground({x: percentX, y: percentY});
            return;
        }

        move({x: percentX, y: percentY});
    }, interval);

</script>
)rawliteral";


IPAddress IP;


void setup() {
    Serial.begin(115200);

    IPAddress ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 254);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, gateway, subnet);

    WiFi.softAP(ssid, password);

    IP = WiFi.softAPIP();

    ws.onEvent(onWsEvent);
    server.addHandler(&ws);

    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(200, "text/html", indexhtml);
    });

    server.onNotFound([](AsyncWebServerRequest *request) {
        request->send(404, "text/plain", "Not found");
    });

    server.begin();

    pinMode(powerLedPin, OUTPUT);
    pinMode(connectionLedPin, OUTPUT);
    pinMode(moveLedPin, OUTPUT);
    pinMode(driverIn1Pin, OUTPUT);
    pinMode(driverIn2Pin, OUTPUT);

    helm.attach(helmPin);

    helm.write(70);

    digitalWrite(driverIn1Pin, LOW);
    digitalWrite(driverIn2Pin, LOW);
    digitalWrite(powerLedPin, HIGH);
    digitalWrite(connectionLedPin, LOW);
    digitalWrite(moveLedPin, LOW);
}

void moveLedEnabled(bool enabled) {
    if (enabled) {
        digitalWrite(moveLedPin, HIGH);
    } else {
        digitalWrite(moveLedPin, LOW);
    }
}

void connectionLedEnabled(bool enabled) {
    if (enabled) {
        digitalWrite(connectionLedPin, HIGH);
    } else {
        digitalWrite(connectionLedPin, LOW);
    }
}

void driverControl(int value) {

    if (value > -20 && value < 20) {
        digitalWrite(driverIn1Pin, LOW);
        digitalWrite(driverIn2Pin, LOW);

        return;
    }

    if (value > 0) {
        digitalWrite(driverIn1Pin, HIGH);
        digitalWrite(driverIn2Pin, LOW);
    } else {
        digitalWrite(driverIn1Pin, LOW);
        digitalWrite(driverIn2Pin, HIGH);
    }
}


void helmControl(int value) {
    if(value >= 0) {
        helm.write((int)map(value, 0, 100, 85, 140));
    } else {
        helm.write((int)map(value, -100, 0, 1, 85));
    }
}

void loop() {

    if (!connected) {
        return;
    }

    if (lastMoveTime == 0) {
        driverControl(0);
        helmControl(0);
        return;
    }

    currentTime = millis();

    if (currentTime - lastMoveTime < moveInterval && moveInitialized) {
        return;
    }

    if (currentTime - lastMoveTime > stopDelay) {
        moveLedEnabled(false);
        moveInitialized = false;
        driverControl(0);
        helmControl(0);
        return;
    }

    helmControl(x);
    driverControl(y);
    moveInitialized = true;
}
