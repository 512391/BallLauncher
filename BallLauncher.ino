#include <MeMegaPi.h>

const int ROTATION_SPEED = 75;
const int LAUNCH_SPEED = 255;
const int RELOAD_SPEED = 100;
const int MAX_AMMO_CAPACITY = 2;


const int RELOAD_ONE_DELAY = 300;
const int RELOAD_ROTATION_DELAY = 500;
const int LAUNCH_FULL_ROTATION_DELAY = 900;
const int INTERMEDIATE_DELAY = 1000;

MeUltrasonicSensor ultraSensor(PORT_8);
MeEncoderOnBoard RotationMotor(SLOT3);
MeEncoderOnBoard LaunchMotor(SLOT4);
MeEncoderOnBoard ReloadMotor(SLOT1);

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
  LaunchMotor.setMotorPwm(LAUNCH_SPEED);
  delay(LAUNCH_FULL_ROTATION_DELAY);
  LaunchMotor.setMotorPwm(0);
}

void reload()
{
  RotationMotor.setMotorPwm(ROTATION_SPEED);
  delay(RELOAD_ROTATION_DELAY);
  RotationMotor.setMotorPwm(0);

  delay(INTERMEDIATE_DELAY);

  if(currentAmmo == 0)
  {
    ReloadMotor.setMotorPwm(RELOAD_SPEED);
    delay(3*RELOAD_ONE_DELAY);
    currentAmmo = MAX_AMMO_CAPACITY;
    ReloadMotor.setMotorPwm(0);
  }
  else
  {
    currentAmmo--;
    ReloadMotor.setMotorPwm(-RELOAD_SPEED);
    delay(RELOAD_ONE_DELAY);
    ReloadMotor.setMotorPwm(0);
  }

  delay(INTERMEDIATE_DELAY);

  RotationMotor.setMotorPwm(-ROTATION_SPEED);
  delay(RELOAD_ROTATION_DELAY*1.2);
  RotationMotor.setMotorPwm(0);
}

void rotateAndLaunch()
{
  launch();
  delay(INTERMEDIATE_DELAY);
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
