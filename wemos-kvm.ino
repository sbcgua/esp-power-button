/***
  Remote power button
*/

#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <StreamString.h>
#include "htmls.h"
#include "secrets.h" // WIFI_SSID, WIFI_PASS ...

#define DEBUG

#ifdef DEBUG
  #define LED_PIN LED_BUILTIN
  #define STATE_ON LOW // LOW for LED_BUILTIN
  #define STATE_OFF HIGH
  #define TRACE(...) Serial.printf(__VA_ARGS__)
#else
  #define LED_PIN D5
  #define STATE_ON HIGH
  #define STATE_OFF LOW
  #define TRACE
#endif

static const char* HOSTNAME = "espkvm";
ESP8266WebServer server(80);

int revState(int state) { return state == LOW ? HIGH : LOW; }

String getFormattedUptime() {
  int ts   = millis();
  int sec  = ts / 1000;
  int min  = sec / 60;
  int hr   = min / 60;
  int days = hr / 24;

  StreamString tmp;
  if (days) {
    tmp.printf("%d days, %02d:%02d:%02d", days, hr % 24, min % 60, sec % 60);
  } else {
    tmp.printf("%02d:%02d:%02d", hr % 24, min % 60, sec % 60);
  }
  return String(tmp.c_str());
}

void pushPower(int ms) {
  digitalWrite(LED_PIN, STATE_ON);
  delay(ms);
  digitalWrite(LED_PIN, STATE_OFF);
}

void pushShort() { pushPower(300); }
void pushLong() { pushPower(5000); }

String getRootHtml() {
  String html = index_page;
  html.replace(F("%CHIP_ID%"), String(ESP.getChipId()));
  html.replace(F("%UP_TIME%"), getFormattedUptime());
  return html;
}

void setCORS(){
  server.sendHeader(F("Access-Control-Allow-Origin"),  F("*"));
  server.sendHeader(F("Access-Control-Max-Age"),       F("600"));
  server.sendHeader(F("Access-Control-Allow-Methods"), F("POST,GET,OPTIONS"));
  server.sendHeader(F("Access-Control-Allow-Headers"), F("*"));
};

void handleRoot() {
  setCORS();
  server.send(200, "text/html", getRootHtml());
}

enum PushType { PUSH_SHORT, PUSH_LONG, PUSH_UNKNOWN };

void handlePush() {
  PushType pushType = PUSH_UNKNOWN;
  if (server.args() == 0) {
    pushType = PUSH_SHORT;
  } else if (server.args() == 1) {
    if (server.argName(0) == "push") {
      if (server.arg(0) == "short") pushType = PUSH_SHORT;
      else if (server.arg(0) == "long") pushType = PUSH_LONG;
    }
  }

  setCORS();
  switch (pushType) {
    case PUSH_SHORT:
      pushShort();
      server.send(200, F("text/plain"), F("OK"));
      break;
    case PUSH_LONG:
      pushLong();
      server.send(200, F("text/plain"), F("OK"));
      break;
    default:
      server.send(422, F("text/plain"), F("Bad param"));
  }
}

void handleHZ() {
  String res = F("OK, ") + getFormattedUptime();
  setCORS();
  server.send(200, "text/plain", res);
}

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: " + server.uri() + "\n";
  message += "Method: " + String(server.method() == HTTP_GET ? "GET" : "POST") + "\n";
  message += "Arguments: " + String(server.args()) + "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  setCORS();
  server.send(404, "text/plain", message);
}

void setup_io() {
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, STATE_OFF);
  Serial.begin(9600);
  delay(1000);
  Serial.println(F("start"));
}

void setup_wifi() {
  TRACE("Connecting to %s", WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);
  WiFi.setHostname(HOSTNAME);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    TRACE(".");
  }

  TRACE("\n");
  TRACE("WiFi connected: %s\n", WIFI_SSID);
  TRACE("IP address: %s\n", WiFi.localIP().toString().c_str());
  TRACE("Gateway: %s\n", WiFi.gatewayIP().toString().c_str());
}

void setup_web_server() {
  #ifdef DEBUG
  server.addHook([](const String& method, const String& url, WiFiClient* client, ESP8266WebServer::ContentTypeFunction contentType) {
    (void)method;       // GET, PUT, ...
    (void)url;          // example: /root/myfile.html
    (void)client;       // the webserver tcp client connection
    (void)contentType;  // contentType(".html") => "text/html"
    TRACE("%s %s\n", method, url);
    return ESP8266WebServer::CLIENT_REQUEST_CAN_CONTINUE;
  });
  #endif

  server.on(F("/"), HTTP_GET, handleRoot);
  server.on(F("/hz"), HTTP_GET, handleHZ);
  server.on(F("/power"), HTTP_POST, handlePush);
  server.onNotFound(handleNotFound);  
  server.begin();
}

void setup_mdns() {
  if (MDNS.begin(HOSTNAME)) { 
    TRACE(PSTR("MDNS responder started\n"));
    MDNS.addService(F("http"), F("tcp"), 80);
  }
}

#ifdef DEBUG
void traceHeartbeat() {
  static int traceTimestamp = 0;
  if (millis() - traceTimestamp > 10000) {
    traceTimestamp = millis();
    TRACE("[%d] Chip ID = %08X\n", traceTimestamp/1000, ESP.getChipId());
  }
}
#endif

/******************************************************
 * ENTRY POINTS
 */

void setup() {
  setup_io();  
  setup_wifi();  
  setup_mdns();  
  setup_web_server();
}

void loop() {
  MDNS.update();
  server.handleClient();

  #ifdef DEBUG
  traceHeartbeat();
  #endif
}
