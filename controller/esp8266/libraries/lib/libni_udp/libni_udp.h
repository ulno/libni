// Library for Network Input
//
// This is a C++ Library for building a network input controller for ESP
// with the help of platformio. This is actually the sending part to send
// to a potential receiver.
//
// Here is the UDP-specific part of the sender
//
// Author: Ulrich Norbisrath
// Created: 05.03.2016
//

#ifndef _LIBNI_UDP_H
#define _LIBNI_UDP_H

#include "libni_sender.h"

/**
 * Creates an object which sends the current key state to a libni receiver.
 */
class Libni_Udp:public Libni_Sender {
public:
  static const int DEFAULT_PORT;
  Libni_Udp(uint32_t client_id, const char *destination, int port );
  Libni_Udp(uint32_t client_id, const char *destination );
  void message_send();
private:
  const char * destination_host;
  int destination_port;
  WiFiUDP udp;
  IPAddress serverIP;

  void init_udp(const char *destination, int port);

};

#endif // Libni_UDP.h
