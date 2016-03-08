// Header file for Ulno_Buttons
//
// author: ulno
// created: 2016-03-01

#ifndef _Ulno_BUTTONS_H
#define _Ulno_BUTTONS_H

#define MAX_BUTTONS 32

/**
  * helper class for ulno_Buttons (representing one instance of a button)
  */
class Ulno_Button {
public:
  static const int GENERIC = 0;
  static const int TOUCH = 1;
  static const int PUSH = 2;
protected:
  int type;
  int id;
  int gpio; // related main gpio pin
  bool state;
  int debounce_max;
  int debouncer;
  bool use_pullup;

  inline bool update_state() {
    if(debouncer>=debounce_max/2) {
      state = true;
    } else {
      state = false;
    }
    return state;
  }

  inline void increase_debouncer() {
    if(debouncer < debounce_max) debouncer ++;
  }

  inline void decrease_debouncer() {
    if(debouncer > 0) debouncer --;
  }

  void debug_base(int debug_level);

public:
  Ulno_Button(int id, int gpio, int debounce, bool pullup);
  inline int get_id() {return id;}
  inline bool get_state() {return state;}
  inline int get_type() {return type;}
  virtual bool check();
  virtual void debug(int debug_level);
};

/**
 * This class handles several touch or push buttons
 */
class Ulno_Buttons {
public:
  static const int FIRE=' ';
  static const int DOWN=14;
  static const int LEFT=2;
  static const int RIGHT=6;
  static const int UP=16;
  static const int ESCAPE=27;
private:
  Ulno_Button * button_array[MAX_BUTTONS];
  int default_threshold;
  int default_debounce_max;
  int default_discharge_delay_ms; // wait after pulling down before measuring in ms
  bool default_use_internal_pullup;
  bool default_measure_direction_ischarge; // when true, a touch is measured when charge did NOT happen in threshold time. When false, measure if charge was succesfull in the time.
  int _size; // number of buttons defined

  /*void pull_down_all();
  void set_input_all();*/


  void init( int debounce, int threshold, int discharge_delay_ms, bool internal_pullup, bool chargedelay );

  int debug_count;
  int debug_level;
  int debug_frame;
public:
  // add info for touchbuttons
  Ulno_Buttons( int debounce, int threshold,  int discharge_delay_ms, bool internal_pullup, bool chargedelay);
  Ulno_Buttons(); // default init for internal pullups and metal touch sensors
  void debug( int level, int count ); // debug level: 0=off, 1=info, 2=all; count: <0: never, n: every n calls
  void add_push_button(int id, int gpio_pin); // add a push button for a given gpio_pin and assign id
  void add_push_button(int id, int gpio_pin, bool internal_pullup); // add a push button for a given gpio_pin and assign id
  void add_touch_button(int id, int gpio_pin); // add a button for a given gpio_pin and assign id
  void add_touch_button(int id, int gpio_pin, int threshold); // also specify button-specific threshold
  void add_touch_button(int id, int gpio_pin, bool internal_pullup, int threshold); // also specify pull-up-use
  bool check(); // check and update state of all buttons, if one pressed return true, else false
  int get_button(int id); // get state of button with specific id

  inline int size() { return _size; }; // number of buttons managed

  inline int get_button_id(int nr) {
    return button_array[nr]->get_id();
  }

  inline int get_button_state(int nr) { // state
    return button_array[nr]->get_state();
  }

  // TODO: add calibration settings
};

#endif // _Ulno_BUTTONS_H
