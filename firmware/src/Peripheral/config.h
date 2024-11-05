#ifndef __config_h__
#define __config_h__
#include <Arduino.h>
#include <driver/i2s.h>
#include <WiFi.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>  // Thư viện mDNS để ánh xạ tên miền cục bộ
#include <nvs_flash.h>
#include <nvs.h>
#include <ArduinoWebsockets.h>
#include "I2SMicSampler.h"
#include "I2SSampler.h"


// are you using an I2S microphone - comment this out if you want to use an analog mic and ADC input
#define USE_I2S_MIC_INPUT

// Analog Microphone Settings - ADC1_CHANNEL_7 is GPIO35
#define ADC_MIC_CHANNEL ADC1_CHANNEL_7

#define I2S_PORT I2S_NUM_0
#define I2S_SAMPLE_RATE   (16000)
#define I2S_SAMPLE_BITS   (16)
#define I2S_READ_LEN      (4096)


// I2S Microphone Settings
// Which channel is the I2S microphone on? I2S_CHANNEL_FMT_ONLY_LEFT or I2S_CHANNEL_FMT_ONLY_RIGHT
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
#define I2S_MIC_CHANNEL             I2S_CHANNEL_FMT_ONLY_RIGHT
#define I2S_MIC_SERIAL_CLOCK        GPIO_NUM_33  //SCK
#define I2S_MIC_LEFT_RIGHT_CLOCK    GPIO_NUM_27 //GPIO_NUM_27  //WS
#define I2S_MIC_SERIAL_DATA         GPIO_NUM_12 //GPIO_NUM_12 //SD

// Định nghĩa các chân SPI và điều khiển
#define TFT_CS   15   // Chân CS
#define TFT_RST  26   // Chân Reset
#define TFT_DC   25   // Chân Data/Command
#define TFT_SCK  14   // Chân SCK (HSPI-SCK)
#define TFT_SDI  13   // Chân MOSI (HSPI-MOSI)
#define TFT_LED  0    // Chân điều khiển đèn nền (nếu cần)
#define TFT_BRIGHTNESS 200

#define SD_CS   5  //SDcard

#define TEST_JSON_PARSE                1
#define ESP32_WIFI_CONFIG              1
#define ESP32_VOICE_WAKEUP             1
#define ESP32_VOICE_CONVERSATION       1
#define ESP32_BUTTON                   0
#define ESP32_SENSOR                   0
#define ESP32_TIMEOUT                  1
#define ESP32_INTERNET                 1


#define LED_PIN             GPIO_NUM_2
#define HOME_PIN            GPIO_NUM_32
#define ENTER_PIN           GPIO_NUM_34
#define LEFT_PIN            GPIO_NUM_35
#define RIGHT_PIN           GPIO_NUM_4

#define SS1_PIN             GPIO_NUM_21
#define SS2_PIN             GPIO_NUM_22

#define RXD2                GPIO_NUM_16 //RXX2 pin
#define TXD2                GPIO_NUM_17 //TX2 pin

#define PUSH_BUTTON         LOW

#define TIME_OUT_3S         100 //5s
#define TIME_OUT_PING       5


typedef enum {
    WAITING_WIFI_CONNECT,
    WIFI_DISCONNECT,
    WAIT_INPUT,
    ROBOT_ONLINE,  //3
    ROBOT_SEND_VOICE,
    ROBOT_RECEIVED_RESPONE,
    ROBOT_SEND_ESP2,
    SENSOR_OFF,
    SLEEPING,
    ROBOT_WAKEUP,  //11
} status_robot;


class Timer_Animation
{
public:
    int count = 0;
    bool flag = false;
    int number_animation;
    int time_animation[100]; //time_animation[0] = 0, 1..n: time end animation[1--n]
};

extern Timer_Animation Timer_Display;


using namespace websockets;
extern int status_Robot; //WAITING_WIFI_CONNECT
extern int cnt_time_out;

extern hw_timer_t *timer;


void LED_ON(void);
void LED_OFF(void);
int read_BTN(const int _pin);
bool push3s_Home(void);
bool check_timeout(void);
bool check_sensor(const int _sensor_pin);
void Animation_timer_init(void);

#endif


