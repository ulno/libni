// Header file for Touch_Buttons
//
// author: ulno
// created: 2016-03-01

#ifndef _TOUCH_BUTTONS_H
#define _TOUCH_BUTTONS_H

#define MAX_BUTTONS 32

/**
 * This class handles several touch-buttons
 */
class Touch_Buttons {
private:
  int button_array[MAX_BUTTONS*2]; // Tuples of (id,state)
  int button_gpio[MAX_BUTTONS];
  long button_time[MAX_BUTTONS]; // used for measuring, how long the charging took
  int debouncer[MAX_BUTTONS]; // used for debouncing
  long threshold;
  int max_touch;
  int initial_wait; // wait after pulling down before measuring in ms
  bool use_internal_pullup;
  bool measure_chargedelay; // when true, a touch is measured when charge did NOT happen in threshold time. When false, measure if charge was succesfull in the time.
  int _size; // number of buttons defined

  void pull_down_all();
  void set_input_all();

  inline void set_button_id(int nr, int id) {
    button_array[nr*2] = id;
  }

  inline int get_button_id(int nr) {
    return button_array[nr*2];
  }

  inline void set_button_state(int nr, int state) {
    button_array[nr*2+1] = state;
  }

  inline int get_button_state(int nr) {
    return button_array[nr*2+1];
  }

  inline void update_state(int nr) {
    set_button_state(nr, debouncer[nr]>=max_touch/2?1:0);
  }

  inline void increase_debouncer(int nr) {
    if(debouncer[nr] < max_touch) debouncer[nr] ++;
    update_state(nr);
  }

  inline void decrease_debouncer(int nr) {
    if(debouncer[nr] > 0) debouncer[nr] --;
    update_state(nr);
  }


public:
  Touch_Buttons();
  void add_button(int id, int gpio_pin); // add a button for a given gpio_pin and assign id
  void check(); // check and update state of all buttons
  int get_button(int id); // get state of button with specific id
  /**
   * returns pointer to list with tuples of (int id,int state)
   */
  inline int* get_all() { return button_array; };
  inline int size() { return _size; };
  // TODO: add calibration settings
};

#endif
