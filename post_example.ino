#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <WiFiClientSecureBearSSL.h>

#include "config.h"

const char* ssid = STASSID;
const char* password = STAPSK;

ESP8266WiFiMulti WiFiMulti;

void setup() {
  Serial.begin(9600);

  Serial.println("Start");

  //WIFI connection
  WiFi.mode(WIFI_STA);
  WiFiMulti.addAP(ssid, password);

  Serial.print("Connecting");

  while (WiFiMulti.run() != WL_CONNECTED) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nWiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());  

  sendSms("1");
}

void loop() {
  //Only one request needed.

}

void sendSms(String sensor) {  
  std::unique_ptr<BearSSL::WiFiClientSecure>client(new BearSSL::WiFiClientSecure);

  client->setFingerprint(fingerprint);
  
  HTTPClient http;
  DynamicJsonDocument doc(2048);
  http.setTimeout(15000);

  //Server api route
  http.begin(*client, serverUrl + "/api/send-sms");

  //Tell the server, the request includes json parameters
  http.addHeader("Content-Type", "application/json");
  //Add the apiToken for authentication
  http.addHeader("X-AUTH-TOKEN", apiToken);

  //Sensor from web Portal
  int httpCode = http.POST("{\"smsTexts\": \"Das ist eine Testnachricht\",\"sensor\": " + sensor + "}");


  Serial.println("Sending");
  if (httpCode > 0) {
    Serial.print(httpCode);

    if (httpCode == HTTP_CODE_OK) {
      Serial.print("Send OK");
    } else {
      Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }
  }
}
