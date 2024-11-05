#include "voice.h"

TaskHandle_t voiceProcessTaskHandle, DisplayTaskHandle;
I2SSampler *i2s_sampler = new I2SMicSampler(i2s_mic_pins, false);

// Trạng thái 1 là thu và gửi dữ liệu, 2 là gửi link qua UART và chờ phản hồi
int state_Conversation = 0;
CommandProcessor *command_processor;

char* i2s_read_buff;
uint8_t* flash_write_buff;

// i2s microphone pins
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};

i2s_config_t i2sMicConfig = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = 16000,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_MIC_CHANNEL,
    .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S),
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 64,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0
};



SemaphoreHandle_t xSemaphore;
Voice_Conversation voice_con;

int status_tranform = 0;



void i2s_adc_convert(uint8_t *i2sData) {
  i2s_adc_data_scale(flash_write_buff, (uint8_t*)i2sData, I2S_READ_LEN);
  client.sendBinary((const char*)flash_write_buff, I2S_READ_LEN);
  client.poll();
}


void split_json(const String& json_data, Voice_Conversation json_voice) {
    // Extract "audio"
    int audio_start = json_data.indexOf("\"audio\":");
    if (audio_start != -1) {
        audio_start += strlen("\"audio\": \"");
        int audio_end = json_data.indexOf("\"", audio_start);
        if (audio_end != -1) {
            json_voice.linkAudio = json_data.substring(audio_start, audio_end); // Use substring method

            Serial.println("Audio: " + json_voice.linkAudio);
        }
    }

    // Extract "emotion"
    int emotion_start = json_data.indexOf("\"emotion\":");
    if (emotion_start != -1) {
        emotion_start += strlen("\"emotion\": \"");
        int emotion_end = json_data.indexOf("\"", emotion_start);
        if (emotion_end != -1) {
            json_voice.emotion = json_data.substring(emotion_start, emotion_end); // Use substring method

            Serial.println("Emotion: " + json_voice.emotion);
        }
    }

    // Extract "servo"
    int servo_start = json_data.indexOf("\"servo\":");
    if (servo_start != -1) {
        servo_start += strlen("\"servo\": ");
        json_voice.servo = json_data.substring(servo_start, json_data.indexOf(",", servo_start)).toInt(); // Convert to int
        Serial.printf("Servo: %d\n", json_voice.servo);
    }

    // Extract "start" values from animations
    int animation_start = json_data.indexOf("\"animations\":");
    if (animation_start != -1) {
        int start_ptr = animation_start;
        int i = 0;
        while ((start_ptr = json_data.indexOf("\"start\":", start_ptr)) != -1) {
            start_ptr += strlen("\"start\": ");
            double start = json_data.substring(start_ptr, json_data.indexOf(",", start_ptr)).toDouble(); // Convert to double
            json_voice.animations[i] = static_cast<int>(start * 100.0);
            Serial.printf("animations[%d]: %d\n", i, json_voice.animations[i]);
            i++;
            start_ptr = json_data.indexOf(",", start_ptr); // Move to the next comma
        }
    }
}


void onMessageCallback(WebsocketsMessage message) {
  String respond_message = message.data();
  Serial.println(respond_message);

  if (respond_message.indexOf("\"audio\":") != -1) {
    // int indexOfText = respond_message.indexOf("\"text\":");
    // int indexOfAudio = respond_message.indexOf("\"audio\":");
    // int indexOfEmotion = respond_message.indexOf("\"emotion\":");
    // String textToSpeech = respond_message.substring(indexOfText + 8, indexOfAudio - 2);
    // voice_con.linkAudio = respond_message.substring(indexOfAudio + 9, indexOfEmotion - 2);
    // Serial.println("Text: " + textToSpeech);
    // Serial.println("Audio Link: " + voice_con.linkAudio);

    
    split_json(respond_message, voice_con);
    voice_con.state = SEND_VOICE_SERVER;
    voice_con.linkSent = false;
  }
}



// void i2s_adc_data_scale(uint8_t* d_buff, uint8_t* s_buff, uint32_t len) {
//   uint32_t j = 0;
//   uint32_t dac_value = 0;
//   for (int i = 0; i < len; i += 4) {  // 4 bytes per 32-bit sample
//     uint32_t sample = ((uint32_t)s_buff[i + 3] << 24) | ((uint32_t)s_buff[i + 2] << 16) | ((uint32_t)s_buff[i + 1] << 8) | (uint32_t)s_buff[i];
//     dac_value = sample / 65537;
//     d_buff[j++] = 0;
//     d_buff[j++] = dac_value * 256 / 2048;
//   }
// }


void i2s_adc_data_scale(uint8_t* d_buff, uint8_t* s_buff, uint32_t len) {
  uint32_t j = 0;

  for (int i = 0; i < len; i += 4) {
    // Lấy hai byte có ý nghĩa nhất (MSB) của mẫu 32-bit làm mẫu 16-bit
    d_buff[j++] = s_buff[i + 2];
    d_buff[j++] = s_buff[i + 3];
  }
}

void onEventsCallback(WebsocketsEvent event, String data) {
  if (event == WebsocketsEvent::ConnectionOpened) {
    Serial.println("WebSocket connection opened");
    internet_connect = true;
  } else if (event == WebsocketsEvent::ConnectionClosed) {
    Serial.println("WebSocket connection closed");
    internet_connect = false;
    // ESP.restart();
  }
}

void startI2S() {
  i2s_driver_install(I2S_PORT, &i2sMicConfig, 4, NULL);
  i2s_set_pin(I2S_PORT, &i2s_mic_pins);
  Serial.println("I2S started");
}

void stopI2S() {
  i2s_driver_uninstall(I2S_PORT);
  Serial.println("I2S stopped");
}

void Voice_Conversation::sendLink2ESP(void) {
  if (this->linkAudio.length() > 0) {
    Serial2.println(this->linkAudio);
    Serial.println("Gửi link qua UART: " + this->linkAudio);
  }
}
void Voice_Conversation::sendVoice2Server(const char *buff) {
  client.sendBinary(buff, I2S_READ_LEN / 2);
  client.poll();
  Serial.println("Send voice to server --------------------------");
}


void process_wait_input(void)
{
#if(ESP32_SENSOR)
    if(!check_sensor(SS1_PIN))   //SS1 on (co vat can)
    {
      voice_con.state = GET_VOICE_CONVERSATION;
    }
#endif

    if(voice_con.state == GET_VOICE_CONVERSATION)
    {
      //Play sound wakeup
        status_Robot = ROBOT_ONLINE;
        
    }
#if(ESP32_BUTTON)
    if(!push3s_Home())
    {
      voice_con.state = NO_READ;
      status_Robot = SLEEPING;
    } 
#endif 

#if(ESP32_INTERNET)
    if(!checkInternet())
    {
      voice_con.state = NO_READ;
      status_Robot = SLEEPING;
    } 
#endif

#if(ESP32_TIMEOUT)
      if(!check_timeout())
      {
        voice_con.state = NO_READ;
        status_Robot = SLEEPING;
      }
#endif

#if(ESP32_SENSOR)
    if(check_sensor(SS2_PIN))  //SS2 off: Nang robot len
    {
      status_Robot = SENSOR_OFF;
    } 
#endif 
}

void process_robot_online(void)
{
    if(voice_con.state == GET_VOICE_CONVERSATION)
    {
      voice_con.sendVoice2Server((const char*)flash_write_buff);
#if(ESP32_BUTTON || ESP32_TIMEOUT)
      // if(!check_timeout() || !push3s_Home())
      // {
      //   voice_con.state = NO_READ;
      //   status_Robot = WAIT_INPUT;
      //   free(i2sData);
      //   free(flash_write_buff);
      //   i2sData = (char*)calloc(I2S_READ_LEN, sizeof(char));
      //   flash_write_buff = (uint8_t *)calloc(I2S_READ_LEN, sizeof(char));
      // }
      if(!check_timeout())
      {
        voice_con.state = NO_READ;
        status_Robot = WAIT_INPUT;
        free(i2sData);
        free(flash_write_buff);
        i2sData = (char*)calloc(I2S_READ_LEN, sizeof(char));
        flash_write_buff = (uint8_t *)calloc(I2S_READ_LEN, sizeof(char));
      }
#endif

#if(ESP32_INTERNET)
      if(!checkInternet())
      {
        voice_con.state = NO_READ;
        status_Robot = SLEEPING;
      } 
#endif
    }      
    else if(voice_con.state == SEND_VOICE_SERVER)
    {
      if (!voice_con.linkSent) {
        status_Robot = ROBOT_RECEIVED_RESPONE;
        voice_con.linkSent = true;
        voice_con.sendLink2ESP();
        //send animation
        
        cnt_time_out = 0;
      }

#if(ESP32_TIMEOUT)
      if(!check_timeout())
      {
        voice_con.state = NO_READ;
        status_Robot = WAIT_INPUT;
      }
#endif

#if(ESP32_BUTTON)
      if(!push3s_Home())
      {
        voice_con.state = NO_READ;
        status_Robot = SLEEPING;
      }
#endif
    
#if(ESP32_INTERNET)
      if(!checkInternet())
      {
        voice_con.state = GET_VOICE_CONVERSATION;
        status_Robot = ROBOT_ONLINE;
      } 
#endif
    }
}

void process_sensor_off(void)
{
      //Bieu cam khi nang robot len

#if(ESP32_BUTTON)
    if(!push3s_Home())
    {
      voice_con.state = NO_READ;
      status_Robot = SLEEPING;
    }
#endif

#if(ESP32_SENSOR)
    if(!check_sensor(SS2_PIN)) //SS2 on, dat robot xuong
    {
      voice_con.state = NO_READ;
      status_Robot = WAIT_INPUT;
    }
#endif
}

void process_sleeping(void)
{
#if(ESP32_BUTTON)
    if(!push3s_Home())
    {
      voice_con.state = NO_READ;
      status_Robot = WAIT_INPUT;
    }
#endif
}


void voiceProcessTask(void *param)
{
  CommandDetector *commandDetector = static_cast<CommandDetector *>(param);
  const TickType_t xMaxBlockTime = pdMS_TO_TICKS(100);

  static int time_out = 0;
  
  while (true)
  {
    // wait for some audio samples to arrive
    uint32_t ulNotificationValue = ulTaskNotifyTake(pdTRUE, xMaxBlockTime);

    if (ulNotificationValue > 0)
    {
        commandDetector->run();
    }

    switch(status_Robot)
    {
      case WAIT_INPUT: process_wait_input(); break;
      case ROBOT_ONLINE: process_robot_online(); break;
      case ROBOT_RECEIVED_RESPONE: receive_from_ESP(); break;
      case SENSOR_OFF: process_sensor_off(); break;
      case SLEEPING: process_sleeping(); break;
      default: break;
    }
    print_status();
    vTaskDelay(pdMS_TO_TICKS(5)); 
  }
}


void DisplayTask(void *param)
{  
  while (true)
  {
    if(status_Robot == ROBOT_ONLINE)
    {
      TFT_Display_jpg(0, 0, "cat1.jpg");
    }
    vTaskDelay(pdMS_TO_TICKS(10)); 
  }
}


void receive_from_ESP(void)
{
  if (Serial2.available()) {
    char response = Serial2.read();
    if (response == '1') {
      //stop animation  
      status_Robot = ROBOT_ONLINE;
      voice_con.state = GET_VOICE_CONVERSATION;
      voice_con.linkSent = false;
    }
  }
  //start animation
}


void print_status(void)
{
  switch(status_Robot)
  {
    case WAITING_WIFI_CONNECT: {Serial.println("Robot is WAITING_WIFI_CONNECT"); break;}
    case WIFI_DISCONNECT: {Serial.println("Robot is WIFI_DISCONNECT"); break;}
    case WAIT_INPUT: {Serial.println("Robot is WAIT_INPUT"); break;}
    case ROBOT_ONLINE: {Serial.println("Robot is ROBOT_ONLINE"); break;}
    case ROBOT_SEND_VOICE: {Serial.println("Robot is ROBOT_SEND_VOICE"); break;}
    case ROBOT_RECEIVED_RESPONE: {Serial.println("Robot is ROBOT_RECEIVED_RESPONE"); break;}
    case ROBOT_SEND_ESP2: {Serial.println("Robot is ROBOT_SEND_ESP2"); break;}
    case SENSOR_OFF: {Serial.println("Robot is SENSOR_OFF"); break;}
    case SLEEPING: {Serial.println("Robot is SLEEPING"); break;}
    case ROBOT_WAKEUP: {Serial.println("Robot is ROBOT_WAKEUP"); break;}
    default: break;
  }
}

