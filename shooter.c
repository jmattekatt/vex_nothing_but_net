// Flywheel shooter functions for Vex Nothing But Net 2015-2016
// Author: Joe Mattekatt


#define ERROR_BUFFER_SIZE 3
#define SPEED_CALC_INTERVAL 50

#define PROPORTIONAL_COEFF_DFLT 2
#define PROPORTIONAL_COEFF_MID  3
#define PROPORTIONAL_COEFF_HIGH 5
#define PROPORTIONAL_COEFF_MAX  10

#define TARGET_INDEX_4  4
#define TARGET_INDEX_5  5
#define TARGET_INDEX_8  8
#define TARGET_INDEX_12 12

#define TARGET_INDEX_4_BUTTON   Btn7D
#define TARGET_INDEX_5_BUTTON   Btn7L
#define TARGET_INDEX_8_BUTTON   Btn7U
#define TARGET_INDEX_12_BUTTON  Btn7R
#define EMERGENCY_STOP_BUTTON   Btn8L


typedef struct _ShooterParameters {
  int targetSpeedIndex;
  int targetSpeed;
  int currentSpeed;
  int currentSignal;
} ShooterParameters;

// GLOBAL
ShooterParameters shooterParams = {RESET_VALUE, RESET_VALUE, RESET_VALUE, RESET_VALUE};

void SetTargetSpeedParams() {
  // perdefined speeds and signals for shooter
  static const int signals[SPEED_OPTIONS] = {0, 20, 25, 30, 33, 37, 42, 46, 49, 54, 58, 62, 68, 72, 76, 80};
  static const int speeds[SPEED_OPTIONS]  = {0, 35, 42, 49, 60, 63, 70, 76, 78, 86, 89, 93, 96, 100, 107, 115};

  shooterParams.targetSpeed = speeds[shooterParams.targetSpeedIndex];
  shooterParams.currentSignal = signals[shooterParams.targetSpeedIndex];
}

// Flywheel control
task speedControl() {
  PICoefficients coeffs;
  PICoefficientsInit(coeffs, PROPORTIONAL_COEFF_DFLT, 0.01);

  int errorBuffer[ERROR_BUFFER_SIZE] = {RESET_VALUE, RESET_VALUE, RESET_VALUE};
  PIErrors errors;
  PIErrorsInit(errors);

  while (true) {
    // Set shooter parameters and store in global shooterParams
    SetTargetSpeedParams();

    // Get actual speed of shooter wheel
    SensorValue[shooterEncoder] = RESET_VALUE;
    wait1Msec(SPEED_CALC_INTERVAL);
    shooterParams.currentSpeed = SensorValue[shooterEncoder];

    PIErrorCalculator(shooterParams.targetSpeed, shooterParams.currentSpeed, errors);
    float avgError = MovingAverage(errorBuffer, ERROR_BUFFER_SIZE, errors.error);

    // Adjust proportional gain
    if(avgError < 0) {
      coeffs.proportionalCoeff = PROPORTIONAL_COEFF_DFLT;
    } else {
      if (avgError <= 1) {
        coeffs.proportionalCoeff = PROPORTIONAL_COEFF_MID;
      } else if (avgError <= 2) {
        coeffs.proportionalCoeff = PROPORTIONAL_COEFF_HIGH;
      } else {
        coeffs.proportionalCoeff = PROPORTIONAL_COEFF_MAX;
      }
    }

    shooterParams.currentSignal = PICalculator(errors, coeffs) + shooterParams.currentSignal;

    // Safety to prevent back drive on inertial flywheel
    if (shooterParams.currentSignal < RESET_VALUE) {
      shooterParams.currentSignal = RESET_VALUE;
    }

    // Control motors
    motor[shoot1] = shooterParams.currentSignal;
    motor[shoot2] = shooterParams.currentSignal;
    motor[shoot3] = shooterParams.currentSignal;
    motor[shoot4] = shooterParams.currentSignal;
  }
}

void SpeedTargetControl(ToggleButton &increment, ToggleButton &decrement) {
  if (increment.state == ACTIVE) {
    increment.state = INACTIVE;
    if (shooterParams.targetSpeedIndex < SPEED_OPTIONS-1) {
      shooterParams.targetSpeedIndex++;
    }
  }
  if (decrement.state == ACTIVE) {
    decrement.state = INACTIVE;
    if (shooterParams.targetSpeedIndex > RESET_VALUE) {
      shooterParams.targetSpeedIndex--;
    }
  }

  // Quick target control
  if (vexRT[TARGET_INDEX_5_BUTTON] == PRESSED) {
    shooterParams.targetSpeedIndex = TARGET_INDEX_5;
  } else if (vexRT[TARGET_INDEX_8_BUTTON] == PRESSED) {
    shooterParams.targetSpeedIndex = TARGET_INDEX_8;
  } else if (vexRT[TARGET_INDEX_12_BUTTON] == PRESSED) {
    shooterParams.targetSpeedIndex = TARGET_INDEX_12;
  } else if (vexRT[TARGET_INDEX_4_BUTTON] == PRESSED) {
    shooterParams.targetSpeedIndex = TARGET_INDEX_4;
  }

  // Safety stop
  if (vexRT[EMERGENCY_STOP_BUTTON] == PRESSED) {
    shooterParams.targetSpeedIndex = RESET_VALUE;
    shooterParams.currentSignal = RESET_VALUE;
  }
}
