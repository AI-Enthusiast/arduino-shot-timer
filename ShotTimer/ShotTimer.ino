////////////////////////////////////////////////////////////
// Shot Timer
// Author: hestenet
// Canonical Repository: https://github.com/hestenet/arduino-shot-timer
////////////////////////////////////////////////////////////
//  This file is part of ShotTimer. 
//
//  This program is free software: you can redistribute it and/or modify
//  it under the terms of the GNU Lesser General Public License as published by
//  the Free Software Foundation, either version 3 of the License, or
//  (at your option) any later version.
//
//  This program is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  GNU Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public License
//  along with this program.  If not, see <http://www.gnu.org/licenses/>.
//  http://www.gnu.org/licenses/lgpl.txt
//
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// HARDWARE
////////////////////////////////////////////////////////////
// Arduino Uno R3
// Adafruit RGB LCD Shield - https://www.adafruit.com/products/714
// Adafruit Electet Mic/Amp - https://www.adafruit.com/products/1063
// Piezzo Buzzer
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// Current Flaws:
// ShotListener(); probably could be redesigned to run on a timer interupt.
// as a result, the g_par_times beep may come as early or late as half the 
// sample window time. However, at most reasonable g_sample_windows this will 
// likely be indistinguishable to the user
//
// ParTimes are not saved to EEPROM, as their frequent updating is more likely 
// to burn out the chip.
//
// I would like to add SD card support to save the strings
//
// I would like to add course scoring of some kind, and maybe even shooter 
// profiles, but the arduino Uno is likely not powerful enough for this.
////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////
// INCLUDES
////////////////////////////////////////////////////////////

//////////////////////////////
// DEBUG
//////////////////////////////

// Comment this out to disable debug information and remove all
// DEBUG messages at compile time
// To preserve SRAM while debugging, set shotLimit to 50 or less
// #define DEBUG
#include "DebugMacros.h"

//////////////////////////////
// Libraries - Core
// These are libraries shipped with Arduino, or that can be installed from the 
// "Manage Libraries" interface of the Arduino IDE:
//            Sketch -> Include Libraries -> Manage Librariess
//////////////////////////////

//PROGMEM aka FLASH memory, non-volatile
#include <avr/pgmspace.h>

// PGMWrap makes it easier to use PROGMEM - declare vars with _p e.g: char_p
#include <PGMWrap.h>

// EEPROM additional non-volatile space 
#include <EEPROM.h>

// EEWrap allows you to read/write from EEPROM without special functions and 
// without directly specifying EEPROM address space. 
#include <EEWrap.h> // the .update() method allows you to only update EEPROM if 
                    // values have changed. 

//Wire library lets you manage I2C and 2 pin
#include <Wire.h>

//Chrono - LightChrono - chronometer - to replace StopWatch
#include <LightChrono.h>

//MenuSystem  
#include <MenuSystem.h>

//Adafruit RGB LCD Shield Library
#include <Adafruit_RGBLCDShield.h>

//////////////////////////////
// Libraries - Other
// These are libraries that cannot be found in the defauilt Arduino library 
// manager - however, they can be added manually. A source url for each library 
// is provided - simply download the library and include in:
// ~/Documents/Arduino/Libraries
//////////////////////////////

// toneAC
// Bit-Bang tone library for piezo buzzer 
// https://bitbucket.org/teckel12/arduino-toneac/wiki/Home#!difference-between-toneac-and-toneac2
#include <toneAC.h>

//////////////////////////////
// Other helpful resources
//////////////////////////////
// Adafruit sound level sampling: 
// http://learn.adafruit.com/adafruit-microphone-amplifier-breakout/measuring-sound-levels
// http://stackoverflow.com/questions/18903528/permanently-changing-value-of-parameter

//////////////////////////////
// Libraries - Mine
//////////////////////////////

//Tones for buttons and buzzer
#include "Pitches.h" // musical pitches - optional - format: NOTE_C4 

//Convert time in ms elapsed to hh:mm:ss.mss
#include "LegibleTime.h"

//Helper functions for managing the LCD Display
#include "LCDHelpers.h"

////////////////////////////////////////////////////////////
// CONSTANTS
////////////////////////////////////////////////////////////

//////////////////////////////
// Hardware Configuration
//////////////////////////////
const uint8_p PROGMEM kMicPin = A0; // Microphone/amplifier input pin
const uint8_p PROGMEM kButtonDur = 80;
const int16_p PROGMEM kBeepDur = 400;
const int16_p PROGMEM kBeepNote = NOTE_C4;

//////////////////////////////
// LCD Display Constants
//////////////////////////////
const uint8_t LCD_COLS = 16;         // LCD width in characters
const uint8_t LCD_ROWS = 2;          // LCD height in rows
const uint8_t LCD_DISPLAY_WIDTH_2 = 2;  // Standard 2-digit display width
const uint8_t LCD_DISPLAY_WIDTH_3 = 3;  // 3-digit display width (for shot counter)
const uint8_t LCD_TIME_WIDTH = 9;    // Width for time display (mm:ss.ms)

//////////////////////////////
// Timer and Delay Constants
//////////////////////////////
const uint16_t MILLIS_PER_SECOND = 1000;
const uint16_t RANDOM_DELAY_2TO6_MIN = 2000;  // 2 seconds in ms
const uint16_t RANDOM_DELAY_2TO6_MAX = 6001;  // 6 seconds in ms (exclusive)
const uint16_t RANDOM_DELAY_1TO4_MIN = 1000;  // 1 second in ms
const uint16_t RANDOM_DELAY_1TO4_MAX = 4001;  // 4 seconds in ms (exclusive)
const uint8_t DELAY_SETTING_MAX = 12;         // Maximum delay setting value
const uint8_t DELAY_SETTING_RANDOM_1TO4 = 11; // Setting value for random 1-4s
const uint8_t DELAY_SETTING_RANDOM_2TO6 = 12; // Setting value for random 2-6s

//////////////////////////////
// Par Time Constants (in milliseconds)
//////////////////////////////
const unsigned long PAR_TIME_MAX = 5999999;        // Maximum par time: 99:59.999
const unsigned long PAR_TIME_INCREMENT_1MS = 1;
const unsigned long PAR_TIME_INCREMENT_10MS = 10;
const unsigned long PAR_TIME_INCREMENT_100MS = 100;
const unsigned long PAR_TIME_INCREMENT_1S = 1000;
const unsigned long PAR_TIME_INCREMENT_10S = 10000;
const unsigned long PAR_TIME_INCREMENT_1MIN = 60000;
const unsigned long PAR_TIME_INCREMENT_10MIN = 600000;

const unsigned long PAR_TIME_MAX_1MS = 5999999;
const unsigned long PAR_TIME_MAX_10MS = 5999990;
const unsigned long PAR_TIME_MAX_100MS = 5999900;
const unsigned long PAR_TIME_MAX_1S = 5999000;
const unsigned long PAR_TIME_MAX_10S = 5990000;
const unsigned long PAR_TIME_MAX_1MIN = 5940000;
const unsigned long PAR_TIME_MAX_10MIN = 5400000;

const unsigned long PAR_TIME_MIN_1MS = 1;
const unsigned long PAR_TIME_MIN_10MS = 10;
const unsigned long PAR_TIME_MIN_100MS = 100;
const unsigned long PAR_TIME_MIN_1S = 1000;
const unsigned long PAR_TIME_MIN_10S = 10000;
const unsigned long PAR_TIME_MIN_1MIN = 60000;
const unsigned long PAR_TIME_MIN_10MIN = 600000;

//////////////////////////////
// Par Cursor Position Constants
//////////////////////////////
const uint8_t PAR_CURSOR_1MS = 1;
const uint8_t PAR_CURSOR_10MS = 2;
const uint8_t PAR_CURSOR_100MS = 3;
const uint8_t PAR_CURSOR_1S = 4;
const uint8_t PAR_CURSOR_10S = 5;
const uint8_t PAR_CURSOR_1MIN = 6;
const uint8_t PAR_CURSOR_10MIN = 7;
const uint8_t PAR_CURSOR_MIN = 1;
const uint8_t PAR_CURSOR_MAX = 7;
const uint8_t PAR_CURSOR_DEFAULT = 4;  // Default to seconds position

//////////////////////////////
// Settings Range Constants
//////////////////////////////
const uint8_t BEEP_VOL_MIN = 0;
const uint8_t BEEP_VOL_MAX = 10;
const uint8_t SENSITIVITY_MIN = 0;
const uint8_t SENSITIVITY_MAX = 20;
const uint8_t SAMPLE_WINDOW_MIN = 10;   // 10ms minimum echo protection
const uint8_t SAMPLE_WINDOW_MAX = 100;  // 100ms maximum echo protection
const uint8_t SAMPLE_WINDOW_STEP = 10;  // 10ms increment steps

//////////////////////////////
// Sensitivity and Threshold Constants
//////////////////////////////
const int THRESHOLD_BASE = 650;           // Base threshold value
const int THRESHOLD_SENSITIVITY_MULTIPLIER = 25;  // Multiplier for sensitivity adjustment
const int THRESHOLD_DEFAULT = 625;        // Default threshold (650 - 25*1)

//////////////////////////////
// Default Settings Values
//////////////////////////////
const uint8_t DEFAULT_DELAY_TIME = 11;    // Default to random 1-4s
const uint8_t DEFAULT_BEEP_VOL = 10;
const uint8_t DEFAULT_SENSITIVITY = 1;
const uint8_t DEFAULT_SAMPLE_WINDOW = 50;

//////////////////////////////
// PROGMEM
//////////////////////////////
// To read these - increment over the array: 
// https://github.com/Chris--A/PGMWrap/blob/master/examples/advanced/use_within_classes/use_within_classes.ino 
// More detailed example of dealing with strings and arrays in PROGMEM:
// http://www.gammon.com.au/progmem
const char PROGMEM kMainName[] = "Shot Timer v.2";
const char PROGMEM kStartName[] = "[Start]";
const char PROGMEM kReviewName[] = "[Review]";
const char PROGMEM kParName[] = "Set Par >>";
const char PROGMEM kParSetName[] = "<< [Toggle Par]";
const char PROGMEM kParTimesName[] = "<< [Par Times]";
const char PROGMEM kSettingsName[] = "Settings >>";
const char PROGMEM kSetDelayName[] = "<< [Set Delay]";
const char PROGMEM kROFName[] = "<<[Rate of Fire]";
const char PROGMEM kBuzzerName[] = "<< [Buzzer Vol]";
const char PROGMEM kSensitivityName[] = "<< [Sensitivity]";
const char PROGMEM kEchoName[] = "<< [Echo Reject]";

const char PROGMEM kClearLine[] = "                ";
const char PROGMEM kSpace[] = " ";
const char PROGMEM kRan1to4[] = "Random 1-4s";
const char PROGMEM kRan2to6[] = "Random 2-6s";
const char PROGMEM kShotNum[] = "Shot #";
const char PROGMEM kSplit[] = "Split";
const char PROGMEM kFirst[] = "   1st";
const char PROGMEM kMS[] = "ms";
const char PROGMEM kDisabled[] = "[DISABLED]";
const char PROGMEM kEnabled[] = "[ENABLED] ";
const char PROGMEM kPlus[] = "+";
const char PROGMEM kCursor[] = "  _  ";

const int PROGMEM kParLimit = 10;
const int PROGMEM kShotLimit = 200;


////////////////////////////////////////////////////////////
// GLOBAL VARIABLES
////////////////////////////////////////////////////////////

byte g_delay_time = DEFAULT_DELAY_TIME;
byte g_beep_vol = DEFAULT_BEEP_VOL;
byte g_sensitivity = DEFAULT_SENSITIVITY;
byte g_sample_window = DEFAULT_SAMPLE_WINDOW;
// do we want to instantiate the size in setup()
uint32_t g_shot_times[kShotLimit]; 
unsigned long g_par_times[kParLimit]; 
uint32_t g_additive_par;
byte g_current_shot; // REFACTOR, MAY NOT NEED TO BE GLOBAL
byte g_review_shot;  // REFACTOR, MAY NOT NEED TO BE GLOBAL
byte g_current_par;  // REFACTOR, MAY NOT NEED TO BE GLOBAL
int g_threshold = THRESHOLD_DEFAULT; // The g_sensitivity is converted to a g_threshold value
byte g_par_cursor = PAR_CURSOR_DEFAULT;


//////////////////////////////
// EEPROM: Settings to be stored in EEPROM
//////////////////////////////
// EEPROM HAS 100,000 READ/WRITE CYCLES, conservatively
// http://tronixstuff.wordpress.com/2011/05/11/discovering-arduinos-internal-eeprom-lifespan/
uint8_e g_delay_setting_e;   // Can be 0
bool_e g_rof_draw_setting_e; // Can be 0 or 1
uint8_e g_beep_setting_e;    // Can be 0
uint8_e g_sens_setting_e;    // Can be 0
uint8_e g_sample_setting_e;  //Cannot be 0  
// ECHO REJECT: Sample window width in mS (50 mS = 20Hz) for function 
// SampleSound()

//////////////////////////////
// Program State Variables
//////////////////////////////
// http://stackoverflow.com/questions/18903528/permanently-changing-value-of-parameter
uint8_t g_buttons_state;
boolean g_par_enabled;
boolean g_include_draw; 
enum ProgramState {
  MENU,         // 0 - Navigating menus
  TIMER,        // 1 - Timer is running   // && g_par_enabled
  REVIEW,       // 2 - Reviewing shots    // && g_include_draw
  SETPARSTATE,  // 3 - Setting Par State  // && g_par_enabled
  SETPARTIMES,  // 4 - Setting Par Times
  SETINDPAR,    // 5 - Setting Single Par
  SETDELAY,     // 6 - Setting Delay
  SETROFDRAW,   // 7 - Setting Incl Draw  // && g_include_draw
  SETBEEP,      // 7 - Setting Beep
  SETSENS,      // 8 - Setting Sensitivity 
  SETECHO       // 9 - Setting Echo
 } g_current_state; 

//////////////////////////////
// Instantiation //@TODO: should maybe have a settings object and timer object? 
//////////////////////////////
LightChrono g_shot_chrono;

// The shield uses the I2C SCL and SDA pins. On classic Arduinos
// this is Analog 4 and 5 so you can't use those for analogRead() anymore
// However, you can connect other I2C sensors to the I2C bus and share
// the I2C bus.
Adafruit_RGBLCDShield g_lcd = Adafruit_RGBLCDShield();

//////////////////////////////
//Menus and Menu Items
//////////////////////////////

MenuSystem tm;
Menu main_menu(kMainName);
  MenuItem menu_start(kStartName);
  MenuItem menu_review(kReviewName);
  Menu par_menu(kParName);
    MenuItem menu_par_state(kParSetName);
    MenuItem menu_par_times(kParTimesName);
  Menu settings_menu(kSettingsName);
    MenuItem menu_start_delay(kSetDelayName);
    MenuItem menu_rof(kROFName);
    MenuItem menu_buzzer(kBuzzerName);
    MenuItem menu_sensitivity(kSensitivityName);
    MenuItem menu_echo(kEchoName);

////////////////////////////////////////////////////////////
// FUNCTIONS
////////////////////////////////////////////////////////////
// Note: Any functions with reference parameters i.e myFunction(char &str); 
// must be prototyped manually
// A prototype is simply an empty declaration
////////////////////////////////////////////////////////////

//////////////////////////////
// Render the current menu screen
//////////////////////////////

void RenderMenu() {
  Menu const* kMenu = tm.get_current_menu();
  g_lcd.setBacklight(WHITE);
  g_lcd.clear();
  g_lcd.setCursor(0, 0);
  lcd_print_p(&g_lcd, kMenu->get_name());
  DEBUG_PRINT(F("Rendering Menu: "));
  DEBUG_PRINTLN_P(kMenu->get_name(),0);
  g_lcd.setCursor(0, 1);
  lcd_print_p(&g_lcd, kMenu->get_selected()->get_name());
  DEBUG_PRINT(F("Rendering Item: "));
  DEBUG_PRINTLN_P(kMenu->get_selected()->get_name(),0);
}

//////////////////////////////
// Sample Sound
//////////////////////////////
int SampleSound() {
  uint32_t start_millis = millis();  // Start of sample window 
  // the peak to peak reading will be the total loudness change across the 
  // sample wiindow!
  int peak_to_peak = 0; // peak_to_peak level
  int sample = 0;
  int signal_max = 0;
  int signal_min = 1024;

  // collect data for duration of g_sample_window
  while (millis() - start_millis < g_sample_window)
  {
    sample = analogRead(kMicPin);
    if (sample < 1024) // toss out spurious readings
    {
      if (sample > signal_max)
      {
        signal_max = sample; // save just the max levels
      }
      else if (sample < signal_min)
      {
        signal_min = sample; // save just the min levels
      }
    }
  }
  peak_to_peak = signal_max - signal_min; // max - min = peak-peak amplitude
  return (peak_to_peak);
}

//////////////////////////////
// Start the Shot Timer
//////////////////////////////
// Consider changing these to be 'on_menu_event()' functions - such that they 
// can have a local variable for whether the menu item is active, rather than 
// using a global. 
//////////////////////////////

void on_menu_start_selected(MenuItem* p_menu_item) {
  DEBUG_PRINTLN(F("Starting Timer"),0);
  g_current_state = TIMER;
  g_lcd.setBacklight(GREEN);
  // reset the values of the array of shots to 0 NOT <= because g_current_shot 
  // is incremented at the end of the last one recorded
  for (int c = 0; c < g_current_shot; c++) { 
    g_shot_times[c] = 0;
  }
  g_current_shot = 0; //start with the first shot in the array
  g_lcd.setCursor(0, 0);
  g_lcd.print(F("Wait for it...  "));
  g_lcd.setCursor(0, 1);
  lcd_print_p(&g_lcd, kClearLine); // create a clearline function? 
                                    // Save fewer strings in progmem?
  StartDelay();
  g_lcd.setCursor(0, 0);
  g_lcd.print(F(" GO!!  Shot#    ")); //g_lcd.setCursor(0, 13);
  g_lcd.setCursor(0, 1);
  g_lcd.print(F("Last:")); //10 chars
  BEEP();
  g_shot_chrono.restart();
}

//////////////////////////////
// Run the shot timer - runs in loop()
//////////////////////////////

// @TODO: Decide if passing in current state as an argument or just accessing 
// as a global variable!
void RunTimer(ProgramState* p_state, boolean* par_state) {
  //DEBUG_PRINTLN(*runState, 0);
  if (*p_state == TIMER)
  { 
    //DEBUG_PRINTLN(F("Enter Run Timer Mode."), 0);
    ShotListener();
    ParBeeps(par_state);
  }
}

//////////////////////////////
// Beep at each par time - runs indirectly in loop()
//////////////////////////////

void ParBeeps(boolean* par_state)
{
    //DEBUG_PRINTLN(*par_state, 0);
    if (*par_state == true) {
      DEBUG_PRINTLN(F("...check for par beep..."),0)
      g_additive_par = 0;
      for (byte i = 0; i < kParLimit; i++) {
        if (g_par_times[i] == 0) {
          break;
        }
        g_additive_par += g_par_times[i]; // add the g_par_times together
        //if (shotTimer.elapsed() <= (g_additive_par + (g_sample_window / 2)) 
        //&& shotTimer.elapsed() >= (g_additive_par - g_sample_window / 2)){
        int time_elapsed = g_shot_chrono.elapsed();
        // Beep if the current time matches the parTime
        // (within the boundaries of sample window) 
        if (time_elapsed <= (g_additive_par + (g_sample_window / 2)) 
          && time_elapsed >= (g_additive_par - g_sample_window / 2)) {
          DEBUG_PRINTLN(F("PAR BEEP!"),0);
          BEEP();  
        }
      }

    }
}

//////////////////////////////
// Stop the shot timer
//////////////////////////////
void StopTimer(boolean out = 0) {
  DEBUG_PRINTLN(F("Stopping Timer"),0);
  if (out == 1) {
    g_lcd.setBacklight(RED);
  }
  else {
    g_lcd.setBacklight(WHITE);
  }
  DEBUG_PRINTLN(F("Timer was stopped at:"), 0);
  g_shot_chrono.elapsed(); // for DEBUG
  for (int i = 0; i < 5; i++) {
    toneAC(kBeepNote, g_beep_vol, 100, false); 
    delay(50);
  }
  if (out == 1) {
    g_lcd.setBacklight(WHITE);
  }
  // Since the timer is stopped move back to last recorded shot.
  // Unless there were only one or no shots. 
  if (g_current_shot > 0) {
    g_current_shot--;
  }
  // Also - transition menus and re-render menu screens in the stop condition. 
  tm.next(); // move the menu down to review mode
  tm.select(); // move into shot review mode immediately
}

//////////////////////////////
// Record shots to the string array
//////////////////////////////

void RecordShot() {
  g_shot_times[g_current_shot] = g_shot_chrono.elapsed();
  DEBUG_PRINT_P(kShotNum); DEBUG_PRINT(g_current_shot + 1); 
  DEBUG_PRINT(F(" - "));
  DEBUG_PRINT(F("\n"));
  g_lcd.setCursor(13, 0);
  lcd_print(&g_lcd, g_current_shot + 1, LCD_DISPLAY_WIDTH_3);
  g_lcd.setCursor(7, 1);
  lcd_print_time(&g_lcd, g_shot_times[g_current_shot], LCD_TIME_WIDTH);
  g_current_shot++;
  if (g_current_shot == kShotLimit) { 
    DEBUG_PRINTLN(F("Out of room for shots"),0);
    StopTimer(1);
  }
}

//////////////////////////////
// review shots - initialize the shot review screen
//////////////////////////////

// g_current_shot is the actual most recent shot from the timer
// g_current_shot + 1 is the total number of shots 
// g_review_shot is the shot being reviewed

void on_menu_review_selected(MenuItem* p_menu_item) {
  if(g_current_state != REVIEW){
    DEBUG_PRINTLN(F("Enter REVIEW Mode"), 0);
    DEBUG_PRINTLN(g_current_state, 0);
    g_current_state = REVIEW;
    DEBUG_PRINTLN(g_current_state, 0);
    //DEBUG FOR LOOP - PRINT ALL SHOT TIMES IN THE STRING TO SERIAL 
    // for (int t = 0; t < g_current_shot; t++) {
    //   DEBUG_PRINT_P(kShotNum);
    //   DEBUG_PRINT(t + 1);
    //   DEBUG_PRINT(F(" - "));
    //   DEBUG_PRINTLN(g_shot_times[t],0) // for DEBUG
    // }
    //END DEBUG 
    g_lcd.setBacklight(VIOLET);
    g_lcd.setCursor(0, 0);
    g_review_shot = g_current_shot; 
    DEBUG_PRINT(F("Reviewing Shot: ")); DEBUG_PRINTLN(g_review_shot + 1,0);
    lcd_print_p(&g_lcd, kShotNum);
    g_lcd.print(g_review_shot + 1);
    lcd_print_p(&g_lcd, kClearLine);
    g_lcd.setCursor(11, 0);
    lcd_print_p(&g_lcd, kSplit);
    g_lcd.setCursor(0, 1);
    lcd_print_time(&g_lcd, g_shot_times[g_review_shot], LCD_TIME_WIDTH);
    lcd_print_p(&g_lcd, kSpace);
    if (g_review_shot == 0) {
      lcd_print_p(&g_lcd, kFirst);
    } else {
      lcd_print_time(&g_lcd, g_shot_times[g_review_shot] - 
                      g_shot_times[g_review_shot - 1], 6);
    }
    DEBUG_PRINTLN(tm.get_current_menu()->get_name(),0);
  } else {
    DEBUG_PRINTLN(F("Return to Menu"), 0);
    DEBUG_PRINTLN(g_current_state, 0);
    g_current_state = MENU;
    RenderMenu();
  }
}

//////////////////////////////
// Helper function to display shot review information
//////////////////////////////

void DisplayShotReview() {
  g_lcd.setCursor(0, 0);
  lcd_print_p(&g_lcd, kShotNum);
  g_lcd.print(g_review_shot + 1);
  lcd_print_p(&g_lcd, kClearLine);
  g_lcd.setCursor(11, 0);
  lcd_print_p(&g_lcd, kSplit);
  g_lcd.setCursor(0, 1);
  lcd_print_time(&g_lcd, g_shot_times[g_review_shot], LCD_TIME_WIDTH);
  lcd_print_p(&g_lcd, kSpace);
  if (g_review_shot == 0) {
    lcd_print_p(&g_lcd, kFirst);
  }
  else {
    lcd_print_time(&g_lcd, g_shot_times[g_review_shot] - 
                   g_shot_times[g_review_shot - 1], 6);
  }
}

//////////////////////////////
// review shots - move to the next shot in the string
//////////////////////////////

// if g_review shot == g_current_shot set g_review_shot = 0
// else increment g_review_shot
// print according to same rules as initial menu

void NextShot() {
  DEBUG_PRINTLN(F("NextShot()"), 0);
  DEBUG_PRINTLN(g_current_state, 0);
  if (g_review_shot == g_current_shot) {
    g_review_shot = 0;
  } else g_review_shot++;
  DisplayShotReview();
}

//////////////////////////////
// review shots - move to the previous shot in the string
//////////////////////////////

// if g_review shot == 0 set g_review_shot = g_current_shot 
// else decrement g_review_shot
// print according to same rules as initial menu

void PreviousShot() {
  DEBUG_PRINTLN(F("PreviousShot()"), 0);
  DEBUG_PRINTLN(g_current_state, 0);
  DEBUG_PRINTLN(g_current_state, 0);
  if (g_review_shot == 0) {
    g_review_shot = g_current_shot;
  } else g_review_shot--;
  DisplayShotReview();
}

//////////////////////////////
// Rate of Fire
//////////////////////////////

void RateOfFire(boolean* draw) {
  DEBUG_PRINTLN(g_current_state, 0);
  DEBUG_PRINTLN(F("rateofFire()"), 0);
  unsigned int avg_split = 0;
  unsigned int rof = 0;
  if (g_current_shot > 0){
    if (*draw == false) {
      avg_split = (g_shot_times[g_current_shot] - 
                   g_shot_times[0]) / (g_current_shot);
    }
    else {
      avg_split = g_shot_times[g_current_shot] / (g_current_shot);
    }
    rof = 60000 / avg_split;
  }
  g_lcd.setCursor(0, 0);
  lcd_print_p(&g_lcd, kClearLine);
  g_lcd.setCursor(0, 0);
  g_lcd.print(F("Avg Splt:"));
  g_lcd.setCursor(10, 0);
  lcd_print_time(&g_lcd, avg_split, 6);
  g_lcd.setCursor(0, 1);
  lcd_print_p(&g_lcd, kClearLine);
  g_lcd.setCursor(0, 1);
  g_lcd.print(F("Shots/mn:"));
  g_lcd.setCursor(10, 1);
  g_lcd.print(rof); 
}

//////////////////////////////
// on_menu_start_delay_selected
//////////////////////////////

void on_menu_start_delay_selected(MenuItem* p_menu_item) {
  if(g_current_state != SETDELAY){
    DEBUG_PRINTLN(F("Enter SETDELAY Mode"), 0);
    g_current_state = SETDELAY;
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Start Delay"));
    g_lcd.setCursor(0, 1);
    if (g_delay_time > 11) {
      lcd_print_p(&g_lcd, kRan2to6);
    }
    else if (g_delay_time == 11) {
      lcd_print_p(&g_lcd, kRan1to4);
    }
    else {
      g_lcd.print(g_delay_time);
    }
  }
  else {
    DEBUG_PRINTLN(F("Save Delay and Return to Menu"), 0);
    g_delay_setting_e = g_delay_time;
    g_current_state = MENU;
    RenderMenu();
  }
}

//////////////////////////////
// Helper function to cycle a value within min/max range and display it
//////////////////////////////

void CycleValueAndDisplay(int8_t &value, int8_t min_val, int8_t max_val, int8_t step, const char* label_p = nullptr) {
  value += step;
  if (value > max_val) {
    value = min_val;
  }
  else if (value < min_val) {
    value = max_val;
  }

  g_lcd.setCursor(0, 1);
  if (value > 11) {
    lcd_print_p(&g_lcd, kRan2to6);
  }
  else if (value == 11) {
    lcd_print_p(&g_lcd, kRan1to4);
  }
  else {
    g_lcd.print(value);
  }
  lcd_print_p(&g_lcd, kClearLine);
}

//////////////////////////////
// IncreaseDelay
//////////////////////////////

void IncreaseDelay() {
  DEBUG_PRINTLN(F("IncreaseDelay()"), 0);
  CycleValueAndDisplay(g_delay_time, 0, DELAY_SETTING_MAX, 1);
}

//////////////////////////////
// DecreaseDelay
//////////////////////////////

void DecreaseDelay() {
  DEBUG_PRINTLN(F("DecreaseDelay()"), 0);
  CycleValueAndDisplay(g_delay_time, 0, DELAY_SETTING_MAX, -1);
}

//////////////////////////////
// StartDelay
//////////////////////////////

void StartDelay() {
  DEBUG_PRINT(F("Start Delay: ")); DEBUG_PRINTLN(g_delay_time, 0);
  if (g_delay_time > DELAY_SETTING_RANDOM_1TO4) {
    delay(random(RANDOM_DELAY_2TO6_MIN, RANDOM_DELAY_2TO6_MAX)); // Random 2-6 seconds
  }
  else if (g_delay_time == DELAY_SETTING_RANDOM_1TO4) {
    delay(random(RANDOM_DELAY_1TO4_MIN, RANDOM_DELAY_1TO4_MAX)); // Random 1-4 seconds
  }
  else {
    delay(g_delay_time * MILLIS_PER_SECOND); // Fixed number of seconds
  }
}

//////////////////////////////
// on_menu_rof_selected
//////////////////////////////

void on_menu_rof_selected(MenuItem* p_menu_item) {
  DEBUG_PRINT(F("State before select: ")); DEBUG_PRINTLN(g_current_state,0);
  DEBUG_PRINTLN(tm.get_current_menu()->get_name(),0);
  if(g_current_state != SETROFDRAW){
    DEBUG_PRINTLN(F("Enter SETROFDRAW Mode"),0);
    g_current_state = SETROFDRAW;
    DEBUG_PRINT(F("State after select: ")); DEBUG_PRINTLN(g_current_state,0);
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Include Draw"));
    g_lcd.setCursor(0, 1);
    if (g_include_draw == false) {
      lcd_print_p(&g_lcd, kDisabled);
    }
    else {
      lcd_print_p(&g_lcd, kEnabled);
    }
  }
  else {
    DEBUG_PRINTLN(F("Save g_include_draw and Return to Menu"), 0);
    g_rof_draw_setting_e = g_include_draw;
    g_current_state = MENU;
    DEBUG_PRINT(F("State after select: ")); DEBUG_PRINTLN(g_current_state,0);
    RenderMenu();
  }
}

//////////////////////////////
// ToggleIncludeDraw
//////////////////////////////

void ToggleIncludeDraw() {
  g_include_draw = !g_include_draw;
  DEBUG_PRINT(F("g_current_state: ")); DEBUG_PRINTLN(g_current_state,0);
  DEBUG_PRINT(F("Toggled Include Draw: "));DEBUG_PRINTLN(g_par_enabled, 0);
  g_lcd.setCursor(0, 1);
  if (g_include_draw == false) {
    lcd_print_p(&g_lcd, kDisabled); //10 characters
  }
  else {
    lcd_print_p(&g_lcd, kEnabled); //10 characters
  }
}

//////////////////////////////
// on_menu_buzzer_selected
//////////////////////////////

void on_menu_buzzer_selected(MenuItem* p_menu_item) {
  if(g_current_state != SETBEEP){
    DEBUG_PRINTLN(F("Enter SETBEEP Mode"), 0);
    g_current_state = SETBEEP;
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Buzzer Volume"));
    g_lcd.setCursor(0, 1);
    lcd_print(&g_lcd, g_beep_vol, 2);
  }
  else {
    DEBUG_PRINTLN(F("Save BeepVol and Return to Menu"), 0);
    g_beep_setting_e = g_beep_vol;
    g_current_state = MENU;
    RenderMenu();
  }
}

//////////////////////////////
// Helper function to cycle a simple integer value and display it
//////////////////////////////

void CycleIntValueAndDisplay(uint8_t &value, uint8_t min_val, uint8_t max_val, int8_t step, uint8_t display_width = 2, const char* suffix_p = nullptr) {
  value += step;
  if (value > max_val) {
    value = min_val;
  }
  else if (value < min_val) {
    value = max_val;
  }

  g_lcd.setCursor(0, 1);
  lcd_print(&g_lcd, value, display_width);
  if (suffix_p != nullptr) {
    lcd_print_p(&g_lcd, suffix_p);
  }
  lcd_print_p(&g_lcd, kClearLine);
}

//////////////////////////////
// IncreaseBeepVol
//////////////////////////////

void IncreaseBeepVol() {
  DEBUG_PRINTLN(F("IncreaseBeepVoly()"), 0);
  CycleIntValueAndDisplay(g_beep_vol, BEEP_VOL_MIN, BEEP_VOL_MAX, 1);
}

//////////////////////////////
// DecreaseBeepVol
//////////////////////////////

void DecreaseBeepVol() {
  DEBUG_PRINTLN(F("DecreaseBeepVoly()"), 0);
  CycleIntValueAndDisplay(g_beep_vol, BEEP_VOL_MIN, BEEP_VOL_MAX, -1);
}

//////////////////////////////
// on_menu_sensitivity_selected
//////////////////////////////

void on_menu_sensitivity_selected(MenuItem* p_menu_item) {
    if(g_current_state != SETSENS){
    DEBUG_PRINTLN(F("Enter SETSENS Mode"), 0);
    g_current_state = SETSENS;
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Sensitivity"));
    g_lcd.setCursor(0, 1);
    lcd_print(&g_lcd, g_sensitivity, 2);
  }
  else {
    DEBUG_PRINTLN(F("Save Sensitivity and Return to Menu"), 0);
    g_sens_setting_e = g_sensitivity;
    g_current_state = MENU;
    RenderMenu();
  }
}

//////////////////////////////
// IncreaseSensitivity
//////////////////////////////

void IncreaseSensitivity() {
  DEBUG_PRINTLN(F("IncreaseSensitivity()"), 0);
  CycleIntValueAndDisplay(g_sensitivity, SENSITIVITY_MIN, SENSITIVITY_MAX, 1);
  SensToThreshold();
}

//////////////////////////////
// DecreaseSensitivity
//////////////////////////////

void DecreaseSensitivity() {
  DEBUG_PRINTLN(F("DecreaseSensitivity()"), 0);
  CycleIntValueAndDisplay(g_sensitivity, SENSITIVITY_MIN, SENSITIVITY_MAX, -1);
  SensToThreshold();
}

//////////////////////////////
// on_menu_echo_selected - EEPROM
//////////////////////////////

void on_menu_echo_selected(MenuItem* p_menu_item) {
  if(g_current_state != SETECHO){
    DEBUG_PRINTLN(F("Enter SETECHO Mode"), 0);
    g_current_state = SETECHO;
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Echo Protect"));
    g_lcd.setCursor(0, 1);
    g_lcd.print(g_sample_window);
    lcd_print_p(&g_lcd, kMS);
  }
  else {
    DEBUG_PRINTLN(F("Save Echo and Return to Menu"), 0);
    g_sample_setting_e = g_sample_window;
    g_current_state = MENU;
    RenderMenu();
  }
}

//////////////////////////////
// Helper function to cycle sample_window and display with "ms" suffix
//////////////////////////////

void CycleSampleWindowAndDisplay(int8_t step) {
  if (step > 0) {
    if (g_sample_window == SAMPLE_WINDOW_MAX) {
      g_sample_window = SAMPLE_WINDOW_MIN;
    }
    else {
      g_sample_window += SAMPLE_WINDOW_STEP;
    }
  }
  else {
    if (g_sample_window == SAMPLE_WINDOW_MIN) {
      g_sample_window = SAMPLE_WINDOW_MAX;
    }
    else {
      g_sample_window -= SAMPLE_WINDOW_STEP;
    }
  }

  g_lcd.setCursor(0, 1);
  g_lcd.print(g_sample_window);
  lcd_print_p(&g_lcd, kMS);
  lcd_print_p(&g_lcd, kClearLine);
}

//////////////////////////////
// IncreaseEchoProtect
//////////////////////////////

void IncreaseEchoProtect() {
  DEBUG_PRINTLN(F("IncreaseEchoProtect()"), 0);
  CycleSampleWindowAndDisplay(1);
}

//////////////////////////////
// DecreaseEchoProtect
//////////////////////////////

void DecreaseEchoProtect() {
  DEBUG_PRINTLN(F("DecreaseEchoProtect()"), 0);
  CycleSampleWindowAndDisplay(-1);
}

//////////////////////////////
// convert g_sensitivity to g_threshold
//////////////////////////////

void SensToThreshold() {
  g_threshold = THRESHOLD_BASE - (THRESHOLD_SENSITIVITY_MULTIPLIER * g_sensitivity);
}

//////////////////////////////
// on_menu_par_state_selected
//////////////////////////////

void on_menu_par_state_selected(MenuItem* p_menu_item) {
  DEBUG_PRINT(F("State before select: ")); DEBUG_PRINTLN(g_current_state,0);
  DEBUG_PRINTLN(tm.get_current_menu()->get_name(),0);
  if(g_current_state != SETPARSTATE){
    DEBUG_PRINTLN(F("Enter SETPARSTATE Mode"),0);
    g_current_state = SETPARSTATE;
    DEBUG_PRINT(F("State after select: ")); DEBUG_PRINTLN(g_current_state,0);
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Par Times"));
    g_lcd.setCursor(0, 1);
    if (g_par_enabled == false) {
      lcd_print_p(&g_lcd, kDisabled);
    }
    else {
      lcd_print_p(&g_lcd, kEnabled);
    }
  }
  else {
    DEBUG_PRINTLN(F("Return to Menu"), 0);
    g_current_state = MENU;
    DEBUG_PRINT(F("State after select: ")); DEBUG_PRINTLN(g_current_state,0);
    RenderMenu();
  }
}

//////////////////////////////
// ToggleParState
//////////////////////////////

void ToggleParState() {
  g_par_enabled = !g_par_enabled;
  DEBUG_PRINT(F("g_current_state: ")); DEBUG_PRINTLN(g_current_state,0);
  DEBUG_PRINT(F("Toggled Par to: "));DEBUG_PRINTLN(g_par_enabled, 0);
  g_lcd.setCursor(0, 1);
  if (g_par_enabled == false) {
    lcd_print_p(&g_lcd, kDisabled); //10 characters
  }
  else {
    lcd_print_p(&g_lcd, kEnabled); //10 characters
  }
}

//////////////////////////////
// on_menu_par_times_selected
//////////////////////////////

void on_menu_par_times_selected(MenuItem* p_menu_item) {
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
    DEBUG_PRINT(F("State before select: ")); DEBUG_PRINTLN(g_current_state,0);
  if(g_current_state != SETPARTIMES){
    DEBUG_PRINTLN(F("Enter SETPARTIMES Mode"), 0);
    g_current_state = SETPARTIMES;
    DEBUG_PRINT(F("State after select: ")); DEBUG_PRINTLN(g_current_state,0);
    g_lcd.clear();
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("<<"));
    g_lcd.setCursor(5, 0);
    g_lcd.print(F("Par"));
    g_lcd.setCursor(9, 0);
    lcd_print(&g_lcd, (g_current_par + 1), LCD_DISPLAY_WIDTH_2);
    g_lcd.setCursor(4, 1);
    if (g_current_par > 0) {
      lcd_print_p(&g_lcd, kPlus);
    }
    else {
      lcd_print_p(&g_lcd, kSpace);
    }
    lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
  }
  else {
    g_current_state = MENU;
    DEBUG_PRINT(F("State after select: ")); DEBUG_PRINTLN(g_current_state,0);
    RenderMenu();
  }
}

//////////////////////////////
// Helper function to display current par information
//////////////////////////////

void DisplayParInfo() {
  g_lcd.setCursor(9, 0);
  lcd_print(&g_lcd, (g_current_par + 1), 2);
  g_lcd.setCursor(4, 1);
  if (g_current_par > 0) {
    lcd_print_p(&g_lcd, kPlus);
  }
  else {
    lcd_print_p(&g_lcd, kSpace);
  }
  lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
}

//////////////////////////////
// ParDown()
//////////////////////////////

void ParDown() {
  DEBUG_PRINTLN(F("ParDown()"), 0);
  if (g_current_par == 0) {
    g_current_par = kParLimit - 1;
  }
  else {
    g_current_par--;
  }
  DisplayParInfo();
}

//////////////////////////////
// ParUp()
//////////////////////////////

void ParUp() {
  DEBUG_PRINTLN(F("ParUp()"), 1);
  DEBUG_PRINTLN(kParLimit,0);
  if (g_current_par == kParLimit - 1) {
    g_current_par = 0;
  }
  else {
    g_current_par++;
  }
  DisplayParInfo();
}

//////////////////////////////
// EditPar()
//////////////////////////////

void EditPar() {
  if(g_current_state != SETINDPAR){
    DEBUG_PRINTLN(F("Enter SETINDPAR Mode"), 0);
    g_current_state = SETINDPAR;
    g_lcd.setBacklight(GREEN);
    g_lcd.setCursor(0, 0);
    g_lcd.print(F("Edit"));
    lcd_print_p(&g_lcd, kClearLine);
    g_lcd.setCursor(0, 1);
    g_lcd.print(F("P")); 
    lcd_print(&g_lcd, g_current_par + 1, LCD_DISPLAY_WIDTH_2);
    g_lcd.setCursor(4, 1);
    if (g_current_par > 0) {
      lcd_print_p(&g_lcd, kPlus);
    }
    else {
      lcd_print_p(&g_lcd, kSpace);
    }
    g_lcd.setCursor(5, 1);
    lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
    g_par_cursor = PAR_CURSOR_DEFAULT; //reset cursor to the seconds position
    LCDCursor();
  }
  else {
    DEBUG_PRINTLN(F("Return to SETPARTIMES"), 0);
    Serial.print(g_current_state);
    DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
    g_lcd.setBacklight(WHITE);
    tm.select();
  }
}

//////////////////////////////
// LeftCursor()
//////////////////////////////

void LeftCursor() {
  DEBUG_PRINTLN(F("LeftCursor()"), 0);
  if (g_par_cursor == PAR_CURSOR_MAX) {
    g_par_cursor = PAR_CURSOR_MIN;
  }
  else {
    g_par_cursor++;
  }
  LCDCursor();
}

//////////////////////////////
// RightCursor()
//////////////////////////////

void RightCursor() {
  DEBUG_PRINTLN(F("RightCursor()"), 0);
  if (g_par_cursor == PAR_CURSOR_MIN) {
    g_par_cursor = PAR_CURSOR_MAX;
  }
  else {
    g_par_cursor--;
  }
  LCDCursor();
}

//////////////////////////////
// LCDCursor()
//////////////////////////////
//switch case for cursor position displayed on screen
void LCDCursor() {
  DEBUG_PRINT(F("Displaying Cursor at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  switch (g_par_cursor) {
    case PAR_CURSOR_1MS: //milliseconds
      g_lcd.setCursor(11, 0); //icon at 13
      g_lcd.print(F("  _"));
      g_lcd.setCursor(5, 0); //left behind icon at 5
      lcd_print_p(&g_lcd, kSpace);
      break;
    case PAR_CURSOR_10MS: //ten milliseconds
      g_lcd.setCursor(10, 0); //icon at 12
      lcd_print_p(&g_lcd, kCursor);
      break;
    case PAR_CURSOR_100MS: //hundred milliseconds
      g_lcd.setCursor(9, 0); //icon at 11
      lcd_print_p(&g_lcd, kCursor);
      break;
    case PAR_CURSOR_1S: //seconds
      g_lcd.setCursor(7, 0); //icon at 9
      lcd_print_p(&g_lcd, kCursor);
      break;
    case PAR_CURSOR_10S: //ten seconds
      g_lcd.setCursor(6, 0); // icon at 8
      lcd_print_p(&g_lcd, kCursor);
      break;
    case PAR_CURSOR_1MIN: //minutes
      g_lcd.setCursor(4, 0); //icon at 6
      lcd_print_p(&g_lcd, kCursor);
      break;
    case PAR_CURSOR_10MIN: //ten minutes
      g_lcd.setCursor(5, 0); //icon at 5
      g_lcd.print(F("_  "));
      g_lcd.setCursor(13, 0); //left behind icon at 13
      lcd_print_p(&g_lcd, kSpace);
      break;
  }
}

//////////////////////////////
// Helper function to adjust par time based on cursor position and direction
//////////////////////////////

void AdjustParTime(int8_t direction) {
  // direction: 1 for increase, -1 for decrease
  unsigned long increment = PAR_TIME_INCREMENT_1MS;
  unsigned long max_val = PAR_TIME_MAX;
  unsigned long min_val = 0;

  switch (g_par_cursor) {
    case PAR_CURSOR_1MS: // milliseconds
      increment = PAR_TIME_INCREMENT_1MS;
      max_val = PAR_TIME_MAX_1MS;
      min_val = PAR_TIME_MIN_1MS;
      break;
    case PAR_CURSOR_10MS: // tens milliseconds
      increment = PAR_TIME_INCREMENT_10MS;
      max_val = PAR_TIME_MAX_10MS;
      min_val = PAR_TIME_MIN_10MS;
      break;
    case PAR_CURSOR_100MS: // hundreds milliseconds
      increment = PAR_TIME_INCREMENT_100MS;
      max_val = PAR_TIME_MAX_100MS;
      min_val = PAR_TIME_MIN_100MS;
      break;
    case PAR_CURSOR_1S: // seconds
      increment = PAR_TIME_INCREMENT_1S;
      max_val = PAR_TIME_MAX_1S;
      min_val = PAR_TIME_MIN_1S;
      break;
    case PAR_CURSOR_10S: // ten seconds
      increment = PAR_TIME_INCREMENT_10S;
      max_val = PAR_TIME_MAX_10S;
      min_val = PAR_TIME_MIN_10S;
      break;
    case PAR_CURSOR_1MIN: // minutes
      increment = PAR_TIME_INCREMENT_1MIN;
      max_val = PAR_TIME_MAX_1MIN;
      min_val = PAR_TIME_MIN_1MIN;
      break;
    case PAR_CURSOR_10MIN: // 10 minutes
      increment = PAR_TIME_INCREMENT_10MIN;
      max_val = PAR_TIME_MAX_10MIN;
      min_val = PAR_TIME_MIN_10MIN;
      break;
  }

  if (direction > 0) {
    // Increase
    if (g_par_times[g_current_par] < max_val) {
      g_par_times[g_current_par] += increment;
    }
  }
  else {
    // Decrease
    if (g_par_times[g_current_par] >= min_val) {
      g_par_times[g_current_par] -= increment;
    }
  }

  g_lcd.setCursor(5, 1);
  lcd_print_time(&g_lcd, g_par_times[g_current_par], LCD_TIME_WIDTH);
}

//////////////////////////////
// IncreaseTime()
//////////////////////////////

void IncreaseTime() {
  DEBUG_PRINT(F("Increase time at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  AdjustParTime(1);
}

//////////////////////////////
// DecreaseTime()
//////////////////////////////

void DecreaseTime() {
  DEBUG_PRINT(F("Decrease time at: "));DEBUG_PRINTLN(g_par_cursor, 0);
  AdjustParTime(-1);
}

//////////////////////////////
// BEEP
//////////////////////////////

void BEEP() {
  toneAC(kBeepNote, g_beep_vol, kBeepDur, true);
}

//////////////////////////////
// ButtonTone
//////////////////////////////

void ButtonTone() {
  toneAC(kBeepNote, g_beep_vol, kButtonDur, true);
}

////////////////////////////////////////////////////////////
// SETUP FUNCTIONS
////////////////////////////////////////////////////////////

//////////////////////////////
// EEPROMSetup
// Note - EEWrap automatically uses an .update() on EEPROM writes, 
// to avoid wearing out the EEPROM if the value being set is the same as 
// the existing value. 
//////////////////////////////

void EEPROMSetup() {
  DEBUG_PRINTLN(F("Checking if EEPROM needs to be set..."), 0);
  // Unset EEPROM values are set to 255, NOT 0
  // if ANY of our EEPROM stored settings come back 255, we'll know that the 
  // EEPROM settings have not been set
  // By checking all 4 settings, we help ensure that legacy EEPROM data doesn't
  // slip in and cause unexpected behavior.
  byte un_set = 255;
  
  if (g_sample_setting_e == un_set || g_sens_setting_e == un_set 
    || g_beep_setting_e == un_set || g_delay_setting_e == un_set) {
    DEBUG_PRINTLN(F("Setting EEPROM"), 0);
    g_delay_setting_e = g_delay_time;
      DEBUG_PRINTLN(F("Set g_delay_setting_e to "), 0);
      DEBUG_PRINTLN(g_delay_time, 0);
    g_rof_draw_setting_e = g_include_draw;
      DEBUG_PRINTLN(F("Set g_rof_draw_setting_e to "), 0);
      DEBUG_PRINTLN(g_include_draw, 0);
    g_beep_setting_e = g_beep_vol;
      DEBUG_PRINTLN(F("Set g_beep_setting_e to "), 0);
      DEBUG_PRINTLN(g_beep_vol, 0);
    g_sens_setting_e = g_sensitivity;
      DEBUG_PRINTLN(F("Set g_sens_setting_e to "), 0);
      DEBUG_PRINTLN(g_sensitivity, 0);
    g_sample_setting_e = g_sample_window;
      DEBUG_PRINTLN(F("Set g_sample_setting_e to "), 0);
      DEBUG_PRINTLN(g_sample_window, 0);
  }
  else {
    DEBUG_PRINTLN(F("Reading settings from EEPROM"), 0);
    g_delay_time = g_delay_setting_e;
      DEBUG_PRINTLN(F("Set g_delay_time to "), 0);
      DEBUG_PRINTLN(g_delay_time, 0);
    g_include_draw = g_rof_draw_setting_e;
      DEBUG_PRINTLN(F("Set g_include_draw to "), 0);
      DEBUG_PRINTLN(g_rof_draw_setting_e, 0);
    g_beep_vol = g_beep_setting_e;
      DEBUG_PRINTLN(F("Set g_beep_vol to "), 0);
      DEBUG_PRINTLN(g_beep_vol, 0);
    g_sensitivity = g_sens_setting_e;
      DEBUG_PRINTLN(F("Set g_sensitivity to "), 0);
      DEBUG_PRINTLN(g_sensitivity, 0);
    g_sample_window = g_sample_setting_e;
      DEBUG_PRINTLN(F("Set g_sample_window to "), 0);
      DEBUG_PRINTLN(g_sample_window, 0);
  }
  SensToThreshold(); 
}

//////////////////////////////
// Menu Setup
//////////////////////////////

void MenuSetup()
{
    DEBUG_PRINTLN(F("Setting up menu:"),0);
    DEBUG_PRINTLN_P(kMainName,0);
  main_menu.add_item(&menu_start, &on_menu_start_selected);
    DEBUG_PRINTLN_P(kStartName,0);
  main_menu.add_item(&menu_review, &on_menu_review_selected);
    DEBUG_PRINTLN_P(kReviewName,0);
  main_menu.add_menu(&par_menu);
    DEBUG_PRINTLN_P(kParName,0);
    par_menu.add_item(&menu_par_state, &on_menu_par_state_selected);
      DEBUG_PRINTLN_P(kParSetName,0);
    par_menu.add_item(&menu_par_times, &on_menu_par_times_selected);
      DEBUG_PRINTLN_P(kParTimesName,0);
  main_menu.add_menu(&settings_menu);
    DEBUG_PRINTLN_P(kSettingsName,0);
    settings_menu.add_item(&menu_start_delay, &on_menu_start_delay_selected);
      DEBUG_PRINTLN_P(kSetDelayName,0);
    settings_menu.add_item(&menu_rof, &on_menu_rof_selected);
      DEBUG_PRINTLN_P(kROFName,0);
    settings_menu.add_item(&menu_buzzer, &on_menu_buzzer_selected);
      DEBUG_PRINTLN_P(kBuzzerName,0);
    settings_menu.add_item(&menu_sensitivity, &on_menu_sensitivity_selected);
      DEBUG_PRINTLN_P(kSensitivityName,0);
    settings_menu.add_item(&menu_echo, &on_menu_echo_selected);
    DEBUG_PRINTLN_P(kEchoName,0);
  tm.set_root_menu(&main_menu); 
    DEBUG_PRINTLN(F("Menu Setup Complete"),0);
}

//////////////////////////////
// LCD Setup
//////////////////////////////

void LCDSetup() {
  DEBUG_PRINTLN(F("Setting up the LCD"),0);
  g_lcd.begin(LCD_COLS, LCD_ROWS);
  g_lcd.setBacklight(WHITE);
  RenderMenu();
}

////////////////////////////////////////////////////////////
// Listeners
////////////////////////////////////////////////////////////

//////////////////////////////
// Button Handler Helper Functions
//////////////////////////////

// Typedef for button action functions
typedef void (*ButtonActionFunc)();

// Structure to define button mappings for each state
struct ButtonMapping {
  uint8_t button;
  ButtonActionFunc action;
};

// Helper functions for common button actions
void HandleMenuSelect() {
  DEBUG_PRINTLN(F("SELECT/SELECT"), 0);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_name(),0);
  tm.select();
  if(g_current_state == MENU){RenderMenu();}
}

void HandleMenuRight() {
  DEBUG_PRINTLN(F("RIGHT/SELECT"), 0);
  tm.select();
  if(g_current_state == MENU){RenderMenu();}
}

void HandleMenuLeft() {
  DEBUG_PRINTLN(F("LEFT/BACK"), 0);
  tm.back();
  RenderMenu();
}

void HandleMenuDown() {
  DEBUG_PRINTLN(F("DOWN/NEXT"), 0);
  tm.next();
  RenderMenu();
}

void HandleMenuUp() {
  DEBUG_PRINTLN(F("UP/PREV"), 0);
  tm.prev();
  RenderMenu();
}

void HandleTimerSelect() {
  DEBUG_PRINTLN(F("SELECT/StopTimer()"), 0);
  StopTimer();
}

void HandleReviewSelect() {
  DEBUG_PRINTLN(F("SELECT/SELECT"), 0);
  tm.select();
}

void HandleReviewRight() {
  DEBUG_PRINTLN(F("RIGHT/RateOfFire()"), 0);
  RateOfFire(&g_include_draw);
}

void HandleReviewLeft() {
  DEBUG_PRINTLN(F("LEFT/g_review_shot--;NextShot()"), 0);
  g_review_shot--;
  NextShot();
}

void HandleReviewDown() {
  DEBUG_PRINTLN(F("DOWN/PreviousShot()"), 0);
  PreviousShot();
}

void HandleReviewUp() {
  DEBUG_PRINTLN(F("UP/NextShot()"), 0);
  NextShot();
}

void HandleParStateSelect() {
  DEBUG_PRINTLN(F("SELECT/(BACK)SELECT"), 0);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_name(),0);
  tm.select();
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_name(),0);
}

void HandleParStateLeft() {
  DEBUG_PRINTLN(F("LEFT/(BACK)SELECT"), 0);
  tm.select();
}

void HandleToggleParStateDown() {
  DEBUG_PRINTLN(F("DOWN/ToggleParState()"), 0);
  ToggleParState();
}

void HandleToggleParStateUp() {
  DEBUG_PRINTLN(F("UP/ToggleParState()"), 0);
  ToggleParState();
}

void HandleParTimesSelect() {
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
  DEBUG_PRINTLN(F("SELECT/EditPar"), 0);
  EditPar();
}

void HandleParTimesLeft() {
  DEBUG_PRINTLN(F("LEFT/(BACK)SELECT"), 0);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
  tm.select();
}

void HandleParDown() {
  DEBUG_PRINTLN(F("DOWN/ParDown()"), 0);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
  ParDown();
}

void HandleParUp() {
  DEBUG_PRINTLN(F("UP/ParUp()"), 0);
  DEBUG_PRINTLN_P(tm.get_current_menu()->get_selected()->get_name(),0);
  ParUp();
}

void HandleIndParSelect() {
  DEBUG_PRINTLN(F("SELECT/EditPar()"), 0);
  EditPar();
}

void HandleRightCursor() {
  DEBUG_PRINTLN(F("RIGHT/RightCursor()"), 0);
  RightCursor();
}

void HandleLeftCursor() {
  DEBUG_PRINTLN(F("LEFT/LeftCursor()"), 0);
  LeftCursor();
}

void HandleDecreaseTime() {
  DEBUG_PRINTLN(F("DOWN/DecreaseTime()"), 0);
  DecreaseTime();
}

void HandleIncreaseTime() {
  DEBUG_PRINTLN(F("UP/IncreaseTime()"), 0);
  IncreaseTime();
}

void HandleSettingsSelect() {
  DEBUG_PRINTLN(F("SELECT/SELECT"), 0);
  tm.select();
}

void HandleSettingsLeft() {
  DEBUG_PRINTLN(F("LEFT/(BACK)SELECT"), 0);
  tm.select();
}

void HandleDecreaseDelay() {
  DEBUG_PRINTLN(F("DOWN/DecreaseDelay()"), 0);
  DecreaseDelay();
}

void HandleIncreaseDelay() {
  DEBUG_PRINTLN(F("UP/IncreaseDelay()"), 0);
  IncreaseDelay();
}

void HandleToggleIncludeDrawDown() {
  DEBUG_PRINTLN(F("DOWN/ToggleIncludeDraw()"), 0);
  ToggleIncludeDraw();
}

void HandleToggleIncludeDrawUp() {
  DEBUG_PRINTLN(F("UP/ToggleIncludeDraw()"), 0);
  ToggleIncludeDraw();
}

void HandleDecreaseBeepVol() {
  DEBUG_PRINTLN(F("DOWN/DecreaseBeepVol()"), 0);
  DecreaseBeepVol();
}

void HandleIncreaseBeepVol() {
  DEBUG_PRINTLN(F("UP/IncreaseBeepVol()"), 0);
  IncreaseBeepVol();
}

void HandleDecreaseSensitivity() {
  DEBUG_PRINTLN(F("DOWN/DecreaseSensitivity())"), 0);
  DecreaseSensitivity();
}

void HandleIncreaseSensitivity() {
  DEBUG_PRINTLN(F("UP/IncreaseSensitivity()"), 0);
  IncreaseSensitivity();
}

void HandleDecreaseEchoProtect() {
  DEBUG_PRINTLN(F("DOWN/DecreaseEchoProtect();)"), 0);
  DecreaseEchoProtect();
}

void HandleIncreaseEchoProtect() {
  DEBUG_PRINTLN(F("UP/IncreaseEchoProtect();"), 0);
  IncreaseEchoProtect();
}

//////////////////////////////
// Button Listener
// Returns true if the button state changed
//////////////////////////////
void ButtonListener(Adafruit_RGBLCDShield* g_lcd,
                    uint8_t* b_state, ProgramState* p_state) {
  ///////////////
  // buttonStateManager
  ///////////////
  uint8_t state_now = g_lcd->readButtons();
  uint8_t new_button = state_now & ~*b_state; // true if state_now != b_state
  if (new_button) {DEBUG_PRINT(F("Button Pushed: "));}
  *b_state = state_now;

  // Early return if no button pressed
  if (!new_button) return;

  ///////////////
  // buttonReactor - Table-driven approach
  ///////////////

  // Define button mappings for each state
  static const ButtonMapping menu_map[] = {
    {BUTTON_SELECT, HandleMenuSelect},
    {BUTTON_RIGHT, HandleMenuRight},
    {BUTTON_LEFT, HandleMenuLeft},
    {BUTTON_DOWN, HandleMenuDown},
    {BUTTON_UP, HandleMenuUp},
    {0, NULL}
  };

  static const ButtonMapping review_map[] = {
    {BUTTON_SELECT, HandleReviewSelect},
    {BUTTON_RIGHT, HandleReviewRight},
    {BUTTON_LEFT, HandleReviewLeft},
    {BUTTON_DOWN, HandleReviewDown},
    {BUTTON_UP, HandleReviewUp},
    {0, NULL}
  };

  static const ButtonMapping setparstate_map[] = {
    {BUTTON_SELECT, HandleParStateSelect},
    {BUTTON_LEFT, HandleParStateLeft},
    {BUTTON_DOWN, HandleToggleParStateDown},
    {BUTTON_UP, HandleToggleParStateUp},
    {0, NULL}
  };

  static const ButtonMapping setpartimes_map[] = {
    {BUTTON_SELECT, HandleParTimesSelect},
    {BUTTON_LEFT, HandleParTimesLeft},
    {BUTTON_DOWN, HandleParDown},
    {BUTTON_UP, HandleParUp},
    {0, NULL}
  };

  static const ButtonMapping setindpar_map[] = {
    {BUTTON_SELECT, HandleIndParSelect},
    {BUTTON_RIGHT, HandleRightCursor},
    {BUTTON_LEFT, HandleLeftCursor},
    {BUTTON_DOWN, HandleDecreaseTime},
    {BUTTON_UP, HandleIncreaseTime},
    {0, NULL}
  };

  static const ButtonMapping setdelay_map[] = {
    {BUTTON_SELECT, HandleSettingsSelect},
    {BUTTON_LEFT, HandleSettingsLeft},
    {BUTTON_DOWN, HandleDecreaseDelay},
    {BUTTON_UP, HandleIncreaseDelay},
    {0, NULL}
  };

  static const ButtonMapping setrofdraw_map[] = {
    {BUTTON_SELECT, HandleParStateSelect},
    {BUTTON_LEFT, HandleParStateLeft},
    {BUTTON_DOWN, HandleToggleIncludeDrawDown},
    {BUTTON_UP, HandleToggleIncludeDrawUp},
    {0, NULL}
  };

  static const ButtonMapping setbeep_map[] = {
    {BUTTON_SELECT, HandleSettingsSelect},
    {BUTTON_LEFT, HandleSettingsLeft},
    {BUTTON_DOWN, HandleDecreaseBeepVol},
    {BUTTON_UP, HandleIncreaseBeepVol},
    {0, NULL}
  };

  static const ButtonMapping setsens_map[] = {
    {BUTTON_SELECT, HandleSettingsSelect},
    {BUTTON_LEFT, HandleSettingsLeft},
    {BUTTON_DOWN, HandleDecreaseSensitivity},
    {BUTTON_UP, HandleIncreaseSensitivity},
    {0, NULL}
  };

  static const ButtonMapping setecho_map[] = {
    {BUTTON_SELECT, HandleSettingsSelect},
    {BUTTON_LEFT, HandleSettingsLeft},
    {BUTTON_DOWN, HandleDecreaseEchoProtect},
    {BUTTON_UP, HandleIncreaseEchoProtect},
    {0, NULL}
  };

  // Select the appropriate button mapping based on state
  const ButtonMapping* current_map = NULL;
  switch (*p_state) {
    case MENU:        current_map = menu_map; break;
    case TIMER:
      // TIMER state only handles SELECT button
      if (new_button & BUTTON_SELECT) HandleTimerSelect();
      return;
    case REVIEW:      current_map = review_map; break;
    case SETPARSTATE: current_map = setparstate_map; break;
    case SETPARTIMES: current_map = setpartimes_map; break;
    case SETINDPAR:   current_map = setindpar_map; break;
    case SETDELAY:    current_map = setdelay_map; break;
    case SETROFDRAW:  current_map = setrofdraw_map; break;
    case SETBEEP:     current_map = setbeep_map; break;
    case SETSENS:     current_map = setsens_map; break;
    case SETECHO:     current_map = setecho_map; break;
  }

  // Execute the action for the pressed button
  if (current_map != NULL) {
    for (int i = 0; current_map[i].action != NULL; i++) {
      if (new_button & current_map[i].button) {
        current_map[i].action();
        break;
      }
    }
  }
}

//////////////////////////////
// Listen for Shots
//////////////////////////////

void ShotListener() {
  //DEBUG_PRINTLN(F("Listen-start:"),0);
  if (SampleSound() >= g_threshold) {
    RecordShot();
  }
  //DEBUG_PRINTLN(F("Listen-end:"),0);
}

////////////////////////////////////////////////////////////
// SETUP AND LOOP
////////////////////////////////////////////////////////////                      

void setup() {
  randomSeed(analogRead(1));
  DEBUG_SETUP();
  EEPROMSetup();
  MenuSetup();
  LCDSetup();
  DEBUG_PRINTLN(F("Setup Complete"), 0);
}

void loop() {
  //Probably all button actions should come before RunTimer()
  ButtonListener(&g_lcd, &g_buttons_state, &g_current_state); 
  RunTimer(&g_current_state, &g_par_enabled); 
} 
