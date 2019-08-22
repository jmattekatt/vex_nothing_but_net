// Autonomous routines for Vex Nothing But Net 2015-2016
// Author: Joe Mattekatt


#define PROGRAMMING_SKILLS              1000
#define PROGRAMMING_SKILLS_TARGET_INDEX 8
#define PROGRAMMING_SKILLS_SHOT_TIME    22000
#define AUTONOMOUS_TARGET_INDEX         12
#define AUTONOMOUS_SHOT_TIME            9000

#define DRIVE_TIMEOUT_MS  5000
#define LIFT_TIMEOUT_MS   3000

#define WAIT_250_MS wait1Msec(250)


void ProgrammingSkills() {
  shooterParams.targetSpeedIndex = PROGRAMMING_SKILLS_TARGET_INDEX;

  clearTimer(T1);
  ToggleButton driveIntakeToggle, driveFeedToggle;
  driveIntakeToggle.state = ACTIVE;
  driveFeedToggle.state   = ACTIVE;

  while(time1[T1] < PROGRAMMING_SKILLS_SHOT_TIME){
    IntakeControl(driveIntakeToggle);
    FeederControl(driveFeedToggle);
  }

  //Stop feeding
  motor[feed] = HARD_STOP;

  for (int i = PROGRAMMING_SKILLS_TARGET_INDEX; i > 0; --i) {
    shooterParams.targetSpeedIndex = i;
    WAIT_250_MS;
  }

  stopTask(speedControl);

  //Drive across
  AutoTurn(-95, DRIVE_TIMEOUT_MS);
  WAIT_250_MS;
  AutoDrive(2100, true, DRIVE_TIMEOUT_MS);
  WAIT_250_MS;
  AutoTurn(85, DRIVE_TIMEOUT_MS);
  WAIT_250_MS;

  startTask(speedControl);

  shooterParams.targetSpeedIndex = PROGRAMMING_SKILLS_TARGET_INDEX;
  clearTimer(T1);

  while(time1[T1] < PROGRAMMING_SKILLS_SHOT_TIME){
    IntakeControl(driveIntakeToggle);
    FeederControl(driveFeedToggle);
  }
  motor[feed]   = HARD_STOP;
  motor[intake] = HARD_STOP;

  for (int i = PROGRAMMING_SKILLS_TARGET_INDEX; i > 0; --i) {
    shooterParams.targetSpeedIndex = i;
    WAIT_250_MS;
  }
}

void NormalAutonomousRoutine() {
  shooterParams.targetSpeedIndex = AUTONOMOUS_TARGET_INDEX;

  clearTimer(T1);
  ToggleButton driveIntakeToggle, driveFeedToggle;
  driveIntakeToggle.state = ACTIVE;
  driveFeedToggle.state   = ACTIVE;

  while(time1[T1] < AUTONOMOUS_SHOT_TIME){
    IntakeControl(driveIntakeToggle);
    FeederControl(driveFeedToggle);
  }

  //Stop feeding
  motor[feed]   = HARD_STOP;
  motor[intake] = HARD_STOP;

  for (int i = AUTONOMOUS_TARGET_INDEX; i > 0; --i) {
    shooterParams.targetSpeedIndex = i;
    WAIT_250_MS;
  }
}

void ChooseAutoRoutine() {
  startTask(speedControl);

  if (SensorValue[autonSelector] > PROGRAMMING_SKILLS) {
    ProgrammingSkills();
  } else {
    NormalAutonomousRoutine();
  }

  stopTask(speedControl);
}
