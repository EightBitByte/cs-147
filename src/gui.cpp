#include <TFT_eSPI.h>
#include "gui.h"

#define VOICE_XY 10, 20
#define FINGER_XY 10, 50
#define PATTERN_XY 10, 80

TFT_eSPI tft = TFT_eSPI();
uint16_t backgroundBlue = tft.color565(0, 107, 233);


void initializeScreen() {
    tft.init();
    tft.fillScreen(backgroundBlue);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.setTextColor(TFT_WHITE, backgroundBlue);

    tft.setCursor(VOICE_XY);
    tft.print("Voice Lock");

    tft.setCursor(FINGER_XY);
    tft.print("Fingerprint");

    tft.setCursor(PATTERN_XY);
    tft.print("Pattern");

}

/// @brief Renders the unlock screen on the TTGO's digital display.
void renderUnlockedScreen() {

}