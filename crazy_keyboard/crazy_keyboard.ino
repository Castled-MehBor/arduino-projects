#define LED 2
#define K1 10
#define K2 9
#define K3 6
#define SP 5
#define B 11
//Include the 'pitches' library
#include "pitches.h"
void setup() 
{
  //Sets all output pins on the UNO R3
        pinMode(K1, INPUT_PULLUP);
        pinMode(K2, INPUT_PULLUP);
        pinMode(K3, INPUT_PULLUP);
        pinMode(SP, INPUT_PULLUP);
        pinMode(LED, OUTPUT);
        //Starts the Serial Monitor
            Serial.begin(9600);
}
//Sine value for the 'crazy' effect
float sine = 0;
//Changing index to modify what note is played
int index;
//Used for the LED flash
int multiplier = 1;
//Prevents multiple 'index' changes
bool prevent;
//All of the notes available
int noteList[] = {
  NOTE_C5, NOTE_D5, NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5, NOTE_C6};
//Checks if the specified button is pressed (prevents extra inputs)
bool buttonPressed(int type)
{
  switch(type)
  {
    case 1:
      return digitalRead(K1) == LOW && digitalRead(K2) != LOW && digitalRead(K3) != LOW && digitalRead(SP) != LOW;
    case 2:
      return digitalRead(K2) == LOW && digitalRead(K3) != LOW && digitalRead(K1) != LOW && digitalRead(SP) != LOW;
    case 3:
      return digitalRead(K3) == LOW && digitalRead(K2) != LOW && digitalRead(K1) != LOW && digitalRead(SP) != LOW;
    case 4:
      return digitalRead(SP) == LOW && digitalRead(K1) != LOW && digitalRead(K2) != LOW && digitalRead(K3) != LOW;
  }
}
//Sets 'f' to 'big' if it exceeds said value, or to 'small' if it is below said value
float clamp(float f, float small, float big)
{
  if (f > big)
    f = big;
  if (f < small)
    f = small;
  return f;
}
void loop() //this will run infinitely
{
  if (buttonPressed(1))
  {
    //Sine change (either by Pi/45, or by Pi/15)
    sine += PI / (index == 0 ? 45 : 15);
    if (sine > PI * 2)
      sine = 0;
    //Play **the tone at pin B (buzzer) for 20 ticks
    //**note 1/3, multiplied by (sine 2 or 1) / 4
    tone(B, (((noteList[0 + index] * clamp(((sin(sine) + 0.5f) / 4), 0, 1)))), 20);
    //Debugging
    Serial.println((((noteList[0 + index] * clamp(((sin(sine) + 0.5f) / 4), 0, 1) - 33))));
  }
  if (buttonPressed(2))
  {
    //Sine change by Pi/30
    sine += PI / 30;
    if (sine > PI * 2)
      sine = 0;
    //Play **the tone at pin B (buzzer) for 20 ticks
    //**note 4/6, multiplied by (sine 1.75 or 0.75) / 4
    tone(B, noteList[3 + index] * clamp(((sin(sine) + 0.75f) / 4), 0, 1), 20);
    //Debugging
    Serial.println(noteList[3 + index] * clamp(((sin(sine) + 0.75f) / 4), 0, 1));
  }
   if (buttonPressed(3))
  {
    //Sine change (either by Pi/15, or by Pi/45)
    sine += PI / (index == 0 ? 15 : 45);
    if (sine > PI * 2)
      sine = 0;
   //Play **the tone at pin B (buzzer) for 20 ticks
   //**note 4/6, multiplied by (sine 1.5 or 0.5) / 4, and increased by 33 to differentiate pitch
   tone(B, (((noteList[5 + index] * clamp(((sin(sine) + 1) / 4), 0, 1) + 33))), 20);
   //Debugging
   Serial.println((((noteList[5 + index] * clamp(((sin(sine) + 1) / 4), 0, 1) + 33))));
  }
  if (buttonPressed(4))
  {
    sine = 0;
        for(int a = 0; a < 5; a++)
        {
          //Toggling / Alternation
          multiplier *= -1;

          if (multiplier == 1)
          //Turn on the LED
            digitalWrite(LED, HIGH);
          else
          //Turn off the LED
            digitalWrite(LED, LOW);
            for(int b = 0; b < 60; b++)
            {
              //Sine alternation by Pi/60
                  sine += PI / 60;
              if (sine > PI * 2)
                sine = 0;
                //Plays a varying note based on the aformentioned index
                tone(B, noteList[5 + index] * clamp(((sin(sine) + 1) / 4), 0, 1), 20);
            }
          delay(150);
    }
    if (!prevent)
    {
      //Toggle the index
      prevent = true;
            index = (index == 0) ? 2 : 0;
    }
    prevent = false;
  }
}
