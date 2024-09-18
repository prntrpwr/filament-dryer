#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // LCD I2C address is 0x27 and it is a 16x2 char display

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  lcd.init();
  lcd.backlight();
}

unsigned int led_on = 0;
void led_blink() {
  digitalWrite(LED_BUILTIN, led_on);
  led_on ^= 0x01;
}

unsigned long last_interval_timestamp_millis = 0;
void lcd_display() {
  lcd.clear();

  /* 0,0 is first character of first row, 0,1 is first character of 2nd row*/
  lcd.setCursor(0,0);
  lcd.print(led_on);

  lcd.setCursor(0,1);
  lcd.print(last_interval_timestamp_millis);
}

void loop() {
  unsigned long this_loop_ts_millis = millis();
  if (this_loop_ts_millis - last_interval_timestamp_millis > 1000) {
    last_interval_timestamp_millis = this_loop_ts_millis;
    lcd_display();
    led_blink();
  }
}