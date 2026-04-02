//libraries
#include <Arduino_LED_Matrix.h>
ArduinoLEDMatrix matrix;
//#include <LiquidCrystal_I2C.h>

//pin declarations
//mode buttons
const int feed_butt = 1;
const int in_thread_butt = 2;
const int met_thread_butt = 3;
const int man_move_butt = 4;
const int settings_butt = 5;
//left and right digital stops
const int l_stop_set_butt = 6;
const int l_stop_clear_butt = 7;
const int r_stop_set_butt = 8;
const int r_stop_clear_butt = 9;
//dro zero
const int dro_zero = 10;
//direction selction
const int l_dir = 11;
const int r_dir = 12;
//physical stop limit switches
const int r_stop_limit = 13;
const int l_stop_limit = 14;
//rotary encoder pins
const int rotary_a = 15;
const int rotary_b = 16;
//lcd pins
const int lcd_sda = 17;
const int lcd_scl = 18;
//stepper pins
const int stepper_1 = 19;
const int stepper_2 = 20;
const int stepper_3 = 21;
const int stepper_4 = 22;

const mode_buttons[] = {
    feed_butt, 
    in_thread_butt, 
    met_thread_butt, 
    man_move_butt, 
    settings_butt 
}

const dir_buttons[] = {
    l_dir,
    r_dir
}
const other_buttons[] = {
    
    l_stop_set_butt,
    l_stop_clear_butt,
    r_stop_set_butt,
    r_stop_clear_butt,
    dro_zero,
    r_stop_limit,
    l_stop_limit,
}

byte matrix_array[8][12] = {
  { 0, 0, 1, 1, 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 0, 0, 1, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 1, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0 },
  { 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 1, 0, 1, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }
};

void setup() {
    //initialize the onboard LED matrix
    Serial.begin(9600);
    matrix.begin();
    //initialize the LCD screen

    // initialize the pushbutton pin as an input:
    for (int val : mode_buttons) {
        pinMode(val, INPUT_PULLUP);
    }
    for (int val : dir_buttons) {
        pinMode(val, INPUT_PULLUP);
    }
    // for (int val : other_buttons) {
    //    pinMode(val, INPUT_PULLUP);
    // }
}

void loop(){
    //change a value
    matrix_array[2][1] = 1;
    Serial.println("test");
    matrix.renderBitmap(matrix_array, 8, 12);
}