#include <TFT_eSPI.h>
#include <chrono>
#include "gui.h"

#define PATTERN_XY 10, 20
#define PATTERN_INPUT_X 10
#define PATTERN_INPUT_Y 30
#define PATTERN_BOX_SIZE 10
#define FINGER_XY 10, 50
#define VOICE_XY 10, 80
#define TIME_XY 5, 227
#define INSTR_XY 70, 227
#define LOGGING_IN_XY 25, 110
#define NAME_XY 25, 140

#define MODAL_XY 5, 80
#define MODAL_W 125
#define MODAL_H 90
#define NAME_PROMPT_TITLE_XY 10, 85
#define NAME_PROMPT_XY 10, 105
#define NAME_INSTR_XY 10, 125
#define USERNAME_CURSOR_XY 65, 105

TFT_eSPI tft = TFT_eSPI();
uint16_t backgroundBlue = tft.color565(0, 107, 233);
uint16_t backgroundGray = tft.color565(85, 85, 85);

/// @brief Initializes the screen.
void initializeScreen() {
    tft.init();
    renderLockScreen();
}


/// @brief Renders the lock screen.
void renderLockScreen() {
    tft.fillScreen(backgroundBlue);
    tft.setTextSize(1);
    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.setTextColor(TFT_WHITE, backgroundBlue);

    // tft.setCursor(VOICE_XY);
    // tft.print("Voice");

    // tft.setCursor(FINGER_XY);
    // tft.print("Finger");

    tft.setCursor(PATTERN_XY);
    tft.print("Pattern");

    tft.setTextFont(NULL);
    tft.setTextDatum(MC_DATUM);
    tft.drawString("Logging in as", tft.width()/2, tft.height()/2);
    
    tft.setFreeFont(&FreeMono9pt7b);
    tft.setCursor(NAME_XY);
    tft.drawString("NEW USER", tft.width()/2, tft.height()/2 + 15);

    renderToolbar();
}


/// @brief Updates the current user to login as.
void updateUser(const std::string& name) {
    tft.fillRect(0, tft.height()/2 + 5, 300, 50, backgroundBlue);
    tft.setTextColor(TFT_WHITE, backgroundBlue);


    tft.setCursor(NAME_XY);
    tft.setFreeFont(&FreeMono9pt7b);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(name.c_str(), tft.width()/2, tft.height()/2 + 15);
}


/// @brief Renders the unlock screen on the TTGO's digital display.
void renderUnlockedScreen() {
    tft.fillScreen(backgroundBlue);
    renderToolbar();

    tft.setTextDatum(MC_DATUM);
    tft.setTextColor(TFT_WHITE, backgroundBlue);
    tft.setFreeFont(&FreeMonoBold9pt7b);
    tft.drawString("My Info", tft.width()/2, 10);
    renderTooltip("LB: Logout");

    tft.setCursor(10, 30);
    tft.print("Name:  Diego Brando");
    tft.setCursor(10, 45);
    tft.print("DOB:   12/3/1997");
    tft.setCursor(10, 60);
    tft.print("SSN:   123 32 3312");
    tft.setCursor(10, 75);
    tft.print("Phone: 555 123 4567");
}


/// @returns A string representing the current time.
std::string getCurrentTime() {
    time_t now = time(0);
    struct tm *localTime = localtime(&now);

    int hour = localTime->tm_hour % 12 + 1;
    int minutes = localTime->tm_min;
    std::string meridiem = localTime->tm_hour < 12 ? "AM" : "PM";

    std::string hour_str = std::to_string(hour);
    if (hour < 10) hour_str.insert(hour_str.begin(), '0');

    std::string min_str = std::to_string(minutes);
    if (minutes < 10) min_str.insert(min_str.begin(), '0');

    return hour_str + ':' + min_str + meridiem;
}


/// @brief Updates the time in the lower left-hand corner.
void updateTime() {
    std::string time = getCurrentTime(); 
    tft.setCursor(TIME_XY);
    tft.setTextFont(NULL);
    tft.print(time.c_str());
}


/// @brief Shows the name prompt modal
void showNamePrompt() {
    updateInstruction("");

    tft.fillRect(MODAL_XY, MODAL_W, MODAL_H, backgroundGray);
    tft.setCursor(NAME_PROMPT_TITLE_XY);
    tft.print("Enrollment");

    tft.setCursor(NAME_PROMPT_XY);
    tft.print("Username:");

    tft.setCursor(NAME_INSTR_XY);
    tft.setTextColor(TFT_GREEN, backgroundGray);
    tft.print("Next");

    tft.setCursor(10, 135);
    tft.setTextColor(TFT_CYAN, backgroundGray);
    tft.print("Previous");

    tft.setCursor(10, 145);;
    tft.setTextColor(TFT_RED, backgroundGray);
    tft.print("Confirm Selection");

    tft.setTextColor(TFT_WHITE, backgroundGray);
    tft.setCursor(10, 155);
    tft.print("LB: Confirm Name");
}

/// @brief Updates the name prompt modal
void updateNamePrompt(const std::string& name) {
    tft.setCursor(USERNAME_CURSOR_XY);
    tft.print(name.c_str());
}

/// @brief Update instruction (empty to clear)
void updateInstruction(const std::string& instruction) {
    tft.setTextFont(NULL);
    tft.setTextColor(TFT_WHITE, backgroundGray);
    tft.fillRect(INSTR_XY, 80, 20, backgroundGray);
    if (instruction.size() != 0) {
        tft.setTextDatum(TR_DATUM);
        tft.drawString(instruction.c_str(), 130, 227);
    }
}

/// @brief Flash the title of the incorrect input
/// @param index The title to flash
void flashIncorrect(int index) {
    std::string title;
    int x, y;

    switch (index) {
        case 0:
            x = 10;
            y = 20;
            title = "Pattern";
            break;
    }

    tft.setFreeFont(&FreeMonoBold9pt7b);
    for (int _ = 0; _ < 3; ++_) {
        tft.setCursor(x, y);
        tft.setTextColor(TFT_RED, backgroundBlue);
        tft.print(title.c_str());
        delay(200);
        tft.setCursor(x, y);
        tft.setTextColor(TFT_WHITE, backgroundBlue);
        tft.print(title.c_str());
        delay(200);
    }
}

/// @brief Renders the current pattern below the pattern input.
/// @param pattern 
void renderPattern(std::string pattern) {
    int x = PATTERN_INPUT_X, y = PATTERN_INPUT_Y;

    tft.fillRect(x, y, 320, 20, backgroundBlue);

    if (pattern.size() == 0)
        return;

    for (auto it = pattern.begin(); it != pattern.end(); ++it) {
        switch (*it) {
            case 'R':
                tft.fillRect(x,y, PATTERN_BOX_SIZE, PATTERN_BOX_SIZE, TFT_RED);
                break;
            case 'G':
                tft.fillRect(x,y, PATTERN_BOX_SIZE, PATTERN_BOX_SIZE, TFT_GREEN);
                break;
            case 'B':
                tft.fillRect(x,y, PATTERN_BOX_SIZE, PATTERN_BOX_SIZE, TFT_BLUE);
                break;
        }

        x += PATTERN_BOX_SIZE;
    }
}

void renderPatternConfirm (int pattern_len) {
    tft.setTextFont(NULL);
    tft.setCursor(PATTERN_INPUT_X + (pattern_len * 10) + 10, PATTERN_INPUT_Y);  
    tft.setTextColor(TFT_GREEN, backgroundBlue);
    tft.print("Yes ");
    tft.setTextColor(TFT_RED, backgroundBlue);
    tft.print("Retry");
}

/// @brief Prompts the user for a pattern length
/// @param clear If true, clears the prompt
void renderPatternPrompt(bool clear)
{
    if (clear) {
        tft.fillRect(10, 30, 300, 50, backgroundBlue);
        return;
    }
    
    tft.setCursor(10, 30);
    tft.setTextColor(TFT_WHITE, backgroundBlue);
    tft.setTextFont(NULL);
    tft.print("Choose length:");
    tft.setCursor(10,40);
    tft.setTextColor(TFT_RED, backgroundBlue);
    tft.print("R: 4");
    tft.setCursor(10,50);
    tft.setTextColor(TFT_GREEN, backgroundBlue);
    tft.print("G: 6");
    tft.setCursor(10,60);
    tft.setTextColor(TFT_CYAN, backgroundBlue);
    tft.print("B: 8");
}

/// @brief Draw attention to a particular heading.
/// @param index The heading to target
void renderArrow (int index) {
    int x, y;

    if (index == -1) {
        tft.fillRect(95, 13, 20, 20, backgroundBlue);
        return;
    }

    switch (index) {
        case 0:
            x = 95;
            y = 13;
            break;
    }

    tft.setCursor(x, y);
    tft.setTextFont(NULL);
    tft.setTextColor(TFT_WHITE, backgroundBlue);
    tft.print("<-");
}

/// @brief Updates the action title (usually Logging in)
/// @param title Title to update to
void updateActionTitle (std::string title) {
    tft.fillRect(0, tft.height()/2 - 10, 300, 20, backgroundBlue);

    tft.setTextFont(NULL);
    tft.setTextColor(TFT_WHITE, backgroundBlue);
    tft.setTextDatum(MC_DATUM);
    tft.drawString(title.c_str(), tft.width()/2, tft.height()/2);
}


/// @brief Renders the bottom toolbar.
void renderToolbar() {
    tft.fillRect(0, 220, 320, 20, backgroundGray);
    tft.setTextColor(TFT_WHITE, backgroundGray);
    updateTime();

    updateInstruction("LB: Enroll");
}


/// @brief Renders a tooltip just above the toolbar.
/// @param tooltip The string to render (empty to clear)
void renderTooltip(std::string tooltip) {
    if (tooltip.size() == 0) {
        tft.fillRect(0, 200, 300, 15, backgroundBlue);
        return;
    }

    tft.setTextDatum(MC_DATUM);
    tft.setTextFont(NULL);
    tft.setTextColor(TFT_WHITE, backgroundBlue);
    tft.drawString(tooltip.c_str(), tft.width()/2, 210);
}
