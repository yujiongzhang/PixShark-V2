/**
 *****************************Copyright (c) 2023  ZJU****************************
 * @file      : imu_task.cpp
 * @brief     : 接收和解析IMU发送过来的数据
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
#include "imu/lpms/lpms.hpp"
#include "user_lib.h"
#include <sensor_msgs/msg/imu.h>

static osThreadId imu_task_handle;


//The buffer is provided with double capacity to prevent DMA transmission from exceeding the limit
uint8_t imu_rx_buf[2 * LPMS::IMU_DATA_LENGTH];

static void imu_task(void *argument)
{
	//serial init
	serial::Serial imu_serial(&huart3);

	imu_serial.setTimeout(osWaitForever);
	imu_serial.open();
	if (!imu_serial.isOpen())
		configASSERT(0);
	LPMS lpms(&imu_serial);
	lpms.Init();
	imu_data_t imu_data;
	//micro-ros
	rcl_ret_t ret = 0;
	sensor_msgs__msg__Imu imu_msg;
	memset(&imu_msg, 0, sizeof(imu_msg));
	for (;;) {
		//blocing unitl get frame header
		uint8_t ret = imu_serial.read(imu_rx_buf, 1);
		if (ret > 0 && imu_rx_buf[0] == 0x3A) {
			//get remain data
			ret = imu_serial.read(imu_rx_buf + 1, LPMS::IMU_DATA_LENGTH - 1);
			if (ret == LPMS::IMU_DATA_LENGTH - 1)
				lpms.imu_data_solve(imu_rx_buf);
			imu_data = lpms.get_imu_data();
			//fill data
			imu_msg.header.stamp.sec = rmw_uros_epoch_millis();
			imu_msg.header.stamp.nanosec = rmw_uros_epoch_nanos();
			imu_msg.header.frame_id.data = (char*)"imu_link";

			imu_msg.orientation.w = imu_data.quaternion[0];
			imu_msg.orientation.x = imu_data.quaternion[1];
			imu_msg.orientation.y = imu_data.quaternion[2];
			imu_msg.orientation.z = imu_data.quaternion[3];
			
			imu_msg.angular_velocity.x = imu_data.Gyrol_x;
			imu_msg.angular_velocity.y = imu_data.Gyrol_y;
			imu_msg.angular_velocity.z = imu_data.Gyrol_z;

			imu_msg.linear_acceleration.x = imu_data.Acc_x;
			imu_msg.linear_acceleration.y = imu_data.Acc_y;
			imu_msg.linear_acceleration.z = imu_data.Acc_z;
			ret = rcl_publish(&imu_publisher, &imu_msg, NULL);
		}
		memset(imu_rx_buf, 0, 2 * LPMS::IMU_DATA_LENGTH);
	}
}

void imu_task_start(void)
{
	const osThreadAttr_t imuTask_attributes = {
		.name		= (char *)"imu",
		.stack_size	= 512 * 4,
		.priority	= osPriorityRealtime,
	};

	imu_task_handle = osThreadNew(imu_task, NULL, &imuTask_attributes);
}
