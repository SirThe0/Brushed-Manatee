/* By SirTheo

   Brushed Motors | Separate dual stage support | FTW Hyperdrive Solenoid

   Designed for Manatee brushed blasters.
   Intended board is Arduino Nano family (except 3.3V models).
   With brushed motors, this blaster is designed to only use 3S LiPo.
   All coding is designed around this intent, thus no battery monitoring
   is being conducted and the user will have to use their own lipo alarm.

   Creative Commons: Attribution, Non-Commercial
   Free for Non-Commercial use

   v1.0 - Initial Release

*/

#include <Bounce2.h>
#include <Wire.h>

// Comment out the following #define line if using a dual stage cage
// and want to control the activation of each stage by using a separate FET
// for each motor pair
#define ENABLE_TWO_FETS

// Comment out the following #define if using an ATMEGA 328 Nano Board. Leave defined if using
// a Nano Every with ATMEGA 4809
#define ATMEGA_4809

// Comment out the following #define to enable burst mode on trigger full pull.
// Modify BURST_SIZE for size of burst on full pull
#define BURST_MODE


//  Solenoid pusher defines. Burst and Auto Modes are future implementations.
#define PUSHER_EXTEND 30
#ifdef BURST_MODE
  #define DARTS_PER_SECOND 8
  #define FIRE_DELAY ((1000 / DARTS_PER_SECOND) - PUSHER_EXTEND)
  #if 0 >= FIRE_DELAY // DPS set too high. Set full pull as full auto instead
    static bool BurstMode = false;
    static int32_t RetractDelay = 70;
  #endif
  #if 0 < FIRE_DELAY // DPS acceptable. Confirm burst, not full auto
    static bool BurstMode = true;
    static int32_t RetractDelay = FIRE_DELAY;
  #endif
  #define BURST_SIZE 3
#endif

#ifndef BURST_MODE
  static bool BurstMode = false;
  static int32_t RetractDelay = 70;
#endif

//  Pin Definitions
#ifdef ATMEGA_4809
#define PIN_TRIGGER_HALF 15 // Register: Port D, Pin 2
#define PIN_TRIGGER_FULL 14 // Register: Port D, Pin 3
#endif
#ifndef ATMEGA_4809
#define PIN_TRIGGER_HALF 3 // Register: Port D, Pin 3
#define PIN_TRIGGER_FULL 2 // Register: Port D, Pin 2
#endif

#define PIN_PUSHER 7 // Register: Port A, Pin 1 (ATMEGA 4809) | Port D, Pin 7 (ATMEGA 328)
#define PIN_MOTOR_FET 9 // Register: Port B, Pin 0
#ifdef ENABLE_TWO_FETS
#define PIN_MOTOR2_FET 10 // Register: Port B, Pin 1
  static bool TwoFets = true;
  #define STAGE_DELAY 30
#endif
#ifndef ENABLE_TWO_FETS
  static bool TwoFets = false;
#endif



// Global Variables
bool MotorsRunning = false;
int32_t TimeSincePusherStart = 0;
int32_t TimeSinceTriggerReleased = 0;
bool TriggerState = false;
bool prevTriggerState = false;


void setup() {
  // Set up comms
  Serial.begin(57600);
  Serial.println( F("Booting.. ") );
  
}

void loop() {
  // put your main code here, to run repeatedly:

}

void ActuatePusher( void )
{
  Serial.println( F("Actuating Pusher") );
  digitalWrite(PIN_PUSHER, HIGH);
  delay( PUSHER_EXTEND );
  digitalWrite(PIN_PUSHER, LOW);
  delay( RetractDelay );
}

void ActivateDualStage( void )
{
  Serial.println( F("Command First Stage Activate") );
  bool CommandRev1 = true;
  bool CommandRev2 = false;
  int32_t TimeSinceStageOneRev = millis();
  if( millis() - TimeSinceStageOneRev < STAGE_DELAY )
  {
    CommandRev2 = false;
  }
  else
  {
    Serial.println( F("Command Second Stage Activate") );
    CommandRev2 = true;
  }

  if( CommandRev1 && CommandRev2 )
  {
    MotorsRunning = true; // Set flag to indicate the motors are running
  }
  
  if( CommandRev1 )
  {
    Serial.println( F("1st Stage Running") );
    digitalWrite(PIN_MOTOR_FET, HIGH);
  }

  if( CommandRev2 )
  {
    Serial.println( F("2nd Stage Running") );
    digitalWrite(PIN_MOTOR2_FET, HIGH);
  }
}

void ActivateMotors( void )
{
  Serial.println( F("Motors Running") );
  digitalWrite(PIN_MOTOR_FET, HIGH);
}

void TurnOffMotors( void )
{
  digitalWrite(PIN_MOTOR_FET, LOW);
  Serial.println( F("First Stage Spindown") );
  if( TwoFets )
  {
    digitalWrite(PIN_MOTOR2_FET, LOW);
    Serial.println( F("Second Stage Spindown") );
  }
}
