#include <Arduino.h>
#include <string>
#include <inttypes.h>
#include <WiFi.h>
#include <vector>
#include "button.h"
// #include <HttpClient.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include "gui.h"
#include "login.h"
#include "user.cpp"


#define RED_PIN 2
#define BLUE_PIN 12
#define GREEN_PIN 13 

#define LEFT_BTN_PIN 0

#define IP_ADDRESS "54.67.101.181"

Button red_button(RED_PIN);
Button green_button(GREEN_PIN);
Button blue_button(BLUE_PIN);
Button left_button(LEFT_BTN_PIN);

int currentUser; // current login user attempt
int currentLock; // Current lock we're working on
bool loggedIn;  // Whether or not we're logged
std::vector<User> users; // User log

uint8_t count = 0;
char ssid[50]; // your network SSID (name)
char pass[50]; // your network password (use for WPA, or use
const int kNetworkTimeout = 30 * 1000;
const int kNetworkDelay = 1000;

void clearUserData(User &user);
std::string createName();
void enrollUser();

void nvs_access() {
  esp_err_t err = nvs_flash_init();

    if (err == ESP_ERR_NVS_NO_FREE_PAGES ||
    err == ESP_ERR_NVS_NEW_VERSION_FOUND) 
    {
      // NVS partition was truncated and needs to be erased
      // Retry nvs_flash_init
      ESP_ERROR_CHECK(nvs_flash_erase());
      err = nvs_flash_init();
    }

  ESP_ERROR_CHECK(err);
  // Open
  Serial.printf("\n");
  Serial.printf("Opening Non-Volatile Storage (NVS) handle... ");
  nvs_handle_t my_handle;
  err = nvs_open("storage", NVS_READWRITE, &my_handle);

  if (err != ESP_OK) 
  {
    Serial.printf("Error (%s) opening NVS handle!\n", esp_err_to_name(err));
  }

  else 

  {
    Serial.printf("Done\n");
    Serial.printf("Retrieving SSID/PASSWD\n");
    size_t ssid_len;
    size_t pass_len;
    err = nvs_get_str(my_handle, "ssid", ssid, &ssid_len);
    err |= nvs_get_str(my_handle, "pass", pass, &pass_len);
    switch (err) 
    {
      case ESP_OK:Serial.printf("Done\n");
      Serial.printf("SSID = %s\n", ssid);
      Serial.printf("PASSWD = %s\n", pass);
      break;
      case ESP_ERR_NVS_NOT_FOUND:
      Serial.printf("The value is not initialized yet!\n");
      break;
      default:
      Serial.printf("Error (%s) reading!\n", esp_err_to_name(err));
    }
  }
  // Close
  nvs_close(my_handle);
}

/// @brief Given a length of the pattern, return a string pattern of button presses
/// @param pattern_len Length of the pattern (2-10)
std::string getPattern (int pattern_len) {
    if (pattern_len < 2 || pattern_len > 10)
        Serial.println("Pattern length out of range [2, 10].\n");
        
    Serial.print("Enter your pattern: ");
    
    int presses = 0;
    bool confirmation = false;
    std::string pattern {""};
    while (confirmation == false) {
        while (presses < pattern_len) {
            if (red_button.pressed()) {
                pattern += "R";
                // Serial.print("R");
                ++presses;
                renderPattern(pattern);
            }

            else if (green_button.pressed()) {
                pattern += "G";
                // Serial.print("G");
                ++presses;
                renderPattern(pattern);
            }


            else if (blue_button.pressed()) {
                pattern += "B";
                // Serial.print("B");
                ++presses;
                renderPattern(pattern);
            }
        }

        // Confirmation
        renderPatternConfirm(pattern_len);
        // Serial.print("\n\nConfirm your pattern?\nGreen: OK\nRed: RETRY\n");
        
        bool selected = false;
        while (!selected) {
            if (red_button.pressed()) {
                selected = true;
                presses = 0;
                pattern.clear();
                renderPattern(pattern);
                // Serial.print("\nEnter your pattern: ");
            }

            else if (green_button.pressed()) {
                selected = true;
                confirmation = true;
                renderPattern("");
            }
        }
    }

    return pattern;
}

/// @brief Get the pattern inputted, return if it is correct or not
/// @return Whether or not it matches the current user
bool unlockPattern() {
    int presses = 0;
    bool confirmation = false;
    std::string pattern {""};
    while (presses < 4) {
        if (red_button.pressed()) {
            pattern += "R";
            ++presses;
            renderPattern(pattern);
        }

        else if (green_button.pressed()) {
            pattern += "G";
            ++presses;
            renderPattern(pattern);
        }


        else if (blue_button.pressed()) {
            pattern += "B";
            ++presses;
            renderPattern(pattern);
        }
    }

    return users[currentUser].verifyButtonPattern(pattern);
}

void setup() {
    Serial.begin(9600);

    initializeScreen();

    // Wonky button initialization
    if (red_button.pressed());
    if (green_button.pressed());
    if (blue_button.pressed());

    currentLock = 0;
    loggedIn = false;

    users.push_back(User());

    renderLockScreen();
}


void loop() 
{
  if (left_button.pressed()) {
    // New User enroll sequence
    if (users.size() == 0) {
      enrollUser();
      updateInstruction("LB: Next");
    }
    // Logout sequence
    else if (loggedIn) {
      renderLockScreen();
      updateUser(users[currentUser].name);
      updateInstruction("LB: Next");
      renderArrow(0);
      currentLock = 0;
    }
    // Next user
    else if (currentUser == users.size()) {
      updateUser("NEW USER");
      updateInstruction("LB: Enroll");
      updateTooltip();
    }
  }

  else if (users[currentUser].name != "NEW USER" && currentLock == 0) {
    while (1) 
    {
      if (unlockPattern()) {
        renderPattern("");
        renderArrow(-1);

        delay(500);

        renderUnlockedScreen();
        updateInstruction(users[currentUser].name);
        loggedIn = true;
        break;
      }
      else 
      {
        flashIncorrect(0);
        renderPattern("");
      }
    }
    ++currentLock;
  }
}

void enrollUser() {
  users[currentUser].name = createName();
  renderArrow(0);
  updateActionTitle("Enrolling");
  updateInstruction("Pattern");
  delay(2000);
  updateUser(users[currentUser].name);
  updateInstruction("Continue");
  //wait till user presses to continue
  while(!left_button.pressed())
  {

  }
  updateActionTitle("Pick a pattern Length. R: 4, G: 6, B: 8");
  //todo probably have to allow the user to set pattern length
  int patternLength = 0;
  while(!patternLength)
  {
    if (red_button.pressed())
    {
      patternLength = 4;
    }
    if (green_button.pressed())
    {
      patternLength = 6;
    }
    if (blue_button.pressed())
    {
      patternLength = 8;
    }
  }
  users[currentUser].setButtonPattern(getPattern(patternLength));
  updateActionTitle("Logging in as");
}


void clearUserData(User &user)
{
    user.setButtonPattern("");
    user.setFingerPrintId(-1);
    user.setVerbalPassword("");
}

std::string createName()
{
  std::string name {""};
  int currentChar = 0;
  bool confirmedCharacter = false, confirmedName = false;

  showNamePrompt();

  while (name.size() < 10 && !confirmedName)
  {
    currentChar = 0;
    name.push_back(char(currentChar + 97));
    updateNamePrompt(name);

    while (!confirmedCharacter && !confirmedName)
    {
      if (red_button.pressed())  {
        confirmedCharacter = true;
      }

      else if (green_button.pressed()) {
        currentChar = (currentChar + 1) % 26;
        name.pop_back();
        name.push_back(char(currentChar + 97));
        updateNamePrompt(name);
      }

      else if (blue_button.pressed()) {
        currentChar = currentChar == 0 ? 25 : currentChar - 1;
        name.pop_back();
        name.push_back(char(currentChar + 97));
        updateNamePrompt(name);
      }

      else if (left_button.pressed()) {
        confirmedName = true;
      }
    }
    confirmedCharacter = false;
  }

  renderLockScreen();

  return name;
}