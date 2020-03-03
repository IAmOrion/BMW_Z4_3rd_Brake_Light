/*=========================================================================*/
/*                          Written by James Tanner                        */
/*                  https://www.facebook.com/TheMeroving1an                */
/*                                  IAmOrion                               */
/*                                Version: 1.5                             */
/*=========================================================================*/

/***********************************************************************************************/
/* Do not modify the code below.  To alter any settings, please look in 'Settings_Variables.h' */
/***********************************************************************************************/

#include "Settings_Variables.h"
#include <Adafruit_NeoPixel.h>
#include "NeoPatterns.h"

void StripComplete();

NeoPatterns Strip(number_of_pixels, pixel_pin, NEO_GRB + NEO_KHZ800, &StripComplete); //RGB

void setup() {
  #ifdef SERIAL_DEBUG
    #ifdef BLUETOOTH_NANO
      Serial.begin(BT_BAUD_RATE);
    #else 
      Serial.begin(BAUD_RATE);
    #endif 
    
    Serial.setTimeout(10);
    
    #ifdef WAIT_FOR_SERIAL
      while (!Serial) { ; }
    #endif
    Serial.println();Serial.println();
    Serial.println(F("============================="));
    Serial.print(F(" BMW Z4 3rd Brake Light v")); Serial.println(VERSION);
    Serial.println(F("============================="));
  #endif
  
  Strip.begin(); Strip.setBrightness(LED_BRIGHTNESS); Strip.ColorSet(Strip.Color(OFF));

  pinMode(brake, INPUT); pinMode(reverse, INPUT); pinMode(left, INPUT);
  pinMode(right, INPUT); pinMode(custom1, INPUT); pinMode(custom2, INPUT);
  pinMode(indicators_enabled, INPUT_PULLUP); pinMode(indicator_mode_select, INPUT_PULLUP);

  checkIndicatorToggle();

  if (STARTUP_ANIMATION) {
    Strip.Scanner(Strip.Color(RED), KNIGHT_RIDER_SPEED);
    while (COUNTER < 2) { Strip.Update(); }
    Strip.setPixelColor(0,Strip.Color(RED));Strip.show();delay(KNIGHT_RIDER_SPEED);
    Strip.ColorSet(Strip.Color(OFF));
  }

  if (INITIAL_TEST) {
    #ifdef SERIAL_DEBUG
      Serial.println(F("LED Test - ALL LEDs Should currently be lit RED"));
      Serial.println();
    #endif
    Strip.ColorSet(Strip.Color(RED));
    while(1);
  }
}

void loop() {
  
  if (!TEST_MODE) { checkInputs(); }
  if (MODE != LAST_MODE) { LAST_MODE = MODE; resetLight(); }
  
  checkIndicatorToggle();
  #ifdef SERIAL_DEBUG
    checkSerial();
  #endif

  switch(MODE) {
    case 0: { // OFF
      resetLight();
    } break;
    
    case 1: { // BRAKE LIGHT
      if (!ACTIVE) {
        ACTIVE=true;
        if (FORCE_BRAKE_ENABLED) {
          Strip.ForceBrake(Strip.Color(RED), FORCE_BRAKE_FLASH_RATE);
        } else {
          Strip.ColorSet(Strip.Color(RED));
        }
        #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
          Serial.println(F("Braking"));
        #endif
      } else {
        if (FORCE_BRAKE_ENABLED) {
          if (COUNTER > ((FORCE_BRAKE_TIMES_FLASH * 2)+1)) {
            Strip.ActivePattern = NONE;
            Strip.ColorSet(Strip.Color(RED));
          }
        }
      }
    } break;

    case 2: { // REVERSING LIGHT
      if (!ACTIVE) {
        ACTIVE=true;
        Strip.ColorSet(Strip.Color(WHITE_));
        #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
          Serial.println(F("Reversing"));
        #endif
      }
    } break;

    case 3: { // TURNING LEFT
      if (INDICATOR_MODE == 1) {
        if (!TEST_MODE) {
          if (!ACTIVE) {
            ACTIVE=true;
            for (int i=(number_of_pixels/2);i<number_of_pixels;i++) {
              if (COLOUR_CORRECTION) {
                Strip.setPixelColor(i, Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])));
              } else {
                Strip.setPixelColor(i, Strip.Color(cRed_b,cGreen_b,cBlue_b));
              }
            }
            Strip.show();
            #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
              Serial.println(F("Indicating Left (Mode 1)"));
            #endif
          }
        } else if (TEST_MODE) {
          unsigned long currentMillis = millis();
          if(currentMillis - previousMillis > interval) {
            previousMillis = currentMillis;
            if (Strip.getPixelColor((number_of_pixels/2)) == 0) {
              for (int i=(number_of_pixels/2);i<number_of_pixels;i++) {
                if (COLOUR_CORRECTION) {
                  Strip.setPixelColor(i, Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])));
                } else {
                  Strip.setPixelColor(i, Strip.Color(cRed_b,cGreen_b,cBlue_b));
                }
                
              }
              Strip.show();
            } else {
              Strip.ColorSet(Strip.Color(OFF));
            }
          }
          if (!ACTIVE) {
            ACTIVE=true;
            #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
              Serial.println(F("Indicating Left (Mode 1 | TEST MODE)"));
            #endif
          }
        }
      } else if (INDICATOR_MODE == 2) {
        if (!ACTIVE) {
          ACTIVE=true;
          if (COLOUR_CORRECTION) {
            Strip.LeftTurn(Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])),pixelrate);
          } else {
            Strip.LeftTurn(Strip.Color(cRed_b,cGreen_b,cBlue_b),pixelrate);
          }
          #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
            Serial.println(F("Indicating Left (Mode 2)"));
          #endif
        }
      }
    } break;

    case 4: { // TURNING RIGHT
      if (INDICATOR_MODE == 1) {
        if (!TEST_MODE) {
          if (!ACTIVE) {
            ACTIVE=true;
            for (int i=0;i<(number_of_pixels/2);i++) {
              if (COLOUR_CORRECTION) {
                Strip.setPixelColor(i, Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])));
              } else {
                Strip.setPixelColor(i, Strip.Color(cRed_b,cGreen_b,cBlue_b));
              }
            }
            Strip.show();
            #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
              Serial.println(F("Indicating Right (Mode 1)"));
            #endif
          }
        } else if (TEST_MODE) {
          unsigned long currentMillis = millis();
          if(currentMillis - previousMillis > interval) {
            previousMillis = currentMillis;
            if (Strip.getPixelColor(0) == 0) {
              for (int i=0;i<(number_of_pixels/2);i++) {
                if (COLOUR_CORRECTION) {
                  Strip.setPixelColor(i, Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])));
                } else {
                  Strip.setPixelColor(i, Strip.Color(cRed_b,cGreen_b,cBlue_b));
                }
              }
              Strip.show();
            } else {
              Strip.ColorSet(Strip.Color(OFF));
            }
          }
          if (!ACTIVE) {
            ACTIVE=true;
            #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
              Serial.println(F("Indicating Right (Mode 1 | TEST MODE)"));
            #endif
          }
        }
      } else if (INDICATOR_MODE == 2) {
        if (!ACTIVE) {
          ACTIVE=true;
          if (COLOUR_CORRECTION) {
            Strip.RightTurn(Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])),pixelrate);
          } else {
            Strip.RightTurn(Strip.Color(cRed_b,cGreen_b,cBlue_b),pixelrate);
          }
          #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
            Serial.println(F("Indicating Right (Mode 2)"));
          #endif
        }
      }
    } break;

    case 5: { // HAZARDS (LEFT + RIGHT)
      if (INDICATOR_MODE == 1) {
        if (!TEST_MODE) {
          if (!ACTIVE) {
            ACTIVE=true;
            if (COLOUR_CORRECTION) { 
              Strip.ColorSet(Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])));
            } else {
              Strip.ColorSet(Strip.Color(cRed_b,cGreen_b,cBlue_b));
            }
            #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
              Serial.println(F("Hazards (Mode 1)"));
            #endif
          }
        } else if (TEST_MODE) {
          unsigned long currentMillis = millis();
          if(currentMillis - previousMillis > interval) {
            previousMillis = currentMillis;
            if (Strip.getPixelColor(0) == 0) {
              if (COLOUR_CORRECTION) {
                Strip.ColorSet(Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])));
              } else {
                Strip.ColorSet(Strip.Color(cRed_b,cGreen_b,cBlue_b));
              }
            } else {
              Strip.ColorSet(Strip.Color(OFF));
            }
          }
        }
      } else if (INDICATOR_MODE == 2) {
        if (!ACTIVE) {
          ACTIVE=true;
          if (COLOUR_CORRECTION) {
            Strip.Hazards(Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue])),pixelrate);
          } else {
            Strip.Hazards(Strip.Color(cRed_b,cGreen_b,cBlue_b),pixelrate);
          }
          #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
            Serial.println(F("Hazards (Mode 2)"));
          #endif
        }
      }
    } break;
    case 6: { // Custom1
      if (!ACTIVE) {
        ACTIVE=true;
        activateCustom(CUSTOM_FUNCTION_1);
        #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
          Serial.println(F("Custom 1"));
        #endif
      }
    } break;
    case 7: { // Custom2
      if (!ACTIVE) {
        ACTIVE=true;
        activateCustom(CUSTOM_FUNCTION_2);
        #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
          Serial.println(F("Custom 2"));
        #endif
      }
    } break;
    case 8: { // Custom3
      if (!ACTIVE) {
        ACTIVE=true;
        activateCustom(CUSTOM_FUNCTION_3);
        #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_ACTIONS)
          Serial.println(F("Custom 3"));
        #endif
      }
    } break;
  }
  
  Strip.Update();
}

void checkInputs() {

  value = analogRead(brake); vout = (value * 5.0) / 1024.0;
  if (PRE_FACELIFT) {
    brake_voltage = vout / (R2/(R1+R2)); if (brake_voltage < brake_min_voltage) { brake_voltage=0.0; BRAKING=false; } else if (brake_voltage > brake_min_voltage) { BRAKING=true; }
  } else {
    brake_voltage = vout / (R2/(R1+R2)); if (brake_voltage < min_voltage) { brake_voltage=0.0; BRAKING=false; } else if (brake_voltage > min_voltage) { BRAKING=true; }
  }

  value = analogRead(reverse); vout = (value * 5.0) / 1024.0;
  reverse_voltage = vout / (R2/(R1+R2)); if (reverse_voltage < min_voltage) { reverse_voltage=0.0; REVERSING=false;} else if (reverse_voltage > min_voltage) { REVERSING=true; }

  value = analogRead(left); vout = (value * 5.0) / 1024.0;
  left_voltage = vout / (R2/(R1+R2)); if (left_voltage < min_voltage) { left_voltage=0.0; if (left_off_couter > counter_max) { TURNING_LEFT=false; } else { left_off_couter++; } } else if (left_voltage > min_voltage) { TURNING_LEFT=true; left_off_couter=0;}

  value = analogRead(right); vout = (value * 5.0) / 1024.0;
  right_voltage = vout / (R2/(R1+R2)); if (right_voltage < min_voltage) { right_voltage=0.0; if (right_off_counter > counter_max) { TURNING_RIGHT=false; } else { right_off_counter++; } } else if (right_voltage > min_voltage) { TURNING_RIGHT=true; right_off_counter=0;}

  value = analogRead(custom1); vout = (value * 5.0) / 1024.0;
  custom1_voltage = vout / (R2/(R1+R2)); if (custom1_voltage < min_voltage) { custom1_voltage=0.0; CUSTOM1=false; } else if (custom1_voltage > min_voltage) { CUSTOM1=true; }

  value = analogRead(custom2); vout = (value * 5.0) / 1024.0;
  custom2_voltage = vout / (R2/(R1+R2)); if (custom2_voltage < min_voltage) { custom2_voltage=0.0; CUSTOM2=false; } else if (custom2_voltage > min_voltage) { CUSTOM2=true; }

  #if defined(SERIAL_DEBUG) && defined(SERIAL_DEBUG_VOLTAGES)
    if (BRAKING || REVERSING || TURNING_LEFT || TURNING_RIGHT || CUSTOM1 || CUSTOM2 ) {
      Serial.println();
      if (BRAKING) {
        Serial.print("Brake: "); Serial.print(brake_voltage); Serial.println("v");
      }
      if (REVERSING) {
        Serial.print("Reverse: "); Serial.print(reverse_voltage); Serial.println("v");
      }
      if (TURNING_LEFT) {
        Serial.print("Left: "); Serial.print(left_voltage); Serial.println("v");
      }
      if (TURNING_RIGHT) {
        Serial.print("Right: "); Serial.print(right_voltage); Serial.println("v");
      }
      if (CUSTOM1) {
        Serial.print("Custom 1: "); Serial.print(custom1_voltage); Serial.println("v");
      }
      if (CUSTOM2) {
        Serial.print("Custom 2: "); Serial.print(custom2_voltage); Serial.println("v");
      }
    }
  #endif

  if (BRAKING) { MODE=1; }
  else if (REVERSING) { MODE=2; }
  else if ((INDICATORS) && (TURNING_LEFT) && (!TURNING_RIGHT) && (!REVERSING) && (!BRAKING)) { MODE=3; }
  else if ((INDICATORS) && (TURNING_RIGHT) && (!TURNING_LEFT) && (!REVERSING) && (!BRAKING)) { MODE=4; }
  else if ((TURNING_LEFT) && (TURNING_RIGHT) && (!REVERSING) && (!BRAKING)) { MODE=5; }
  else if ((CUSTOM1) && (!CUSTOM2) && (!TURNING_LEFT) && (!TURNING_RIGHT) && (!REVERSING) && (!BRAKING)) { MODE=6; }
  else if ((CUSTOM2) && (!CUSTOM1) && (!TURNING_LEFT) && (!TURNING_RIGHT) && (!REVERSING) && (!BRAKING)) { MODE=7; }
  else if ((CUSTOM1) && (CUSTOM2) && (!TURNING_LEFT) && (!TURNING_RIGHT) && (!REVERSING) && (!BRAKING)) { MODE=8; }
  else { 
    #ifdef BLUETOOTH_NANO
      if ((!BT_BRAKING) && (!BT_REVERSING) && (!BT_TURNING_LEFT) && (!BT_TURNING_RIGHT) && (!BT_HAZARDS) && (!BT_CUSTOM1) && (!BT_CUSTOM2) && (!BT_CUSTOM3)) {
        MODE=0;
      } 
    #else
      MODE=0;
    #endif
  }
}

void resetLight() {
  Strip.ActivePattern = NONE;
  Strip.setBrightness(LED_BRIGHTNESS);
  Strip.ColorSet(Strip.Color(OFF));
  ACTIVE=false;
  COUNTER=0;
}

void checkSerial() {
  #ifdef BLUETOOTH_NANO
    while (Serial.available() > 0) {
      char inChar = Serial.read();
      if (inChar == '\n') {
        serial_i = inString.toInt();
        if ((serial_i >= 0) && (serial_i <= 8) && TEST_MODE) {
          MODE = serial_i;
          Serial.print(F("MODE Set to: ")); Serial.println(serial_i);
        } else if (serial_i == 99) {
          TEST_MODE=!TEST_MODE;
          if (TEST_MODE) {
            Serial.println(F("TEST MODE ENABLED"));
          } else {
            Serial.println(F("TEST MODE DISABLED"));
          }
        } else {
          listAvailableModes();
        }
        inString = "";
      } else if (inChar == ',') {
        if (!TEST_MODE) {
          if ((!BRAKING) && (!REVERSING) && (!TURNING_LEFT) && (!TURNING_RIGHT) && (!CUSTOM1) && (!CUSTOM2)) {
            if(inString.indexOf("BR") >=0) {
              MODE=1;
              BT_BRAKING=true; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=false;
            } else if(inString.indexOf("RE") >=0) {
              MODE=2;
              BT_BRAKING=false; BT_REVERSING=true; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=false;
            } else if(inString.indexOf("LT") >=0) {
              MODE=3;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=true; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=false;
            } else if(inString.indexOf("RT") >=0) {
              MODE=4;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=true; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=false;
            } else if(inString.indexOf("HA") >=0) {
              MODE=5;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=true; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=false;
            } else if(inString.indexOf("C1") >=0) {
              MODE=6;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=true; BT_CUSTOM2=false; BT_CUSTOM3=false;
            } else if(inString.indexOf("C2") >=0) {
              MODE=7;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=true; BT_CUSTOM3=false;
            } else if(inString.indexOf("C3") >=0) {
              MODE=8;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=true;
            } else if(inString.indexOf("OF") >=0) {
              MODE=0;
              BT_BRAKING=false; BT_REVERSING=false; BT_TURNING_LEFT=false; BT_TURNING_RIGHT=false; BT_HAZARDS=false; BT_CUSTOM1=false; BT_CUSTOM2=false; BT_CUSTOM3=false;
            }
          }
        }
        inString = "";
      } else {
        inString += inChar;
      }
    }
  #else 
    if (Serial.available() > 0) {
      serial_i = Serial.parseInt();
      if ((serial_i >= 0) && (serial_i <= 8) && TEST_MODE) {
        if (((serial_i == 3) && (!INDICATORS)) || ((serial_i == 4) && (!INDICATORS))) {
          Serial.println(F("Indicators are NOT currently enabled.  Please enable them via the toggle switch and re-send the Serial Input"));
          MODE=0;
        } else {
          MODE = serial_i;
        }
        Serial.print(F("MODE Set to: ")); Serial.println(MODE);
      } else if (serial_i == 99) {
        TEST_MODE=!TEST_MODE;
        if (TEST_MODE) {
          Serial.println(F("TEST MODE ENABLED"));
        } else {
          Serial.println(F("TEST MODE DISABLED"));
        }
      } else {
        listAvailableModes();
      }
    }
  #endif
}

void checkIndicatorToggle() {
  if (digitalRead(indicators_enabled) == LOW){ INDICATORS=false; } else { INDICATORS=true; }
  if (digitalRead(indicator_mode_select) == LOW){ INDICATOR_MODE=1; } else { INDICATOR_MODE=2; }

  #ifdef SERIAL_DEBUG
    if (INDICATORS != LAST_INDICATORS) {
      Serial.println();
      Serial.print(F("Indicators are ")); if (INDICATORS) { Serial.println(F("ENABLED")); } else { Serial.println(F("DISABLED")); }
      Serial.print(F("Indicator mode: ")); Serial.println(INDICATOR_MODE);
      Serial.println();
      LAST_INDICATORS = INDICATORS;
    }
  #endif
}

void listAvailableModes() {
  #ifdef SERIAL_DEBUG
    Serial.println();
    Serial.println(F("Invalid serial input"));
    Serial.println(F("Available modes:"));
    Serial.println(F("0 = Off"));
    Serial.println(F("1 = Brake"));
    Serial.println(F("2 = Reverse"));
    Serial.println(F("3 = Left Turn"));
    Serial.println(F("4 = Right Turn"));
    Serial.println(F("5 = Hazards"));
    Serial.println(F("6 = Custom1"));
    Serial.println(F("7 = Custom2"));
    Serial.println(F("8 = Custom3 (Turn on Custom 1 + Custom 2 at same time)"));
    Serial.println();
    Serial.println(F("99 = Enabled/Disable Test Mode"));
    Serial.println();
  #endif
}

void activateCustom(int effect) {
  switch (effect) {
    case 1: {
      Strip.Scanner(Strip.Color(RED), KNIGHT_RIDER_SPEED);
    } break;
    case 2: {
      Strip.RainbowCycle(3);
    } break;
    case 3: {
      Strip.TheaterChase(Strip.Color(255,255,0), Strip.Color(0,0,50), 100);
    } break;
    case 4: {
      Strip.ColorWipe(Strip.Wheel(random(255)),50);
    } break;
    case 5: {
      Strip.Police(Strip.Color(BLUE),Strip.Color(RED),8,55);
    } break;
    case 6: {
      Strip.Police(Strip.Color(BLUE),Strip.Color(BLUE),8,55);
    } break;
    default: {
      
    } break;
  }
}

void StripComplete() { 
  //Serial.println(F("StripComplete done"));

  if (Strip.ActivePattern == LEFT_TURN) {
    if (Strip.getPixelColor(Strip.numPixels()-1) == 0) {
      if (COLOUR_CORRECTION) {
        Strip.Color1 = Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue]));
      } else {
        Strip.Color1 = Strip.Color(cRed_b,cGreen_b,cBlue_b);
      }
    } else {
      Strip.Color1 = Strip.Color(OFF);
    }
    Strip.Index=10;
  }

  if (Strip.ActivePattern == RIGHT_TURN) {
    if (Strip.getPixelColor((Strip.numPixels()/2)-1) == 0) {
      if (COLOUR_CORRECTION) {
        Strip.Color1 = Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue]));
      } else {
        Strip.Color1 = Strip.Color(cRed_b,cGreen_b,cBlue_b);
      }
    } else {
      Strip.Color1 = Strip.Color(OFF);
    }
    Strip.Index=10;
  }

  if (Strip.ActivePattern == HAZARDS) {
    if (Strip.getPixelColor(19) == 0) {
      if (COLOUR_CORRECTION) {
        Strip.Color1 = Strip.Color(pgm_read_byte(&gamma8[cRed]),pgm_read_byte(&gamma8[cGreen]),pgm_read_byte(&gamma8[cBlue]));
      } else {
        Strip.Color1 = Strip.Color(cRed_b,cGreen_b,cBlue_b);
      }
    } else {
      Strip.Color1 = Strip.Color(0,0,0);
    }
    Strip.Index=10;
    Strip.AnimationCounter=0;
  }

  if (Strip.ActivePattern == POLICE) {
    CompleteCount++;
    if (CompleteCount == 2) {
      CompleteCount = 0;
    }
  }

  if (Strip.ActivePattern == FORCE_BRAKE) {
    if (Strip.getPixelColor(0) == 0) {
      Strip.Color1 = Strip.Color(RED);
    } else {
      Strip.Color1 = Strip.Color(OFF);
    }
  }

  if (MODE==7) {
    if (Strip.ActivePattern == COLOR_WIPE) {
      Strip.Color1 = Strip.Wheel(random(255));
      Strip.Reverse();
    }
  }
  
  COUNTER++;
}
