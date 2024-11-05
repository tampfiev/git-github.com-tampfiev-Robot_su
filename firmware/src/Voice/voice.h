#ifndef __voice_h__
#define __voice_h__



#include <Arduino.h>
#include <iostream>
#include <string>
#include <regex>
#include <driver/i2s.h>
#include <esp_task_wdt.h>
#include "I2SMicSampler.h"
#include "ADCSampler.h"
#include <WiFi.h>
#include <ArduinoWebsockets.h>
#include "driver/i2s.h"
#include <HardwareSerial.h>
#include "../Peripheral/config.h"
#include "../CommandDetector.h"
#include "../CommandProcessor.h"
#include "I2SMicSampler.h"
#include "I2SSampler.h"
#include "Cert/cert.h"
#include "../Wifi_config/wifi_config.h"
#include "../Peripheral/TFT_Screen.h"



extern i2s_config_t i2sMicConfig;
extern i2s_pin_config_t i2s_mic_pins;
extern TaskHandle_t voiceProcessTaskHandle, DisplayTaskHandle;
extern I2SSampler *i2s_sampler;


extern const char* websockets_server_host;
extern const uint16_t websockets_server_port;

using namespace websockets;
extern WebsocketsClient client;

extern uint8_t* flash_write_buff;
extern SemaphoreHandle_t xSemaphore;

extern bool internet_connect;
extern CommandProcessor *command_processor;

extern int status_tranform;

extern char* i2s_read_buff;
extern uint8_t* flash_write_buff;


typedef enum {
    NO_READ,
    GET_VOICE_CONVERSATION,
    SEND_VOICE_SERVER
} state_voice;

class Voice_Conversation
{
public:
    int state = NO_READ; //NO_READ;
    bool linkSent = false;
    String linkAudio;
    String emotion;
    int servo;
    int animations[100];  //start time

// protected:
//     void addSample(int16_t sample);
//     virtual void configureI2S() = 0;
//     virtual void processI2SData(uint8_t *i2sData, size_t bytesRead) = 0;
//     virtual void processI2SData_scale(uint8_t* s_buff, uint8_t* d_buff, uint32_t len) = 0;
//     i2s_port_t getI2SPort()
//     {
//         return m_i2s_port;
//     }

public:
    void sendLink2ESP(void);
    void sendVoice2Server(const char *buff);
};

extern Voice_Conversation voice_con;


void i2s_adc_task();
void i2s_adc_convert(uint8_t *i2sData);
void sendLinkToESP2();
void onMessageCallback(WebsocketsMessage message);
void i2s_adc_data_scale(uint8_t* d_buff, uint8_t* s_buff, uint32_t len);
void onEventsCallback(WebsocketsEvent event, String data);
void startI2S();
void stopI2S();
void receive_from_ESP(void);
void changeBitsPerSampleTo16Bit(int _bit_sample) ;
void print_status(void);

void voiceProcessTask(void *param);
void DisplayTask(void *param);
void split_json(const String& json_data, Voice_Conversation json_voice);



#endif


