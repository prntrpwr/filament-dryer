/*


/home/pi/.platformio/penv/bin/platformio run --target upload

*/


#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <LiquidCrystal_I2C.h>
#include <BME280I2C.h>
#include <TCA9548.h>

#define COMMAND_LINE_MAX_LENGTH_CHARS 16

#define HEATER_2_PIN 8
#define FAN_1_PIN 10
#define FAN_2_PIN 9

#define NUM_SENSORS 4

LiquidCrystal_I2C lcd(0x27,16,2);  // LCD I2C address is 0x27 and it is a 16x2 char display
BME280I2C bme;
TCA9548 tca(0x70);

uint8_t fan_1_pwm_duty_cycle_255 = 0;
uint8_t fan_2_pwm_duty_cycle_255 = 0;
uint8_t heater_2_pwm_duty_cycle_255 = 0;

unsigned long last_interval_timestamp_millis = 0;
String display_text = "";

unsigned int led_on = 0;
void led_blink() {
  digitalWrite(LED_BUILTIN, led_on);
  led_on ^= 0x01;
}

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  led_blink();

  Serial.begin(115200);
  while(!Serial) {}
  Serial.println("\n\n");

  Serial.println("System Startup");
  Serial.println("==============");

  Wire.begin();
  Serial.println("I2C Bus initialized");

  while (!tca.begin())
  {
    Serial.println("TCA9548 failed.  Retrying");
    delay(1000);
  }
  Serial.println("TCA9548 initialized");
  tca.disableAllChannels();
  tca.enableChannel(0);

  while(!bme.begin())
  {
    Serial.println("Could not connect to BME280");
    delay(1000);
  }
  Serial.println("BME280 initialized");

  tca.disableChannel(0);

  pinMode(FAN_1_PIN, OUTPUT);
  pinMode(FAN_2_PIN, OUTPUT);
  pinMode(HEATER_2_PIN, OUTPUT);

  lcd.init();
  lcd.backlight();

  Serial.println("Operational");
  Serial.println("==============");
}

float total_temp[NUM_SENSORS], total_hum[NUM_SENSORS], total_pres[NUM_SENSORS];
unsigned int sensor_data_point_count = 0;
void read_sensors() { 
  for (int i=0; i<NUM_SENSORS; i++) {
    float temp(NAN), hum(NAN), pres(NAN);
    tca.enableChannel(i);
    delay(10);
    bme.read(pres, temp, hum);
    tca.disableChannel(i);
    total_temp[i] += temp;
    total_hum[i] += hum;
    total_pres[i] += pres;
  }
  sensor_data_point_count++;
}

unsigned long data_print_count = 0;
void data_print(unsigned long interval_start_timestamp_millis, unsigned long interval_end_timestamp_millis) {
  Serial.print(++data_print_count);
  Serial.print(",");
  Serial.print(interval_start_timestamp_millis/1000.0);
  Serial.print(",");
  Serial.print( (interval_end_timestamp_millis-interval_start_timestamp_millis)/1000.0);
  Serial.print(",");
  Serial.print(sensor_data_point_count);
  Serial.print(",");
  Serial.print(fan_1_pwm_duty_cycle_255);
  Serial.print(",");
  Serial.print(fan_2_pwm_duty_cycle_255);
  Serial.print(",");
  Serial.print(heater_2_pwm_duty_cycle_255);

  for (int i=0; i<NUM_SENSORS; i++) {
    Serial.print(", ,");
    Serial.print(total_temp[i]/sensor_data_point_count);
    Serial.print(",");
    Serial.print(total_pres[i]/sensor_data_point_count);
    Serial.print(",");
    Serial.print(total_hum[i]/sensor_data_point_count);
    }
}

void data_reset() {
  sensor_data_point_count = 0;
  for (int i=0; i<NUM_SENSORS; i++) {
    total_temp[i] = total_hum[i] = total_pres[i] = 0;
  }
}

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
    Serial.print("OK F ");
    Serial.print(fan_num);
    Serial.print(" ");
    Serial.print(fan_duty_cycle);
    Serial.println();
  } else if (fan_num==2) {
    fan_2_pwm_duty_cycle_255 = fan_duty_cycle;
    analogWrite(FAN_2_PIN, fan_2_pwm_duty_cycle_255);
    Serial.print("OK F ");
    Serial.print(fan_num);
    Serial.print(" ");
    Serial.print(fan_duty_cycle);
    Serial.println();
  } else {
    Serial.println("E Invalid fan number");
  }
}

void set_heater(int heater_num, int heater_duty_cycle) {
  if (heater_num==2) {
    heater_2_pwm_duty_cycle_255 = heater_duty_cycle;
    analogWrite(HEATER_2_PIN, heater_2_pwm_duty_cycle_255);
    Serial.print("OK H ");
    Serial.print(heater_num);
    Serial.print(" ");
    Serial.print(heater_duty_cycle);
    Serial.println();
  } else {
    Serial.println("E Invalid heater number");
  }
}

void start_measurement_interval(unsigned int interval_duration_s) {
  unsigned long interval_start_timestamp_millis = millis();
  while (millis() - interval_start_timestamp_millis < interval_duration_s*1000) {
    read_sensors();
  }
  unsigned long interval_end_timestamp_millis = millis();
  Serial.print("OK R ");
  Serial.print(interval_duration_s);
  Serial.print(",");
  data_print(interval_start_timestamp_millis, interval_end_timestamp_millis);
  Serial.println();

  data_reset();
}

void describe_sensors() {
  Serial.print("OK,Describing sensors");
  Serial.println();
}

void process_command_line(String serial_in) {
      serial_in.trim();
      if (serial_in.length()==0) {
        Serial.print("OK status ");
        Serial.print(serial_in);
        Serial.print(",");
        Serial.print(fan_1_pwm_duty_cycle_255);
        Serial.print(",");
        Serial.print(fan_2_pwm_duty_cycle_255);
        Serial.print(",");
        Serial.print(heater_2_pwm_duty_cycle_255);
        Serial.println();
        return;
      }
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
        Serial.print("OK D ");
        Serial.print(display_text);
        Serial.println();
      } else {
        Serial.print("E Unparseable command \"");
        Serial.print(serial_in);
        Serial.println("\"");
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
    } else if (c=='\r') {
      // ignore
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