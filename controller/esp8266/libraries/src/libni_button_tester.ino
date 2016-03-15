#include "ulno_esp_utils.h"
//#include "libni_udp.h"
#include "libni_mqtt.h"
#include "ulno_buttons.h"

// network related stuff
const char* SSID     = "iotempire";
const char* PASSWORD = "internetofthings";
const char* TOPIC = "iotempire/libni";
//const char* DESTINATION_HOST = "192.168.23.175";
const char* DESTINATION_HOST = "192.168.15.100";
const int MY_ID = 1; // controller id for identification

const int STATUS_LED = 16;
Ulno_Buttons *tb;
Libni_Mqtt *libni_sender;

void initAllButtons() {
  tb->add_touch_button(Ulno_Buttons::FIRE,  4); // fire
  tb->add_touch_button(Ulno_Buttons::DOWN,  5); // down
  tb->add_touch_button(Ulno_Buttons::LEFT, 12); // left
  tb->add_touch_button(Ulno_Buttons::RIGHT,13); // right
  tb->add_touch_button(Ulno_Buttons::UP,   14); // up
  // up 15 is pulled down all the time as it seems -> does not work easily
  //tb->add_push_button(16,15,false);
}

void send() {
  libni_sender->message_new();
  for( int i=0; i<tb->size(); i++ ) {
    libni_sender->message_add_button( tb->get_button_id(i), tb->get_button_state(i) );
  }
  libni_sender->message_send();
}

void setup() {
  ulno_esp_init("Wire touch controller started.",SSID,PASSWORD);
  libni_sender = new Libni_Mqtt(MY_ID,DESTINATION_HOST,TOPIC);
  tb = new Ulno_Buttons(3, 8, 4, true, true); // better for aluminum than the defaults
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
  if(frames > 4) { // increase for network message debug, but lower to send more updates
    frames = 0;
    send();
  }
  delay(10); // needs to be long enough for discharge (>10?)
}
