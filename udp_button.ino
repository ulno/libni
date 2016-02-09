#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

const char* ssid     = "tulix-upstairs";
const char* password = "vivalasalsa07";

const char* DESTINATION_HOST = "ulno-work";
//const char* DESTINATION_HOST = "localhost";
//const char* DESTINATION_HOST = "192.168.15.194";
int DESTINATION_PORT = 19877;

const int NUMBER_OF_BUTTONS = 256;
const int NUMBER_OF_BUTTON_BYTES = (NUMBER_OF_BUTTONS + 7)/8;
const int NUMBER_OF_AXIS = 16;
const int NUMBER_OF_AXIS_BYTES = NUMBER_OF_AXIS * 2;
byte message[NUMBER_OF_BUTTON_BYTES + NUMBER_OF_AXIS_BYTES];
const int MAX_NUMBER_OF_BUTTONS = 16;
const int BUTTON_THRES = 16; // how many times has an on be counted to assume this button is on
//const int STATUS_LED = 2;
const int STATUS_LED = 16;

void newButton( unsigned char code, int pin, bool pullup );
void initAllButtons() {
  newButton( 'w', 15, false ); // up
  newButton( 'a', 12, true ); // left
  newButton( 's',  5, true ); // down
  newButton( 'd', 13, true ); // right
  newButton( ' ',  4, true ); // fire
  newButton( 'm',  2, true ); // menu
}

// other global vars
unsigned char buttonCode[MAX_NUMBER_OF_BUTTONS];
unsigned int buttonPin[MAX_NUMBER_OF_BUTTONS];
bool buttonPullup[MAX_NUMBER_OF_BUTTONS];
unsigned int buttonThresCounter[MAX_NUMBER_OF_BUTTONS];
int buttonsAllocated=0;
int frame = 0;
WiFiUDP udp;
IPAddress serverIP;

void initButton( int buttonIndex, unsigned char code, int pin, bool pullup ) {
  buttonCode[ buttonIndex ] = code;
  buttonPin[ buttonIndex] = pin;
  buttonPullup[ buttonIndex ] = pullup;
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

void setButton(int nr, boolean isPressed) {
    int byteNr = nr / 8;
    int bitNr = nr % 8;
    
    if( isPressed ) {
        message[byteNr] |= 1 << bitNr;
    } else {
        message[byteNr] &= 255 - (1<<bitNr);
    }
}

bool getButton( int nr ) {
    int byteNr = nr / 8;
    int bitNr = nr % 8;

    return (message[byteNr] & (1<<bitNr))>0;
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
}

void send() {
  // Serial.println("sending UDP packet...");
  udp.beginPacket(serverIP, DESTINATION_PORT);
  udp.write(message, sizeof(message));
  udp.endPacket();
}

void loop() {

  frame ++;
  
  bool anyPressed = false;
  bool changed = false;
  for( int i=0; i<buttonsAllocated; i++ ) {
    bool lastState = getButton(buttonCode[i]);
    bool buttonPressed = checkButton(i);
    if( lastState != buttonPressed) {
      setButton(buttonCode[i], buttonPressed);
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
