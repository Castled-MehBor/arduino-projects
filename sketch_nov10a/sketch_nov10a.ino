//Include Pitches Library
#include "pitches.h"
#define PLAYER 11
#define T1 3
#define T2 10
#define T3 9
#define BUTTON 13
#define AB 6
#define PB 5

void setup() 
{
  //Sets all output pins on the UNO R3
        pinMode(BUTTON, INPUT_PULLUP);
        pinMode(T1, OUTPUT);
        pinMode(T2, OUTPUT);
        pinMode(T3, OUTPUT);
        pinMode(AB, OUTPUT);
        pinMode(PB, OUTPUT);
        //Starts the Serial Monitor
            Serial.begin(9600);
}
//Returns a specific LED pin (used by Projectile)
int getLED(int num)
{
  switch(num)
  {
    case 1:
      return PLAYER;
    case 2:
      return T1;
    case 3:
      return T2;
    case 4:
      return T3;
  }
}
//The Player class
class Player
{
  int mult = 1;
  int cooldown;
  public: bool onGround = true;
  public: float jumpTimer;
  public: float jumpHeight;
  void Update()
  {
    //Cooldown system (prevents repeated jumps)
    if (cooldown-- < 0)
      cooldown = 0;
      if (digitalRead(BUTTON) == LOW)
      {
        if (onGround)
        {
          //"Jump", only if you are "on the ground"
          if (cooldown < 1)
          {
                      cooldown = 750;
          onGround = false;
          jumpTimer += (float)PI / 75;
          }
        }
      }
      if (!onGround)
      {
        //Increase the jump timer
        jumpTimer += (float)((PI / 500)) * mult;
        //A sine wave, for the "rising and falling" feeling
        jumpHeight = (float)round(sin(jumpTimer) * 255);
        //Reverses the sine
        if (jumpTimer >= PI / 2)
          mult *= -1;
        if (mult == -1 && jumpTimer <= 0.05)
        {
          //Land on the ground (jump timer reset)
          onGround = true;
            jumpTimer = 0;
            mult = 1;
        }
      }
  }
};
class Projectile
{
  public: int currentSlot = 4;
  public: int velocity = 10;
  int velTime;
  bool dead;
  void Update()
  {
    //If the projectile is NOT dead, update this projectile
    if (!dead)
    {
          velTime += velocity;
      if (velTime >= 1000)
      {
        //Move to the left by 1 LED
        velTime = 0;
        currentSlot--;
      }
      //If the projectile has reached/passed the player, it will not update
      if (currentSlot < 1)
        dead = true;
    }
    //the LED behind is turned off
    if (getLED(currentSlot + 1) != PLAYER)
      digitalWrite(getLED(currentSlot + 1), LOW);
      //the current LED is turned on
          if (getLED(currentSlot) != PLAYER)
      digitalWrite(getLED(currentSlot), HIGH);
      //Debug
    Serial.println(currentSlot);
  }
};
//Starts the game
bool start;
//Timer for resetting the projectile
int timer = 0;
//The player
Player player;
//The projectile
Projectile projectile;
//Multiplier (used for the player "hit" effect)
int mult = 1;
void hit()
{
  //Play 2 quick sounds on the active buzzer
  for(int b = 0; b < 2; b++)
  {
        for(int i=0;i<20;i++)
    {
      digitalWrite(AB,HIGH);
      delay(3);//wait for 1ms
      digitalWrite(AB,LOW);
      delay(2);//wait for 1ms
    }
    delay(50);
  }
  for(int c = 0; c < 4; c++)
  {
    mult *= -1;
    //Turn on the player LED
    if (mult == 1)
      analogWrite(PLAYER, 255);
    else
        //Turn off the player LED
      analogWrite(PLAYER, 0);
    delay(150);
  }
}
//Returns a specific note
  int getTone(int num)
  {
    switch(num)
    {
      case 0:
      return NOTE_C5;
      case 1:
      return NOTE_E5;
      case 2:
      return NOTE_G5;
      case 3:
      return NOTE_C6;
    }
  }
void spawnProj()
{
  //Keeps the player LED turned on
            analogWrite(PLAYER, 255);
  for(int i = 0; i < 4; i++)
  {
    //Plays 4 varying notes
    tone(PB, getTone(i), 100);
    delay(100);
  }
  //"Revive" the projectile
  projectile.dead = false;
  projectile.currentSlot = 4;
  //Set its velocity to a random value
  projectile.velocity = (int)random(5, 20);
  //Reset the timer
  timer = 0;
}
void loop() //this will run infinitely
{
  if (start)
  {
    //"Update" the player (make it function)
      player.Update();
  //If the player is on the ground, the projectile "hits" the player and "dies"
  if (projectile.currentSlot == 1 && player.onGround)
  {
    hit();
    //Set the current slot to 0 (dies)
    projectile.currentSlot--;
  }
  //Sets the player height (fades the LED based on the jumpHeight variable)
  analogWrite(PLAYER, 255 - player.jumpHeight);
  //Spawn a projectile one second after spawning the previous one
  if (timer++ > 1000)
    spawnProj();
    //"Update" the projectile (make it function)
    projectile.Update();
  }
  //Starts the game
    if (!start && digitalRead(BUTTON) == LOW)
    {
          start = true;
            analogWrite(PLAYER, 255);
          delay(1000);
    }
}
