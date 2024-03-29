#pragma config(Sensor, in1,    autonSelector,  sensorPotentiometer)
#pragma config(Sensor, dgtl1,  shooterEncoder, sensorRotation)
#pragma config(Sensor, dgtl2,  bottomLimit,    sensorTouch)
#pragma config(Sensor, dgtl3,  topLimit,       sensorTouch)
#pragma config(Sensor, dgtl4,  leftEncoder,    sensorQuadEncoder)
#pragma config(Sensor, dgtl6,  rightEncoder,   sensorQuadEncoder)
#pragma config(Sensor, dgtl8,  middleLimit,    sensorTouch)
#pragma config(Motor,  port1,  intake,         tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,  driveLF,        tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port3,  shoot1,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port4,  driveL,         tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port5,  shoot2,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port6,  shoot3,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port7,  driveR,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port8,  shoot4,         tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port9,  driveRF,        tmotorVex393_MC29, openLoop, reversed)
#pragma config(Motor,  port10, feed,           tmotorVex393_HBridge, openLoop)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*            Description: Competition Code for VEX EDR                      */
/*                                                                           */
/*---------------------------------------------------------------------------*/

// Robot Code for Vex Nothing But Net 2015-2016
// Author: Joe Mattekatt

// This code is for the VEX cortex platform
#pragma platform(VEX2)

// Select Download method as "competition"
#pragma competitionControl(Competition)

//Main competition background code
#include "Vex_Competition_Includes.c"

#include "utilities.c"
#include "shooter.c"
#include "intake.c"
#include "drive.c"
#include "autonomous.c"

#define DEBUG
#define SHOW_BATTERY

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                          Pre-Autonomous Functions                         */
/*                                                                           */
/*---------------------------------------------------------------------------*/

void pre_auton()
{
  bStopTasksBetweenModes = true;
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              Autonomous Task                              */
/*                                                                           */
/*  This task is used to control the robot during the autonomous phase of    */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

task autonomous()
{
  // Use selection potentiometer to select approriate autonomous routine
  ChooseAutoRoutine();
}

/*---------------------------------------------------------------------------*/
/*                                                                           */
/*                              User Control Task                            */
/*                                                                           */
/*  This task is used to control the robot during the user control phase of  */
/*  a VEX Competition.                                                       */
/*                                                                           */
/*---------------------------------------------------------------------------*/

task usercontrol()
{
  // Toggle buttons initialize
  ToggleButton intakeToggle, feedToggle, incrementTargetToggle, decrementTargetToggle;
  ToggleButtonInit(intakeToggle, Btn5U);
  ToggleButtonInit(feedToggle, Btn8R);
  ToggleButtonInit(incrementTargetToggle, Btn8U);
  ToggleButtonInit(decrementTargetToggle, Btn8D);

  //Start speed control task
  startTask(speedControl);

  while (true)
  {
    // This is the main execution loop for the user control program.
    // Each time through the loop the program should update motor + actuator
    // values based on feedback from the joysticks.

    // Update toggle states
    ToggleButtonUpdate(intakeToggle);
    ToggleButtonUpdate(feedToggle);
    ToggleButtonUpdate(incrementTargetToggle);
    ToggleButtonUpdate(decrementTargetToggle);

    // Target speed control
    SpeedTargetControl(incrementTargetToggle, decrementTargetToggle);

    // Intake and Feeder controls
    IntakeControl(intakeToggle);
    FeederControl(feedToggle);

    // Drive control
    JoystickDrive();

    #ifdef SHOW_BATTERY
      DisplayBatteryLevels();
    #endif

    #ifdef DEBUG
      DisplayRow(1, "%d %t %d", shooterParams.targetSpeed, shooterParams.currentSpeed);
    #endif

    LOOP_DELAY;
  }
}
