/**
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : gps_task.cpp
 * @brief     : 解析GPS发送过来的数据
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
#include "gps/oemr980/oemr980.hpp"
#include <underwater_robot_msgs/msg/gps.h>

static osThreadId gps_task_handle;

#define GPS_DATA_LENGTH 100

//The buffer is provided with double capacity to prevent DMA transmission from exceeding the limit
uint8_t gps_rx_buf[2 * GPS_DATA_LENGTH];

static void gps_task(void *argument)
{
	//serial init
	serial::Serial gps_serial(&huart2);

	gps_serial.setTimeout(osWaitForever);
	gps_serial.open();
	if (!gps_serial.isOpen())
		configASSERT(0);
	OEMR980 oemr980(&gps_serial);
	//micro-ros
	rcl_ret_t ret = 0;
	underwater_robot_msgs__msg__Gps gps_msg;
    gps_data_t gps_data;
	for (;;) {
		//blocing unitl buffer is fill or get the <LF>
		gps_serial.readline(gps_rx_buf, 2 * GPS_DATA_LENGTH);
		oemr980.gps_data_solve(gps_rx_buf);
		gps_data = oemr980.get_gps_data();
		gps_msg.header.stamp.sec = rmw_uros_epoch_millis();
		gps_msg.header.stamp.nanosec = rmw_uros_epoch_nanos();
		gps_msg.header.frame_id.data = (char*)"base_link";
        gps_msg.latitude = gps_data.latitude;
        gps_msg.longitude = gps_data.longitude;
		ret = rcl_publish(&gps_publisher, &gps_msg, NULL);
		memset(gps_rx_buf, 0, 2 * GPS_DATA_LENGTH);
	}
}

void gps_task_start(void)
{
	const osThreadAttr_t gpsTask_attributes = {
		.name           = (char *)"gps",
		.stack_size     = 256 * 4,
		.priority       = osPriorityRealtime,
	};

	gps_task_handle = osThreadNew(gps_task, NULL, &gpsTask_attributes);
}