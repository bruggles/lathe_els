#include <Arduino.h>
//libraries
#include <Arduino_LED_Matrix.h>
ArduinoLEDMatrix matrix;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 30, 4);
#include <EEPROM.h>
int int_default_units;
int rotary_steps_left;
int rotary_steps_right;
int rotary_steps;
int screw_pitch_e;
float screw_pitch;
//#include <RotaryEncoder.h>

//pin declarations
//mode buttons
const int feed_butt = 0;
const int in_thread_butt = 1;
const int met_thread_butt = 2;
const int man_move_butt = 3;
const int settings_butt = 4;
//left and right digital stops
const int l_stop_set_butt = 5; //if wired to a pysical switch as well this will serve as a physical stop button too.
const int l_stop_clear_butt = 6; //serves as clear and rapid left
const int r_stop_set_butt = 7; //if wired to a pysical switch as well this will serve as a physical stop button too.
const int r_stop_clear_butt = 8; //serves as clear and rapid right
//dro zero
const int dro_zero = 9;
//direction selction
const int l_dir = 10;
const int r_dir = 11;
//control rotary encoder pins and settings
const int control_rotary_a = 12;
const int control_rotary_b = 13;
int control_pos = 0;
int control_last_state_a = 0; 
int control_last_step = 0; 
int control_value = 0;
int last_control_value = 0;
float feed_rate = 0.001;
float display_feed_rate = 0.001;
//spindle speed rotary encoder pins
const int spindle_rotary_a = A2; //A2 and A3 support interrupts, which is necessary for accurate reading of the rotary encoder at high speeds. If you change these, make sure to change the interrupt settings in the code as well.
const int spindle_rotary_b = A3;

//stepper pins
const int stepper_1 = A0;
const int stepper_2 = A1;
const int stepper_3 = A4;
const int stepper_4 = A5;

const int mode_buttons[] = {
    feed_butt, 
    in_thread_butt, 
    met_thread_butt, 
    man_move_butt, 
    settings_butt 
};

const int other_buttons[] = {
    l_dir,
    r_dir,
    control_rotary_a,
    control_rotary_b,
    l_stop_set_butt,
    l_stop_clear_butt,
    r_stop_set_butt,
    r_stop_clear_butt,
    dro_zero,
    stepper_1,
    stepper_2,
    stepper_3,
    stepper_4
};

const float in_threads[] = {
    0.02381,  //42 TPI
    0.02500,  //40 TPI
    0.02632,  //38 TPI
    0.02778,  //36 TPI
    0.02941,  //34 TPI
    0.03125,  //32 TPI
    0.03333,  //30 TPI
    0.03571,  //28 TPI
    0.03846,  //26 TPI
    0.04167,  //24 TPI
    0.04545,  //22 TPI
    0.05000,  //20 TPI
    0.05556,  //18 TPI
    0.06250,  //16 TPI
    0.06667,  //15 TPI
    0.07143,  //14 TPI
    0.07692,  //13 TPI
    0.08333,  //12 TPI
    0.08696,  //11.5 TPI
    0.09091,  //11 TPI
    0.10000,  //10 TPI
    0.11111,  //9 TPI
    0.12500,  //8 TPI
    0.14286,  //7 TPI
    0.16667,  //6 TPI
    0.18182,  //5.5 TPI
    0.20000,  //5 TPI
    0.22222,  //4.5 TPI
    0.25000,  //4 TPI
};
int in_thread_key = 11;
int max_in_threads = 28;

const float met_threads[] = {
    0.4,  
    0.5,
    0.7,
    0.75,
    0.8,
    0.9,
    1.0,
    1.25,
    1.5,
    1.75,
    2.0,
    2.5,
    3.0,
    3.5,
    4.0,
    4.5,
    5.0,
    5.5,
    6.0,
    6.5,
    7.0
};
int met_thread_key = 7;
int max_met_threads = 20;

const float man_feed_rates[] = {
    0.0005,
    0.001,  
    0.005,
    0.010,
    0.050,
    0.100,
};
int man_feed_key = 1;
int max_man_feed_key = 5;
float man_feed_speed = man_feed_rates[man_feed_key];
float man_move_dist = 0;
int man_move_val = 0;
int feed_key = 6;

String mode = "feed";
String last_mode = "feed";
String default_units;
int mode_change = 0;

int direction = 0;
int last_direction = 0;



int rpm = 0;
float l_stop_loc = -999.0;
float r_stop_loc = 2.0;
float dro_pos = 0.0;
float last_dro_pos = 0.0;
int stepper_rpm = 0;
int rapid_rpm = 1000;

byte matrix_array[8][12] = {
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

int read_dir(int left_dir_button, int right_dir_button){
    int dir;

        if (digitalRead(left_dir_button) == LOW) {
            dir = -1;
            //button is pressed
        }
        else if (digitalRead(right_dir_button) == LOW) {
            dir = 1;
            //button is pressed
        }
        else {
            dir = 0;
        }
    //Serial.println(("Direction: " + String(dir)));
    return dir;
}

String read_mode(String last_mode){
    String md = "none";
    for (int i = 0; i < 5; i++) {
        if (digitalRead(mode_buttons[i]) == LOW) {
            switch (i) {
                case 0:
                    md = "feed";
                    break;
                case 1:
                    md = "in_thread";
                    break;
                case 2:
                    md = "met_thread";
                    break;
                case 3:
                    md = "man_move";
                    break;
                case 4:
                    md = "settings";
                    break;
            }
        }
    }
    if (md == "none") {
        md = last_mode;
    }
    //Serial.println(("Mode: " + md));
    return md;
}

int control_rotary_read(){
    int current_state_a = digitalRead(control_rotary_a);
    int movement;
    if (current_state_a != control_last_state_a) {
        if (digitalRead(control_rotary_b) != current_state_a) {
            movement = 1;
        }
        else {
            movement = -1;
        }
    }
    control_last_state_a = current_state_a;
    return movement;
    delay(30);
}

int rotary_step_read(int steps, int &position, int &last_state_a, int &last_step, int &value){
    //This function reads the rotary encoder and converts the readings into output values. It can change what size the step is before the output changes.
    //Step is how many clicks create one change in output
    //position is where the encoder is now - this tracks changes between steps
    //last_state_a is to track when the state has changed
    //last_step is the value when the last step was hit - used to calculate when the steps have been hit
    //value is the output value
    int current_state_a = digitalRead(control_rotary_a);
    if (current_state_a != last_state_a) {
        if (digitalRead(control_rotary_b) != current_state_a) {
            position++;
            //Serial.println("plus");
            if (position >= last_step + steps) {
                value++;
                last_step = position;
            }
        }
        else {
            position--;
            //Serial.println("minus");
            if (position <= last_step - steps) {
                value--;
                last_step = position;
            }
        }
    }
    last_state_a = current_state_a;
    //Serial.println("value: " + String(value));
    return value;
}

String screen_mode(String mode){
    String md;
    if (mode == "feed") {
        md = "Feed Md ";
    }
    else if (mode == "in_thread") {
        md = "IN Thrd ";
    }
    else if (mode == "met_thread") {
        md = "MM Thrd ";
    }
    else if (mode == "man_move") {
        md = "Move Md ";
    }
    else if (mode == "settings") {
        md = "Settings";
    }
    else {
        md = "Unknown";
    }
    return md;
}
String fill_string(String str, int length, String fill_char = " "){
    while (str.length() < length) {
        str = fill_char + str; // Prepend the fill character until length is reached
    }
    return str;
}

void lcd_print(String lcd_print_mode = "normal"){  
    if (lcd_print_mode == "normal") {
        String row2;
        String row4;
        String row1 = screen_mode(mode) + " | RPM: " + fill_string(String(rpm), 4, " ");
        String ls;
        String rs;
        lcd.setCursor(0, 0);
        lcd.print(row1);
        if (mode == "feed"){
            row2 = "Feed Rt: " + String(display_feed_rate, 3) + " " + default_units + "/R";
        }
        else if (mode == "in_thread"){
            row2 = "TPI: " + String(1.0 / display_feed_rate, 0) + " | " + String(display_feed_rate, 4) + " I/R";
        }
        else if (mode == "met_thread"){
            row2 = "Pitch: " + String(display_feed_rate, 2) + " MM/R";
        }
        else if (mode == "man_move"){
            row2 = "Speed: " + String(man_feed_speed, 4) + " " + default_units + "/C";
        }
        lcd.setCursor(0, 1);
        lcd.print(row2);
        if (l_stop_loc <= -999) {
            ls = "NA";
        }
        else {
            ls = String(l_stop_loc, 3);
        }
        if (r_stop_loc <= -999) {
            rs = "NA";
        }
        else {
            rs = String(r_stop_loc, 3);
        }
        lcd.setCursor(0, 2);
        lcd.print("< " + ls);
        lcd.setCursor(11, 2);
        lcd.print(fill_string(rs, 7, " ") + " >");
        row4 = "Screw DRO:" + fill_string(String(dro_pos, 3), 7, " ") + " " + default_units;
        lcd.setCursor(0, 3);
        lcd.print(row4);
    }
    else if (lcd_print_mode == "fast"){
        //only update the parts of the LCD that change frequently, like the DRO position, to reduce flickering and improve performance.
        lcd.setCursor(10, 3);
        lcd.print(fill_string(String(dro_pos, 3), 7, " "));
    }
}

int read_spindle_speed(){
    //read the spindle speed from the spindle rotary encoder and update the rpm variable. Use interrupts for accurate reading at high speeds.
    
    return rpm;
}

int calculate_stepper_rpm(){
    //calculate the appropriate stepper motor speed based on feed rate, spindle speed, and screw_pitch.
    //set as stepper_rpm when called
    return (feed_rate * rpm) / screw_pitch;
}

void set_stepper_speed(int stepper_rpm, int direction = 0){
    //set the stepper motor speed based on the feed rate and spindle speed. This will involve calculating the appropriate delay between steps to achieve the desired feed rate at the current spindle speed.
    int delete_me = 0;
}

void feed_rate_calc(int key_val, const float list_of_vals[] = {}){
    //calculate the feed rate based on the current mode and control value. In feed mode, the feed rate is directly proportional to the control value. In thread modes, the feed rate is determined by the selected thread pitch.
    if (mode == "feed") {
        display_feed_rate = control_value * 0.001; //convert to in/rev
        feed_rate = display_feed_rate;
    }
    else if (mode == "in_thread") {
        display_feed_rate = list_of_vals[key_val];
        feed_rate = display_feed_rate;
    }
    else if (mode == "met_thread") {
        display_feed_rate = list_of_vals[key_val];
        feed_rate = display_feed_rate/25.4; //convert to in/rev
    }
}

void auto_move(int step_size, int max_val, int &key_val, const float list_of_vals[] = {}){
    //move the lathe at the specified feed rate in the specified direction until a stop button is pressed or a stop limit switch is triggered. If in thread mode, also monitor the spindle speed and adjust the feed rate to maintain the correct thread pitch.
    if (direction == 0) {
        // stop all stepper movement
        set_stepper_speed(0);
        //act if the mode changed since the last loop
        if (mode_change) {
            rpm = read_spindle_speed();
            last_control_value = key_val;
            control_value = key_val;
            //Serial.println("Mode changed to " + mode + " Control Value: " + String(control_value));
            control_pos = 0;
            control_last_state_a = digitalRead(control_rotary_a);
            control_last_step = 0;
            feed_rate_calc(key_val, list_of_vals);
            lcd.clear();
        }
        //rotary jumps through thread pitches, and the LCD displays the current TPI and feed rate
        rotary_step_read(step_size, control_pos, control_last_state_a, control_last_step, control_value);
        if (control_value != last_control_value || mode_change == 1) {
            if (control_value < 0) {
                control_value = max_val;
            }
            else if (control_value > max_val) {
                control_value = 0;
            }
            key_val = control_value;
            feed_rate_calc(key_val, list_of_vals);
            last_control_value = control_value;
            lcd_print();
        }
        //set and clear digital stops
          if (digitalRead(l_stop_set_butt) == LOW) {
              l_stop_loc = dro_pos;
              lcd.clear();
              lcd_print();
          }
          if (digitalRead(l_stop_clear_butt) == LOW) {
              l_stop_loc = -999.0;
              lcd.clear();
              lcd_print();
          }
          if (digitalRead(r_stop_set_butt) == LOW) {
              r_stop_loc = dro_pos;
              lcd_print();
          }
          if (digitalRead(r_stop_clear_butt) == LOW) {
              r_stop_loc = -999.0;
              lcd_print();
          }
        //clear the DRO position when the DRO zero button is pressed
         if (digitalRead(dro_zero) == LOW) {
            last_dro_pos = dro_pos;
            l_stop_loc = l_stop_loc - dro_pos;
            r_stop_loc = r_stop_loc - dro_pos;
            dro_pos = 0.0;
            lcd_print();
        }
    }
    // directions are selected efficiently read and move
    else {
        //monitor stop buttons and stop limits
        if (digitalRead(l_stop_set_butt) == LOW || digitalRead(r_stop_set_butt) == LOW || l_stop_loc <= dro_pos || r_stop_loc >= dro_pos) {
            //stop the movement
            set_stepper_speed(0);
            Serial.println("Stop button pressed or stop limit reached. Stopping movement.");
        }
        //monitor rapid buttons (clear stop buttons)
        else if (digitalRead(l_stop_clear_butt) == LOW){
            //Rapid Speed left
            set_stepper_speed(rapid_rpm, -1);
            Serial.println("Rapid Left button pressed. Use rapid_rpm speed");
        }
        else if (digitalRead(r_stop_clear_butt) == LOW) {
            //Rapid Speed right
            set_stepper_speed(rapid_rpm, 1);
            Serial.println("Rapid Right button pressed. Use rapid_rpm speed");
        }
        else {
            //no rapid buttons pressed, use normal feed rate
            //read speed
            rpm = read_spindle_speed();
            //set stepper speed and direction
            set_stepper_speed(rpm, direction);
            
        }
        
    }
}

void man_move(){
    //manual movement mode, where the feed rate is directly controlled by the rotary encoder and the
    if (direction == 0) {
          // stop all stepper movement
          set_stepper_speed(0);
          if (mode_change) {
              rpm = read_spindle_speed();
              control_last_state_a = digitalRead(control_rotary_a);
              control_last_step = 0;
              lcd.clear();
          }
          //set click increments
          if (digitalRead(man_move_butt) == LOW){
              man_feed_key ++;
              if (man_feed_key > max_man_feed_key) {
                  man_feed_key = 0;
              }
              else if (man_feed_key < 0) {
                  man_feed_key = max_man_feed_key;
              }
              man_feed_speed = man_feed_rates[man_feed_key];
              delay(100);
          }

          lcd_print();
          //read the rotary encoder to move the stepper manually.
          int temp_val = 0;
          int temp_control_value = 0;
          man_move_val = rotary_read(2, control_pos, control_last_state_a, control_last_step, temp_control_value);
          man_move_dist = man_move_val * man_feed_speed;
          Serial.println("Man move val: " + String(man_move_val) + " Man move dist: " + String(man_move_dist) + " Control Pos: " + String(control_pos) + " Control Last Step: " + String(control_last_step));
          if (man_move_val != 0) {
              //move the stepper the appropriate distance as fast as possible
              //probably need a manual move function
              Serial.println('moved ' + String(man_feed_speed));
              //Serial.println('control_pos'+String(control_pos));
              man_move_dist = 0;
          }
    }
  }

void setup() {
    //get stored variables from EEPROM
    if (EEPROM.read(0) == 255) {
        //values have been stored, read them
        int int_default_units = 0;
        EEPROM.update(0, int_default_units);
    }
    else {
        int int_default_units = EEPROM.read(0);
    }
    if (int_default_units == 0) {
        default_units = "IN";
    }
    else if (int_default_units == 1) {
        default_units = "MM";
    }
    else if (int_default_units == 2) {
        default_units = "CM";
    }
    if (EEPROM.read(1) == 255) {
        //values have been stored, read them
        int rotary_steps_left = 6;
        int rotary_steps_right = 0;
        EEPROM.update(1, rotary_steps_left);
        EEPROM.update(2, rotary_steps_right);
    }
    else {
        int rotary_steps_left = EEPROM.read(1);
        int rotary_steps_right = EEPROM.read(2);
    }
    int rotary_steps = rotary_steps_left * 100 + rotary_steps_right;
    if (EEPROM.read(3) == 255) {
        //values have been stored, read them
        int screw_pitch_e = 100;
        EEPROM.update(3, screw_pitch_e);
    }
    else {
        int screw_pitch_e = EEPROM.read(3);
    }
    screw_pitch = screw_pitch_e /1000.0; //convert to in/rev
    // initialize control rotary encoder variables


    //initialize the LCD screen

    // initialize the mode pushbuttons as inputs:
    for (int val : mode_buttons) {
        pinMode(val, INPUT_PULLUP);
    }
    // initialize the other buttons as inputs:
    for (int val : other_buttons) {
        pinMode(val, INPUT_PULLUP);
    }
    control_last_state_a = digitalRead(control_rotary_a);

    //initialize the onboard LED matrix
    matrix.begin();
    lcd.init();  // initialize the lcd
    lcd.backlight();
    lcd_print();
    Serial.begin(9600);
    
}

void loop(){

    
    // Mode selection
    if (direction == 0) {
        mode = read_mode(mode);
        if (mode != last_mode) {
            last_mode = mode;
            mode_change = 1;
        }
        else {
            mode_change = 0;
        }
    }
    if (mode == "feed") {
        //feed mode selected
        auto_move(5, 1000, feed_key);
    }
    else if (mode == "in_thread") {
        //Serial.println("In Thread Mode");
        auto_move(20, max_in_threads, in_thread_key, in_threads);
    }
    else if (mode == "met_thread") {
      //Serial.println(" Metric Thread Mode");
        auto_move(20, max_met_threads, met_thread_key, met_threads);
    }
    else if (mode == "man_move") {
        man_move();
    }
    else if (mode == "settings") {
        Serial.println(" Settings Mode");
    }

    // Direction selection
    direction = read_dir(l_dir, r_dir);
    if (direction != last_direction) {
        last_direction = direction;
        rpm = read_spindle_speed();
        lcd_print();
    }
    if (direction == -1) {
        matrix_array[7][9] = 1;
        matrix_array[7][10] = 0;
        matrix_array[7][11] = 0;
        //move left
    }
    else if (direction == 1) {
        matrix_array[7][9] = 0;
        matrix_array[7][10] = 0;
        matrix_array[7][11] = 1;
        //move right
    }
    else {
        matrix_array[7][9] = 0;
        matrix_array[7][10] = 1;
        matrix_array[7][11] = 0;
        //no direction selected
    }

    // Write to matrix
    matrix.renderBitmap(matrix_array, 8, 12);
}

