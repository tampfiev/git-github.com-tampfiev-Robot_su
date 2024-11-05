#include "TFT_Screen.h"
#include "TFT_Image.h"

#if defined(ESP32)
SPIClass hspi(HSPI);  
#endif

TFT_eSPI tft = TFT_eSPI();

bool tft_output(int16_t x, int16_t y, uint16_t w, uint16_t h, uint16_t* bitmap)
{
   // Stop further decoding as image is running off bottom of screen
  if ( y >= tft.height() ) return 0;

  // This function will clip the image block rendering automatically at the TFT boundaries
  tft.pushImage(x, y, w, h, bitmap);

  // This might work instead if you adapt the sketch to use the Adafruit_GFX library
  // tft.drawRGBBitmap(x, y, bitmap, w, h);

  // Return 1 to decode next block
  return 1;
}

void SDcard_init(void)
{
      // Initialise SD before TFT
  if (!SD.begin(SD_CS, SPI, 80000000)) {
    Serial.println(F("SD.begin failed!"));
    while (1) delay(0);
  }
  Serial.println("\r\nInitialisation done.");
}

void TFT_init(void)
{
      // Initialise the TFT
  tft.begin();
  tft.setTextColor(0xFFFF, 0x0000);
  tft.fillScreen(TFT_BLACK);
  tft.setSwapBytes(true); // We need to swap the colour bytes (endianess)

  // The jpeg image can be scaled by a factor of 1, 2, 4, or 8
  TJpgDec.setJpgScale(1);

  // The decoder must be given the exact name of the rendering function above
  TJpgDec.setCallback(tft_output);
}

void TFT_Display_jpg(uint16_t _x, uint16_t _y, const char *_file_name)
{
  char *root = "/";
  size_t len_root = 1;
  size_t len_file_name = strlen(_file_name);
  size_t total_lens = len_root + len_file_name;
  char* result_file = new char[total_lens];
  strcpy(result_file, root);   // Copy str1 into result
  strcat(result_file, _file_name);   // Append str2 to result
  TJpgDec.drawSdJpg(_x, _y, result_file);
  delete[] result_file;
}


