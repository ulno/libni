#include <ESP8266WiFi.h>
#include <lwip/ip.h> // for endian conversion htons, htonl, ...
#include <WiFiUdp.h>
#include "Touch_Buttons.h"

// network related stuff
const char* ssid     = "iotempire";
const char* password = "internetofthings";
const char * DESTINATION_HOST = "192.168.23.175";
int DESTINATION_PORT = 19877;
WiFiUDP udp;
IPAddress serverIP;

const char *MAGIC = "LBNI"; // magic identifier for Game Network Controller
const int PROTOCOL_VERSION = 1;
const int NUMBER_OF_BUTTONS = 256;
const int NUMBER_OF_BUTTON_BYTES = (NUMBER_OF_BUTTONS + 7)/8;
const int NUMBER_OF_AXIS = 16;
const int NUMBER_OF_AXIS_BYTES = NUMBER_OF_AXIS * 2;
const int MAX_BUFFER_SIZE = 128;
const int BUFFER_HEADER_SIZE = 16;
byte message[MAX_BUFFER_SIZE];


const int STATUS_LED = 16;
Touch_Buttons tb;

void initAllButtons() {

  //tb.add_button(16,15); // up 15 is pulled down all the time as it seems -> does not work easily
  tb.add_button(16,14); // up
  tb.add_button( 2,12); // left
  tb.add_button(14, 5); // down
  tb.add_button( 6,13); // right
  tb.add_button(' ',4); // fire


}

void init_header() {
  int pos = 0;
  for(int i=0; i<4; i++) {
    message[pos] = MAGIC[i];
    pos += 1;
  }
  long version = htonl(PROTOCOL_VERSION);
  for(int i=0; i<4; i++) {
    message[pos] = ((char *)&version)[i];
    pos += 1;
  }
  int protocol_type = htons(1); //TODO: read or define 1 elsewhere
  for(int i=0; i<2; i++) {
    message[pos] = ((char *)&protocol_type)[i];
    pos += 1;
  }
  for(int i=0; i<4; i++) {
    message[pos] = random(256);
    pos += 1;
  }
  byte CLIENT_ID[4] = {0,0,0,1};
  for(int i=0; i<4; i++) {
    message[pos] = CLIENT_ID[i];
    pos += 1;
  }
}

void send() {
  // Serial.println("sending UDP packet...");
  // build package
  byte *msg = message + BUFFER_HEADER_SIZE;
  for( int i=0; i<tb.size(); i++ ) {
    *msg = tb.get_button_state(i)?2:1;
    msg ++;
    *msg = tb.get_button_id(i);
    msg ++;
  }   // TODO eventually check overflow

  udp.beginPacket(serverIP, DESTINATION_PORT);
  udp.write(message, BUFFER_HEADER_SIZE + tb.size()*2);
  udp.endPacket();
}

void setup() {
   Serial.begin(115200);
   delay(10);
   Serial.println("Wire touch controller started.");

   Serial.println();
   Serial.println();
   Serial.print("Connecting to ");
   Serial.println(ssid);

   WiFi.begin(ssid, password);

   while (WiFi.status() != WL_CONNECTED) {
     delay(500);
     Serial.print(".");
   }

   Serial.println("");
   Serial.println("WiFi connected");
   Serial.println("IP address: ");
   Serial.println(WiFi.localIP());
   Serial.println("");

   for(int i=0; i<sizeof(message); i++) {
     message[i]=0; // init
   }

   pinMode(STATUS_LED, OUTPUT);

   WiFi.hostByName(DESTINATION_HOST, serverIP); // resolve name

   // init header
   init_header();

   pinMode(STATUS_LED, OUTPUT);
   digitalWrite(STATUS_LED, LOW);

   initAllButtons();
}

long frames = 0;
void loop() {
  tb.check();
  /*if(tb.get_button(BUTTON_ID)) {
    digitalWrite(STATUS_LED, LOW);
  } else {
    digitalWrite(STATUS_LED, HIGH);
  }*/
  frames ++;
  if(frames > 50) {
    frames = 0;
    send();
    for( int i = tb.size()-1; i>=0; i-- ) {
      int state = tb.get_button_state(i);
      int id = tb.get_button_id(i);
      Serial.print("ID: ");
      Serial.print(id);
      Serial.print(" State: ");
      Serial.print(state);
      Serial.print("  ");
    }
    Serial.println();
  }
  delay(1); // increase to debug
}
