#ifndef ROBOTICS_H
#define ROBOTICS_H
#include <iostream>

namespace VT {
	namespace Robotics {
		typedef int16_t speed_t;
		typedef int8_t direction_t;
		typedef int32_t position_t;
		
		//Senor classes
		class Sensor {
		};
		
		class PositionSensor : public Sensor
		{
		};

		//Motordirver Interfaces
		class Bidirectional {
		public:
			virtual void SetDirection(direction_t direction) = 0;
			virtual direction_t GetDirection() {return _direction;};
		private:
			direction_t _direction;
		};

		class Positionable {
		public:
			Positionable(PositionSensor* sensor) : _positionSensor(sensor) {};
			virtual void SetTargetPositon(position_t targetPosition) {_targetPosition = targetPosition;};
		protected:
			PositionSensor *_positionSensor;
			position_t _targetPosition;
		};


		//Motor classes
		class Motor {
		public:
			Motor() {std::cout << "New Motor\n";};
			virtual void Update() {};
		};

		class DCMotor : public Motor
		{
			DCMotor() {std::cout << "New DCMotor\n";};
		};

		class StepperMotor : public Motor
		{
			StepperMotor() {std::cout << "New StepperMotor\n";};
		};
		//MotorDriver classes
		class MotorDriver {
		public:
			MotorDriver() {std::cout << "New MotorDriver\n";};
			virtual void SetSpeed() = 0;
			virtual speed_t GetSpeed() {return _speed;};
		protected:
			speed_t _speed;
		};

		class DCMotorDriver : public MotorDriver
		{
		public:	
			DCMotorDriver() {std::cout << "New DCMotorDriver\n";};
			virtual void SetSpeed(speed_t speed) {_speed = speed;};
		};

		class VNH5019Driver : public DCMotorDriver
		{
		public:	
			VNH5019Driver() {std::cout << "New VNH509Driver\n";};
		};


	};
};
#endif

