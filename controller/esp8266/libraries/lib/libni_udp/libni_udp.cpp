#include <Arduino.h>
#include "libni_sender.h"
#include "libni_udp.h"

const int Libni_Udp::DEFAULT_PORT = 19877;

void Libni_Udp::message_send() {
  /*Serial.print("Sending UDP packet to ");
  Serial.print(serverIP.toString());
  Serial.print(":");
  Serial.println(destination_port);*/
  udp.beginPacket(serverIP, destination_port);
  udp.write(message, message_size);
  udp.endPacket();
}

Libni_Udp::Libni_Udp(uint32_t client_id, const char *destination) : Libni_Sender(client_id) {
  init_udp(destination, DEFAULT_PORT);
}

Libni_Udp::Libni_Udp(uint32_t client_id, const char *destination, int port) : Libni_Sender(client_id) {
  init_udp(destination,port);
}

void Libni_Udp::init_udp( const char *destination, int port) {
  destination_host = destination;
  destination_port = port;

  WiFi.hostByName(destination_host, serverIP); // resolve name
}
