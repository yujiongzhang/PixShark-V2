#ifndef _THRUSTER_HPP_
#define _THRUSTER_HPP_

#include "common.hpp"



class Thruster
{
public:
    Thruster()
    {

    }
    virtual ~Thruster(){};
    virtual void set_expect_speed(int32_t speed_set) = 0;
    virtual int16_t get_rpm() = 0;
protected:

};


#endif /* _THRUSTER_HPP_ */
