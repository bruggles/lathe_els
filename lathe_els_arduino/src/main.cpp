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

//#include <RotaryEncoder.h>

//pin declarations
//mode buttons
const int feed_butt = 0;
const int in_thread_butt = 1;
const int met_thread_butt = 2;
const int man_move_butt = 3;
const int settings_butt = 4;
//left and right digital stops
const int l_stop_set_butt = 5;
const int l_stop_clear_butt = 6;
const int r_stop_set_butt = 7;
const int r_stop_clear_butt = 8;
//dro zero
const int dro_zero = 9;
//direction selction
const int l_dir = 10;
const int r_dir = 11;
//physical stop limit switches
const int r_stop_limit = A4;
const int l_stop_limit = A5;
//control rotary encoder pins and settings
const int control_rotary_a = 12;
const int control_rotary_b = 13;
int control_pos = 0;
int control_last_state_a = 0; 
int control_last_step = 0; 
int control_value = 0;
int last_control_value = 0;
float feed_rate = 0.001;
//spindle speed rotary encoder pins
const int spindle_rotary_a = A2; //A2 and A3 support interrupts, which is necessary for accurate reading of the rotary encoder at high speeds. If you change these, make sure to change the interrupt settings in the code as well.
const int spindle_rotary_b = A3;

//lcd pins
const int lcd_sda = 17;
const int lcd_scl = 18;
//stepper pins
const int stepper_1 = 19;
const int stepper_2 = 20;
const int stepper_3 = 21;
const int stepper_4 = 22;

const int mode_buttons[] = {
    feed_butt, 
    in_thread_butt, 
    met_thread_butt, 
    man_move_butt, 
    settings_butt 
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

String mode = "feed";
String last_mode = "feed";
int mode_change = 0;

int direction = 0;

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
    r_stop_limit,
    l_stop_limit,
};

int rpm = 0;
float l_stop_dist = -1.0;
float r_stop_dist = 2.0;
float dro_pos = 0.0;

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
    Serial.println(("Mode: " + md));
    return md;
}

int rotary_read(int steps, int &position, int &last_state_a, int &last_step, int &value){
    //int pin_a = control_rotary_a
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
        md = "Feed Md";
    }
    else if (mode == "in_thread") {
        md = "In Thrd";
    }
    else if (mode == "met_thread") {
        md = "MM Thread";
    }
    else if (mode == "man_move") {
        md = "Move Md";
    }
    else if (mode == "settings") {
        md = "Settings";
    }
    else {
        md = "Unknown";
    }
    return md;
}

void lcd_print(String mode, int rpm, float feed_rate, float l_stop_dist, float r_stop_dist, float dro_pos){
    
    String row2;
    String row3;
    String row4;
    String row1 = screen_mode(mode) + " | RPM: " + String(rpm);
    String ls;
    String rs;
    lcd.setCursor(0, 0);
    lcd.print(row1);
    if (mode == "feed"){
        row2 = "Feed Rate: " + String(feed_rate, 3) + " IPR";
    }
    else if (mode == "in_thread"){
        row2 = "TPI: " + String(1.0 / feed_rate, 0) + " | " + String(feed_rate, 4) + " IPR";
    }
    else if (mode == "met_thread"){
        row2 = "Thread Pitch: " + String(feed_rate, 2) + " MMR";
    }
    else if (mode == "man_move"){
        row2 = "Movement Per Mark: " + String(feed_rate, 3) + " IN";
    }
    lcd.setCursor(0, 1);
    lcd.print(row2);
    if (l_stop_dist < 0) {
        ls = "NA";
    }
    else if (l_stop_dist >= 0) {
        ls = String(l_stop_dist, 3);
    }
    if (r_stop_dist < 0) {
        rs = "NA";
    }
    else if (r_stop_dist >= 0) {
        rs = String(r_stop_dist, 3);
    }
    row3 = "<- " + ls + " | " + rs + " ->";
    lcd.setCursor(0, 2);
    lcd.print(row3);
    row4 = "Screw DRO: " + String(dro_pos, 3);
    lcd.setCursor(0, 3);
    lcd.print(row4);
}

void auto_move(String mode,int step_size, int max_val, int &key_val, const float list_of_vals[] = {}){
    //move the lathe at the specified feed rate in the specified direction until a stop button is pressed or a stop limit switch is triggered. If in thread mode, also monitor the spindle speed and adjust the feed rate to maintain the correct thread pitch.
    if (direction == 0) {
        if (mode_change) {
            last_control_value = key_val;
            control_value = key_val;
            control_pos = 0;
            control_last_state_a = digitalRead(control_rotary_a);
            control_last_step = 0;
            if (mode == "feed") {
                feed_rate = control_value * 0.001; //convert to in/rev
            }
            else {
                feed_rate = list_of_vals[key_val];
            }
            Serial.println("Auto Move Mode: " + mode);
            lcd_print(mode, rpm, feed_rate, l_stop_dist, r_stop_dist, dro_pos);
        }
        else {
            //rotary jumps through thread pitches, and the LCD displays the current TPI and feed rate
            rotary_read(5, control_pos, control_last_state_a, control_last_step, control_value);
            if (control_value != last_control_value) {
                if (control_value < 0) {
                    control_value = max_val;
                }
                else if (control_value > max_val) {
                    control_value = 0;
                }
                key_val = control_value;
                if (mode == "feed") {
                    feed_rate = control_value * 0.001; //convert to in/rev
                }
                else {
                    feed_rate = list_of_vals[control_value];
                }
                last_control_value = control_value;
                lcd_print(mode, rpm, feed_rate, l_stop_dist, r_stop_dist, dro_pos);
            }
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
        String default_units = "IN";
    }
    else if (int_default_units == 1) {
        String default_units = "MM";
    }
    else if (int_default_units == 2) {
        String default_units = "CM";
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
    float screw_pitch = screw_pitch_e /1000.0; //convert to in/rev
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
    Serial.begin(9600);
    
}

void loop(){

    
    // Mode selection
    mode = read_mode(mode);
    if (mode != last_mode) {
        last_mode = mode;
        mode_change = 1;
    }
    else {
        mode_change = 0;
    }

    if (mode == "feed") {
        //feed mode selected
        lcd_print(mode, rpm, feed_rate, l_stop_dist, r_stop_dist, dro_pos);
        if (direction == 0) {
            //rotary jumps through feed rates, and the LCD displays the current feed rate
            //increments in .001 in/rev if in inches or .01 mm/rev if in metric
            rotary_read(1, control_pos, control_last_state_a, control_last_step, control_value);
            if (control_value < 1) {
                control_value = 1;
            }
            else if (control_value > 1000) {
                control_value = 1000;
            }
            feed_rate = control_value * 0.001; //convert to in/rev
            
        }
    }
    else if (mode == "in_thread") {
        //Serial.println("In Thread Mode");
        if (direction == 0) {
            if (mode_change) {
                control_value = in_thread_key;
                control_pos = 0;
                control_last_state_a = digitalRead(control_rotary_a);
                control_last_step = 0;
            }
            //rotary jumps through thread pitches, and the LCD displays the current TPI and feed rate
            rotary_read(5, control_pos, control_last_state_a, control_last_step, control_value);
            if (control_value < 0) {
                control_value = max_in_threads;
            }
            else if (control_value > max_in_threads) {
                control_value = 0;
            }
            in_thread_key = control_value;
        }
        if (mode_change) {
            feed_rate = in_threads[13];
        }
        lcd_print(mode, rpm, in_threads[in_thread_key], l_stop_dist, r_stop_dist, dro_pos);

    }
    else if (mode == "met_thread") {
      //Serial.println(" Metric Thread Mode");
        //auto_move(mode, 5, max_met_threads, met_thread_key, met_threads);
        Serial.println("Metric Thread Mode");
         
    }
    else if (mode == "man_move") {
        matrix_array[0][0] = 0;
        matrix_array[0][1] = 0;
        matrix_array[0][2] = 0;
        matrix_array[0][3] = 1;
        matrix_array[0][4] = 0;
    }
    else if (mode == "settings") {
        matrix_array[0][0] = 0;
        matrix_array[0][1] = 0;
        matrix_array[0][2] = 0;
        matrix_array[0][3] = 0;
        matrix_array[0][4] = 1;
    }

    // Direction selection
    direction = read_dir(l_dir, r_dir);
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

