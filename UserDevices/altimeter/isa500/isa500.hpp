#ifndef _ISA500_HPP_
#define _ISA500_HPP_

#include "altimeter/altimeter.hpp" 
#include "serial/serial.hpp"

class ISA500 : public Altimeter
{
public:
	ISA500(serial::Serial *serialPtr);
	~ISA500()
	{

	}
	/**
	 * @brief          初始化硬件
	 * @param[in]      none
	 * @return  	   none
	*/
	void Init()
	{
		if(!SerialPtr_->isOpen())
		{
			SerialPtr_->open(O_RDONLY);	
		}
	}
	/**
	 * @brief          高度计协议解析
	 * @param[in]      depth_frame: 原生数据指针
	 * @return  	   none
	*/
	void altimeter_data_solve(volatile const uint8_t *depth_frame);
public:
	/**这里高度计为ASCII字符输出，设置ID为107，即ddd.ddm<CR><LF>*/
	static constexpr auto AM_DATA_LENGTH = 9; //高度计上传数据长度
	serial::Serial *SerialPtr_;	//高度计串口指针

};

#endif /* _ISA500_HPP_ */
