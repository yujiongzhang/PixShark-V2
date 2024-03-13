/**
 ****************************(C) COPYRIGHT 2022 ZJU****************************
 * @file       servo_task.cpp
 * @brief      servo控制任务
 * @note
 * @history
 *  Version    Date            Author          Modification
 *  V1.0.0     OCT-7-2023     HaoLion(郝亮亮)    1. done
 *
   @verbatim
   ==============================================================================

   ==============================================================================
   @endverbatim
 ****************************(C) COPYRIGHT 2022 ZJU****************************
 */

#include "servo_task.hpp"
#include "servo/aus/aus.hpp"

osThreadId servo_task_handle;
osSemaphoreId servo_semHandle;
int8_t servo_angle;

serial::Serial* servo_serialPtr = nullptr;
Aus* ausPtr = nullptr;


void servo_init()
{
	//初始化串口
	servo_serialPtr = new serial::Serial(&huart2,flowcontrol_software,GPIOD,GPIO_PIN_4);
	servo_serialPtr->open(O_WRONLY);
	ausPtr = new Aus(servo_serialPtr, 0x00);
	ausPtr->Init();
	ausPtr->set_servo_angle(servo_angle);
}

static void servo_task(void * argument)
{
	servo_init();
	//create semaphore
	osSemaphoreDef(servo_sem);
	servo_semHandle = osSemaphoreNew(1,0,osSemaphore(servo_sem));
	for(;;)
	{
		//等待控制指令
		osSemaphoreAcquire(servo_semHandle, osWaitForever);
		ausPtr->set_servo_angle(servo_angle);
	}

}


void servo_task_start(void)
{
	const osThreadAttr_t servoTask_attributes  = {
		.name = (char*)"servo",
		.stack_size = 128,
		.priority = osPriorityNormal,
	};
	servo_task_handle = osThreadNew(servo_task, NULL, &servoTask_attributes);
}