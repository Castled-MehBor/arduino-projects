//Libraries used
#include <LiquidCrystal.h>
#include "pitches.h"
#include "SR04.h"

//Main pins
#define TRIG_PIN 13
#define ECHO_PIN 3 
#define SWITCH 5
#define RIGHT 2
#define LEFT 4
#define PB 6

//Variables used
SR04 sr04 = SR04(ECHO_PIN,TRIG_PIN);
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
//The pattern that is generated
String mem = "";
//The main input from the joystick
String in = "";
//Each time the sensor is blocked
String inBlock = "";
//When the player is expected to block
String block = "";
//Prevents confusion with string inputs
const String blockStandard = "-0";
//Prevents confusion with string inputs
const String standard = "><";
//Determines how long the joystick switch is held for
//(Used to reset input or replay the pattern)
int switchHold;
//Random variable
int ran;
//Determines the 'round'
int roundNum;
//Prevents extra input from the joystick
bool input[2] = { false, false };
//Up to 5; determines how many times an input has been put in
short inputCount;

void setup() {
 pinMode(RIGHT, OUTPUT);
 pinMode(LEFT, OUTPUT);
 pinMode(PB, OUTPUT);
 pinMode(SWITCH, INPUT);
 digitalWrite(SWITCH, HIGH);
 //Starts the lcd.
    lcd.begin(16, 2);
}

void CreateMemorize()
{
  //Resets the main memorization strings
  mem = "-----";
  block = "-----";
  for(int a = 0; a < 5; a++)
  {
    //Creates a new random pattern, containing 5 required inputs
        ran = random(2);
    if (ran == 0)
        mem.setCharAt(a, standard.charAt(0));
    else
        mem.setCharAt(a, standard.charAt(1));
  }
  if (roundNum >= 1)
  {
      //Creates a new random pattern (blocking the sensor), containing 5 required inputs
      for(int a = 0; a < 5; a++)
  {
        ran = random(2);
    if (ran == 0)
        block.setCharAt(a, blockStandard.charAt(0));
    else
        block.setCharAt(a, blockStandard.charAt(1));
  }
  }
  //Indicates the round
  roundNum++;
  lcd.print("Round ");
  lcd.print(roundNum);
  delay(1500);
  lcd.clear();
  //Plays the pattern
  PrintTop("Now playing...");
  delay(1000);
  PlayPattern(mem, block);
}
//Prints at the top row on the LCD
void PrintTop(String output)
{
  lcd.setCursor(0, 0);
  lcd.print(output);
}
//Prints at the bottom row on the LCD
void PrintBottom(String output)
{
  lcd.setCursor(0, 1);
  lcd.print(output);
}
void PlayPattern(String pattern, String block)
{
  for(int a = 0; a < pattern.length(); a++)
  {
    //Plays G, and turns on the right LED
    if (pattern.charAt(a) == standard.charAt(0))
    {
      digitalWrite(RIGHT, HIGH);
      digitalWrite(LEFT, LOW);
      tone(PB, NOTE_G5, 500);
      if (block.charAt(a) == blockStandard.charAt(1))
      {
        //Blinks twice
        PrintBottom("Cover sensor!");
        delay(250);
        digitalWrite(RIGHT, LOW);
        delay(250);
        digitalWrite(RIGHT, HIGH);
      }
      delay(500);
      digitalWrite(RIGHT, LOW);
      delay(250);
      PrintBottom("                  ");
    }
    //Plays C, and turns on the left LED
    if (pattern.charAt(a) == standard.charAt(1))
    {
      digitalWrite(RIGHT, LOW);
      digitalWrite(LEFT, HIGH);
      tone(PB, NOTE_C5, 500);
      if (block.charAt(a) == blockStandard.charAt(1))
      {
        //Blinks twice
        PrintBottom("Cover sensor!");
        delay(250);
        digitalWrite(LEFT, LOW);
        delay(250);
        digitalWrite(LEFT, HIGH);
      }
      delay(500);
      digitalWrite(LEFT, LOW);
      delay(250);
      PrintBottom("                  ");
    }
  }
  //Turns off both LEDs
        digitalWrite(RIGHT, LOW);
      digitalWrite(LEFT, LOW);
}
//Replays the pattern (called when the joystick switch is held)
void Replay()
{
  lcd.clear();
  PrintTop("Replaying...");
  PlayPattern(mem, block);
  delay(500);
}
//Joystick switch update method
void Switch()
{
  switchHold = 0;
  //If the switch is held for at least a second, the pattern will replay
  while(digitalRead(SWITCH) == LOW)
  {
    switchHold++;
    delay(1);
    if (switchHold >= 1000)
    Replay();
  }
  //Resets all current inputs
  in = "-----";
  inBlock = "-----";
  inputCount = 0;
  lcd.clear();
  PrintTop("Input cleared!");
  delay(1500);
  PrintTop("Taking input...");
}
void TakeInput()
{
  if (digitalRead(SWITCH) == LOW)
    Switch();
  //Helpful incase the sensor is accidentally obstructed
  if (sr04.Distance() < 100)
         PrintBottom("Sensor covered!");
  else
    //Also helpful incase the sensor is accidentally obstructed (shows when you covered the sensor)
    PrintBottom(inBlock);
  if (!input[0])
  {
      //If the joystick is moved to the left, set the appropriate input (left)
      if (analogRead(0) > 600)
      {
      //Play C, and turn on the left LED
      digitalWrite(LEFT, HIGH);
      digitalWrite(RIGHT, LOW);
      tone(PB, NOTE_C5, 500);
      delay(500);
      input[0] = true;
      //Puts in the input (also the block input when necessary)
        if (sr04.Distance() < 100)
          inBlock.setCharAt(inputCount, blockStandard.charAt(1));
      in.setCharAt(inputCount, standard.charAt(1));
      inputCount++;
      }
      //If the joystick is moved to the right, set the appropriate input (right)
      if (analogRead(0) < 400)
      {
      //Play G, and turn on the right LED
      digitalWrite(LEFT, LOW);
      digitalWrite(RIGHT, HIGH);
      tone(PB, NOTE_G5, 500);
      delay(500);
      input[0] = true;
      //Puts in the input (also the block input when necessary)
      if (sr04.Distance() < 100)
          inBlock.setCharAt(inputCount, blockStandard.charAt(1));
      in.setCharAt(inputCount, standard.charAt(0));
      inputCount++;
      }
  }
  //Reset
  digitalWrite(LEFT, LOW);
  digitalWrite(RIGHT, LOW);
  if (input[0] && analogRead(0) > 400 && analogRead(0) < 600)
    input[0] = false;
  if (sr04.Distance() >= 100)
    PrintBottom("               ");
}
void loop() 
{
  //Create the pattern
  CreateMemorize();
  delay(1000);
  lcd.clear();
  //Indicates that the player can now put in their inputs
  PrintTop("Taking input...");
  in = "-----";
  inBlock = "-----";
  inputCount = 0;
  //Keeps taking inputs until all inputs are put in
  while(inputCount < 5)
    TakeInput();
  lcd.clear();
  //Shows the inputs put in (not the expected inputs)
  PrintTop(in);
  PrintBottom(inBlock);
  delay(2000);
  if(in == mem && inBlock == block)
  {
    //Shows if both main inputs and sensor blocks are matching with the expected inputs
    lcd.clear();
    PrintTop("Match");
  }
  else
  {
        lcd.clear();
        PrintTop("Not Match");
        delay(2000);
        lcd.clear();
        //Shows what the expected inputs and sensor blocks were
        PrintBottom(block);
        PrintTop(mem);
  }
  //Restarts
  delay(2000);
  lcd.clear();
}
