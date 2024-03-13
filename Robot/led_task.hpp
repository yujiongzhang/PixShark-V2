#ifndef _LED_TASK_HPP_
#define _LED_TASK_HPP_

#include "common.hpp"

#ifdef __cplusplus
extern "C" {
#endif
#include <stdint.h>
extern osSemaphoreId led_semHandle;
extern uint8_t led_brihtness;

void led_task_start(void);

#ifdef __cplusplus
}
#endif



#endif /* _LED_TASK_HPP_ */
