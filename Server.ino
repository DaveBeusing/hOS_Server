/*
 * Copyright (c) 2020, Dave Beusing <david.beusing@gmail.com>
 * All rights reserved.
 * 
 * https://www.amazon.de/gp/B0754HWZSQ
 * NodeMCU Amica ESP8266 ESP-12F
 * 
 * variants/nodemcu/pins_arduino.h
 * #define LED_BUILTIN 2
 * #define LED_BUILTIN_AUX 16
 * #define PIN_WIRE_SDA (4)
 * #define PIN_WIRE_SCL (5)
 * 
 * 
 * 
 * // Read the first line of the request
  String req = client.readStringUntil('\r');
  Serial.println(req);
  client.flush();

  // Match the request
  int val;
  if (req.indexOf("/gpio/0") != -1) {
    val = 0;
  } else if (req.indexOf("/gpio/1") != -1) {
    val = 1;
  } else {
    Serial.println("invalid request");
    client.print("HTTP/1.1 404 Not Found\r\nContent-Type: text/html\r\n\r\n<!DOCTYPE HTML>\r\n<html><body>Not found</body></html>");
    return;
  }

*/

#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>

//used for path arguments
#include <uri/UriBraces.h>
#include <uri/UriRegex.h>

//
#include "HomeServer.h"

#ifndef STASSID
  #define STASSID "AF8Y"
  #define STAPSK  "zZA74B1CuY"
#endif

const char *ssid = STASSID;
const char *password = STAPSK;

ESP8266WebServer server(80);

const int led = 16;

void setup(void) {
  pinMode(led, OUTPUT);
  digitalWrite(led, 0);
  Serial.begin(115200);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  if (MDNS.begin("esp8266")) {
    Serial.println("MDNS responder started");
  }
/*
  server.on("/", handleRoot);
  server.on("/test.svg", drawGraph);
  server.on("/inline", []() {
    server.send(200, "text/plain", "this works as well");
  });
*/

  

  //pathargserver
  server.on(F("/"), []() {
    handleRoot();
    //server.send(200, "text/plain", "hello from esp8266!");
  });

  server.on(F("/test.svg"), drawGraph);


  server.on(UriBraces("/users/{}"), []() {
    String user = server.pathArg(0);
    server.send(200, "text/plain", "User: '" + user + "'");
  });

  server.on(UriRegex("^\\/users\\/([0-9]+)\\/devices\\/([0-9]+)$"), []() {
    String user = server.pathArg(0);
    String device = server.pathArg(1);
    server.send(200, "text/plain", "User: '" + user + "' and Device: '" + device + "'");
  });


  
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
}

void loop(void) {
  server.handleClient();
  MDNS.update();
}
