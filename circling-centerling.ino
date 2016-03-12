#include <Arduino.h>
#include <EEPROM.h>

const uint8_t pin_pot = A0;
const uint8_t pin_switch = 7;
const uint8_t pin_motor_a = 5;
const uint8_t pin_motor_b = 6;
const uint8_t pin_led = 3;

const uint8_t max_speed = 255; // max 255

const uint8_t rand_seed_addr = 0;

uint32_t next_time = 0;
int16_t next_speed = 0;

//The setup function is called once at startup of the sketch
void setup()
{
  pinMode(pin_led, OUTPUT);
  pinMode(pin_motor_a, OUTPUT);
  pinMode(pin_motor_b, OUTPUT);
  pinMode(pin_pot, INPUT);
  pinMode(pin_switch, INPUT);
  digitalWrite(pin_switch, HIGH); // int pullup

  // Generate new seed and put new random seed in in eeprom
  randomSeed(EEPROM.read(rand_seed_addr));
  EEPROM.write(rand_seed_addr, random(256));
}

void set_motor(int16_t s)
{
  if(s > 0) {
    analogWrite(pin_motor_a, s);
    analogWrite(pin_motor_b, 0);
  } else {
    analogWrite(pin_motor_a, 0);
    analogWrite(pin_motor_b, max_speed+s);
  }
}

// The loop function is called in an endless loop
void loop()
{  
  
  switch ( digitalRead(pin_switch) )
  {
  case HIGH: // CRAZY MODE
    if(next_time < millis()) {
      // special distribution of values: 1/10 backwards, 2/10 stop, 7/10 forwards
      uint8_t next_mode = random(1,11);
      if(next_mode == 1){
        next_speed = random(-255, -51);
        next_time = millis() + random(200, 1000);
      } else if(next_mode <= 3) {
        next_speed = 0;
        next_time = millis() + random(200, 2000);
      } else {
        next_speed = random(50,256);
        next_time = millis() + random(500, 10000);
      }
    }
    set_motor(next_speed);

    // Blink LED
    // danger crazy (500ms before change)
    if(next_time - millis() < 500) {
      if(millis() % 100 < 50) {
        digitalWrite(pin_led, HIGH);
      } else {
        digitalWrite(pin_led, LOW);
      }
    } else // normal crazy
    if(millis() % 500 < 250) {
      digitalWrite(pin_led, HIGH);
    } else {
      digitalWrite(pin_led, LOW);
    }
    break;


    
  case LOW: // NORMAL MODE
    {
      digitalWrite(pin_led, HIGH);
      int16_t p = analogRead(pin_pot);
      uint8_t s = map(p, 0, 1023, 0, max_speed);
        if(p > 512) {
          analogWrite(pin_motor_a, s);
          analogWrite(pin_motor_b, 0);
        } else {
          analogWrite(pin_motor_a, 0);
          analogWrite(pin_motor_b, max_speed-s);
        }
      break;
    }
  }
    
}

