#include "Touch_Buttons.h"

const int BUTTON_ID = 42;
const int STATUS_LED = 16;
Touch_Buttons tb;

void setup() {
   Serial.begin(115200);
   delay(10);
   Serial.println("Touch test started.");

   pinMode(STATUS_LED, OUTPUT);
   digitalWrite(STATUS_LED, LOW);

   tb.add_button(BUTTON_ID,5);
}

long frames = 0;
void loop() {
  tb.check();
  bool t = tb.get_button(BUTTON_ID)==1;
  if(t) {
    digitalWrite(STATUS_LED, LOW);
  } else {
    digitalWrite(STATUS_LED, HIGH);
  }
  frames ++;
  if(frames > 10) {
    frames = 0;
    if(t) {
      Serial.println("touched");
    } else {
      Serial.println("untouched");
    }
  }
  //  delayMicroseconds(1000);
  delay(500);
}
