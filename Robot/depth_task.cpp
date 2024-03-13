/**
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : depth_task.cpp
 * @brief     : 解析高度计发送过来的数据
 * @note
 * @history   :
 *  Version     Date            Author          Note
 *  V1.0.0    2023-09-28       Hao Lion        1. <note>
 *  V2.0.0    2023-12-11       Hao Lion        1. 修改为ros2形式
 *******************************************************************************
 * @verbatim :
 *==============================================================================
 *
 *
 *==============================================================================
 * @endverbatim :
 *****************************Copyright (c) 2023  ZJU****************************
 */



#include "common_inc.h"
#include "depth/keller/keller.hpp"
#include <std_msgs/msg/Float32.h>

static osThreadId depth_task_handle;

//The buffer is provided with double capacity to prevent DMA transmission from exceeding the limit
uint8_t depth_rx_buf[2 * KELLER::DEPTH_DATA_LENGTH];
float external_temperature = 0.0f;

/**
 * @brief          请求深度计数据
 */
static void depth_request(void * argument)
{
	if(argument != nullptr)
	{
		KELLER* kellerPtr = (KELLER*)argument;
		kellerPtr->request_data();
		osDelay(1000/DEPTH_FREQUNCY);
	}
}

static void depth_task(void *argument)
{
	//serial init
	serial::Serial depth_serial(&huart6);
	depth_serial.setTimeout(osWaitForever);
	depth_serial.open();
	if (!depth_serial.isOpen())
		configASSERT(0);
	KELLER keller(&depth_serial, 0x01);
	keller.Init();
	//depth_data request
	osTimerId_t depth_requestHandle;
	const osTimerAttr_t depth_request_attributes = {
		.name = "depth_request"
	};
	depth_requestHandle = osTimerNew(depth_request, osTimerPeriodic, &keller, &depth_request_attributes);
	osTimerStart(depth_requestHandle, 1000/DEPTH_FREQUNCY);
	//micro-ros
	rcl_ret_t ret = 0;
	std_msgs__msg__Float32 depth_msg;
	for (;;) {
		//blocing unitl get frame header
		uint8_t ret = depth_serial.read(depth_rx_buf, 1);
		if (ret > 0 && depth_rx_buf[0] == 0x01) {
			//get remain data
			ret = depth_serial.read(depth_rx_buf + 1, KELLER::DEPTH_DATA_LENGTH - 1);
			if (ret == KELLER::DEPTH_DATA_LENGTH - 1)
				keller.depth_data_solve(depth_rx_buf);
			depth_msg.data = keller.get_depth_data();
			external_temperature = keller.get_temperature_data();
			ret = rcl_publish(&depth_publisher, &depth_msg, NULL);
			memset(depth_rx_buf, 0, 2 * KELLER::DEPTH_DATA_LENGTH);
		}
	}
}

const float get_external_temperature(void)
{
	return external_temperature;
}


void depth_task_start(void)
{
	const osThreadAttr_t depthTask_attributes = {
		.name           = (char *)"depth",
		.stack_size     = 256 * 4,
		.priority       = osPriorityRealtime,
	};
	depth_task_handle = osThreadNew(depth_task, NULL, &depthTask_attributes);
}