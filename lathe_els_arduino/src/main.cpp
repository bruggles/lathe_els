#include <Arduino.h>
//libraries
//#include <Arduino_LED_Matrix.h>
//ArduinoLEDMatrix matrix;
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 30, 4);
#include <EEPROM.h>
int rotary_steps_left;
int rotary_steps_right;
int rotary_steps;
//#include <avr/interrupt.h>
String mode = "feed";
String last_mode = "feed";

int mode_change = 0;

int direction = 0;
int last_direction = 0;
//#include <RotaryEncoder.h>

//pin declarations
//mode buttons
const int feed_butt = A0;
const int in_thread_butt = A1;
const int met_thread_butt = A2;
const int man_move_butt = A3;
const int settings_butt = 6; //Note, don't use A4 or A5 for conflicts with the i2c - maybe better package for i2c could help or somehow explicitly not allowing those pins to interact
//left and right digital stops
const int l_stop_set_butt = 0; //if wired to a pysical switch as well this will serve as a physical stop button too.
const int l_stop_clear_butt = 1; //serves as clear and rapid left
const int r_stop_set_butt = 7; //if wired to a pysical switch as well this will serve as a physical stop button too.
const int r_stop_clear_butt = 8; //serves as clear and rapid right
//rapid and dro zero
const int rapid = 6;
const int dro_zero = 9;
//direction selction
const int l_dir = 10;
const int r_dir = 11;
//control rotary encoder pins and settings
//const int control_rotary_a = 12;
const int l_move = 12;
//const int control_rotary_b = 13;
const int r_move = 13;
int control_pos = 0;
//int control_last_state_a = 0; 
//int control_last_step = 0; 
int control_value = 0;
int last_control_value = 0;
float feed_rate = 0.001;
float steps_per_rot_ind = 0.0;
float display_feed_rate = 0.001;
//spindle speed rotary encoder pins
const int spindle_rotary_a = 2; //2 and 3 support interrupts, which is necessary for accurate reading of the rotary encoder at high speeds. If you change these, make sure to change the interrupt settings in the code as well.
const int spindle_rotary_b = 3;
//stepper pins
const int stepper_dir = 4;
const int stepper_step = 5; //pwm pin, not sure if this is helpful or not, but it should work either way

//ROTARY ENCODER CODE --------------------------------------------------

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
    //control_rotary_a,
    //control_rotary_b,
    l_move,
    r_move,
    l_stop_set_butt,
    l_stop_clear_butt,
    r_stop_set_butt,
    r_stop_clear_butt,
    dro_zero,
    spindle_rotary_a,
    spindle_rotary_b,
    rapid
};

const int stepper_outputs[] = {
    stepper_dir,
    stepper_step
};

int step_loc = 0; //location of the stepper motor in steps
float loc = 0; //location in inches
int move_steps = 0;

void set_stepper_speed(int stepper_rpm, int direction = 0){
    //set the stepper motor speed based on the feed rate and spindle speed. This will involve calculating the appropriate delay between steps to achieve the desired feed rate at the current spindle speed.
    int delete_me = 0;
}

const float feed_rates[] = {
    0.0005,
    0.0010,  
    0.0015,
    0.0020,
    0.0030,
    0.0040,
    0.0050,
    0.0060,
    0.0080,
    0.0100,
    0.0120,
    0.0150,
    0.0200,
    0.0250,
    0.0300
};

const int feed_rates_int[] = {
    50,
    100,  
    150,
    200,
    300,
    400,
    500,
    600,
    800,
    1000,
    1200,
    1500,
    2000,
    2500,
    3000
};
int feed_rate_key = 4;
int max_feed_rate = 14;

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

const int in_threads_int[] = {
    2381,  //42 TPI
    2500,  //40 TPI
    2632,  //38 TPI
    2778,  //36 TPI
    2941,  //34 TPI
    3125,  //32 TPI
    3333,  //30 TPI
    3571,  //28 TPI
    3846,  //26 TPI
    4167,  //24 TPI
    4545,  //22 TPI
    5000,  //20 TPI
    5556,  //18 TPI
    6250,  //16 TPI
    6667,  //15 TPI
    7143,  //14 TPI
    7692,  //13 TPI
    8333,  //12 TPI
    8696,  //11.5 TPI
    9091,  //11 TPI
    10000,  //10 TPI
    11111,  //9 TPI
    12500,  //8 TPI
    14286,  //7 TPI
    16667,  //6 TPI
    18182,  //5.5 TPI
    20000,  //5 TPI
    22222,  //4.5 TPI
    25000,  //4 TPI
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

const int met_threads_in_int[] = {
    1575,
    1969,
    2756,
    2953,
    3150,
    3543,
    3937,
    4921,
    5906,
    6890,
    7874,
    9843,
    11811,
    13780,
    15748,
    17717,
    19685,
    21654,
    23622,
    25591,
    27559
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

const int man_feed_rates_int[] = {
    50,
    100,
    500,
    1000,
    5000,
    10000
};
int man_feed_key = 1;
int max_man_feed_key = 5;
float man_feed_speed = man_feed_rates[man_feed_key];
float man_move_dist = 0;
int man_move_delay = 0;
int man_move_val = 0;
int feed_key = 6;
int int_screw_pitch;
float screw_pitch;
int int_reverse_feed;
int int_default_units;
String default_units;
//int int_stepper_ratio;
int int_rot_enc_steps_hund;
int int_stepper_steps_hund;
//const String menu_items[] = {"Default Units", "Screw Pitch", "Reverse Direction"};
const char* du_options[] = {"IN","MM","CM"};
const char* sp_options[] = {"float_div_thou"};
const char* tf[] = {"T","F"};
const char* int_val[] = {"int"};
const char* int_hund_val[] = {"int_mult_hundred"};

struct MenuVars {
  const char* title;
  int* variable; // Use void* to store pointers to different types
  const char** options; // Options for the menu
  int max_option;   // Length of the options array
  int default_key;
  int eeprom_start;
  int eeprom_end;
};

MenuVars menu_vars[] = {
    //{"Title", $variable_to_change, {options}, max_index_of_options}
    {"Default Units", &int_default_units, du_options, 2, 0, 0, 0},
    {"Screw Ptch", &int_screw_pitch, sp_options, 250, 100, 1, 1},   
    {"Reverse Scrw", &int_reverse_feed, tf, 1, 1, 2, 2},  
    //{"Step Ratio", &int_stepper_ratio, int_val, 100, 5, 3, 3},
    {"Step Steps", &int_stepper_steps_hund, int_hund_val, 40, 4, 5, 5},
    {"Enc Clicks", &int_rot_enc_steps_hund, int_hund_val, 20, 6, 4, 4}
};

int menu_key = 0;
int max_menu_key = 4;

int rpm = 0;
float l_stop_loc = -999.0;
float r_stop_loc = 2.0;
float dro_pos = 0.0;
float last_dro_pos = 0.0;
int stepper_rpm = 0;
int rapid_rpm = 1000;


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

void delayed_read_dir(int &value, int mili_delay = 50){
    int movement = read_dir(l_move,r_move);
    value += movement;
    
    delay(mili_delay);
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
String fill_string(String str, int length, String fill_char = " ", String side = "left"){
    if (side == "left"){
        while (str.length() < length) {
            str = fill_char + str; // Prepend the fill character until length is reached
        }
    } else {
        while (str.length() < length) {
            str = str + fill_char; // Prepend the fill character until length is reached
        }
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

// STEPPER FUNCTIONS ------------------------------------------------------------
//Default microstep mode function
void stop_stepper(){
    digitalWrite(stepper_dir, LOW);
    digitalWrite(stepper_step, LOW);
}

void move_stepper(int steps, int delay_time){
    
    if(steps < 0) {
        digitalWrite(stepper_dir, LOW); //Pull direction pin low to move "forward"
        //Serial.println("Moving reverse at default step mode.");
    }
    else if(steps > 0) {
        digitalWrite(stepper_dir, HIGH); //Pull direction pin high to move in "reverse"
        //Serial.println("Moving forward at default step mode.");
    }
    else {
        return;
    }
    for(int x = 0; x <= abs(steps); x++){  //Loop the forward stepping enough times for motion to be visible
        digitalWrite(stepper_step,HIGH); //Trigger one step forward
        delayMicroseconds(100);
        digitalWrite(stepper_step,LOW); //Pull step pin low so it can be triggered again
        delayMicroseconds(delay_time);
        
    }
}
int calculate_stepper_rpm(){
    //calculate the appropriate stepper motor speed based on feed rate, spindle speed, and screw_pitch.
    //set as stepper_rpm when called
    return (feed_rate * rpm) / screw_pitch;
}

//ROTARY ENCODER CODE --------------------------------------------------
volatile byte aFlag = 0; // let's us know when we're expecting a rising edge on pinA to signal that the encoder has arrived at a detent
volatile byte bFlag = 0; // let's us know when we're expecting a rising edge on pinB to signal that the encoder has arrived at a detent (opposite direction to when aFlag is set)
volatile byte encoderPos = 0; //this variable stores our current value of encoder position. Change to int or uin16_t instead of byte if you want to record a larger range than 0-255
volatile byte oldEncPos = 0; //stores the last encoder position value so we can compare to the current reading and see if it has changed (so we know when to print to the serial monitor)
volatile byte a = 0; //somewhere to store the direct values we read from our interrupt pins before checking to see if we have moved a whole detent
volatile byte b = 0;
volatile int dest_steps = 0;
static R_PORT0_Type * const port_table[] = { R_PORT0, R_PORT1, R_PORT2, R_PORT3, R_PORT4, R_PORT5, R_PORT6, R_PORT7 };

static const uint16_t mask_table[] = { 1 << 0, 1 << 1, 1 << 2, 1 << 3, 1 << 4, 1 << 5, 1 << 6, 1 << 7,
                                       1 << 8, 1 << 9, 1 << 10, 1 << 11, 1 << 12, 1 << 13, 1 << 14, 1 << 15 };

static inline uint16_t digitalReadFast(pin_size_t pin) {
  uint16_t hardware_port_pin = g_pin_cfg[pin].pin;
  uint16_t pin_mask = mask_table[hardware_port_pin & 0xf];
  R_PORT0_Type * const portX = port_table[hardware_port_pin >> 8];

  return (portX->PIDR & pin_mask);
}

void PinA(){
  noInterrupts(); //stop interrupts happening before we read pin values
  //reading = PIND & 0xC; // read all eight pin values then strip away all but pinA and pinB's values
  a = digitalReadFast(spindle_rotary_a);
  b = digitalReadFast(spindle_rotary_b); 
  if(a && b && aFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
      //move_stepper(1,0); 
      dest_steps ++; //steps_per_rot_ind*direction;
      //Serial.println("dest_steps: "+String(dest_steps)+" steps_per_rot: "+String(steps_per_rot_ind)+" dir: "+String(direction));
      bFlag = 0;
      aFlag = 0;
  }
  else if (a){
      bFlag = 1;
  }

  //  bFlag = 0; //reset flags for the next turn
  //  aFlag = 0; //reset flags for the next turn
  //}
  //else if (reading == B00000100) bFlag = 1; //signal that we're expecting pinB to signal the transition to detent from free rotation
  interrupts(); //restart interrupts
}

void PinB(){
  noInterrupts(); //stop interrupts happening before we read pin values
  //reading = PIND & 0xC; //read all eight pin values then strip away all but pinA and pinB's values
  //if (reading == B00001100 && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
    //encoderPos ++; //increment the encoder's position count
  //  bFlag = 0; //reset flags for the next turn
  //  aFlag = 0; //reset flags for the next turn
  //}
  //else if (reading == B00001000) aFlag = 1; //signal that we're expecting pinA to signal the transition to detent from free rotation
  a = digitalReadFast(spindle_rotary_a);
  b = digitalReadFast(spindle_rotary_b); 
  if(a && b && bFlag) { //check that we have both pins at detent (HIGH) and that we are expecting detent on this pin's rising edge
      //move_stepper(-1,0); 
      dest_steps --; //= steps_per_rot_ind*direction;
      //Serial.println("dest_steps: "+String(dest_steps));
      bFlag = 0;
      aFlag = 0;
  }
  else if (b){
      aFlag = 1;
  }
  interrupts(); //restart interrupts
}



void feed_rate_calc(int key_val, const float list_of_vals[], const int list_of_ints[]){
    //calculate the feed rate based on the current mode and control value. In feed mode, the feed rate is directly proportional to the control value. In thread modes, the feed rate is determined by the selected thread pitch.
    int rev_dir;
    if (int_reverse_feed == 0){
        rev_dir = 1;
    }
    else {
        rev_dir = -1;
    }
    display_feed_rate = list_of_vals[key_val];
    //dist_per_stepper_step = (int_screw_pitch/int_stepper_steps_hund);
    //desired_dist_per_read = (list_of_ints[key_val]/(int_rot_enc_steps_hund * 100));
    steps_per_rot_ind =  (rev_dir)*(list_of_ints[key_val]/(int_rot_enc_steps_hund * 100))/(int_screw_pitch/int_stepper_steps_hund);
    //Serial.println("fc: val: ("+String(list_of_ints[key_val])+"/"+String(int_rot_enc_steps_hund)+"*100)/("+String(int_screw_pitch)+"/"+int_stepper_steps_hund+")");
}

void set_clear_stops_dro(){
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

void auto_move(int mili_delay, int max_val, int &key_val, const float list_of_vals[], const int list_of_int_vals[]){
    //move the lathe at the specified feed rate in the specified direction until a stop button is pressed or a stop limit switch is triggered. If in thread mode, also monitor the spindle speed and adjust the feed rate to maintain the correct thread pitch.
    if (direction == 0) {
      //if(steps_per_rot_ind == 0){
      //    feed_rate_calc(key_val, list_of_vals, list_of_int_vals);
      //}
        // stop all stepper movement
        set_stepper_speed(0);
        //act if the mode changed since the last loop
        if (mode_change) {
            rpm = read_spindle_speed();
            last_control_value = key_val;
            control_value = key_val;
            feed_rate_calc(key_val, list_of_vals, list_of_int_vals);
            lcd.clear();
        }
        //rotary jumps through thread pitches, and the LCD displays the current TPI and feed rate
        //rotary_step_read(step_size, control_pos, control_last_step, control_value);
        delayed_read_dir(control_value, mili_delay);
        if (control_value != last_control_value || mode_change == 1) {
            if (control_value < 0) {
                control_value = max_val;
            }
            else if (control_value > max_val) {
                control_value = 0;
            }
            key_val = control_value;
            feed_rate_calc(key_val, list_of_vals, list_of_int_vals);
            last_control_value = control_value;
            lcd_print();
        }
        set_clear_stops_dro();
    }
    // directions are selected efficiently read and move
    else {
        //monitor stop buttons and stop limits
        if (digitalRead(l_stop_set_butt) == LOW || digitalRead(r_stop_set_butt) == LOW || l_stop_loc >= dro_pos || r_stop_loc <= dro_pos) {
            //stop the movement
            //set_stepper_speed(0);
            Serial.println("dro_pos: "+String(dro_pos)+" l stop: "+String(l_stop_loc)+" r stop: "+String(r_stop_loc));
            Serial.println("Stop button pressed or stop limit reached. Stopping movement.");
        }
        //monitor rapid buttons (clear stop buttons)
        else if (digitalRead(rapid) == LOW){
            //Rapid Speed left
            set_stepper_speed(rapid_rpm, direction);
            Serial.println("Rapid button pressed. Use rapid_rpm speed");
        }
        else {
            //no rapid buttons pressed, use normal feed rate
            //read speed
            

            move_steps = int(dest_steps*steps_per_rot_ind*direction)-step_loc;
            Serial.println("dest_steps: "+String(dest_steps)+" step_loc: "+String(step_loc)+" move_steps: "+String(move_steps));
            Serial.println("dir: "+String(direction)+" steps_per_rot: "+String(steps_per_rot_ind));
            move_stepper(move_steps, 50);
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
              //control_last_state_a = digitalRead(control_rotary_a);
              //control_last_step = 0;
              lcd.clear();
              lcd_print();
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
              man_move_delay = (1/man_feed_speed)*100;
              lcd_print();
              delay(100);
          }

          
          //read the rotary encoder to move the stepper manually.
          int temp_val = 0;
          int temp_control_value = 0;
          man_move_val = read_dir(l_move,r_move);
          
          //Serial.println("Man move val: " + String(man_move_val) + " Man move dist: " + String(man_move_dist) + " Feed Speed: " + String(man_feed_speed));
          if (man_move_val != 0) {
              move_stepper(man_move_val, man_move_delay);
              //Serial.println(man_move_val);
          }
          set_clear_stops_dro();
    }
}



void read_eeprom(){
    int eeprom_val;
    for (int i = 0; i<= max_menu_key; i++){
        if (menu_vars[i].eeprom_start == menu_vars[i].eeprom_end){
            eeprom_val = EEPROM.read(menu_vars[i].eeprom_start);
            if(eeprom_val == 255){
                EEPROM.update(menu_vars[i].eeprom_start, menu_vars[i].default_key); 
                *menu_vars[i].variable = menu_vars[i].default_key;
            }
            else{
                *menu_vars[i].variable = eeprom_val;
            }
        }
        //need an else here if we get longer values
        if (i == 0){
            default_units = String(menu_vars[i].options[*(static_cast<int*>(menu_vars[i].variable))]);
        }
    }
}

void write_eeprom(){
    int eeprom_val;
    for (int i = 0; i<= max_menu_key; i++){
        if (menu_vars[i].eeprom_start == menu_vars[i].eeprom_end){
            EEPROM.update(menu_vars[i].eeprom_start, *menu_vars[i].variable);
        }
        //need an else here if we get longer values
        if (i == 0){
            default_units = String(menu_vars[i].options[*(static_cast<int*>(menu_vars[i].variable))]);
        }
    }
}

void menu_lcd_print(){
    //lcd.clear();
    //Serial.println("LCD Menu Print");
    int i_start = 0;
    String str_variable_val;
    String first_menu_look_val;
    if (menu_key > 3){
        i_start = menu_key - 3;
    }
    //Serial.println(menu_key);
    for (int i = i_start; i<= min(i_start + 3,max_menu_key); i++){
        lcd.setCursor(0,i-i_start);
        first_menu_look_val = String(menu_vars[i].options[0]);
        if (first_menu_look_val == "float_div_thou") { // Assuming menu_key 1 corresponds to a float variable
            str_variable_val = String(*(static_cast<int*>(menu_vars[i].variable)) * 0.001,3);
        } else if (first_menu_look_val == "int"){
            str_variable_val = String(*(static_cast<int*>(menu_vars[i].variable)));
        } else if (first_menu_look_val == "int_mult_hundred"){
            str_variable_val = String(*(static_cast<int*>(menu_vars[i].variable)) * 100);
        } else { // Assuming other keys correspond to int or String variables
            str_variable_val = String(menu_vars[i].options[*(static_cast<int*>(menu_vars[i].variable))]);
        }
        if (i == menu_key){
            lcd.print(fill_string("->"+String(menu_vars[i].title) + ": " + str_variable_val,20," ","right"));
            //Serial.println("->"+String(menu_vars[i].title) + ": " + str_variable_val);
        } else {
            lcd.print(fill_string("  "+String(menu_vars[i].title) + ": " + str_variable_val,20," ","right"));
            //Serial.println("  "+String(menu_vars[i].title) + ": " + str_variable_val);
        }
    }
}



void menu(){
    int rd_lr_clicks = 0;
    int menu_next_key;
    //set_stepper_speed(0);
    if (mode_change) {
        //control_last_state_a = digitalRead(control_rotary_a);
        //control_last_step = 0;
        //lcd.clear();
        menu_lcd_print();
    }
  
    if (digitalRead(settings_butt) == LOW){
        menu_key ++;
        if (menu_key > max_menu_key) {
            menu_key = 0;
        }
        else if (menu_key < 0) {
            menu_key = max_menu_key;
        }
        menu_lcd_print();
        delay(100);
    }
    int next_key_value = *menu_vars[menu_key].variable;
    rd_lr_clicks = read_dir(l_move,r_move);
    if (rd_lr_clicks != 0){
        Serial.println(String(rd_lr_clicks) + " original_value: " + String(next_key_value));
        next_key_value += rd_lr_clicks;
        Serial.println("new_value: " + String(next_key_value));
        if (next_key_value > menu_vars[menu_key].max_option){
            *menu_vars[menu_key].variable = 0;
        }
        else if (next_key_value < 0){
            *menu_vars[menu_key].variable = menu_vars[menu_key].max_option;
        }
        else {
            *menu_vars[menu_key].variable = next_key_value;
        }
        menu_lcd_print();
        delay(50);
    } 
    //screw_pitch
    //reverse_feed
    //default_units

}


void setup() {
    //get stored variables from EEPROM
    read_eeprom();
    screw_pitch = int_screw_pitch /1000.0; //convert to in/rev

    // initialize the mode pushbuttons as inputs:
    for (int val : mode_buttons) {
        pinMode(val, INPUT_PULLUP);
    }
    // initialize the other buttons as inputs:
    for (int val : other_buttons) {
        pinMode(val, INPUT_PULLUP);
    }
    // initialize the stepper outputs as outputs:
    for (int val : stepper_outputs) {
        pinMode(val, OUTPUT);
    }
    //initialize the LCD screen
    lcd.init();  // initialize the lcd
    lcd.backlight();
    lcd_print();
    //Initialize rotary encoder interrupts
    attachInterrupt(digitalPinToInterrupt(spindle_rotary_a),PinA,RISING); // set an interrupt on PinA, looking for a rising edge signal and executing the "PinA" Interrupt Service Routine (below)
    attachInterrupt(digitalPinToInterrupt(spindle_rotary_b),PinB,RISING); // set an interrupt on PinB, looking for a rising edge signal and executing the "PinB" Interrupt Service Routine (below)
    //interrupts();
    Serial.begin(9600);
    
}

void loop(){

    
    // Mode selection
    if (direction == 0) {
        mode = read_mode(mode);
        if (mode != last_mode) {
            if (last_mode == "settings"){
                write_eeprom();
            }
            last_mode = mode;
            mode_change = 1;
            Serial.println(String(mode));
        }
        else {
            mode_change = 0;
        }
    }
    if (mode == "feed") {
        //feed mode selected
        auto_move(50, max_feed_rate, feed_rate_key, feed_rates, feed_rates_int);
    }
    else if (mode == "in_thread") {
        //Serial.println("In Thread Mode");
        auto_move(50, max_in_threads, in_thread_key, in_threads, in_threads_int);
    }
    else if (mode == "met_thread") {
      //Serial.println(" Metric Thread Mode");
        auto_move(50, max_met_threads, met_thread_key, met_threads, met_threads_in_int);
    }
    else if (mode == "man_move") {
        man_move();
    }
    else if (mode == "settings") {
        menu();
    }

    // Direction selection
    direction = read_dir(l_dir, r_dir);
    if (direction != last_direction) {
        last_direction = direction;
        rpm = read_spindle_speed();
        lcd_print();
    }

    //if(oldEncPos != encoderPos) {
        //Serial.println(encoderPos);
        //oldEncPos = encoderPos;
    //}

}

