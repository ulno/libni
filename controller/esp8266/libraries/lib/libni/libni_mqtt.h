// Library for Network Input
//
// This is a C++ Library for building a network input controller for ESP
// with the help of platformio. This is actually the sending part to send
// to a potential receiver.
//
// Here is the MQTT-specific part of the sender
//
// Author: Ulrich Norbisrath
// Created: 05.03.2016
//

#ifndef _LIBNI_MQTT_H
#define _LIBNI_MQTT_H

#include "libni_sender.h"
#include <PubSubClient.h>

/**
 * Creates an object which sends the current key state to a libni receiver.
 */
class Libni_Mqtt : public Libni_Sender {
public:
  static const int DEFAULT_PORT;
  Libni_Mqtt(uint32_t client_id, const char* destination_host, int port, const char* topic );
  Libni_Mqtt(uint32_t client_id, const char* destination_host, const char* topic );
  void message_send();
private:
  const char* destination_host;
  const char* topic;
  int destination_port;

  PubSubClient* mqtt; 

  void init_mqtt(const char* destination_host, int port, const char* topic);

};

#endif // libni_mqtt
