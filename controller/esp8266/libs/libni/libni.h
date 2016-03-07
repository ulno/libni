// Library for Network Input
//
// This is a C++ LIbrary for building a network input controller for ESP
// with the help of platformio
//
// Author: Ulrich Norbisrath
// Created: 07.03.2016
//

#ifndef _LIBNI_H
#define _LIBNI_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

/**
 * Creates an object which sends the current key state to a libni receiver.
 */
class Libni_Sender {
public:
  static const int PROTOCOL_VERSION;
  static const int NUMBER_OF_BUTTONS;
  static const int NUMBER_OF_BUTTON_BYTES;
  static const int NUMBER_OF_AXIS;
  static const int NUMBER_OF_AXIS_BYTES;
  static const int MAX_BUFFER_SIZE;
  static const int BUFFER_HEADER_SIZE;
  static const int DEFAULT_PORT;
  static const char* MAGIC; // magic identifier for Game Network Controller
  Libni_Sender(uint32_t client_id, const char *destination, int port );
  Libni_Sender(uint32_t client_id, const char *destination );
  void message_new(); // start a new message to send
  void message_add_button(int button_nr, bool state); // add a button status
  void message_add_analog(int analog_nr, long state); // add an analog status
  void message_send(); // send message
private:
  uint32_t client_id;
  const char * destination_host;
  int destination_port;
  WiFiUDP udp;
  IPAddress serverIP;

  byte *message;
  int message_size;

  void init(uint32_t client_id, const char *destination, int port);
  void init_header();

};

#endif
