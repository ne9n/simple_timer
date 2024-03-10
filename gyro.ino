/*
 *   Need a look up table for SIN  
 */
 #define SIN_TBL_SZ 36 
#define OFFSET 400
const int sinetbl[37] PROGMEM= {
500,587,671,750,821,883,933,970,
992,1000,992,970,933,883,821,750,
671,587,500,413,329,250,179,117,
67,30,8,0,8,30,67,117,
179,250,329,413,500};




void speedGyro()
{
  int pitch;
  int roll;
  int sinX;
  int siny;
 
  mpu6050.update();
  angleX = mpu6050.getAngleX();
  //Serial.print("angleX : ");      Serial.print(angleX);
  angleY = mpu6050.getAngleY();
  //Serial.print("angleY : ");      Serial.print(angleY);
 angleZ = mpu6050.getAngleZ();
  //Serial.print("angleZ : ");      Serial.print(angleZ);
 
angleX = (uint8_t) angleX/10 % 36;
if (angleX <0)
{ 
  angleX = 36- angleX;
}
angleY = (uint8_t) angleY/10 % 36;
if (angleY <0)
{ 
  angleY = 36- angleY;
}

   pitch = TimerSetup.px * sinetbl[angleX]+ TimerSetup.py * sinetbl[angleY];
   roll = TimerSetup.rx *sinetbl[angleX] +TimerSetup.ry*sinetbl[angleY];

   // pitch = TimerSetup.px * sin(angleX)+ TimerSetup.py * sin(angleY);
   // roll = TimerSetup.rx *sin(angleX) +TimerSetup.ry*sin(angleY);

    posTrim =  pitch + abs(roll);
    if (posTrim <0)
      posTrim = 0;
    else if(posTrim > 180)
      posTrim = 180;  
}
