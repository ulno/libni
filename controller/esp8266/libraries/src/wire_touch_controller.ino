#include "ulno_esp_utils.h"
#include "libni.h"
#include "libni_buttons.h"

// network related stuff
const char* ssid     = "iotempire";
const char* password = "internetofthings";
const char * DESTINATION_HOST = "192.168.23.175";
const int MY_ID = 1; // controller id for identification

const int STATUS_LED = 16;
Libni_Buttons *tb;
Libni_Sender *libni_sender;

void initAllButtons() {
  tb->add_touch_button(Libni_Buttons::FIRE,  4); // fire
  tb->add_touch_button(Libni_Buttons::DOWN,  5); // down
  tb->add_touch_button(Libni_Buttons::LEFT, 12); // left
  tb->add_touch_button(Libni_Buttons::RIGHT,13); // right
  tb->add_touch_button(Libni_Buttons::UP,   14); // up
  //tb->add_touch_button(16,15); // up 15 is pulled down all the time as it seems -> does not work easily
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
  libni_sender = new Libni_Sender(MY_ID,DESTINATION_HOST);
  tb = new Libni_Buttons(3, 8, 1, true, true); // better for aluminum than the defaults
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
  if(frames > 50) {
    frames = 0;
    send();
  }
  delay(10); // needs to be long enough for discharge (>10?)
}
