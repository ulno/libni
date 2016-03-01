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
   tb.add_button(BUTTON_ID+1,12);
   tb.add_button(BUTTON_ID+2,4);
}

long frames = 0;
void loop() {
  tb.check();
  if(tb.get_button(BUTTON_ID)) {
    digitalWrite(STATUS_LED, LOW);
  } else {
    digitalWrite(STATUS_LED, HIGH);
  }
  frames ++;
  if(frames > 100) {
    frames = 0;
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
