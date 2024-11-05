#include <Arduino.h>
#include <driver/i2s.h>
#include <esp_task_wdt.h>
#include "I2SMicSampler.h"
#include "I2SSampler.h"
#include "ADCSampler.h"
#include "CommandDetector.h"

#include "Peripheral/config.h"

#include "Wifi_config/wifi_config.h"
#include "Voice/voice.h"
#include "Cert/cert.h"
#include "FSM/fsm.h"
#include "Peripheral/TFT_Screen.h"

CommandDetector *commandDetector;

void setup()
{
  Serial.begin(115200);
  Serial2.begin(115200, SERIAL_8N1, RXD2, TXD2);   // Khởi tạo UART 2
  delay(1000);
  Serial.println("Starting up");
  pinMode(LED_PIN, OUTPUT);

  pinMode(HOME_PIN, INPUT);
  pinMode(ENTER_PIN, INPUT);
  pinMode(LEFT_PIN, INPUT);
  pinMode(RIGHT_PIN, INPUT);
  pinMode(SS1_PIN, INPUT);
  pinMode(SS2_PIN, INPUT);

  SDcard_init();
  TFT_init();
  Animation_timer_init();

#if (TEST_JSON_PARSE)
  String json_data = R"([
        {
            "text": "Okay. We both know Tinderella is just my nickname on the internet. My name is Ella. What's your name?",
            "audio": "https://tts-file-mgc-52.hacknao.edu.vn/data/tts_female_Jenifer/0b6b59902cbc633f55191e3d0529f643_3_1.mp3",
            "emotion": "Smile",
            "servo": 1,
            "animations": [
                {
                    "cmu": "expression",
                    "start": 0.0,
                    "end": 0.19,
                    "duration": null,
                    "animation_id": 0,
                    "animation": "trigger_mouth_0"
                },
                {
                    "cmu": "expression",
                    "start": 0.19,
                    "end": 0.27,
                    "duration": null,
                    "animation_id": 5,
                    "animation": "trigger_mouth_5"
                }
            ]
        }
    ])";

    split_json(json_data, voice_con);
#endif

#if (ESP32_WIFI_CONFIG)
  // connect_wifi_server(); //config wifi on web server
  // while(status_Robot == WAITING_WIFI_CONNECT)
  // {
  //     if(nvs_config())
  //     {
  //         status_Robot = WAIT_INPUT;
  //     }
  //     delay(10);
  // }
  // disconnect_wifi_server();

  // const char *ssid = "1805";
  // const char *password = "123456a@";
  const char *ssid = "Ntguitar";
  const char *password = "12345678";

  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WiFi connected");
  status_Robot = WAIT_INPUT;
#endif
  // make sure we don't get killed for our long running tasks
  esp_task_wdt_init(10, false);
  disableCore0WDT();  // Disable watchdog for core 0
  disableCore1WDT();  // Disable watchdog for core 1
  // startI2S();
#if (ESP32_VOICE_CONVERSATION)
  // i2s_read_buff = (char*)calloc(I2S_READ_LEN, sizeof(char));
  i2sData = (char*)calloc(I2S_READ_LEN, sizeof(char));
  flash_write_buff = (uint8_t *)calloc(I2S_READ_LEN, sizeof(char));

  client.onEvent(onEventsCallback);
  client.onMessage(onMessageCallback);
  client.setCACert(ssl_cert);
  while (!client.connect("wss://tofutest.stepup.edu.vn/tofu/ws/conversation"))
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("WebSocket connected!");
#endif

#if (ESP32_VOICE_WAKEUP)
  command_processor = new CommandProcessor();
  CommandDetector *commandDetector = new CommandDetector(i2s_sampler, command_processor);

  xTaskCreatePinnedToCore(voiceProcessTask, "Wakeup", 8192, commandDetector, 1, &voiceProcessTaskHandle, 0);
  xTaskCreatePinnedToCore(DisplayTask, "Display", 1024, NULL, 1, &DisplayTaskHandle, 1);
  i2s_sampler->start(I2S_NUM_0, i2sMicConfig, voiceProcessTaskHandle);
#endif
}

void loop()
{
  vTaskDelay(pdMS_TO_TICKS(10));
}