#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "libni_sender.h"
#include "libni_mqtt.h"

const int Libni_Mqtt::DEFAULT_PORT = 1883;

static WiFiClient wifiClient; // why do I need this for PubSubClient?

void Libni_Mqtt::message_send() {
  /*Serial.print("Sending MQTT packet to ");
  Serial.print(serverIP.toString());
  Serial.print(":");
  Serial.println(destination_port);*/
  mqtt->publish(topic,message,message_size);
}

Libni_Mqtt::Libni_Mqtt(uint32_t client_id, const char* destination_host, const char* topic) : Libni_Sender(client_id) {
  init_mqtt(destination_host, DEFAULT_PORT, topic);
}

Libni_Mqtt::Libni_Mqtt(uint32_t client_id, const char* destination_host, int port, const char* topic) : Libni_Sender(client_id) {
  init_mqtt(destination_host, port, topic);
}

void Libni_Mqtt::init_mqtt( const char* destination_host, int port, const char* topic ) {
  this->destination_host = destination_host;
  this->destination_port = port;
  this->topic = topic;

  // Generate client name
  String clientName;
  clientName += "esp-libni-";
  clientName += String(client_id);

  mqtt = new PubSubClient(destination_host, destination_port,wifiClient);
  mqtt->connect((char*) clientName.c_str());
}
