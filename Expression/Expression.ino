#include <ESP8266WiFi.h>
#include <PubSubClient.h>
// vars
unsigned pedalstate = 0;
unsigned prevpedalstate = 0;
long previousMillis = 0;
char lightstr[4];

// WiFi/MQTT parameters
#include "network_settings.h" // NOTE: For now, I'm storing wifi params in a .gitignore'd header

WiFiClient client;
PubSubClient mqttclient(client);

void setup() {
  Serial.begin(115200);

  // connect to wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  // connect to mqtt server
  mqttclient.setServer(BROKER_IP, 1883);
  mqttclient.setCallback(callback);
  connect();
}

void callback (char* topic, byte* payload, unsigned int length) {
  return;
}

void loop() {
  if (!mqttclient.connected()) {
    connect();
  }
  //read analog sensor
  pedalstate = analogRead(A0);
  // ipedalstate != prevpedalstate) {
  itoa(pedalstate, lightstr, 10);
  mqttclient.publish("/expr/value", lightstr);
  prevpedalstate = pedalstate;
    // print value
    // Serial.print(lightstr);
    // Serial.print("\r");
}

void connect() {
  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("Wifi issue");
    delay(300);
  }
  Serial.print("Connecting to MQTT server... ");
  while(!mqttclient.connected()) {
    if (mqttclient.connect(WiFi.macAddress().c_str())) {
      Serial.println("MQTT server Connected!");

       mqttclient.subscribe("/test"); // subscribe to topic

      
    } else {
      Serial.print("MQTT server connection failed! rc=");
      Serial.print(mqttclient.state());
      Serial.println("try again in 10 seconds");
      // Wait a bit and try again
      delay(200);
    }
  }
}
