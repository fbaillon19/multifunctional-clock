/**
 * @file DisplayManager.h
 * @brief Simplified Display Manager for initial testing
 * @author Your Name
 * @version 1.0
 * @date 2025
 */

#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "config.h"
#include "ClockManager.h"
#include "SensorManager.h"
#include <FastLED.h>

/**
 * @class DisplayManager
 * @brief Simplified display manager for testing
 */
class DisplayManager {
private:
  CRGB airQualityLEDs[LED_STRIP_AIR_COUNT];
  unsigned long lastUpdate;

public:
  /**
   * @brief Constructor
   */
  DisplayManager() : lastUpdate(0) {}
  
  /**
   * @brief Initialize display manager
   */
  bool init() {
    DEBUG_PRINTLN("Initializing DisplayManager...");
    
    // Initialize air quality LED strip
    FastLED.addLeds<WS2812B, LED_STRIP_AIR_PIN, GRB>(airQualityLEDs, LED_STRIP_AIR_COUNT);
    
    // Clear air quality LEDs
    for (int i = 0; i < LED_STRIP_AIR_COUNT; i++) {
      airQualityLEDs[i] = CRGB::Black;
    }
    FastLED.show();
    
    lastUpdate = millis();
    
    DEBUG_PRINTLN("DisplayManager initialized (test mode)");
    return true;
  }
  
  /**
   * @brief Show boot message
   */
  void showBootMessage(const char* message) {
    DEBUG_PRINT("Boot: ");
    DEBUG_PRINTLN(message);
    // In real implementation, this would show on LCD
  }
  
  /**
   * @brief Show clock information
   */
  void showClock(TimeInfo timeInfo) {
    // In real implementation, this would show time info on LCD
    // For now, just debug output occasionally
    static unsigned long lastClockDisplay = 0;
    if (millis() - lastClockDisplay > 10000) { // Every 10 seconds
      DEBUG_PRINT("Clock Display - ");
      DEBUG_PRINT(timeInfo.hours);
      DEBUG_PRINT(":");
      if (timeInfo.minutes < 10) DEBUG_PRINT("0");
      DEBUG_PRINT(timeInfo.minutes);
      DEBUG_PRINT(":");
      if (timeInfo.seconds < 10) DEBUG_PRINT("0");
      DEBUG_PRINTLN(timeInfo.seconds);
      lastClockDisplay = millis();
    }
  }
  
  /**
   * @brief Update LED clock display
   */
  void updateLEDClock(TimeInfo timeInfo) {
    // This is handled by ClockManager itself
    // Just placeholder for interface compatibility
  }
  
  /**
   * @brief Show sensor data
   */
  void showSensorData(SensorData data, SensorPage page) {
    static unsigned long lastSensorDisplay = 0;
    if (millis() - lastSensorDisplay > 5000) { // Every 5 seconds
      DEBUG_PRINT("Sensor Display - Page ");
      DEBUG_PRINT(page);
      DEBUG_PRINT(": Temp=");
      DEBUG_PRINT(data.tempIndoor);
      DEBUG_PRINT("°C, AQ=");
      DEBUG_PRINTLN(data.airQuality);
      lastSensorDisplay = millis();
    }
  }
  
  /**
   * @brief Show network information
   */
  void showNetworkInfo(int status) {
    DEBUG_PRINT("Network Display - Status: ");
    DEBUG_PRINTLN(status);
  }
  
  /**
   * @brief Show settings menu
   */
  void showSettings(int menuItem) {
    DEBUG_PRINT("Settings Display - Item: ");
    DEBUG_PRINTLN(menuItem);
  }
  
  /**
   * @brief Update air quality LED display
   */
  void updateAirQualityLED(int airQuality) {
    // Map air quality to color
    CRGB color;
    if (airQuality <= AIR_EXCELLENT_MAX) {
      color = CRGB(0, 255, 0);  // Green
    } else if (airQuality <= AIR_GOOD_MAX) {
      color = CRGB(128, 255, 0);  // Yellow-green
    } else if (airQuality <= AIR_MODERATE_MAX) {
      color = CRGB(255, 255, 0);  // Yellow
    } else if (airQuality <= AIR_POOR_MAX) {
      color = CRGB(255, 128, 0);  // Orange
    } else if (airQuality <= AIR_UNHEALTHY_MAX) {
      color = CRGB(255, 0, 0);  // Red
    } else {
      color = CRGB(128, 0, 128);  // Purple
    }
    
    // Calculate how many LEDs to light based on air quality level
    int ledsToLight;
    if (airQuality <= AIR_EXCELLENT_MAX) {        // 0-50 PPM
      ledsToLight = map(airQuality, 0, 50, 2, 4);
    } else if (airQuality <= AIR_GOOD_MAX) {      // 51-100 PPM  
      ledsToLight = map(airQuality, 51, 100, 4, 6);
    } else if (airQuality <= AIR_MODERATE_MAX) {  // 101-200 PPM
      ledsToLight = map(airQuality, 101, 200, 6, 8);
    } else if (airQuality <= AIR_POOR_MAX) {      // 201-300 PPM
      ledsToLight = map(airQuality, 201, 300, 8, 9);
    } else {                                       // 300+ PPM
      ledsToLight = 10;  // All LEDs
    }
    ledsToLight = constrain(ledsToLight, 1, LED_STRIP_AIR_COUNT);
    
    // Clear all LEDs
    for (int i = 0; i < LED_STRIP_AIR_COUNT; i++) {
      airQualityLEDs[i] = CRGB::Black;
    }
    
    // Light appropriate number of LEDs
    for (int i = 0; i < ledsToLight; i++) {
      airQualityLEDs[i] = color;
    }
    
    FastLED.show();
    
    // Debug output occasionally
    static unsigned long lastAirDisplay = 0;
    static int lastAirQuality = -1;
    if (airQuality != lastAirQuality && millis() - lastAirDisplay > 2000) {
      DEBUG_PRINT("Air Quality LEDs updated: ");
      DEBUG_PRINT(airQuality);
      DEBUG_PRINT(" PPM, ");
      DEBUG_PRINT(ledsToLight);
      DEBUG_PRINTLN(" LEDs lit");
      lastAirDisplay = millis();
      lastAirQuality = airQuality;
    }
  }
};

#endif // DISPLAY_MANAGER_H