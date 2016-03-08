// Implementation for Libni_Buttons
//
// attention, to be fast, this needs the -O3 compiler option to be set!
//
// author: ulno
// created: 2016-03-01

#include <Arduino.h>
#include "libni_buttons.h"

#define ulno_do_2x(exp) {exp;exp;}
#define ulno_do_5x(exp) {exp;exp;exp;exp;exp;}
#define ulno_do_10x(exp) {ulno_do_2x(ulno_do_5x(exp))}

inline static void pull_down( int gpio ) {
  pinMode(gpio, OUTPUT);
  digitalWrite(gpio, LOW);
}

void Libni_Button::debug_base(int debug_level) {
  if(debug_level>=1) {
    Serial.print("ID");
    Serial.print(get_id());
    Serial.print(" PN");
    Serial.print(gpio);
    Serial.print(" ST");
    Serial.print(get_state());
  }
}

void Libni_Button::debug(int debug_level) {
  if(debug_level>=1) {
    Serial.print(" Generic ");
    debug_base(debug_level);
  }
}

bool Libni_Button::check() {
  return false;
}

Libni_Button::Libni_Button(int id, int gpio, int debounce) {
    this->type = GENERIC;
    this->id = id;
    this->gpio = gpio;
    this->state = false;
    this->debounce_max = debounce*2;
    this->debouncer = 0;
  }

class Libni_Touch_Button : public Libni_Button {
private:
  int threshold;
  bool use_internal_pullup; // we use it in the simplest version to not have external resistors
  bool measure_direction_ischarge; // true should be used when using internal pullup
  int discharge_delay_ms;
  int_fast16_t charge_time1; // the (last) time1 it took to charge this button
  int_fast16_t charge_time2; // the (last) time2 it took to charge this button
public:
  Libni_Touch_Button( int id, int gpio, int debounce,
    int threshold, int discharge_delay_ms,
    bool use_internal_pullup, bool measure_direction_ischarge )
    : Libni_Button( id, gpio, debounce ) {
    this->type = TOUCH;
    this->threshold = threshold;
    this->discharge_delay_ms = discharge_delay_ms;
    this->use_internal_pullup = use_internal_pullup;
    this->measure_direction_ischarge = measure_direction_ischarge;
    this->charge_time1 = 0;
    this->charge_time2 = 0;
    // needs to be pulled down by default to be discharged
    pull_down(gpio);
  }

  bool check() {
    // this is a fixed constant reflecting the up to 100 single reads in this function
    #define MAX_DIRECT_READS 100
    uint32_t regcache[MAX_DIRECT_READS];
    int_fast16_t threshold_left;

  /*    //PIN_PULLUP_DIS(PERIPHS_IO_MUX_GPIO5_U);
    //GPIO_OUTPUT_SET(gpio, 0); // pull down */
    pull_down(gpio);
    //os_delay_us(100);
    delay(discharge_delay_ms);

    // (GPIO_REG_READ(GPIO_IN_ADDRESS) = READ_PERI_REG(PERIPHS_GPIO_BASEADDR + GPIO_IN_ADDRESS)
    //volatile uint32_t *gpio_ports = (volatile uint32_t *)(PERIPHS_GPIO_BASEADDR + GPIO_IN_ADDRESS); slower than fixed address
    uint32_t bitmask = 1 << gpio;
    uint32_t *regcache_writer = regcache;

    //GPIO_DIS_OUTPUT(gpio);
    //PIN_PULLUP_EN(PERIPHS_IO_MUX_GPIO5_U);
    int pullup_mode =  use_internal_pullup?INPUT_PULLUP:INPUT;
    threshold_left = threshold;
    int_fast16_t threshold_10steps = (threshold_left - 1)/ 10;
    if(threshold_10steps>9) threshold_10steps = 9;
    else if(threshold_10steps<0) threshold_10steps = 0;
    threshold_10steps ++; // so 0-10: 1 | 11-20: 2 | 21-30: 3 | ... 91-100: 10
    uint16_t direct_reads = threshold_10steps * 10;
    threshold_left -= direct_reads;
    // the following is extremely time critical as the recharging is pretty fast
    // read directly to be maximum fast
    switch(threshold_10steps) {
      case 1: // 0-10 -> 10 steps
        pinMode(gpio, pullup_mode);
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 2: // 11-20 -> 20 steps
        pinMode(gpio, pullup_mode);
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 3: // 21-30 -> 30 steps
        pinMode(gpio, pullup_mode);
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 4:
        pinMode(gpio, pullup_mode);
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 5:
        pinMode(gpio, pullup_mode);
        ulno_do_5x(ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS)));
        break;
      case 6:
        pinMode(gpio, pullup_mode);
        ulno_do_5x(ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS)));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 7:
        pinMode(gpio, pullup_mode);
        ulno_do_5x(ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS)));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 8:
        pinMode(gpio, pullup_mode);
        ulno_do_2x(ulno_do_2x(ulno_do_2x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS))));
        break;
      case 9:
        pinMode(gpio, pullup_mode);
        ulno_do_2x(ulno_do_2x(ulno_do_2x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS))));
        ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS));
        break;
      case 10:
        pinMode(gpio, pullup_mode);
        ulno_do_10x(ulno_do_10x(*((regcache_writer) ++) = GPIO_REG_READ(GPIO_IN_ADDRESS)));
        break;
    }
    // read the potential rest a little slower
    //while (!(gpio_input_get()&(1<<gpio)) && (riseTime < threshold)) { // slow?
    //while (!(gpio_input_get()&32) && (riseTime < threshold)) { // slow?
    //while (!(*gpio_ports&32) && (timer > 0)) { // slower than fixed address
    while ((threshold_left > 0) && !(GPIO_REG_READ(GPIO_IN_ADDRESS)&bitmask)) {
        --threshold_left;
    }

    pull_down(gpio); // needs to be pulled down as early as possible to not acumulate too much charge

    threshold_left = threshold - (threshold_10steps*10) - threshold_left;
    // adjust by the fast read direct accesses
    int timer2 = 0;
    for(int i=0; i<direct_reads; i++) {
      if(regcache[i]&bitmask) {
        break;
      }
      timer2++;
    }
    if( timer2 < direct_reads) threshold_left = timer2;
    else threshold_left += direct_reads;
    charge_time1 = threshold_left; // save time1 for this button
    charge_time2 = timer2; // save time2 for this button

    if (threshold_left < threshold) {
      if(measure_direction_ischarge) { // in this case being under the time means, no human touched the wire -> it is untouched
        decrease_debouncer();
      } else {
        increase_debouncer();
      }
    } else {
      if(measure_direction_ischarge) { // in this case being under the time means, a human touched the wire -> it is touched
        increase_debouncer();
      } else {
        decrease_debouncer();
      }
    }
    return update_state();
  }

  void debug(int debug_level) {
    debug_base(debug_level);
    if(debug_level>=2) {
      Serial.print(" T1");
      Serial.print(charge_time1);
      Serial.print(" T2");
      Serial.print(charge_time1);
      Serial.print(" DB");
      Serial.print(debouncer);
    }
    if(debug_level>=1) {
      Serial.print("  ");
    }
  }
};

class Libni_Push_Button : public Libni_Button {
public:
  Libni_Push_Button( int id, int gpio, int debounce )
  : Libni_Button( id, gpio, debounce ) {
    this->type = PUSH;
  }

  void debug(int debug_level) {
    debug_base(debug_level);
    if(debug_level>=1) {
      Serial.print("  ");
    }
  }
};

void Libni_Buttons::init(int debounce, int threshold,
  int discharge_delay_ms, bool internal_pullup, bool measure_direction_ischarge ) {
  // threshold = 108; // 1/2mOhm resistor + graphite + scotch tape
  // threshold = 400; // > 50000 with 1MOhm
  //threshold = 4;// internal resistor (set to 20 to see speed)
  this->default_threshold = threshold;
  this->default_debounce_max = debounce; // reasonable is 5
  this->default_use_internal_pullup = internal_pullup; // we use it in the simplest version to not have external resistors
  this->default_measure_direction_ischarge = measure_direction_ischarge; // true should be used when using internal pullup
  this->default_discharge_delay_ms = discharge_delay_ms;

  // empty button list
  _size = 0;
  debug(0,0); // no debugging by default
}

void Libni_Buttons::debug( int level, int count ) { // debug level: 0=off, 1=info, 2=all; count: <0: never, n: every n calls
  this->debug_level = level;
  this->debug_count = count;
  debug_frame = 0;
}

Libni_Buttons::Libni_Buttons(int threshold, int debounce,
  int discharge_delay_ms, bool internal_pullup, bool chargedelay ) {
  init( threshold, debounce, discharge_delay_ms, internal_pullup, chargedelay);
}

Libni_Buttons::Libni_Buttons() {
  init( 9, 5, 1, true, true);
}

/* not needed anymore
  void Libni_Buttons::pull_down_all() {
  // pull all buttons down
  for(int b=0; b<_size; b++) {
    int gpio = button_gpio[b];
    pull_down(gpio);
  }
}*/

/*void Libni_Buttons::set_input_all() {
  // pull all buttons down
  for(int b=0; b<_size; b++) {
    int gpio =  button_gpio[b];
    if( use_internal_pullup )
      pinMode(gpio, INPUT_PULLUP);
    else
      pinMode(gpio, INPUT);
  }
} too slow */

void Libni_Buttons::add_touch_button(int id, int gpio_pin, int _threshold) {
  if(_size < MAX_BUTTONS ) {
    button_array[_size] = new Libni_Touch_Button(
        id, gpio_pin, default_debounce_max,
        _threshold, default_discharge_delay_ms,
        default_use_internal_pullup, default_measure_direction_ischarge
    );
    _size ++;
  } else {
    Serial.println("Maximum numbers of buttons defined, not adding new one.\n");
  }
}

void Libni_Buttons::add_touch_button(int id, int gpio_pin) {
  add_touch_button(id,gpio_pin,default_threshold);
}

void Libni_Buttons::add_push_button(int id, int gpio_pin) {
  if(_size < MAX_BUTTONS ) {
    button_array[_size] = new Libni_Push_Button(
        id, gpio_pin, default_debounce_max
    );
    _size ++;
  } else {
    Serial.println("Maximum numbers of buttons defined, not adding new one.\n");
  }
}

bool Libni_Buttons::check() {
  bool one_pressed = false;

  Libni_Touch_Button* tb;
  Libni_Push_Button* pb;

  for(int bnr=_size-1; bnr>=0; bnr--) {
    Libni_Button* b = button_array[bnr];
    if(b->check()) one_pressed = true;
    if(debug_frame >= debug_count) b->debug(debug_level);
  } // end of loop through all buttons
  if(debug_level>=1) {
    if(debug_frame >= debug_count) {
      debug_frame = 0;
      Serial.println();
    }
    debug_frame ++;
  }
  return one_pressed;
}

int Libni_Buttons::get_button(int id) {
  // find button id
  for(int b=_size-1; b>=0; b--) {
    if(get_button_id(b) == id)
      return get_button_state(b);
  }
  return -1;
}
