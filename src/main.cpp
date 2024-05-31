#include <Arduino.h>
#include <string>
#include "button.h"
#include <HttpClient.h>
#include <WiFi.h>
#include <inttypes.h>
#include <stdio.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"
#include <random>
#include <sstream>
#include <iomanip>


#define RED_PIN 2
#define YELLOW_PIN 15
#define GREEN_PIN 13 
#define BLUE_PIN 12

#define IP_ADDRESS "54.67.101.181"

Button red_button(RED_PIN);
Button yellow_button(YELLOW_PIN);
Button green_button(GREEN_PIN);
Button blue_button(BLUE_PIN);

uint8_t count = 0;
char ssid[50]; // your network SSID (name)
char pass[50]; // your network password (use for WPA, or use
const int kNetworkTimeout = 30 * 1000;
const int kNetworkDelay = 1000;

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
                Serial.print("R");
                ++presses;
            }

            else if (yellow_button.pressed()) {
                pattern += "Y";
                Serial.print("Y");
                ++presses;
            }

            else if (green_button.pressed()) {
                pattern += "G";
                Serial.print("G");
                ++presses;
            }


            else if (blue_button.pressed()) {
                pattern += "B";
                Serial.print("B");
                ++presses;
            }
        }

        // Confirmation
        Serial.print("\n\nConfirm your pattern?\nGreen: OK\nRed: RETRY\n");
        
        bool selected = false;
        while (!selected) {
            if (red_button.pressed()) {
                selected = true;
                presses = 0;
                pattern.clear();
                Serial.print("\nEnter your pattern: ");
            }

            else if (green_button.pressed()) {
                selected = true;
                confirmation = true;
            }
        }
    }

    return pattern;
}

void setup() {
    nvs_access();
    // We start by connecting to a WiFi network
    delay(1000);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) 
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("");
    Serial.println("WiFi connected");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    Serial.println("MAC address: ");
    Serial.println(WiFi.macAddress());

    red_button.begin();
    yellow_button.begin();
    green_button.begin();
    blue_button.begin();

    Serial.begin(9600);

    for (int i = 0; i < 5; i++)
    {
        // Prompt for pattern length
        Serial.print("Select your pattern length:\nRed - 4 (default)\nYellow - 6\nGreen - 8\nBlue: - 10\n");
        int pattern_len = 0;

        while (!pattern_len) {
            if (red_button.pressed()) 
                pattern_len = 4;

            else if (yellow_button.pressed()) 
                pattern_len = 6;

            else if (green_button.pressed()) 
                pattern_len = 8;

            else if (blue_button.pressed()) 
                pattern_len = 10;
        }

        std::string pattern = getPattern(pattern_len);
        Serial.printf("Pattern selected as %s\n", pattern.c_str());
        WiFiClient c;
        HttpClient http(c);

        std::string informationToSend = "/?pattern=";
        informationToSend += pattern;
        
        int err = http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL);

        if (err == 0) 
        { 
            Serial.println("startedRequest ok");
            err = http.responseStatusCode();
            if (err >= 0) {
                Serial.print("Got status code: ");
                Serial.println(err);
            }
        } 
        else 
        {
            Serial.print("Connect failed: ");
            Serial.println(err);
        }
    }
    
}

void loop() {

}