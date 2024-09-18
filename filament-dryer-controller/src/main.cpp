#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27,20,4);  // LCD I2C address is 0x27 and it is a 16x2 char display

unsigned int led_on = 0;
void led_blink() {
  digitalWrite(LED_BUILTIN, led_on);
  led_on ^= 0x01;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  led_blink();

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
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

void set_fan(int fan_num, int fan_duty_cycle) {
  Serial.print("Set fan ");
  Serial.print(fan_num);
  Serial.print(" to duty cycle ");
  Serial.print(fan_duty_cycle);
  Serial.println();
}

void set_heater(int heater_num, int heater_duty_cycle) {
  Serial.print("Set heater ");
  Serial.print(heater_num);
  Serial.print(" to duty cycle ");
  Serial.print(heater_duty_cycle);
  Serial.println();
}

void start_measurement_interval(int interval_duration_ms) {
  Serial.print("Starting measurement interval duration ");
  Serial.print(interval_duration_ms);
  Serial.println();
}

void describe_sensors() {
  Serial.print("Describing sensors");
  Serial.println();
}

void process_command_line(String serial_in) {
      char command = serial_in.charAt(0);
      int first_space = serial_in.indexOf(' ');
      int second_space = serial_in.indexOf(' ', first_space+1);
      long first_int = serial_in.substring(first_space,second_space).toInt();
      long second_int = serial_in.substring(second_space).toInt();

      if (command=='f' || command == 'F') {
        set_fan(first_int, second_int);
      } else if (command=='h' || command == 'H') {
        set_heater(first_int, second_int);
      } else if (command=='r' || command == 'R') {
        start_measurement_interval(first_int);
      } else if (command=='s' || command == 'S') {
        describe_sensors();
      } else {
        Serial.println("E Unparseable command");
      }
}

String serial_in;
void loop() {
  unsigned long this_loop_ts_millis = millis();

  if (Serial.available()) {
    char c = char(Serial.read());
    if (c=='\n') { 
      process_command_line(serial_in);
      serial_in = "";
    } else {
      serial_in += c;
    }
  }

  if (this_loop_ts_millis - last_interval_timestamp_millis > 1000) {
    last_interval_timestamp_millis = this_loop_ts_millis;
    lcd_display();
    led_blink();
  }
}