#include <ESP8266WiFi.h>
#include <lwip/ip.h> // for endian conversion htons, htonl, ...
#include <WiFiUdp.h>

const char* ssid     = "iotempire";
const char* password = "internetofthings";

const char * DESTINATION_HOST = "192.168.23.175";
int DESTINATION_PORT = 19877;

const char *MAGIC = "LBNI"; // magic identifier for Game Network Controller
const int PROTOCOL_VERSION = 1;
const int NUMBER_OF_BUTTONS = 256;
const int NUMBER_OF_BUTTON_BYTES = (NUMBER_OF_BUTTONS + 7)/8;
const int NUMBER_OF_AXIS = 16;
const int NUMBER_OF_AXIS_BYTES = NUMBER_OF_AXIS * 2;
const int MAX_BUFFER_SIZE = 128;
const int BUFFER_HEADER_SIZE = 16;
byte message[MAX_BUFFER_SIZE];
const int MAX_NUMBER_OF_BUTTONS = 16;
const int BUTTON_THRES = 16; // how many times has an on be counted to assume this button is on
//const int STATUS_LED = 2;
const int STATUS_LED = 16;

void newButton( unsigned char code, int pin, bool pullup );
void initAllButtons() {
  newButton('w', 15, false ); // up
  newButton('a', 12, true ); // left
  newButton('s',  5, true ); // down
  newButton('d', 13, true ); // right
  newButton('q',  4, true ); // fire
}

// other global vars
unsigned char buttonCode[MAX_NUMBER_OF_BUTTONS];
unsigned int buttonPin[MAX_NUMBER_OF_BUTTONS];
bool buttonPullup[MAX_NUMBER_OF_BUTTONS];
bool buttonLastState[MAX_NUMBER_OF_BUTTONS];
unsigned int buttonThresCounter[MAX_NUMBER_OF_BUTTONS];
int buttonsAllocated=0;
int frame = 0;
WiFiUDP udp;
IPAddress serverIP;


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
  // TODO: check ID construction
  byte CLIENT_ID[4] = {0,0,0,1};
  for(int i=0; i<4; i++) {
    message[pos] = CLIENT_ID[i];
    pos += 1;
  }
}

void initButton( int buttonIndex, unsigned char code, int pin, bool pullup ) {
  buttonCode[ buttonIndex ] = code;
  buttonPin[ buttonIndex] = pin;
  buttonPullup[ buttonIndex ] = pullup;
  buttonLastState[ buttonIndex ] = false;
  //setButtonState(code,false); //done already
  if(pullup) pinMode(pin, INPUT_PULLUP);
  else pinMode(pin, INPUT); // needs to be manually pulled down  
}

void newButton( unsigned char code, int pin, bool pullup ) {
  if(buttonsAllocated < MAX_NUMBER_OF_BUTTONS ) {
    initButton( buttonsAllocated, code, pin, pullup );
    buttonsAllocated ++;
  } else {
    Serial.println("MaxButtons allocated.");
  }
}

   
bool checkButton( int buttonIndex ) {
    if(digitalRead( buttonPin[buttonIndex] ) == (buttonPullup[buttonIndex]?LOW:HIGH)) {
        if(buttonThresCounter[buttonIndex] < BUTTON_THRES)
            buttonThresCounter[buttonIndex] ++;
    } else {
        if(buttonThresCounter[buttonIndex]>0)
            buttonThresCounter[buttonIndex] --;
    }
    return buttonThresCounter[buttonIndex] > BUTTON_THRES/2;
}

void setup() {

  Serial.begin(115200);
  delay(10);

  // We start by connecting to a WiFi network

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

  for(int i=0; i<sizeof(message); i++) {
    message[i]=0; // init
  }
  initAllButtons();
  
  pinMode(STATUS_LED, OUTPUT);

  WiFi.hostByName(DESTINATION_HOST, serverIP); // resolve name

  // init header
  init_header();
}

void send() {
  // Serial.println("sending UDP packet...");
  // build package
  byte *msg = message + BUFFER_HEADER_SIZE;
  for( int i=0; i<buttonsAllocated; i++ ) {
    *msg = buttonLastState[i]?2:1;
    msg ++;
    *msg = buttonCode[i];
    msg ++;
  }   // TODO eventually check overflow

  udp.beginPacket(serverIP, DESTINATION_PORT);
  udp.write(message, BUFFER_HEADER_SIZE + buttonsAllocated*2);
  udp.endPacket();
}

void loop() {

  frame ++;
  
  bool anyPressed = false;
  bool changed = false;
  for( int i=0; i<buttonsAllocated; i++ ) {
    bool lastState = buttonLastState[i];
    bool buttonPressed = checkButton(i);
    if( lastState != buttonPressed) {
      buttonLastState[i] = buttonPressed;
      changed = true;
    }
    anyPressed |= buttonPressed;
  }

  if(changed || frame>=100) {
    send();
    //Serial.println("Alive\n");
    frame = 0;
  }

  if (anyPressed) {
    digitalWrite(STATUS_LED, LOW);
    //Serial.println("HIGH");
  } else {
    digitalWrite(STATUS_LED, HIGH);
    //Serial.println("LOW");
  }

  delay(1); // or yield();
}
