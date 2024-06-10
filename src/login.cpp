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
#include <WiFi.h>
#include <driver/i2s.h>
#include "login.h"
#include <Adafruit_Fingerprint.h>
//TODO: get these two libraries installed
// ^ https://github.com/cucumberMahach/Esp32_Mic/blob/main/src/main.cpp#L203

bool voiceConfirmed = false;
bool fingerPrintConfirmed = false;
bool patternConfirmed = false;

// #pragma region fingerprint
// #if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// // For UNO and others without hardware serial, we must use software serial...
// // pin #2 is IN from sensor (GREEN wire)
// // pin #3 is OUT from arduino  (WHITE wire)
// // Set up the serial port to use softwareserial..

// #define RX_PIN 13 
// #define TX_PIN 15

// SoftwareSerial mySerial(DATAINF, DATAOUTF);



// #else
// // On Leonardo/M0/etc, others with hardware serial, use hardware serial!
// // #0 is green wire, #1 is white
// #define mySerial Serial1

// #endif

// Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// uint8_t id = 1;

// #pragma endregion

#define RED_PIN 2
#define YELLOW_PIN 15
#define GREEN_PIN 13 
#define BLUE_PIN 12

// const int bufLen = 2048;
// const int bufLenBytes = bufLen * 4; //Bytes to read from i2s
// uint32_t buf[bufLen];
// size_t bytes_read = 0; //unused

// #pragma region setupForFingerprint
// #if (defined(__AVR__) || defined(ESP8266)) && !defined(__AVR_ATmega2560__)
// // For UNO and others without hardware serial, we must use software serial...
// // pin #2 is IN from sensor (GREEN wire)
// // pin #3 is OUT from arduino  (WHITE wire)
// // Set up the serial port to use softwareserial..
// SoftwareSerial mySerial(2, 3);

// #else
// #define mySerial Serial1
// #endif

// #pragma endregion
// //====PINS====
// #define I2S_WS 15
// #define I2S_SD 13
// #define I2S_SCK 2
// //============

// #define I2S_PORT I2S_NUM_0
// #define SAMPLE_BITS 32


#define IP_ADDRESS "54.67.101.181"

extern Button red_button;
extern Button yellow_button;
extern Button green_button;
extern Button blue_button;

extern HttpClient http;

int main()
{
    getFingerPrint();

    //start the login process, on button press
    if (green_button.pressed())
    {
        // startLogin();
    }
}


void startLogin()
{
    // getVoicePasscode();
    getFingerPrint();
}

#pragma region i2sFunctions
//prebuilt functions for esp32 initialization

void getVoicePasscode()
{
    initializeVoice();
    readFromMicrophone();
    pushMicrophoneToServer();
}

// esp_err_t i2s_install()
// {
//   const i2s_config_t i2s_config = {
//       .mode = i2s_mode_t(I2S_MODE_MASTER | I2S_MODE_RX),
//       .sample_rate = 44100,
//       .bits_per_sample = i2s_bits_per_sample_t(SAMPLE_BITS),
//       .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
//       .communication_format = i2s_comm_format_t(I2S_COMM_FORMAT_I2S | I2S_COMM_FORMAT_I2S_MSB),
//       .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1, // default interrupt priority
//       .dma_buf_count = 8,
//       .dma_buf_len = 2048,
//       .use_apll = true
//       };

//   return i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
// }

// esp_err_t i2s_setpin()
// {
//   const i2s_pin_config_t pin_config = {
//       .bck_io_num = I2S_SCK,
//       .ws_io_num = I2S_WS,
//       .data_out_num = -1,
//       .data_in_num = I2S_SD};

//   return i2s_set_pin(I2S_PORT, &pin_config);
// }

// #pragma endregion

// #pragma region voicePasscode
// void initializeVoice()
// {
//     auto res = i2s_install();
//     Serial.printf("i2s_install: %d\n", res);
//     if (res != 0)
//     {
//         Serial.printf("error with i2s install");
//     }

//     res = i2s_setpin();
//     Serial.printf("i2s_setpin: %d\n", res);
//     if (res != 0)
//     {
//         Serial.printf("error with i2s setpin");
//     }

//     res = i2s_start(I2S_PORT);
//     Serial.printf("i2s_start: %d\n", res);
//     if (res != 0)
//     {
//         Serial.printf("error with i2s start");
//     }
// }

// void readFromMicrophone()
// {
//     i2s_read(I2S_PORT, buf, bufLenBytes, &bytes_read, portMAX_DELAY); //Read from i2s
//     pushMicrophoneToServer();
// }

    
// void pushMicrophoneToServer()
// {
//     std::string informationToSend = "?voiceActivation=";
//     for(int i = 0; i < bufLen; i++)
//     {
//         informationToSend+= buf[i];
//     }
//     int err = http.get(IP_ADDRESS, 5000, informationToSend.c_str(), NULL);
// }

// #pragma endregion

// #pragma region fingerPrint
// void initializeFingerPrint()
// {
//     getFingerPrint();
//     //Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);

// }

// uint8_t getFingerprintEnroll() {

//   int p = -1;
//   Serial.print("Waiting for valid finger to enroll as #"); Serial.println(id);
//   while (p != FINGERPRINT_OK) {
//     p = finger.getImage();
//     switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image taken");
//       break;
//     case FINGERPRINT_NOFINGER:
//       Serial.print(".");
//       break;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       break;
//     case FINGERPRINT_IMAGEFAIL:
//       Serial.println("Imaging error");
//       break;
//     default:
//       Serial.println("Unknown error");
//       break;
//     }
//   }

//   // OK success!

//   p = finger.image2Tz(1);
//   switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image converted");
//       break;
//     case FINGERPRINT_IMAGEMESS:
//       Serial.println("Image too messy");
//       return p;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       return p;
//     case FINGERPRINT_FEATUREFAIL:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     case FINGERPRINT_INVALIDIMAGE:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     default:
//       Serial.println("Unknown error");
//       return p;
//   }

//   Serial.println("Remove finger");
//   delay(2000);
//   p = 0;
//   while (p != FINGERPRINT_NOFINGER) {
//     p = finger.getImage();
//   }
//   Serial.print("ID "); Serial.println(id);
//   p = -1;
//   Serial.println("Place same finger again");
//   while (p != FINGERPRINT_OK) {
//     p = finger.getImage();
//     switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image taken");
//       break;
//     case FINGERPRINT_NOFINGER:
//       Serial.print(".");
//       break;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       break;
//     case FINGERPRINT_IMAGEFAIL:
//       Serial.println("Imaging error");
//       break;
//     default:
//       Serial.println("Unknown error");
//       break;
//     }
//   }

//   // OK success!

//   p = finger.image2Tz(2);
//   switch (p) {
//     case FINGERPRINT_OK:
//       Serial.println("Image converted");
//       break;
//     case FINGERPRINT_IMAGEMESS:
//       Serial.println("Image too messy");
//       return p;
//     case FINGERPRINT_PACKETRECIEVEERR:
//       Serial.println("Communication error");
//       return p;
//     case FINGERPRINT_FEATUREFAIL:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     case FINGERPRINT_INVALIDIMAGE:
//       Serial.println("Could not find fingerprint features");
//       return p;
//     default:
//       Serial.println("Unknown error");
//       return p;
//   }

//   // OK converted!
//   Serial.print("Creating model for #");  Serial.println(id);

//   p = finger.createModel();
//   if (p == FINGERPRINT_OK) {
//     Serial.println("Prints matched!");
//   } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//     Serial.println("Communication error");
//     return p;
//   } else if (p == FINGERPRINT_ENROLLMISMATCH) {
//     Serial.println("Fingerprints did not match");
//     return p;
//   } else {
//     Serial.println("Unknown error");
//     return p;
//   }

//   Serial.print("ID "); Serial.println(id);
//   p = finger.storeModel(id);
//   if (p == FINGERPRINT_OK) {
//     Serial.println("Stored!");
//   } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
//     Serial.println("Communication error");
//     return p;
//   } else if (p == FINGERPRINT_BADLOCATION) {
//     Serial.println("Could not store in that location");
//     return p;
//   } else if (p == FINGERPRINT_FLASHERR) {
//     Serial.println("Error writing to flash");
//     return p;
//   } else {
//     Serial.println("Unknown error");
//     return p;
//   }

//   return true;
// }

// void getFingerPrint()
// {
//     delay(1000);
//     finger.begin(57600);
//     delay(1000);
//     if (finger.verifyPassword()) {
//     Serial.println("Found fingerprint sensor!");
//     } else {
//         Serial.println("Did not find fingerprint sensor :(");
//         while (1) { delay(1); }
//     }

//     Serial.println(F("Reading sensor parameters"));
//     finger.getParameters();
//     Serial.print(F("Status: 0x")); Serial.println(finger.status_reg, HEX);
//     Serial.print(F("Sys ID: 0x")); Serial.println(finger.system_id, HEX);
//     Serial.print(F("Capacity: ")); Serial.println(finger.capacity);
//     Serial.print(F("Security level: ")); Serial.println(finger.security_level);
//     Serial.print(F("Device address: ")); Serial.println(finger.device_addr, HEX);
//     Serial.print(F("Packet len: ")); Serial.println(finger.packet_len);
//     Serial.print(F("Baud rate: ")); Serial.println(finger.baud_rate);

//     getFingerprintEnroll();
//     id++;
// }


// #pragma endregion