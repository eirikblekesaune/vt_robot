#ifndef KOPP_ROBOT_H
#define KOPP_ROBOT_H
#include <Arduino.h>
#include "Motor.h"
#include "KoppAPI.h"


class KoppRobot
{
	public:

		KoppRobot();
		void init();
		void update();
		KoppMotor *motor;
		void GoToParkingPosition();
};



#endif
