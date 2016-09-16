#include "ulno_esp_utils.h"
#include "libni_udp.h"
#include "ulno_buttons.h"

// network related stuff
const char* ssid     = "iotempire";
const char* password = "internetofthings";
const char * DESTINATION_HOST = "192.168.23.123";
const int MY_ID = 12; // controller id for identification

const int STATUS_LED = 16;
Ulno_Buttons *tb;
Libni_Udp *libni_sender;

void initAllButtons() {
//  tb->add_push_button('q',  4); // fire
//  tb->add_push_button('s',  5); // down
//  tb->add_push_button('a', 12); // left
//  tb->add_push_button('d', 13); // right
//  tb->add_push_button('w', 15, false); // // up 15 is pulled down all the time as it seems -> does not work easily
//  tb->add_push_button(Ulno_Buttons::ESCAPE,14);

  tb->add_push_button(Ulno_Buttons::FIRE,  4); // fire
  tb->add_push_button(Ulno_Buttons::DOWN,  5); // down
  tb->add_push_button(Ulno_Buttons::LEFT, 12); // left
  tb->add_push_button(Ulno_Buttons::RIGHT, 13); // right
  tb->add_push_button(Ulno_Buttons::UP, 15, false); // // up 15 is pulled down all the time as it seems -> does not work easily

  // // ulno
  // tb->add_push_button(Ulno_Buttons::UP,   4);
  // tb->add_push_button(Ulno_Buttons::RIGHT, 5);
  // tb->add_push_button(Ulno_Buttons::DOWN, 12);
  // tb->add_push_button(Ulno_Buttons::FIRE, 13);
  // tb->add_push_button(Ulno_Buttons::LEFT, 14);

  // ben
//  tb->add_push_button(Ulno_Buttons::UP,    4);
//  tb->add_push_button(Ulno_Buttons::LEFT,  5);
//  tb->add_push_button(Ulno_Buttons::RIGHT,12);
//  tb->add_push_button(Ulno_Buttons::DOWN, 13);
//  tb->add_push_button(Ulno_Buttons::FIRE, 14);

}

void send() {
  libni_sender->message_new();
  for( int i=0; i<tb->size(); i++ ) {
    libni_sender->message_add_button( tb->get_button_id(i), tb->get_button_state(i) );
  }
  libni_sender->message_send();
}

void setup() {
  ulno_esp_init("Wire touch controller started.",ssid,password);
  libni_sender = new Libni_Udp(MY_ID,DESTINATION_HOST);
  tb = new Ulno_Buttons(); // defaults are fine for normal buttons
  pinMode(STATUS_LED, OUTPUT);
  digitalWrite(STATUS_LED, LOW);

  initAllButtons();
  tb->debug(2,50); // approx every 50x10 (delay) a debug message from the buttons (every 50th call)
}

long frames = 0;
void loop() {
  if( tb->check() ) {
    digitalWrite(STATUS_LED, LOW);
  } else {
    digitalWrite(STATUS_LED, HIGH);
  }
  frames ++;
  if(frames > 4) {
    frames = 0;
    send();
  }
  delay(10); // needs to be long enough for discharge (>10?)
}
