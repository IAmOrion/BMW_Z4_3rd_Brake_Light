/*=========================================================================*/
/*                          Written by James Tanner                        */
/*                  https://www.facebook.com/TheMeroving1an                */
/*                                  IAmOrion                               */
/*                                Version: 1.4                             */
/*=========================================================================*/
/*     Modified version of Adafruit's NeoPattern class, availble here     */
/* https://learn.adafruit.com/multi-tasking-the-arduino-part-3/put-it-all-together-dot-dot-dot */
/*=========================================================================*/

// Pattern types supported:
enum  pattern { NONE, RAINBOW_CYCLE, THEATER_CHASE, COLOR_WIPE, SCANNER, FADE, POLICE, LEFT_TURN, RIGHT_TURN, HAZARDS, FORCE_BRAKE };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
    public:

    // Member Variables:  
    pattern  ActivePattern;  // which pattern is running
    direction Direction;     // direction to run the pattern
    
    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;  // total number of steps in the pattern
    uint16_t Index;  // current step within the pattern
    uint16_t AnimationCounter;
    
    void (*OnComplete)();  // Callback on completion of pattern
    
    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
    :Adafruit_NeoPixel(pixels, pin, type)
    {
        OnComplete = callback;
    }
    
    // Update the pattern
    void Update()
    {
        if((millis() - lastUpdate) > Interval) // time to update
        {
            lastUpdate = millis();
            switch(ActivePattern)
            {
                case RAINBOW_CYCLE:
                    RainbowCycleUpdate();
                    break;
                case THEATER_CHASE:
                    TheaterChaseUpdate();
                    break;
                case COLOR_WIPE:
                    ColorWipeUpdate();
                    break;
                case SCANNER:
                    ScannerUpdate();
                    break;
                case FADE:
                    FadeUpdate();
                    break;
                case POLICE:
                    PoliceUpdate();
                    break;
                case LEFT_TURN:
                    LeftTurnUpdate();
                    break;
                case RIGHT_TURN:
                    RightTurnUpdate();
                    break;
                case HAZARDS:
                    HazardsUpdate();
                    break;
                case FORCE_BRAKE:
                    ForceBrakeUpdate();
                    break;
                default:
                    break;
            }
        }
    }
  
    // Increment the Index and reset at the end
    void Increment()
    {
        if (Direction == FORWARD)
        {
           Index++;
           if (Index >= TotalSteps)
            {
                Index = 0;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        else // Direction == REVERSE
        {
            --Index;
            if (Index <= 0)
            {
                Index = TotalSteps-1;
                if (OnComplete != NULL)
                {
                    OnComplete(); // call the comlpetion callback
                }
            }
        }
        AnimationCounter++;
    }
    
    // Reverse pattern direction
    void Reverse()
    {
        if (Direction == FORWARD)
        {
            Direction = REVERSE;
            Index = TotalSteps-1;
        }
        else
        {
            Direction = FORWARD;
            Index = 0;
        }
    }
    
    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = RAINBOW_CYCLE;
        Interval = interval;
        TotalSteps = 255;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        }
        show();
        Increment();
    }

    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = THEATER_CHASE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
   }
    
    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
        for(int i=0; i< numPixels(); i++)
        {
            if ((i + Index) % 3 == 0)
            {
                setPixelColor(i, Color1);
            }
            else
            {
                setPixelColor(i, Color2);
            }
        }
        show();
        Increment();
    }

    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = COLOR_WIPE;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
        setPixelColor(Index, Color1);
        show();
        Increment();
    }
    
    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
        ActivePattern = SCANNER;
        Interval = interval;
        TotalSteps = (numPixels() - 1) * 2;
        Color1 = color1;
        Index = 0;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    { 
        for (int i = 0; i < numPixels(); i++)
        {
            if (i == Index)  // Scan Pixel to the right
            {
                 setPixelColor(i, Color1);
            }
            else if (i == TotalSteps - Index) // Scan Pixel to the left
            {
                 setPixelColor(i, Color1);
            }
            else // Fading tail
            {
                 setPixelColor(i, DimColor(getPixelColor(i)));
            }
        }
        show();
        Increment();
    }
    
    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FADE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Fade Pattern
    void FadeUpdate()
    {
        // Calculate linear interpolation between Color1 and Color2
        // Optimise order of operations to minimize truncation error
        uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
        uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
        uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;
        
        ColorSet(Color(red, green, blue));
        show();
        Increment();
    }

   void Police(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = POLICE;
        Interval = interval;
        TotalSteps = steps;
        Color1 = color1;
        Color2 = color2;
        Index = 0;
        Direction = dir;
    }

    void PoliceUpdate()
    {
      for (int i = 0; i < (numPixels() / 2); i++)
      {
        if (CompleteCount == 0) {
          if (getPixelColor(0) == 0) {
            setPixelColor(((numPixels() / 2) - 1) - i, Color1);
          } else {
            setPixelColor(((numPixels() / 2) - 1) - i, Color(0, 0, 0));
          }
        } else {
          if (getPixelColor(numPixels() / 2) == 0) {
            setPixelColor((numPixels() - 1) - i, Color2);
          } else {
            setPixelColor((numPixels() - 1) - i, Color(0, 0, 0));
          }
        }
      }
      show();
      Increment();
    }

    // Initialize Left Turn
    void LeftTurn(uint32_t color1, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = LEFT_TURN;
        Direction = dir;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Index = numPixels()/2;
    }

    void LeftTurnUpdate()
    {
        setPixelColor(Index, Color1);
        show();
        Increment();
    }

    // Initialize Right Turn
    void RightTurn(uint32_t color1, uint8_t interval, direction dir = REVERSE)
    {
        ActivePattern = RIGHT_TURN;
        Direction = REVERSE;
        Interval = interval;
        TotalSteps = numPixels()/2;
        Color1 = color1;
        Index = numPixels()/2;
    }

    void RightTurnUpdate()
    {
        setPixelColor(Index-1, Color1);
        show();
        Increment();
    }

    // Initialize Hazards
    void Hazards(uint32_t color1, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = HAZARDS;
        Interval = interval;
        TotalSteps = numPixels();
        Color1 = color1;
        Index = numPixels()/2;
        Direction = dir;
        AnimationCounter = 1;
    }

    void HazardsUpdate()
    {
        if (Index == (numPixels()/2)) {
          setPixelColor(Index, Color1);
          setPixelColor(Index-1, Color1);
        } else {
          setPixelColor(Index, Color1);
          setPixelColor((numPixels()/2)-AnimationCounter, Color1);
        }
        
        show();
        Increment();
    }

     // Initialize for a Fade
    void ForceBrake(uint32_t color1, uint8_t interval, direction dir = FORWARD)
    {
        ActivePattern = FORCE_BRAKE;
        Interval = interval;
        TotalSteps = 1;
        Color1 = color1;
        Index = 0;
        Direction = dir;
    }
    
    // Update the Fade Pattern
    void ForceBrakeUpdate()
    {
        ColorSet(Color1);
        show();
        Increment();
    }
   
    uint32_t DimColor(uint32_t color) { uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1); return dimColor; }
    void ColorSet(uint32_t color) { for (int i = 0; i < numPixels(); i++) { setPixelColor(i, color); } show(); }
    uint8_t Red(uint32_t color) { return (color >> 16) & 0xFF; }
    uint8_t Green(uint32_t color) { return (color >> 8) & 0xFF; }
    uint8_t Blue(uint32_t color) { return color & 0xFF; }
    uint32_t Wheel(byte WheelPos) { WheelPos = 255 - WheelPos; if(WheelPos < 85) { return Color(255 - WheelPos * 3, 0, WheelPos * 3); } else if(WheelPos < 170) { WheelPos -= 85; return Color(0, WheelPos * 3, 255 - WheelPos * 3); } else { WheelPos -= 170; return Color(WheelPos * 3, 255 - WheelPos * 3, 0); }}
};
