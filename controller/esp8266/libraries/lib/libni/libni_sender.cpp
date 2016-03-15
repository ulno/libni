#include <Arduino.h>
#include "libni_sender.h"
#include <lwip/ip.h> // for endian conversion htons, htonl, ...

const int Libni_Sender::PROTOCOL_VERSION = 1;
const int Libni_Sender::NUMBER_OF_BUTTONS = 256;
const int Libni_Sender::NUMBER_OF_BUTTON_BYTES = (Libni_Sender::NUMBER_OF_BUTTONS + 7)/8;
const int Libni_Sender::NUMBER_OF_AXIS = 16;
const int Libni_Sender::NUMBER_OF_AXIS_BYTES = Libni_Sender::NUMBER_OF_AXIS * 2;
const int Libni_Sender::MAX_BUFFER_SIZE = 128;
const int Libni_Sender::BUFFER_HEADER_SIZE = 16;
const char *Libni_Sender::MAGIC = "LBNI";

void Libni_Sender::init_header() {
  int pos = 0;
  for(int i=0; i<4; i++) {
    message[pos] = MAGIC[i];
    pos += 1;
  }
  u16_t version = htons(PROTOCOL_VERSION);
  for(int i=0; i<4; i++) {
    message[pos] = ((char *)&version)[i];
    pos += 1;
  }
  u16_t protocol_type = htons(1); //TODO: read or define 1 elsewhere - 1 means event based protocol
  for(int i=0; i<2; i++) {
    message[pos] = ((char *)&protocol_type)[i];
    pos += 1;
  }
  for(int i=0; i<4; i++) {
    message[pos] = random(256);
    pos += 1;
  }
  //byte CLIENT_ID[4] = {0,0,0,1};
  uint32_t client_id_network = htonl(client_id);
  byte *client_id_ptr = (byte *)&client_id_network;
  for(int i=0; i<4; i++) {
    message[pos] = client_id_ptr[i];
    pos += 1;
  }
  message_new(); // reset message size to header size
}

void Libni_Sender::message_new() {
    message_size = BUFFER_HEADER_SIZE;
}

void Libni_Sender::message_add_button(int button_nr, bool state) {
  if( message_size >= MAX_BUFFER_SIZE-1 ) {
    Serial.println("No space in sent buffer left.");
  } else {
    byte *msg = message + message_size;
    *msg = state?2:1;
    msg ++;
    *msg = button_nr;
    message_size += 2;
  }
}

void Libni_Sender::message_add_analog(int analog_nr, long state ) {
  if( message_size >= MAX_BUFFER_SIZE-3 ) {
    Serial.println("No space in sent buffer left.");
  } else {
    byte *msg = message + message_size;
    uint32_t state_sent = htonl(state);
    byte *state_sent_ptr = (byte *)(&state_sent);
    *msg = 3; // send analog
    msg ++;
    *msg = state_sent_ptr[0];
    msg ++;
    *msg = state_sent_ptr[1];
    msg ++;
    *msg = state_sent_ptr[2];
    msg ++;
    *msg = state_sent_ptr[3];
    message_size += 4;
  }
}

void Libni_Sender::message_send() {
  // This is virtual, should be not used.
  Serial.println("Wrong (virtual) send method called.");
}

Libni_Sender::Libni_Sender(uint32_t client_id) {
  init(client_id);
}

void Libni_Sender::init(uint32_t client_id) {
  this->client_id = client_id;
  message = new byte[MAX_BUFFER_SIZE];
  for(int i=0; i<MAX_BUFFER_SIZE; i++) {
    message[i]=0; // init
  }

  // init header
  init_header();
}
