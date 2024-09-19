#include <Arduino.h>
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

#define COMMAND_LINE_MAX_LENGTH_CHARS 16

#define HEATER_2_PIN 8
#define FAN_1_PIN 10
#define FAN_2_PIN 9

LiquidCrystal_I2C lcd(0x27,16,2);  // LCD I2C address is 0x27 and it is a 16x2 char display

unsigned int led_on = 0;
void led_blink() {
  digitalWrite(LED_BUILTIN, led_on);
  led_on ^= 0x01;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  led_blink();

  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  pinMode(HEATER_2_PIN, OUTPUT);

  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
}

uint8_t fan_1_pwm_duty_cycle_255 = 0;
uint8_t fan_2_pwm_duty_cycle_255 = 0;
uint8_t heater_2_pwm_duty_cycle_255 = 0;

unsigned long last_interval_timestamp_millis = 0;
String display_text = "";
void lcd_display() {
  lcd.clear();

  /* 0,0 is first character of first row, 0,1 is first character of 2nd row*/
  lcd.setCursor(0,0);
  lcd.print((last_interval_timestamp_millis / 1000) % 10000);

  lcd.setCursor(5,0);
  lcd.print("1F");
  lcd.print(fan_1_pwm_duty_cycle_255);
  lcd.setCursor(11,0);
  lcd.print("2F");
  lcd.print(fan_2_pwm_duty_cycle_255);

  lcd.setCursor(0,1);
  lcd.print(display_text.substring(0, 10));
  // lcd.setCursor(5,1);
  // lcd.print("1H");
  // lcd.print(0);
  lcd.setCursor(11,1);
  lcd.print("2H");
  lcd.print(heater_2_pwm_duty_cycle_255);
}

void set_fan(int fan_num, int fan_duty_cycle) {
  if (fan_num==1) {
    fan_1_pwm_duty_cycle_255 = fan_duty_cycle;
    analogWrite(FAN_1_PIN, fan_1_pwm_duty_cycle_255);
    Serial.println("OK");
  } else if (fan_num==2) {
    fan_2_pwm_duty_cycle_255 = fan_duty_cycle;
    analogWrite(FAN_2_PIN, fan_2_pwm_duty_cycle_255);
    Serial.println("OK");
  } else {
    Serial.println("E Invalid fan number");
  }
}

void set_heater(int heater_num, int heater_duty_cycle) {
  if (heater_num==2) {
    heater_2_pwm_duty_cycle_255 = heater_duty_cycle;
    analogWrite(HEATER_2_PIN, heater_2_pwm_duty_cycle_255);
    Serial.println("OK");
  } else {
    Serial.println("E Invalid heater number");
  }
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
      } else if (command=='d' || command=='D') {
        display_text = serial_in.substring(first_space+1);
        display_text.trim();
        Serial.println("OK");
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
      if (serial_in.length() > COMMAND_LINE_MAX_LENGTH_CHARS) {
        Serial.println("E Command line length limit exceeded");
        serial_in = "";
      } else {
        serial_in += c;
      }
    }
  }

  if (this_loop_ts_millis - last_interval_timestamp_millis > 1000) {
    last_interval_timestamp_millis = this_loop_ts_millis;
    lcd_display();
    led_blink();
  }
}