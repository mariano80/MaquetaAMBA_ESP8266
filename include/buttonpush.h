///////////////////////////////////////
// CLASS buttonpush
// A button device C++ class for use in
// command servos in AMBA ModelRailroad layout
// Author: Mariano C. Ucha
// License: CC BY-SA 4.0
// https://creativecommons.org/licenses/by-sa/4.0/
//
// Class declaration and implementation
///////////////////////////////////////

#include <Arduino.h>

// ----------------------------------------------------------------------------
// Definition of the Button component
// ----------------------------------------------------------------------------

// Button debouncing

const uint8_t DEBOUNCE_DELAY = 10; // in milliseconds

struct BUTTON_PARAMS {
  uint8_t pin;
  uint8_t webPush;
  bool moreOneServo;
  int servoManaged1;
  int servoManaged2;
  bool lastReading;
  uint32_t lastDebounceTime;
  uint16_t state;
  };

class Button {

public:

// Constructor de buttonpush
  uint8_t pin;
  bool webPush;
  bool moreOneServo;
  int servoManaged;
  int servoManaged1;
  int servoManaged2;
  bool lastReading;
  uint32_t lastDebounceTime;
  uint16_t state;
  
Button(BUTTON_PARAMS *parameter){
  pin = parameter->pin;
  webPush = parameter->webPush;
  moreOneServo = parameter->moreOneServo;
  servoManaged1 = parameter->servoManaged1;
  servoManaged2 = parameter->servoManaged2;
  lastReading = parameter->lastReading;
  lastDebounceTime = parameter->lastDebounceTime;
  state = parameter->state;

}

bool mgdServo(){
  return moreOneServo;
  }

bool pressed()                { return state == 1; }
bool released()               { return state == 0xffff; }
bool held(uint16_t count = 0) { return state > 1 + count && state < 0xffff; }

void read() {
    // reads the voltage on the pin connected to the button
    bool reading = digitalRead(pin);
    /*if (reading || webPush == true)
      reading = true;
    else{
      reading = false;
    }*/
    // if the logic level has changed since the last reading,
    // we reset the timer which counts down the necessary time
    // beyond which we can consider that the bouncing effect
    // has passed.
    if (reading != lastReading) {
        lastDebounceTime = millis();
    }

    // from the moment we're out of the bouncing phase
    // the actual status of the button can be determined
    if (millis() - lastDebounceTime > DEBOUNCE_DELAY) {
        // don't forget that the read pin is pulled-up
        bool pressed = reading == LOW;
        if (pressed) {
                 if (state  < 0xfffe) state++;
            else if (state == 0xfffe) state = 2;
        } else if (state) {
            state = state == 0xffff ? 0 : 0xffff;
        }
    }

    // finally, each new reading is saved
    lastReading = reading;
}

};
