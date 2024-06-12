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
#define IP_ADDRESS "54.67.101.181"

uint8_t count = 0;
char ssid[50] = "Yakov"; // your network SSID 
char pass[50] = "yamlpass"; // your network password 

WiFiClient c;
HttpClient http(c);

void pushSuccessfulLogin(std::string userName)
{
    Serial.printf("Pushing Success for %s... ", userName.c_str());
    std::string informationToSend = "/?userName=";
    informationToSend +=  userName;
    informationToSend += "&isSuccess=1";
    
    if (http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL) == 0)
        Serial.println("Done\n");
    else 
        Serial.printf("Error %d\n\n", http.responseStatusCode());
}

void pushUnsuccessfulLogin(std::string userName)
{
    Serial.printf("Pushing Unsuccess for %s... ", userName.c_str());
    std::string informationToSend = "/?userName=";
    informationToSend += userName;
    informationToSend += "&isSuccess=0";
    
    if (http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL) == 0)
        Serial.println("Done\n");
    else
        Serial.printf("Error %d\n\n", http.responseStatusCode());
}


void setupServer()
{
    // We start by connecting to a WiFi network
    delay(1000);
    Serial.println();
    Serial.println();
    Serial.print("Connecting to ");
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