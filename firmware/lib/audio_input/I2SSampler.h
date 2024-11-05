#ifndef __sampler_base_h__
#define __sampler_base_h__
#include <Arduino.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <driver/i2s.h>
#include <algorithm>

#include "RingBuffer.h"


// #include "../../src/Wifi_config/wifi_config.h"


#define AUDIO_BUFFER_COUNT 11


extern int status_Robot;
extern uint8_t* flash_write_buff;
extern SemaphoreHandle_t xSemaphore;

extern bool flag_I2S;
extern char *i2sData;
/**
 * Base Class for both the ADC and I2S sampler
 **/
class I2SSampler
{
private:
    // audio buffers
    AudioBuffer **m_audio_buffers;
    RingBufferAccessor *m_write_ring_buffer_accessor;
    // current audio buffer
    int m_current_audio_buffer;
    // I2S reader task
    TaskHandle_t m_reader_task_handle;
    // processor task
    TaskHandle_t m_processor_task_handle;
    // i2s reader queue
    QueueHandle_t m_i2s_queue;
    // i2s port
    i2s_port_t m_i2s_port;

protected:
    void addSample(int16_t sample);
    virtual void configureI2S() = 0;
    virtual void processI2SData(uint8_t *i2sData, size_t bytesRead) = 0;
    virtual void processI2SData_scale(uint8_t* s_buff, uint8_t* d_buff, uint32_t len) = 0;
    i2s_port_t getI2SPort()
    {
        return m_i2s_port;
    }

public:
    I2SSampler();
    void I2SSampler_clean();
    void start(i2s_port_t i2s_port, i2s_config_t &i2s_config, TaskHandle_t processor_task_handle);
    void restart(i2s_port_t i2s_port, i2s_config_t &i2s_config, TaskHandle_t processor_task_handle);
    void stop();
    
    RingBufferAccessor *getRingBufferReader();

    int getCurrentWritePosition()
    {
        return m_write_ring_buffer_accessor->getIndex();
    }
    int getRingBufferSize()
    {
        return AUDIO_BUFFER_COUNT * SAMPLE_BUFFER_SIZE;
    }

    friend void i2sReaderTask(void *param);
};

#endif