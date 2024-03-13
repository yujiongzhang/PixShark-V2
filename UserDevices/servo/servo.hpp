#ifndef _SERVO_HPP_
#define _SERVO_HPP_


#include "common.hpp"

class Servo
{
public:
    Servo(){};
    virtual ~Servo(){};
    virtual void set_servo_angle(int16_t angle_set) = 0;
    virtual int16_t get_servo_angle() = 0;
private:

};



#endif /* _SERVO_HPP_ */
