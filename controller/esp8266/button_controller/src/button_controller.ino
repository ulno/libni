#include "ulno_esp_utils.h"
#include "libni.h"

const char* ssid     = "iotempire";
const char* password = "internetofthings";
const char * DESTINATION_HOST = "192.168.23.175";
const int MY_ID = 2;

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
  newButton( 27,  2, true ); // menu/escape (touch button on espresso lite)
}

// other global vars
unsigned char buttonCode[MAX_NUMBER_OF_BUTTONS];
unsigned int buttonPin[MAX_NUMBER_OF_BUTTONS];
bool buttonPullup[MAX_NUMBER_OF_BUTTONS];
bool buttonLastState[MAX_NUMBER_OF_BUTTONS];
unsigned int buttonThresCounter[MAX_NUMBER_OF_BUTTONS];
int buttonsAllocated=0;
int frame = 0;
Libni_Sender *libni_sender;

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
  ulno_esp_init("Push-button controller started.",ssid,password);
  libni_sender = new Libni_Sender(MY_ID,DESTINATION_HOST);

  pinMode(STATUS_LED, OUTPUT);

  initAllButtons();
}

void send() {
  libni_sender->message_new();
  for( int i=0; i<buttonsAllocated; i++ ) {
    libni_sender->message_add_button( buttonCode[i], buttonLastState[i] );
  }
  libni_sender->message_send();
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

  delay(5); // or yield();
}
