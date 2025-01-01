// ####################
//      Created by                                                   
// ╔═╗╔╦╗╔═╗╔╦╗╔╗╔╔═╗╔╦╗  
// ╠═╣ ║ ║ ║║║║║║║╠╣  ║   
// ╩ ╩ ╩ ╚═╝╩ ╩╝╚╝╚   ╩  
// https://github.com/ATOMNFT 
// ####################   

/* FLASH SETTINGS
Board: XIAO_ESP32C3
Flash Frequency: 80MHz
Flash Mode: QIO
Partition Scheme: Huge APP (3MB No OTA/1MB SPIFFS)
*/

#include <Arduino.h>
#include "WiFi.h"
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <BLEDevice.h>
#include <BLEScan.h>
#include <BLEUtils.h>

#define LEDS_COUNT  8
#define LEDS_PIN    8
#define CHANNEL     0

Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
BLEScan* pBLEScan;

void setup() {
  Serial.begin(115200);

  // Initialize the RGB LED strip
  strip.begin();
  strip.setBrightness(20);

  // Set WiFi to station mode and disconnect from any previous connections
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  // Initialize BLE
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan(); // Create new scan
  pBLEScan->setActiveScan(true);   // Active scan uses more power, but gets results faster

  delay(2000);
  Serial.println("Setup done");
}

void setColor(int r, int g, int b) {
  for (int i = 0; i < LEDS_COUNT; i++) {
    strip.setLedColorData(i, r, g, b);
  }
  strip.show();
}

void scanWiFi() {
  Serial.println("WiFi Scan start");

  // Turn RGB LED yellow to indicate WiFi scanning
  setColor(255, 255, 0); // Yellow

  // Perform WiFi scan
  int n = WiFi.scanNetworks();

  // Turn RGB LED purple to indicate results
  setColor(128, 0, 128); // Purple

  Serial.println("WiFi Scan done");
  if (n == 0) {
    Serial.println("no networks found");
  } else {
    Serial.print(n);
    Serial.println(" networks found");
    Serial.println("Nr | SSID                             | RSSI | CH | Encryption");
    for (int i = 0; i < n; ++i) {
      Serial.printf("%2d", i + 1);
      Serial.print(" | ");
      Serial.printf("%-32.32s", WiFi.SSID(i).c_str());
      Serial.print(" | ");
      Serial.printf("%4ld", WiFi.RSSI(i));
      Serial.print(" | ");
      Serial.printf("%2ld", WiFi.channel(i));
      Serial.print(" | ");
      switch (WiFi.encryptionType(i)) {
        case WIFI_AUTH_OPEN:            Serial.print("open"); break;
        case WIFI_AUTH_WEP:             Serial.print("WEP"); break;
        case WIFI_AUTH_WPA_PSK:         Serial.print("WPA"); break;
        case WIFI_AUTH_WPA2_PSK:        Serial.print("WPA2"); break;
        case WIFI_AUTH_WPA_WPA2_PSK:    Serial.print("WPA+WPA2"); break;
        case WIFI_AUTH_WPA2_ENTERPRISE: Serial.print("WPA2-EAP"); break;
        case WIFI_AUTH_WPA3_PSK:        Serial.print("WPA3"); break;
        case WIFI_AUTH_WPA2_WPA3_PSK:   Serial.print("WPA2+WPA3"); break;
        case WIFI_AUTH_WAPI_PSK:        Serial.print("WAPI"); break;
        default:                        Serial.print("unknown");
      }
      Serial.println();
      delay(10);
    }
  }
  Serial.println("");

  // Delete the scan result to free memory for next iteration
  WiFi.scanDelete();

  // Delay to allow results to be shown longer
  delay(5000);
}

void scanBLE() {
  Serial.println("BLE Scan start");

  // Turn RGB LED blue to indicate BLE scanning
  setColor(0, 0, 255); // Blue

  // Perform BLE scan
  BLEScanResults foundDevices = pBLEScan->start(5, false);

  // Turn RGB LED violet to indicate results
  setColor(128, 0, 128); // Purple

  Serial.println("BLE Scan done");

  int count = foundDevices.getCount();
  if (count == 0) {
    Serial.println("no BLE devices found");
  } else {
    Serial.print(count);
    Serial.println(" BLE devices found");
    for (int i = 0; i < count; i++) {
      BLEAdvertisedDevice device = foundDevices.getDevice(i);
      Serial.print("Device ");
      Serial.print(i + 1);
      Serial.print(": ");
      Serial.println(device.toString().c_str());
    }
  }
  Serial.println("");

  // Turn off BLE scanning
  pBLEScan->clearResults();

  // Delay to allow results to be shown longer
  delay(5000);
}

void loop() {
  scanWiFi();
  delay(2000); // Delay before starting BLE scan
  scanBLE();
  delay(2000); // Delay before looping back to WiFi scan
}
