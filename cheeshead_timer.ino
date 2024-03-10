#include <Servo.h>
#include <EEPROM.h>
#include <MPU6050_tockn.h>
#include <Wire.h>
#include<NoDelay.h>

// Dave Siegler ne9n.dave@gmail.com
// This is a timer that steps through a seguence of speeds
// to control the throttle poistion of a control line airplane
// in includes user set up values
// LED;s sho state of the time
// an MUP is used to trim the throttle based on
// pitch and roll values of te airplane
// create servo object to control a servo
// state constants part of config values
//

// acclermotoer support using MPU6050 tockn laibrat

// SHORT TERM
// 1) improve LED ,
// 2) clean up menu
// 3) range checl control functions
// 4) better burp, on startup 
// 5) Draw it up and write docs

// LONG TERM
// 1) RC transmitter interface
// 2) Add acceleration boost to control function

typedef struct {
  unsigned int FlySpeed; // 0 to 180 for degrees of rotation of a servo
  unsigned int FlyTime; // air time in seconds
  unsigned int ArmTime; // wait time in seconds
  unsigned int accelTime; // in msec
  int calX;
  int calY;
  int calZ;
  byte px;
  byte py;
  byte rx;
  byte ry;
} param;

//int i;
param TimerSetup;
MPU6050 mpu6050(Wire);
Servo esc;

#define IncThrottle 1
#define BurpMax 180
#define BURPTIME 500
#define RDYTIME 5000
#define LANDTIME 0x1FF
#define INCTIME 50
#define SKIP 500
#define FLASHON 500
#define FLASHOFF 500
#define MAX_SPEED  180

#define WAIT 0
#define ARMED 1
#define TAKEOFF 2
#define FLY 3
#define RAMPDWN 4
#define LAND 5


// sub state for flyng
#define FLYING 0
#define BURP 1
#define RDYLAND 2

// hardware pin def's
#define BUTTONPIN 13
#define SW1 8
#define SERVO 6
#define LED3 10
#define LED4 11
#define LED5 12 //works




extern void terminal();
extern void speedState();
extern void gyro();
extern void printDebug();
extern void led_slow();
extern void led_fast();



// Gyro related
bool gyro_flag = false;
int angleX;
int angleY;
int angleZ;

// speed variables
int maxThrottle = TimerSetup.FlySpeed;
int curThrottle = 0; // current speed
int posTrim;


// time variables 
noDelay LEDtime(1000);//Creats a noDelay varible set to 1000ms
unsigned long currentMillis = millis();
unsigned long state_tmr;
int incTime = 0;


// state variables
int speed_state = WAIT;
int inFlight = FLYING;
int led_state = 0;




void setup()
{
  // attaches the servo on pin 9 to the servo object
  esc.attach(SERVO, 1000, 2000);
  esc.write(curThrottle);
  // read in all the set up variales 
  int eeAddress = 0;
  EEPROM.get(eeAddress, TimerSetup );

  //mpu6050.setGyroOffsets(calX, calY, calZ);
  // set up the I/O pins
 
  pinMode(BUTTONPIN, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED3, OUTPUT);
  pinMode(LED4, OUTPUT);
  pinMode(LED5, OUTPUT);


  Serial.begin(19200);
  Wire.begin();
  mpu6050.begin();
  //mpu6050.calcGyroOffsets(true);
  mpu6050.setGyroOffsets(TimerSetup.calX / 100.0, TimerSetup.calY / 100.0, TimerSetup.calZ / 100.0);
  Serial.print(" init complte");
}


void plotDebug(void)
{

  //Serial.print( (digitalRead(BUTTONPIN))  ? "\t button up": "\t button down" );

  Serial.print(F("angleX:"));      Serial.print(angleX); Serial.print(F(","));
  Serial.print(F("angleY:"));    Serial.print(angleY );Serial.print(F(","));
  //Serial.print(F("\angleZ:"));    Serial.print(angleZ );Serial.print(F(","));
  //Serial.print(F("!button:"));    Serial.print(!digitalRead(BUTTONPIN) ); Serial.print(F(","));
  Serial.print(F("posTrim:"));  Serial.print(posTrim);Serial.print(F(","));
  Serial.print(F("throttle:")); Serial.print(curThrottle);Serial.print(F(","));
  //Serial.print(F(" speed_st:"));   Serial.print(speed_state);Serial.print(F(","));
 // Serial.print(F("currentMillis -state_tmr:")); Serial.print(currentMillis-state_tmr);
 Serial.print(F("\n"));
   
  //Serial.println();
}
void term_ctrl()
{
  char keypress;
  keypress = Serial.read();
  if (keypress == '?')
  {
    terminal();
  }
}

int i;
void loop()
{
  term_ctrl();
  speedState(); // this manages the state machunes for takeoff flyingand landing
  if (i++ >200){
     plotDebug();  // this prints a serial stream for the Arduino debugger.  un comment the intems to plot. 
     i = 0;}
  speedGyro();       // this reads the gyro and trims the speed 
  ledUpdate();  // updates the LED's 
}
