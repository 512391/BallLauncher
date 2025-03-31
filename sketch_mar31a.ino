#include <MeMegaPi.h>

const int ROTATION_SPEED = 75;
const int LAUNCH_SPEED = 200;
const int RELOAD_SPEED = 50;
const int INTERMEDIATE_DELAY = 1000;
const int MAX_AMMO_CAPACITY = 3;


const int RELOAD_BALL_INDEX_POSITION[4] = {150, 100, 50, 0};
const int RELOAD_ROTATION_POSITION = 50;

const int STANDARD_ROTATION_POSITION = 0;

const int STANDARD_LAUNCHER_ROTATION = 0;

MeUltrasonicSensor ultraSensor(PORT_8);
MeEncoderOnBoard RotationMotor(SLOT1);
MeEncoderOnBoard LaunchMotor(SLOT2);
MeEncoderOnBoard ReloadMotor(SLOT4);

int currentAmmo = MAX_AMMO_CAPACITY;

void isr_process_encoder1(void)
{
  if(digitalRead(RotationMotor.getPortB()) == 0)
  {
    RotationMotor.pulsePosMinus();
  }
  else
  {
    RotationMotor.pulsePosPlus();
  }
}

void isr_process_encoder2(void)
{
  if(digitalRead(LaunchMotor.getPortB()) == 0)
  {
    LaunchMotor.pulsePosMinus();
  }
  else
  {
    LaunchMotor.pulsePosPlus();
  }
}

void isr_process_encoder3(void)
{
  if(digitalRead(ReloadMotor.getPortB()) == 0)
  {
    ReloadMotor.pulsePosMinus();
  }
  else
  {
    ReloadMotor.pulsePosPlus();
  }
}


void launch()
{
  LaunchMotor.moveTo(STANDARD_LAUNCHER_ROTATION+180, LAUNCH_SPEED);
  LaunchMotor.moveTo(STANDARD_LAUNCHER_ROTATION, LAUNCH_SPEED);
}

void reload()
{
  RotationMotor.moveTo(RELOAD_ROTATION_POSITION, -ROTATION_SPEED);

  if(currentAmmo == 0)
  {
    ReloadMotor.moveTo(RELOAD_BALL_INDEX_POSITION[3], RELOAD_SPEED);
    currentAmmo = MAX_AMMO_CAPACITY;
  }
  else
  {
    currentAmmo--;
    ReloadMotor.moveTo(RELOAD_BALL_INDEX_POSITION[currentAmmo], RELOAD_SPEED);
  }
}

void rotateAndLaunch()
{
  RotationMotor.moveTo(STANDARD_ROTATION_POSITION, -ROTATION_SPEED);
  launch();
  reload();
}

void setup()
{
  attachInterrupt(RotationMotor.getIntNum(), isr_process_encoder1, RISING);
  attachInterrupt(LaunchMotor.getIntNum(), isr_process_encoder2, RISING);
  attachInterrupt(ReloadMotor.getIntNum(), isr_process_encoder3, RISING);
  Serial.begin(115200);
  
  //Set PWM 8KHz
  TCCR1A = _BV(WGM10);
  TCCR1B = _BV(CS11) | _BV(WGM12);

  TCCR2A = _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS21);

  RotationMotor.setPulse(7);
  LaunchMotor.setPulse(7);
  RotationMotor.setRatio(26.9);
  LaunchMotor.setRatio(26.9);
  RotationMotor.setPosPid(1.8, 0, 1.2);
  LaunchMotor.setPosPid(1.8, 0, 1.2);
  RotationMotor.setSpeedPid(0.18, 0, 0);
  LaunchMotor.setSpeedPid(0.18, 0, 0);
}


void loop() {
  // put your main code here, to run repeatedly:
  Serial.print("Distance : ");
  Serial.print(ultraSensor.distanceCm());
  Serial.println(" cm");

  while(true)
  {
    if(ultraSensor.distanceCm() < 30)
    {
      rotateAndLaunch();
    }
  }
}
