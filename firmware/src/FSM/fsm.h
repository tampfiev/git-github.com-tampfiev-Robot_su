#ifndef __fsm_h__
#define __fsm_h__

#include "../Peripheral/config.h"
#include "../Voice/voice.h"
#include "../Wifi_config/wifi_config.h"


int config_wifi(void);
int wait_input(void);
int robot_wakeup(void);
int robot_online(void);
int robot_sleeping(void);

#endif