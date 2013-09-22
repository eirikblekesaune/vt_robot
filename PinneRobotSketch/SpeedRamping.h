#ifndef SpeedRamping_h
#define SpeedRamping_h

#include <Arduino.h>

class SpeedRamping {
  public:
  SpeedRamping();
  void SetMinSpeed(int speed);
  void SetMaxSpeed(int speed);
  void SetSpeedScaling(double speedScaling);
  void SetRampUp(double value);
  void SetRampDown(double value);
  
  int GetMinSpeed() {return static_cast<int>(_minSpeed); };
  int GetMaxSpeed() {return static_cast<int>(_maxSpeed); };
  double GetSpeedScaling() {return _speedScaling; };
  double GetRampUp() {return _rampUp; };
  double GetRampDown() {return _rampDown; };
  
  private:
  double _minSpeed;
  double _maxSpeed;
  double _speedScaling;
  double _rampUp;
  double _rampDown;
  int _halfwayPosition;
  int _startPosition;
  int _endPosition;
  boolean _halfwayReached;
};


#endif
