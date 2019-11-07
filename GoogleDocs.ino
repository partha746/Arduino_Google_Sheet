#include <ESP8266WiFi.h>
#include "HTTPSRedirect.h"
#include <ESP8266WiFiMulti.h>

#define TRIGGERPIN D1
#define ECHOPIN    D2

ESP8266WiFiMulti wifiMulti;

const char* host = "script.google.com";
const char* googleRedirHost = "script.googleusercontent.com";
const char *GScriptId = "AKfycbyt2xy6KtumV7Eu8ODkkto4JUWNGJ6bYU1HQ7RBNdmI7FWw2Vw";

const int httpsPort = 443;

// echo | openssl s_client -connect script.google.com:443 |& openssl x509 -fingerprint -noout
const char* fingerprint = "37 83 9B 99 A1 C9 7D 64 9B 3D 93 1F F0 55 EB A5 F1 49 34 34";

void setup() {
  Serial.begin(9600);

  WiFi.mode(WIFI_STA);
  wifiMulti.addAP("T3", "diwana_746");
  wifiMulti.addAP("T2", "diwana_746");
  wifiMulti.addAP("nvtestwireless", "Sp33doflight");
  
  pinMode(TRIGGERPIN, OUTPUT);
  pinMode(ECHOPIN, INPUT);
  
  Serial.println();
  Serial.print("Connecting to wifi: ");
  while (wifiMulti.run() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // Use HTTPSRedirect class to create TLS connection
  HTTPSRedirect client(httpsPort);
  client.setInsecure();

  Serial.print("Connecting to ");
  Serial.println(host);

  bool flag = false;
  for (int i = 0; i < 5; i++) {
    int retval = client.connect(host, httpsPort);
    if (retval == 1) {
      flag = true;
      break;
    }
    else
      Serial.println("Connection failed. Retrying...");
    delay(1000);
  }

  Serial.flush();
  if (!flag) {
    Serial.print("Could not connect to server: ");
    Serial.println(host);
    Serial.println("Exiting...");
    return;
  }

  Serial.flush();
  if (client.verify(fingerprint, host)) {
    Serial.println("Certificate match.");
  } else {
    Serial.println("Certificate mis-match");
  }
}

void loop() {
  HTTPSRedirect client(httpsPort);
  client.setInsecure();
  if (!client.connected())           
    client.connect(host, httpsPort);

  long duration, distance;
  digitalWrite(TRIGGERPIN, LOW);
  delayMicroseconds(3);
  digitalWrite(TRIGGERPIN, HIGH);
  delayMicroseconds(12);
  digitalWrite(TRIGGERPIN, LOW);
  duration = pulseIn(ECHOPIN, HIGH);
  
  distance = (duration / 2) / 29.1;
  distance = distance - 1.5;

  Serial.print( "Read dist(CM): " );
  Serial.println( distance );

  String url = String("/macros/s/") + GScriptId + "/exec?tmp=" + distance;
  client.printRedir(url, host, googleRedirHost);

  delay(15*60000UL);
}
