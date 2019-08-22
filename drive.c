// Drive functions for Vex Nothing But Net 2015-2016
// Author: Joe Mattekatt


#define TANK_LEFT_CHAN  Ch3
#define TANK_RIGHT_CHAN Ch2

#define SPEED_BUFFER_SIZE   10
#define E_BRAKE_SPEED       10
#define DEGREES_TO_ROTATION 3


word FilterAnalogChannel(TVexJoysticks channel) {
  // Analog Re-mapping
  static const word driveMap[128] =
  {0,0,0,0,0,0,0,0,0,0,
  20,20,21,21,22,22,23,23,24,24,
  25,25,26,26,27,27,28,28,29,29,
  30,30,31,31,32,32,33,33,34,34,
  35,35,36,36,37,37,38,38,39,39,
  40,40,41,41,42,42,43,43,44,44,
  45,45,46,46,47,47,48,48,49,49,
  50,50,51,51,52,52,53,53,54,54,
  55,55,56,56,57,57,58,58,59,59,
  60,60,61,62,63,64,65,66,67,68,
  69,70,71,72,73,74,75,76,77,78,
  79,80,81,82,83,84,85,86,87,88,
  89,90,91,92,94,96,127,127};

  int filteredValue = driveMap[abs(vexRT[channel])];
  return filteredValue * sgn(vexRT[channel]);
}

void Drive(word leftSpeed, word rightSpeed){

  // Mecanum drive motion equations
  motor[driveL]  = leftSpeed;
  motor[driveLF] = leftSpeed;
  motor[driveRF] = rightSpeed;
  motor[driveR]  = rightSpeed;
}

void JoystickDrive() {
  static int leftSpeedBuffer[SPEED_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  static int rightSpeedBuffer[SPEED_BUFFER_SIZE] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

  word avgLeftSpeed = MovingAverage(leftSpeedBuffer, SPEED_BUFFER_SIZE, FilterAnalogChannel(TANK_LEFT_CHAN));
  word avgRightpeed = MovingAverage(rightSpeedBuffer, SPEED_BUFFER_SIZE, FilterAnalogChannel(TANK_RIGHT_CHAN));

  Drive(avgLeftSpeed, avgRightpeed);
}

void EBrake(word previousSpeed) {
  word brakingSpeed = -sgn(previousSpeed) * E_BRAKE_SPEED;

  Drive(brakingSpeed, brakingSpeed);
  wait1Msec(50);

  Drive(HARD_STOP, HARD_STOP);
  wait1Msec(5);
}

void AutoDrive(int steps, bool intakeBalls, int timeOut) {
  static const PICoefficients coeffs = {0.45, 0.0003};

  PIErrors rightErrors, leftErrors;
  PIErrorsInit(rightErrors);
  PIErrorsInit(leftErrors);

  ToggleButton driveIntakeToggle, driveFeedToggle;
  driveIntakeToggle.state = ACTIVE;
  driveFeedToggle.state = INACTIVE;

  //Reset encoders
  SensorValue[leftEncoder]  = RESET_VALUE;
  SensorValue[rightEncoder] = RESET_VALUE;
  clearTimer(T1);

  int absSteps = abs(steps);
  int rightSpeed, leftSpeed;

  while (abs(SensorValue[rightEncoder]) < absSteps && abs(SensorValue[leftEncoder]) < absSteps) {
    PIErrorCalculator(steps, SensorValue[rightEncoder], rightErrors);
    PIErrorCalculator(steps, SensorValue[leftEncoder], leftErrors);

    rightSpeed = PICalculator(rightErrors, coeffs);
    leftSpeed  = PICalculator(leftErrors, coeffs);

    motor[driveR]  = rightSpeed;
    motor[driveRF] = rightSpeed;
    motor[driveLF] = leftSpeed;
    motor[driveL]  = leftSpeed;

    if (intakeBalls) {
      // Intake and Feeder controls
      IntakeControl(driveIntakeToggle);
      FeederControl(driveFeedToggle);
    }

    CHECK_TIMEOUT(timeOut);
  }

  //Stop intaking
  motor[feed]   = HARD_STOP;
  motor[intake] = HARD_STOP;

  EBrake(rightSpeed);
}

void AutoTurn(int degrees, int timeOut) {
  static const PICoefficients coeffs = {0.4, 0.0004};

  PIErrors errors;
  PIErrorsInit(errors);

  //Reset encoders
  SensorValue[leftEncoder]  = RESET_VALUE;
  SensorValue[rightEncoder] = RESET_VALUE;
  clearTimer(T1);

  int avgPosition, speed;
  int absDegrees = abs(degrees) * DEGREES_TO_ROTATION;

  while(avgPosition < absDegrees){
    avgPosition = (abs(SensorValue[leftEncoder]) + abs(SensorValue[rightEncoder]))/2;
    PIErrorCalculator(absDegrees, avgPosition, errors);
    speed = PICalculator(errors, coeffs);

    motor[driveR]  = speed*sgn(degrees);
    motor[driveRF] = speed*sgn(degrees);
    motor[driveLF] = -speed*sgn(degrees);
    motor[driveL]  = -speed*sgn(degrees);

    CHECK_TIMEOUT(timeOut);
  }

  EBrake(speed);
}
