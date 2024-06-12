#include <Arduino.h>
#include <string>
#include <inttypes.h>
#include <WiFi.h>
#include <vector>
#include "button.h"
#include <HttpClient.h>
#include "esp_system.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "nvs.h"
#include "nvs_flash.h"

#define IP_ADDRESS "54.67.101.181"

uint8_t count = 0;
char ssid[50]; // your network SSID (name)
char pass[50]; // your network password (use for WPA, or use


WiFiClient c;
HttpClient http(c);

void pushSuccessfulLogin(std::string userName)
{
    std::string informationToSend = "/?userName=";
    informationToSend +=  userName;
    informationToSend += "&isSuccess=1";
    
    http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL);
}

void pushUnsuccesfulLogin(std::string userName)
{
    std::string informationToSend = "/?userName=";
    informationToSend += userName;
    informationToSend += "&isSuccess=0";
    
    http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL);
}

void pushCreateUser()
{
    std::string informationToSend = "/?createUser";
    http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL);
}

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

void setupServer()
{
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

}