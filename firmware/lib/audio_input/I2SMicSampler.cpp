#include "I2SMicSampler.h"
#include "driver/i2s.h"
#include "soc/i2s_reg.h"

I2SMicSampler::I2SMicSampler(i2s_pin_config_t &i2sPins, bool fixSPH0645) : I2SSampler()
{
    m_i2sPins = i2sPins;
    m_fixSPH0645 = fixSPH0645;
}

void I2SMicSampler::configureI2S()
{
    if (m_fixSPH0645)
    {
        // FIXES for SPH0645
        REG_SET_BIT(I2S_TIMING_REG(getI2SPort()), BIT(9));
        REG_SET_BIT(I2S_CONF_REG(getI2SPort()), I2S_RX_MSB_SHIFT);
    }

    i2s_set_pin(getI2SPort(), &m_i2sPins);
}

void I2SMicSampler::processI2SData(uint8_t *i2sData, size_t bytes_read)
{
    int32_t *samples = (int32_t *)i2sData;
    for (int i = 0; i < bytes_read / 4; i++)
    {
        // addSample(samples[i] >> 11);
        float normalised = samples[i] / 2147483648.0f;  //2^31
        addSample(32768 * normalised);
    }
}


// void I2SMicSampler::processI2SData_scale(uint8_t* s_buff, uint8_t* d_buff, uint32_t len) {
//   uint32_t j = 0;
//   uint32_t dac_value = 0;
// //   for (int i = 0; i < len; i += 4) {  // 4 bytes per 32-bit sample
// //     uint32_t sample = ((uint32_t)s_buff[i + 3] << 24) | ((uint32_t)s_buff[i + 2] << 16) | ((uint32_t)s_buff[i + 1] << 8) | (uint32_t)s_buff[i];
// //     dac_value = sample / 65537;
// //     d_buff[j++] = 0;
// //     d_buff[j++] = dac_value * 256 / 2048;  // *256/256
// //   }
//     for (int i = 0; i < len; i += 2) {
//     dac_value = ((((uint16_t)(s_buff[i + 1] & 0xf) << 8) | (s_buff[i])));
//     d_buff[j++] = 0;
//     d_buff[j++] = dac_value * 256 / 2048;
//   }
// }


void I2SMicSampler::processI2SData_scale(uint8_t* s_buff, uint8_t* d_buff, uint32_t len) {
  uint32_t j = 0;

  for (int i = 0; i < len; i += 4) {
    // Lấy hai byte có ý nghĩa nhất (MSB) của mẫu 32-bit làm mẫu 16-bit
    d_buff[j++] = s_buff[i + 2];
    d_buff[j++] = s_buff[i + 3];
  }
}
