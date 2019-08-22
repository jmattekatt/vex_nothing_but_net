// End-effector functions for Vex Nothing But Net 2015-2016
// Author: Joe Mattekatt


#define RELEASE_INTAKE_BALLS  Btn5D
#define SHOOT_FEEDER_BALLS    Btn6U
#define RELEASE_FEEDER_BALLS  Btn6D

#define AUTOMATIC_FEEDING_SPEED       80
#define MOVE_BALLS_TOWARDS_SHOOTER    FULL_SPEED_BACKWARD
#define MOVE_BALLS_AWAY_FROM_SHOOTER  FULL_SPEED_FORWARD


void IntakeControl(ToggleButton &intakeToggle) {
  if (vexRT[RELEASE_INTAKE_BALLS] == PRESSED) {
    motor[intake] = MOVE_BALLS_AWAY_FROM_SHOOTER;
  } else if(intakeToggle.state == ACTIVE) {
    motor[intake] = MOVE_BALLS_TOWARDS_SHOOTER;
  } else {
    motor[intake] = HARD_STOP;
  }
}

void FeederControl(ToggleButton &feedToggle) {
  static bool firstBallInFeeder, secondBallInFeeder;

  if (feedToggle.state == INACTIVE) {
    if (vexRT[SHOOT_FEEDER_BALLS] == PRESSED) {
      motor[feed] = MOVE_BALLS_TOWARDS_SHOOTER;
    } else if(vexRT[RELEASE_FEEDER_BALLS] == PRESSED) {
      motor[feed] = MOVE_BALLS_AWAY_FROM_SHOOTER;
    } else {
      // Automatic feeding controls
      if (SensorValue[bottomLimit] == PRESSED && SensorValue[middleLimit] == RELEASED && SensorValue[topLimit] == RELEASED) {
        firstBallInFeeder = true;
      } else if (firstBallInFeeder && SensorValue[middleLimit] == PRESSED) {
        firstBallInFeeder = false;
      }

      if (SensorValue[bottomLimit] == PRESSED && SensorValue[middleLimit] == PRESSED && SensorValue[topLimit] == RELEASED) {
        secondBallInFeeder = true;
      } else if(secondBallInFeeder && SensorValue[topLimit] == PRESSED) {
        secondBallInFeeder = false;
      }

      if (firstBallInFeeder || secondBallInFeeder) {
        motor[feed] = AUTOMATIC_FEEDING_SPEED;
      } else {
        motor[feed] = HARD_STOP;
      }
    }
  } else if (feedToggle.state == ACTIVE) {
    if (shooterParams.currentSpeed >= shooterParams.targetSpeed) {
      motor[feed] = MOVE_BALLS_TOWARDS_SHOOTER;
    } else {
      motor[feed] = HARD_STOP;
    }
  }
}
