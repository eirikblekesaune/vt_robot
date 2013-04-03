/***************************************************************************/
/* file motor_controll.h						   */
/* 									   */
/* Simple header file for motor_controll and positioning. Uses		   */
/* geared dc motor with hall effect sensor for positioning. 		   */
/* 									   */
/* This file is part of motor_controll.					   */
/* 									   */
/* motor_controll is free software: you can redistribute it and/or modify  */
/* it under the terms of the GNU General Public License as published by	   */
/* the Free Software Foundation, either version 3 of the License, or	   */
/* (at your option) any later version.					   */
/* 									   */
/* motor_controll is distributed in the hope that it will be useful,	   */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of	   */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the	   */
/* GNU General Public License for more details.				   */
/* 									   */
/* You shoould have received a copy of the GNU General Public License	   */
/* along with motor_controll.  If not, see <http://www.gnu.org/licenses/>. */
/***************************************************************************/



/* left and right motor position is taken from the interrupetd variables  */
/* int PosLeft PosRight and should be in increments of  */
/* 1cm when calibrated. */

#include "DualVNH5019MotorShield.h"

#ifndef motorControll_h
#define motorControll_h

#include <Arduino.h>

class motorControll : public DualVNH5019MotorShield {
 private:
    DualVNH5019MotorShield motorShield;//('5','4','6','A0','7','8','12','A1');
    enum dir { DOWN, STOP, UP} ;
    struct motorData{
	dir direction;
	uint8_t foo;
	int setSpeed;
	int setPos;
	int curPos; // global for now
    }motor;
    int i; // general var contains trash
    unsigned char _stopSw;
    unsigned char _sensor;
    
 public:

    motorControll();//(unsigned char motor, unsigned char stopSw, unsigned char sensor);
    motorControll(int iterrupt, int motor, int stopSw, int sensor);
    void init();
    void initMotorShield();
    void initShield();
    void goToPos();
    int getPos();
    int getTarget();
    int getAngle();
    void position();
    void setPos(int _setPos);
    void setSpeed(int);
    void setAngle();//implement later
    void resetPos();
    void stopMotor();
};

#endif
