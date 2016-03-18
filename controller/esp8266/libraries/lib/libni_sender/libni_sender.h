// Library for Network Input
//
// This is a C++ Library for building a network input controller for ESP
// with the help of platformio. This is actually the sending part to send
// to a potential receiver.
//
// Here the general sending mechanism and corresponding memory management is implemented.
//
// Author: Ulrich Norbisrath
// Created: 07.03.2016
//

#ifndef _LIBNI_SENDER_H
#define _LIBNI_SENDER_H

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
  static const char* MAGIC; // magic identifier for Game Network Controller
  void message_new(); // start a new message to send
  void message_add_button(int button_nr, bool state); // add a button status
  void message_add_analog(int analog_nr, long state); // add an analog status
  virtual void message_send(); // send message
  Libni_Sender(uint32_t client_id);
private:
  void init(uint32_t client_id);
  void init_header();
protected:
  uint32_t client_id;
  byte *message;
  int message_size;

};

#endif // libni_sender.h
