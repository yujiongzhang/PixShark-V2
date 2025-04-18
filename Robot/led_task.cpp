/**
  ****************************(C) COPYRIGHT 2022 ZJU****************************
  * @file       led_task.cpp
  * @brief      led控制任务         
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

#include "led_task.hpp"
#include "led/barlus/barlus.hpp"

osThreadId led_task_handle;

uint8_t led_brihtness;
osSemaphoreId led_semHandle;
serial::Serial* led_serialPtr = nullptr;
Barlus* barlusPtr = nullptr;

void led_init()
{
    //初始化串口
    led_serialPtr = new serial::Serial(&huart2,flowcontrol_software,GPIOD,GPIO_PIN_4);
    led_serialPtr->open(O_WRONLY);
    barlusPtr = new Barlus(led_serialPtr);
    barlusPtr->Init();
    barlusPtr->off();
    osDelay(1000);
    barlusPtr->enhance();
    osDelay(1000);
    barlusPtr->off();
}

static void led_task(void * argument)
{
     //初始化led
    led_init();
    //create semaphore
    osSemaphoreDef(led_sem);
    led_semHandle = osSemaphoreNew(1,0,osSemaphore(led_sem));
    uint8_t current_brightness_level = 0;  //当前barlus灯光亮度档位
    for(;;)
    {
        //等待控制指令
        osSemaphoreAcquire(led_semHandle, osWaitForever);
        uint8_t brightness_level = led_brihtness/100.0*10.0;
        if(brightness_level == 0)
        {
            barlusPtr->off();
            osDelay(100);
            barlusPtr->off();
            current_brightness_level = 0;
            continue;
        }
        while (current_brightness_level != brightness_level)
        {
            if(current_brightness_level < brightness_level){
                barlusPtr->enhance();
                current_brightness_level++;
            } 
            else if(current_brightness_level > brightness_level){
                barlusPtr->weaken();
                current_brightness_level--;
            }
            osDelay(100);
        }
    }
}



void led_task_start(void)
{
    const osThreadAttr_t  ledTask_attributes  = {
    .name = (char*)"led",
    .stack_size = 128,
    .priority = osPriorityNormal,
    };
    led_task_handle = osThreadNew(led_task, NULL, &ledTask_attributes);
}