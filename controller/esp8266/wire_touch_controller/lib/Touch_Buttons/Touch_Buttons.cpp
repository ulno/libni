// Implementation for Touch_Buttons
//
// attention, to be fast, this needs the -O3 compiler option to be set!
//
// author: ulno
// created: 2016-03-01

#include <Arduino.h>
#include "Touch_Buttons.h"

void Touch_Buttons::init(int threshold, int debounce, bool internal_pullup, bool chargedelay ) {
  // threshold = 108; // 1/2mOhm resistor + graphite + scotch tape
  // threshold = 400; // > 50000 with 1MOhm
  //threshold = 4;// internal resistor (set to 20 to see speed)
  this->threshold = threshold;
  this->debounce_value = debounce*2; // reasonable is 5
  this->use_internal_pullup = internal_pullup; // we use it in the simplest version to not have external resistors
  this->measure_chargedelay = chargedelay; // true should be used when using internal pullup

  _size = 0;
  initial_wait = 1;
  for(int i=0; i < MAX_BUTTONS * 2; i++) {
    button_array[i] = 0;
  }
  for(int i=0; i < MAX_BUTTONS; i++) {
    debouncer[i] = 0;
  }
}

Touch_Buttons::Touch_Buttons(int threshold, int debounce, bool internal_pullup, bool chargedelay ) {
  init( threshold, debounce, internal_pullup, chargedelay);
}

Touch_Buttons::Touch_Buttons() {
  init( 9, 5, true, true);
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

/*void Touch_Buttons::set_input_all() {
  // pull all buttons down
  for(int b=0; b<_size; b++) {
    int gpio =  button_gpio[b];
    if( use_internal_pullup )
      pinMode(gpio, INPUT_PULLUP);
    else
      pinMode(gpio, INPUT);
  }
} too slow */

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
  const int DIRECT_READS = 20;  // this is a fixed constant reflecting the 20 single reads in this function
  uint32_t regcache[DIRECT_READS];
  int_fast16_t timer = 0;

/*    //PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);
  //GPIO_OUTPUT_SET(gpio, 0); // pull down
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, LOW);
  //os_delay_us(500);
  //os_delay_us(100);
  delay(1);*/
  pull_down_all();
  delay(initial_wait);

  for(int b=_size-1; b>=0; b--) {
    int gpio = button_gpio[b];

    timer = threshold - DIRECT_READS;

    // (GPIO_REG_READ(GPIO_IN_ADDRESS) = READ_PERI_REG(PERIPHS_GPIO_BASEADDR + GPIO_IN_ADDRESS)
    //volatile uint32_t *gpio_ports = (volatile uint32_t *)(PERIPHS_GPIO_BASEADDR + GPIO_IN_ADDRESS); slower than fixed address
    uint32_t bitmask = 1 << gpio;
    uint32_t *regcache_writer = regcache;

    //GPIO_DIS_OUTPUT(gpio);
    //PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
    // TODO: add if condition
    pinMode(gpio, INPUT_PULLUP);

    // the following is extremely time critical as the recharging is pretty fast
    // read directly to be maximum fast
    // 5x
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    // 5x
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    // 5x
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    // 5x
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    *(regcache_writer ++) = GPIO_REG_READ(GPIO_IN_ADDRESS);
    // read a little slower
    //while (!(gpio_input_get()&(1<<gpio)) && (riseTime < threshold)) { // slow?
    //while (!(gpio_input_get()&32) && (riseTime < threshold)) { // slow?
    //while (!(*gpio_ports&32) && (timer > 0)) { // slower than fixed address
    while ((timer > 0) && !(GPIO_REG_READ(GPIO_IN_ADDRESS)&bitmask)) {
        --timer;
    }

    pull_down(gpio);

    timer = threshold - DIRECT_READS - timer;
    // adjust by the fast read direct accesses
    int timer2 = 0;
    for(int i=0; i<DIRECT_READS; i++) {
      if(regcache[i]&bitmask) {
        break;
      }
      timer2++;
    }
    if( timer2 < DIRECT_READS) timer = timer2;
    else timer += DIRECT_READS;
    button_time[b] = timer; // save time for this button
    if (timer < threshold) {
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
    /* debug */
    /*Serial.print("Button Id: ");
    Serial.print(get_button_id(b));
    Serial.print(" Time: ");
    Serial.print(timer);
    Serial.print(" Time2: ");
    Serial.print(timer2);
    Serial.print(" Debounce: ");
    Serial.print(debouncer[b]);
    Serial.println();*/
  } // end of loop through all buttons
}

int Touch_Buttons::get_button(int id) {
  // find button id
  for(int b=_size-1; b>=0; b--) {
    if(get_button_id(b) == id)
      return get_button_state(b);
  }
  return -1;
}
