#ifndef TFT_SCREEN_H
#define TFT_SCREEN_H

#include <TJpg_Decoder.h>
#include "config.h"

// Include SD
#define FS_NO_GLOBALS
#include <FS.h>
#ifdef ESP32
  #include "SPIFFS.h" // ESP32 only
#endif



// Include the TFT library https://github.com/Bodmer/TFT_eSPI
#include "SPI.h"
#include <TFT_eSPI.h>              // Hardware-specific library
extern TFT_eSPI tft; //= TFT_eSPI();         // Invoke custom library

void SDcard_init(void);
void TFT_init(void);
void TFT_Display_jpg(uint16_t _x, uint16_t _y, const char *_file_name);

#endif
