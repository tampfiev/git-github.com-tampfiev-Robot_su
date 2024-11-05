#ifndef __wifi_config_h__
#define __Wifi_config_h__

#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>  // Thư viện mDNS để ánh xạ tên miền cục bộ
#include <nvs_flash.h>
#include <nvs.h>
#include "I2SMicSampler.h"
#include "I2SSampler.h"
#include "../Peripheral/config.h"

#include <ArduinoWebsockets.h>
#include <ESP32Ping.h>  // Include Ping library

extern const char* robot_ssid;
extern const char* robot_password;

extern const char* websockets_server_host;
extern const uint16_t websockets_server_port;
extern WebsocketsClient client;


extern String ssid_robot;     //Wifi configured
extern String password_robot; //Wifi configured

extern bool internet_connect;




bool connectToWiFi(const char* ssid, const char* password);
bool check_Wifi_connect(const char* ssid, const char* password);
bool checkInternet();
void handleConnect(AsyncWebServerRequest *request);
void connect_wifi_server();
void disconnect_wifi_server();
bool nvs_config(void);

#endif