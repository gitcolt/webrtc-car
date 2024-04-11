#ifndef MOTOR_STATE_HPP
#define MOTOR_STATE_HPP

struct InputStat {
  unsigned forward : 1;
  unsigned backward : 1;
  unsigned left : 1;
  unsigned right : 1;
};

enum MotorState {
  STOP = 'S',
  FORWARD = 'F',
  REVERSE = 'R',
  STRAIGHT = '^',
  LEFT = '<',
  RIGHT = '>',
};

#endif