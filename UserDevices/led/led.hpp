#ifndef _LED_HPP_
#define _LED_HPP_


#include "common.hpp"



class LED
{
public:
    LED(){};
    virtual ~LED(){};
    virtual void on(uint8_t brightness) = 0;
    virtual void off(void) = 0;
private:

};

#endif /* _LED_HPP_ */
