#include <Arduino.h>

#ifdef ESP32
#include <WiFi.h>
#include <AsyncTCP.h>
#elif defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncTCP.h>
#endif

#include <ESPAsyncWebServer.h>

#define greenLedpin 5
#define blueLedpin 4

#define motorLeftIn1Pin 14
#define motorLeftIn2Pin 12

#define motorRightIn1Pin 13
#define motorRightIn2Pin 15


AsyncWebServer server(80);

const char *ssid = "ssid";
const char *password = "password";

const char *PARAM_X = "x";
const char *PARAM_Y = "y";
const char *PARAM_DELAY = "delay";
const char *PARAM_TIME = "time";

int x = 0;
int y = 0;
int moveDelay = 20;
bool moveInitialized = false;
unsigned int lastMoveTime = 0;
unsigned long int lastMoveTimeFromClient = 0;
unsigned long int lastMoveTimeFromClientTmp = 0;

void notFound(AsyncWebServerRequest *request) {
    request->send(404, "text/plain", "Not found");
}

const char indexhtml[]
PROGMEM = R"rawliteral(
<!DOCTYPE html>
<head>
    <meta charset="utf-8">
    <meta http-equiv="X-UA-Compatible" content="IE=edge,chrome=1">
    <meta name="viewport" content="width=device-width, initial-scale=1 maximum-scale=1 user-scalable=no">
    <title>Sphere</title>
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
        }
    </style>
</head>

<body>
<div id="logs"></div>
</body>

<script>

    var query = function (url) {
        log(url);
        var xhr = new XMLHttpRequest();
        xhr.open('GET', url, false);
        xhr.send();
    };

    var urlParams = new URLSearchParams(window.location.search);
    var delay = urlParams.has('delay') ? parseInt(urlParams.get('delay')) : 100;

    var active = false;
    var percentX = 0;
    var percentY = 0;

    setInterval(function () {
        if (!active) {
            return;
        }
        query('/move?x=' + percentX + '&y=' + percentY + '&delay=' + delay + '&time=' + Date.now());
    }, parseInt(delay));

    var logs = document.getElementById('logs');

    var log = function (message) {
        var block = document.createElement('DIV');
        block.innerText = Date.now() + ' ' + message;
        logs.appendChild(block);

        while (logs.children.length > 3) {
            logs.removeChild(logs.firstChild)
        }
    };

    window.addEventListener('touchforcechange', function (e) {
        e.preventDefault();
        e.stopPropagation();
        log('touchstart')
    }, {passive: false});


    window.addEventListener('touchstart', function (e) {
        active = true;
        e.preventDefault();
        e.stopPropagation();
        mathCoords(e);
        log('touchstart')
    }, {passive: false});

    window.addEventListener('touchend', function (e) {
        e.preventDefault();
        e.stopPropagation();
        log('touchend');
        active = false;
        percentX = 0;
        percentY = 0;
    }, {passive: false});

    window.addEventListener('touchmove', function (e) {
        e.preventDefault();
        e.stopPropagation();
        active = true;
        mathCoords(e);
    }, {passive: false});

    window.onerror = function (error, url, line) {
        log(JSON.stringify({acc: 'error', data: 'ERR:' + error + ' URL:' + url + ' L:' + line}));
    };

    var mathCoords = function (e) {
        var touchobj = e.targetTouches[e.targetTouches.length - 1];

        log('e.targetTouches: ' + e.targetTouches.length);

        var touchX = touchobj.clientX;
        var touchY = touchobj.clientY;

        var windowWidth = window.innerWidth;
        var windowWidthHalf = windowWidth / 2;

        var windowHeight = window.innerHeight;
        var windowHeightHalf = windowHeight / 2;

        var x = touchX > windowWidthHalf ? touchX - windowWidthHalf : (windowWidthHalf - touchX) * -1;
        var y = touchY > windowHeightHalf ? (touchY - windowHeightHalf) * -1 : windowHeightHalf - touchY;

        percentX = parseInt(x * 100 / windowWidthHalf);
        percentY = parseInt(y * 100 / windowHeightHalf);

        log('x:' + percentX + ' y:' + percentY);
    }
</script>
)rawliteral";


IPAddress IP;

char text[100];

void setup() {

    Serial.begin(115200);
    pinMode(greenLedpin, OUTPUT);
    pinMode(blueLedpin, OUTPUT);

    digitalWrite(blueLedpin, HIGH);

    pinMode(motorLeftIn1Pin, OUTPUT);
    pinMode(motorLeftIn2Pin, OUTPUT);

    pinMode(motorRightIn1Pin, OUTPUT);
    pinMode(motorRightIn2Pin, OUTPUT);

    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, LOW);

    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, LOW);

    IPAddress ip(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 254);
    IPAddress subnet(255, 255, 255, 0);

    WiFi.mode(WIFI_AP_STA);
    WiFi.softAPConfig(ip, gateway, subnet);

    WiFi.softAP(ssid, password);

    IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP);

    // Print ESP8266 Local IP Address
    Serial.println(WiFi.localIP());

    server.on("/", HTTP_GET,[](AsyncWebServerRequest * request)
    {
        request->send(200, "text/html", indexhtml);
    });

    // Send a GET request to <IP>/get?message=<message>
    server.on("/move", HTTP_GET,[](AsyncWebServerRequest * request)
    {
       if (request->hasParam(PARAM_TIME)) {
         char charBuf[50];
         char *eptr;
         request->getParam(PARAM_TIME)->value().toCharArray(charBuf, 50);
           lastMoveTimeFromClientTmp = strtoul(charBuf, &eptr, 10);

           if(lastMoveTimeFromClient != 0 && lastMoveTimeFromClient >= lastMoveTimeFromClientTmp) {
             sprintf(text, "reqClientTime:%d lastClientTime:%d\n", lastMoveTimeFromClientTmp, lastMoveTimeFromClient);
             request->send(200, "text/plain", String(text));
             return;
           }

           lastMoveTimeFromClient = lastMoveTimeFromClientTmp;
       }

        if (request->hasParam(PARAM_X)) {
            x = request->getParam(PARAM_X)->value().toInt();
        }

        if (request->hasParam(PARAM_Y)) {
            y = request->getParam(PARAM_Y)->value().toInt();
        }

        if (request->hasParam(PARAM_DELAY)) {
            moveDelay = request->getParam(PARAM_DELAY)->value().toInt();
        }

        lastMoveTime = millis();
        moveLedEnabled(true);
        moveInitialized = false;

        sprintf(text, "x:%d y:%d delay:%d lastMoveTime:%d\n", x, y, moveDelay, lastMoveTime);

        request->send(200, "text/plain", String(text));
    });

    server.onNotFound(notFound);

    server.begin();
}

void up() {
    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, HIGH);
    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, HIGH);
}

void down() {
    digitalWrite(motorLeftIn1Pin, HIGH);
    digitalWrite(motorLeftIn2Pin, LOW);
    digitalWrite(motorRightIn1Pin, HIGH);
    digitalWrite(motorRightIn2Pin, LOW);
}

void left() {
    digitalWrite(motorLeftIn1Pin, HIGH);
    digitalWrite(motorLeftIn2Pin, LOW);
    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, HIGH);
}

void right() {
    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, HIGH);
    digitalWrite(motorRightIn1Pin, HIGH);
    digitalWrite(motorRightIn2Pin, LOW);
}

void stop() {
    digitalWrite(motorLeftIn1Pin, LOW);
    digitalWrite(motorLeftIn2Pin, LOW);
    digitalWrite(motorRightIn1Pin, LOW);
    digitalWrite(motorRightIn2Pin, LOW);
}

void moveLedEnabled(bool enabled) {
    if (enabled) {
        digitalWrite(blueLedpin, LOW);
        digitalWrite(greenLedpin, HIGH);
    } else {
        digitalWrite(blueLedpin, HIGH);
        digitalWrite(greenLedpin, LOW);
    }
}

int currentTime = 0;


void loop() {

    if (lastMoveTime == 0) {
        return;
    }

    currentTime = millis();

    if (currentTime - lastMoveTime < moveDelay && moveInitialized) {
        return;
    }

    if (currentTime - lastMoveTime > moveDelay + 10) {
        stop();
        moveLedEnabled(false);
        moveInitialized = false;
        return;
    }

    moveInitialized = true;

    if (x > 50) {
        right();
        return;

    }

    if (x < -50) {
        left();
        return;
    }

    if (y > 0) {
        up();
    }

    if (y < 0) {
        down();
    }
}
