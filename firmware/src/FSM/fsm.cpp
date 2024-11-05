#include "fsm.h"


// int config_wifi(void)
// {
//   connect_wifi_server(); //config wifi on web server
//   while(status_Robot == WAITING_WIFI_CONNECT)
//   {
//       if(nvs_config())
//       {
//           status_Robot = WAIT_INPUT;
//       }
//       delay(10);
//   }
//   disconnect_wifi_server();
//   return status_Robot;
// }

// int wait_input(void)
// {
//     if(status_Robot == ROBOT_WAKEUP)  //MARVIN
//     {
//         status_Robot = ROBOT_ONLINE;
//     }
//     else if((status_Robot == WIFI_DISCONNECT) || (status_Robot == BUTTON_DOWN))
//     {
//         status_Robot = SLEEPING;
//     }
//     else if(status_Robot == SENSOR_ON)  //trang thai cb online
//     {
//         //action
//         status_Robot = WAIT_INPUT;
//     }
//     return status_Robot;
// }

// int robot_wakeup(void)
// {
//     if(status_Robot == TIMEOUT)
//     {
//         status_Robot = WAIT_INPUT;
//     }
//     else if(status_Robot == SOCKET_CONNECT)
//     {
//         status_Robot = ROBOT_ONLINE;
//     }
//     else if(status_Robot == WIFI_DISCONNECT)
//     {
//         status_Robot = SLEEPING;
//     }
//     return status_Robot;
// }

// int robot_online(void)
// {
//     if(status_Robot == TIMEOUT)
//     {
//         status_Robot = WAIT_INPUT;
//     } 
//     else if(status_Robot == BUTTON_DOWN)
//     {
//         status_Robot = SLEEPING;
//     }
//     else if(status_Robot == VOICE_RESPONSE)
//     {
//         status_Robot = SEND_VOICE;
//     }
//     return status_Robot;
// }

// int robot_sleeping(void)
// {
//     if(status_Robot == BUTTON_DOWN)
//     {
//         status_Robot = WAIT_INPUT;
//     }
//     return status_Robot;
// }

