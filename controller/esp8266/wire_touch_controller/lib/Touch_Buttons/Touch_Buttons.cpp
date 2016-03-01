// Implementation for Touch_Buttons
//
// author: ulno
// created: 2016-03-01

#include <Arduino.h>
#include "Touch_Buttons.h"

Touch_Buttons::Touch_Buttons() {
  // threshold = 108; // 1/2mOhm resistor + graphite + scotch tape
  // threshold = 400; // > 50000 with 1MOhm
  threshold = 50000;// internal resistor
  max_touch = 10;
  use_internal_pullup = true; // we use it in the simples version
  measure_chargedelay = true; // true should be used when using internal pullup
  _size = 0;
  initial_wait = 1;
  for(int i=0; i < MAX_BUTTONS * 2; i++) {
    button_array[i] = 0;
  }
  for(int i=0; i < MAX_BUTTONS; i++) {
    debouncer[i] = 0;
  }
}

static void pull_down( int gpio ) {
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, LOW);
}

void Touch_Buttons::pull_down_all() {
  // pull all buttons down
  for(int b=0; b<_size; b++) {
    int gpio = button_gpio[b];
    pull_down(gpio);
  }
}

void Touch_Buttons::set_input_all() {
  // pull all buttons down
  for(int b=0; b<_size; b++) {
    int gpio =  button_gpio[b];
    if( use_internal_pullup )
      pinMode(gpio, INPUT_PULLUP);
    else
      pinMode(gpio, INPUT);
  }
}

void Touch_Buttons::add_button(int id, int gpio_pin) {
  if(_size < MAX_BUTTONS ) {
    set_button_id(_size, id);
    set_button_state(_size, -1);
    button_gpio[_size] = gpio_pin;
    _size ++;
    // needs to be pulled down by default to be discharged
    pull_down(gpio_pin);
  } else {
    Serial.println("Maximum numbers of buttons defined, not adding new one.\n");
  }
}

void Touch_Buttons::check() {
  // Set all buttons to read in process state
  for(int b=0; b<_size; b++) {
    button_time[b] = threshold;
  }
  pull_down_all();
  delay(initial_wait); // wait for discharge

  /* this is too slow
  // prepare measuring
  set_input_all();
  // check the time how long it takes to get each port charged
  for(long time=threshold-1; time>=0; time--) {
    bool active = false; // are we still reading buttons?
    for(int b=_size-1; b>=0; b--) {
      if(button_time[b]==threshold) { // no decent value found yet
        active = true; // we found at least one button for which we are still waiting
        int gpio = button_gpio[b];
        if( digitalRead(gpio) == HIGH ) { // it is now charged
          pull_down(gpio);
          button_time[b] = threshold-time-1;
        }
      }
    }
    if(!active) break; // all charged, we can stop
  } // end threshold loop
  */
  // do each button on its own to measure faster
  for(int b=_size-1; b>=0; b--) {
    int gpio = button_gpio[b];
    //int_fast16_t timer = threshold-1;
    int timer = threshold;
    //if( use_internal_pullup )
      pinMode(gpio, INPUT_PULLUP);
    //else
    //  pinMode(gpio, INPUT);
    //GPIO_DIS_OUTPUT(gpio);
    //PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
    /*while(timer >= 0) {
      if(digitalRead(gpio) == HIGH) break;
      timer --;
    }*/
    while (!(GPIO_REG_READ(GPIO_IN_ADDRESS)&32) && (timer > 0)) {
        // timer--; slow?
        --timer;
    }

    // make sure to pull down again
    pull_down(gpio);
    button_time[b] = threshold-timer; // save time for this button
  }

  // evaluate timing list
  for(int b=_size-1; b>=0; b--) {
    Serial.print("Button Id: ");
    Serial.print(get_button_id(b));
    Serial.print(" Time: ");
    Serial.print(button_time[b]);
    Serial.println();
    if(button_time[b] < threshold) {
      if(measure_chargedelay) { // in this case being under the time means, no human touched the wire -> it is untouched
        decrease_debouncer(b);
      } else {
        increase_debouncer(b);
      }
    } else {
      if(measure_chargedelay) { // in this case being under the time means, a human touched the wire -> it is touched
        increase_debouncer(b);
      } else {
        decrease_debouncer(b);
      }
    }
  }
  // make sure again, that all are pulled down
  pull_down_all();
}

int Touch_Buttons::get_button(int id) {
  // find button id
  for(int b=_size-1; b>=0; b--) {
    if(get_button_id(b) == id)
      return get_button_state(b);
  }
  return -1;
}
