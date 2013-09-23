#include "SpeedRamping.h"
#include "kRISE_TABLE.h"
const unsigned long SpeedRamping::DEFAULT_CALC_INTERVAL = 50;

SpeedRamping::SpeedRamping(int minSpeed, int maxSpeed) :
  _minSpeed(minSpeed),
  _maxSpeed(maxSpeed),
  _calcInterval(DEFAULT_CALC_INTERVAL),
  _halfwayReached(false),
  _startPosition(0),
  _endPosition(0),
  _direction(RISING_RAMP)
{
}

void SpeedRamping::Start(int startPosition, int endPosition, int duration)
{
  //calculate halfway point
  _startPosition = startPosition;
  _endPosition = endPosition;
  _halfwayPosition = ((_endPosition - _startPosition) / 2) + _startPosition;
  _speedRange = abs(_maxSpeed - _minSpeed);
  if(_startPosition <= _endPosition)
  {
    _direction = RISING_RAMP;
  } else {
    _direction = FALLING_RAMP;
  }
  _riseFactor = 1.01;
  _fallFactor = 0.99;
  _currentFloatIndex = 0.0;
  _halfwayReached = false;
}

boolean SpeedRamping::Calculate(int currPosition)
{
  unsigned long nowTime = millis();
  if((nowTime - _lastCalcTime) >= _calcInterval)
  {
    if(_direction == RISING_RAMP)
    {
      if(currPosition >= _halfwayPosition)
      {
        _halfwayReached = true;
      }
    } else {
      if(currPosition <= _halfwayPosition)
      {
        _halfwayReached = true;
      }
    }
    if(!_halfwayReached)
    {
      _currentFloatIndex = min(1.0, _currentFloatIndex * _riseFactor);
    } else {
      _currentFloatIndex = max(0.0, _currentFloatIndex * _fallFactor);
    }
    _lastCalcTime = nowTime;
    return true;
  } else {
    return false;
  }
}

void SpeedRamping::Stop()
{

}

int SpeedRamping::GetCurrentValue()
{
  int index;
  float result;
  index = static_cast<int>(floor(_currentFloatIndex * TABLE_SIZE));
  result = (kRampTable[index] * _speedRange * _speedScaling) + _minSpeed;
  return static_cast<int>(result);
}

void SpeedRamping::SetMinSpeed(int speed)
{
  _minSpeed = speed;
}

void SpeedRamping::SetMaxSpeed(int speed)
{
  _maxSpeed = speed;
}

void SpeedRamping::SetSpeedScaling(float speedScaling)
{
  _speedScaling = constrain(speedScaling, 0.0, 1.0);
}

void SpeedRamping::SetRampUp(float value)
{
  _rampUp = value;
}

void SpeedRamping::SetRampDown(float value)
{
  _rampDown = value;
}

