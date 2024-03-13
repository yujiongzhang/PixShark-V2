#ifndef _CRAWLER_HPP_
#define _CRAWLER_HPP_

#include "common.hpp"


/* ----------------------- Data Struct ------------------------------------- */
#pragma pack(push, 1)


#pragma pack(pop)
/* ----------------------- extern Function ----------------------------------- */


class Crawler
{
public:
    Crawler(/* args */)
    {

    }
    virtual ~Crawler(){};
    virtual void set_expect_speed(int16_t speed_set) = 0;
    virtual int16_t get_rpm() = 0;

protected:
   
    

};


#endif /* _CRAWLER_HPP_ */
