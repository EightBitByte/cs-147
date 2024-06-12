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
#include "server.h"
#include "user.cpp"


#define RED_PIN 2
#define BLUE_PIN 12
#define GREEN_PIN 13 

#define LEFT_BTN_PIN 0


Button red_button(RED_PIN);
Button green_button(GREEN_PIN);
Button blue_button(BLUE_PIN);
Button left_button(LEFT_BTN_PIN);

int currentUser; // current login user attempt
bool loggedIn;  // Whether or not we're logged
std::vector<User> users; // User log

std::string createName();
void enrollUser();

/// @brief Given a length of the pattern, return a string pattern of button presses
std::string getPattern () {
    renderPatternPrompt(false);
    int patternLength = 0;
    while (!patternLength) {
      if (red_button.pressed())
        patternLength = 4;
      else if (green_button.pressed())
        patternLength = 6;
      else if (blue_button.pressed())
        patternLength = 8;
    }
    renderPatternPrompt(true);
    
    int presses = 0;
    bool confirmation = false;
    std::string pattern {""};
    while (confirmation == false) {
        while (presses < patternLength) {
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

        // Confirmation
        renderPatternConfirm(patternLength);
        
        bool selected = false;
        while (!selected) {
            if (red_button.pressed()) {
                selected = true;
                presses = 0;
                pattern.clear();
                renderPattern(pattern);
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
/// @return 1 if match, 0 if no match, 2 if exit
int unlockPattern(int pattern_len) {
  int presses = 0;
  std::string pattern {""};

  while (presses < pattern_len) {
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

      else if (left_button.pressed()) {
          renderPattern("");
          return 2;
      }
  }

  return users[currentUser].verifyButtonPattern(pattern);
}

void setup() {
    Serial.begin(9600);
    setupServer();
    initializeScreen();

    // Wonky button initialization
    if (red_button.pressed());
    if (green_button.pressed());
    if (blue_button.pressed());

    currentUser = 0;
    loggedIn = false;

    users = {};
    users.push_back(User());

    renderLockScreen();
}


void loop() 
{
  // If the left button is pressed
  if (left_button.pressed()) {
    // New User enroll sequence
    if (users[currentUser].name == "NEW USER") {
      Serial.println("Enrolling new User");
      enrollUser();
      pushSuccessfulLogin(users[users.size()-1].name);
      updateInstruction("LB: Next");
      ++currentUser;
      updateUser(users[currentUser].name);
    }
    // Logout sequence
    else if (loggedIn) {
      Serial.println("Logging Out");
      renderLockScreen();
      updateUser(users[currentUser].name);
      updateInstruction("LB: Next");
      renderArrow(0);
      loggedIn = false;
    }
    else {
      Serial.println("Moving to next user");
      currentUser = currentUser == users.size() - 1 ? 0 : currentUser + 1;
      updateUser(users[currentUser].name);

      // If we're back at the new user
      if (currentUser == 0) {
        renderTooltip("Green: Skip");
        updateInstruction("LB: Enroll");
      }
    }
  }

  // Unlocking sequence
  else if (users[currentUser].name != "NEW USER") {
    bool force_exit = false;
    while (!loggedIn && !force_exit) {
      int result = unlockPattern(users[currentUser].pattern_len);

      switch (result) {
        // Incorrect pattern attempt
        case 0:
          Serial.println("Incorrect Pattern");
          flashIncorrect(0);
          renderPattern("");
          pushUnsuccessfulLogin(users[currentUser].name);
          break;

       // Correct pattern attempt
        case 1:
          Serial.println("Correct Pattern, logging in");
          renderPattern("");
          renderArrow(-1);
          pushSuccessfulLogin(users[currentUser].name);

          delay(500);

          renderUnlockedScreen();
          updateInstruction(users[currentUser].name);
          loggedIn = true;
          break;

        // Force Exit
        case 2:
          Serial.println("Force exit occurred");
          currentUser = currentUser == users.size() - 1 ? 0 : currentUser + 1;
          updateUser(users[currentUser].name);

          // If we're back at the new user
          if (currentUser == 0) {
            renderTooltip("Green: Skip");
            updateInstruction("LB: Enroll");
          }

          force_exit = true;
          break;
      }
    }
  }
  
  else if (green_button.pressed() && currentUser == 0) {
    Serial.println("Skipping..");
    ++currentUser;
    updateUser(users[currentUser].name);
    renderTooltip("");
    updateInstruction("LB: Next");
  }
}

void enrollUser() {
  users.push_back(User());
  users[users.size() - 1].name = createName();
  renderArrow(0);
  updateActionTitle("Enrolling");
  updateInstruction("Pattern");
  updateUser(users[users.size() - 1].name);

  // Prompt for pattern
  std::string pattern = getPattern();
  users[users.size() - 1].setButtonPattern(pattern);
  users[users.size() - 1].pattern_len = pattern.size();

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