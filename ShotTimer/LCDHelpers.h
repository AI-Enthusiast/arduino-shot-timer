////////////////////////////////////////////////////////////
// LCD Helpers
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
//
// @TODO: Consider compatibility with MicroLCD for the 0.96" IC2 OLED
// http://forum.arduino.cc/index.php?topic=105866.30
//
// @TODO: Figure out the processing/memory cost of the time display conversion
//
////////////////////////////////////////////////////////////

#ifndef LCDHELPERS_H_
#define LCDHELPERS_H_

  ////////////////////////////////////////////////////////////
  // DEFINITIONS
  ////////////////////////////////////////////////////////////
  
  // These #defines make it easy to set the backlight color on the 
  // Adafruit RGB LCD
  #define RED 0x1
  #define YELLOW 0x3
  #define GREEN 0x2
  #define TEAL 0x6
  #define BLUE 0x4
  #define VIOLET 0x5
  #define WHITE 0x7
  
  ////////////////////////////////////////////////////////////
  // FUNCTIONS
  ////////////////////////////////////////////////////////////
  
  //////////////////////////////
  // PROGMEM Helper - Print a string from PROGMEM to an LCD Screen
  //////////////////////////////
  
  void lcd_print_p(Adafruit_RGBLCDShield* lcd, const char * kStr)
  {
    char c;
    if (!kStr)
    {
      return;
    }
    while (c = pgm_read_byte(kStr++))
      lcd->print(c);
  }
  
  //////////////////////////////
  // Print to an LCD
  //////////////////////////////
  
  void lcd_print(Adafruit_RGBLCDShield* lcd, uint32_t t, byte digits)
  {
    DEBUG_PRINT(F("Printing to LCD:"));
    char lcd_output[11];
    convert_num(t, digits, lcd_output);
    DEBUG_PRINTLN(lcd_output,0);
    lcd->print(lcd_output);
  }
  
  //////////////////////////////
  // Print time to an LCD screen
  //////////////////////////////
  
  void lcd_print_time(Adafruit_RGBLCDShield* lcd, uint32_t t, byte digits)
  {
    DEBUG_PRINTLN(F("Printing Time to LCD"),0);
    char lcd_output[11];
    convert_time(t, digits, lcd_output);
    DEBUG_PRINTLN(lcd_output,0);
    lcd->print(lcd_output);
  }
  
  //////////////////////////////
  // Clear LCD and display a two-line header
  //////////////////////////////

  void lcd_display_header(Adafruit_RGBLCDShield* lcd, const __FlashStringHelper* line1, const __FlashStringHelper* line2)
  {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(line1);
    lcd->setCursor(0, 1);
    lcd->print(line2);
  }

  //////////////////////////////
  // Clear LCD and display a two-line header (with PROGMEM for line 2)
  //////////////////////////////

  void lcd_display_header_p(Adafruit_RGBLCDShield* lcd, const __FlashStringHelper* line1, const char* line2_p)
  {
    lcd->clear();
    lcd->setCursor(0, 0);
    lcd->print(line1);
    lcd->setCursor(0, 1);
    lcd_print_p(lcd, line2_p);
  }

  //////////////////////////////
  // Display a value at a specific cursor position
  //////////////////////////////

  void lcd_display_at(Adafruit_RGBLCDShield* lcd, uint8_t col, uint8_t row, uint32_t value, uint8_t digits)
  {
    lcd->setCursor(col, row);
    lcd_print(lcd, value, digits);
  }

  //////////////////////////////
  // Display a time value at a specific cursor position
  //////////////////////////////

  void lcd_display_time_at(Adafruit_RGBLCDShield* lcd, uint8_t col, uint8_t row, uint32_t time, uint8_t digits)
  {
    lcd->setCursor(col, row);
    lcd_print_time(lcd, time, digits);
  }

  //////////////////////////////
  // Display a PROGMEM string at a specific cursor position
  //////////////////////////////

  void lcd_display_p_at(Adafruit_RGBLCDShield* lcd, uint8_t col, uint8_t row, const char* str_p)
  {
    lcd->setCursor(col, row);
    lcd_print_p(lcd, str_p);
  }

  //////////////////////////////
  // Display a Flash string at a specific cursor position
  //////////////////////////////

  void lcd_display_f_at(Adafruit_RGBLCDShield* lcd, uint8_t col, uint8_t row, const __FlashStringHelper* str)
  {
    lcd->setCursor(col, row);
    lcd->print(str);
  }

  //////////////////////////////
  // Display an integer at a specific cursor position
  //////////////////////////////

  void lcd_display_int_at(Adafruit_RGBLCDShield* lcd, uint8_t col, uint8_t row, int value)
  {
    lcd->setCursor(col, row);
    lcd->print(value);
  }

  //////////////////////////////
  // Update line 2 with enabled/disabled status
  //////////////////////////////

  void lcd_display_enabled_status(Adafruit_RGBLCDShield* lcd, bool enabled, const char* enabled_p, const char* disabled_p)
  {
    lcd->setCursor(0, 1);
    if (enabled) {
      lcd_print_p(lcd, enabled_p);
    } else {
      lcd_print_p(lcd, disabled_p);
    }
  }

#endif // LCDHELPERS_H_