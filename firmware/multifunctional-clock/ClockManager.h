/**
 * @file ClockManager.h
 * @brief Clock and time management for Multifunctional Clock
 * @author Your Name
 * @version 1.0
 * @date 2025
 * 
 * This class handles time keeping, NTP synchronization, and LED clock display.
 * It manages two LED rings (60 LEDs for minutes/seconds, 12 LEDs for hours)
 * and provides animations for hour transitions.
 */

#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include "config.h"
#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <FastLED.h>
#include <TimeLib.h>

/**
 * @struct TimeInfo
 * @brief Structure to hold complete time information
 */
struct TimeInfo {
  int hours;        ///< Hours (0-23)
  int minutes;      ///< Minutes (0-59)
  int seconds;      ///< Seconds (0-59)
  int day;          ///< Day of month (1-31)
  int month;        ///< Month (1-12)
  int year;         ///< Year (full year, e.g., 2025)
  int weekday;      ///< Day of week (1=Sunday, 7=Saturday)
  bool isValid;     ///< Whether time is valid/synchronized
};

/**
 * @class ClockManager
 * @brief Manages time keeping and LED clock display
 * 
 * The ClockManager handles:
 * - Time synchronization via NTP
 * - LED ring management for visual clock display
 * - Hour transition animations
 * - Night mode brightness adjustment
 * - Time validation and error handling
 */
class ClockManager {
private:
  // NTP client for time synchronization
  WiFiUDP ntpUDP;
  NTPClient* timeClient;
  
  // LED arrays for clock display
  CRGB minutesLEDs[LED_RING_MINUTES_COUNT];  ///< 60 LEDs for minutes/seconds
  CRGB hoursLEDs[LED_RING_HOURS_COUNT];      ///< 12 LEDs for hours
  
  // Current time information
  TimeInfo currentTime;
  unsigned long lastTimeUpdate;
  bool timeValidated;
  
  // Animation state
  bool inHourAnimation;
  unsigned long animationStart;
  int animationStep;
  
  // Night mode state
  bool nightModeActive;
  uint8_t currentBrightness;
  
  // Last displayed values (to detect changes)
  int lastHour;
  int lastMinute;
  int lastSecond;

public:
  /**
   * @brief Constructor
   * 
   * Initializes time client and LED arrays.
   */
  ClockManager() : 
    timeClient(nullptr),
    lastTimeUpdate(0),
    timeValidated(false),
    inHourAnimation(false),
    animationStart(0),
    animationStep(0),
    nightModeActive(false),
    currentBrightness(255),
    lastHour(-1),
    lastMinute(-1),
    lastSecond(-1) {
    
    // Initialize time structure
    currentTime.hours = 0;
    currentTime.minutes = 0;
    currentTime.seconds = 0;
    currentTime.day = 1;
    currentTime.month = 1;
    currentTime.year = 2025;
    currentTime.weekday = 1;
    currentTime.isValid = false;
  }
  
  /**
   * @brief Initialize the clock manager
   * 
   * Sets up NTP client, LED strips, and initial time.
   * 
   * @return true if initialization successful, false otherwise
   */
  bool init() {
    DEBUG_PRINTLN("Initializing ClockManager...");
    
    // Initialize LED strips
    FastLED.addLeds<WS2812B, LED_RING_MINUTES_PIN, GRB>(minutesLEDs, LED_RING_MINUTES_COUNT);
    FastLED.addLeds<WS2812B, LED_RING_HOURS_PIN, GRB>(hoursLEDs, LED_RING_HOURS_COUNT);
    
    // Set initial brightness
    FastLED.setBrightness(255);
    
    // Clear all LEDs
    clearAllLEDs();
    FastLED.show();
    
    // Initialize NTP client
    timeClient = new NTPClient(ntpUDP, NTP_SERVER, TIMEZONE_OFFSET * 3600, 60000);
    
    if (!timeClient) {
      DEBUG_PRINTLN("Failed to create NTP client");
      return false;
    }
    
    lastTimeUpdate = millis();
    
    DEBUG_PRINTLN("ClockManager initialized successfully");
    return true;
  }
  
  /**
   * @brief Update clock state
   * 
   * Should be called every second to update time and LED display.
   */
  void update() {
    unsigned long currentMillis = millis();
    
    // Update internal time (every second)
    if (currentMillis - lastTimeUpdate >= 1000) {
      updateInternalTime();
      lastTimeUpdate = currentMillis;
    }
    
    // Check for night mode
    updateNightMode();
    
    // Handle hour animations
    if (inHourAnimation) {
      updateHourAnimation();
    }
    
    // Update LED display if time changed
    if (hasTimeChanged()) {
      updateLEDDisplay();
      updateLastDisplayedTime();
    }
  }
  
  /**
   * @brief Synchronize time with NTP server
   * 
   * Attempts to get current time from NTP server and update internal clock.
   * 
   * @return true if synchronization successful, false otherwise
   */
  bool syncWithNTP() {
    if (!timeClient || WiFi.status() != WL_CONNECTED) {
      DEBUG_PRINTLN("Cannot sync NTP: WiFi not connected");
      return false;
    }
    
    DEBUG_PRINTLN("Synchronizing with NTP server...");
    
    timeClient->begin();
    
    // Try to update time (with timeout)
    unsigned long startTime = millis();
    while (!timeClient->update() && millis() - startTime < 5000) {
      delay(100);
    }
    
    if (timeClient->isTimeSet()) {
      // Get epoch time and convert to local time
      unsigned long epochTime = timeClient->getEpochTime();
      setTime(epochTime);
      
      // Update our time structure
      updateTimeFromEpoch(epochTime);
      
      timeValidated = true;
      
      DEBUG_PRINT("NTP sync successful. Time: ");
      DEBUG_PRINT(currentTime.hours);
      DEBUG_PRINT(":");
      DEBUG_PRINT(currentTime.minutes);
      DEBUG_PRINT(":");
      DEBUG_PRINTLN(currentTime.seconds);
      
      return true;
    } else {
      DEBUG_PRINTLN("NTP sync failed");
      return false;
    }
  }
  
  /**
   * @brief Get current time information
   * 
   * @return TimeInfo structure with current time
   */
  TimeInfo getCurrentTime() const {
    return currentTime;
  }
  
  /**
   * @brief Check if time is valid/synchronized
   * 
   * @return true if time has been synchronized with NTP
   */
  bool isTimeValid() const {
    return timeValidated && currentTime.isValid;
  }
  
  /**
   * @brief Force LED display update
   * 
   * Updates LED rings immediately regardless of time changes.
   */
  void forceDisplayUpdate() {
    updateLEDDisplay();
  }
  
  /**
   * @brief Trigger hour change animation
   * 
   * Starts the special animation sequence for hour transitions.
   */
  void triggerHourAnimation() {
    if (!inHourAnimation) {
      inHourAnimation = true;
      animationStart = millis();
      animationStep = 0;
      DEBUG_PRINTLN("Starting hour animation");
    }
  }

private:
  /**
   * @brief Update internal time counter
   * 
   * Increments seconds and handles minute/hour rollovers.
   */
  void updateInternalTime() {
    currentTime.seconds++;
    
    if (currentTime.seconds >= 60) {
      currentTime.seconds = 0;
      currentTime.minutes++;
      
      if (currentTime.minutes >= 60) {
        currentTime.minutes = 0;
        currentTime.hours++;
        
        // Trigger hour animation
        triggerHourAnimation();
        
        if (currentTime.hours >= 24) {
          currentTime.hours = 0;
          // Day rollover - could be enhanced to handle dates properly
        }
      }
    }
  }
  
  /**
   * @brief Update time structure from epoch timestamp
   * 
   * @param epochTime Unix timestamp
   */
  void updateTimeFromEpoch(unsigned long epochTime) {
    // Apply timezone offset
    epochTime += TIMEZONE_OFFSET * 3600;
    
    // Apply DST offset if needed (simplified - could be more sophisticated)
    // TODO: Implement proper DST calculation
    epochTime += DST_OFFSET * 3600;
    
    currentTime.seconds = epochTime % 60;
    epochTime /= 60;
    currentTime.minutes = epochTime % 60;
    epochTime /= 60;
    currentTime.hours = epochTime % 24;
    epochTime /= 24;
    
    // Calculate day of week (Sunday = 0)
    currentTime.weekday = (epochTime + 4) % 7; // Epoch started on Thursday
    
    // Calculate year, month, day (simplified)
    unsigned long days = epochTime;
    currentTime.year = 1970;
    
    // Simple year calculation
    while (days >= 365) {
      if (isLeapYear(currentTime.year) && days >= 366) {
        days -= 366;
        currentTime.year++;
      } else if (!isLeapYear(currentTime.year)) {
        days -= 365;
        currentTime.year++;
      } else {
        break;
      }
    }
    
    // Simple month/day calculation
    currentTime.month = 1;
    currentTime.day = days + 1;
    
    currentTime.isValid = true;
  }
  
  /**
   * @brief Check if it's a leap year
   * 
   * @param year Year to check
   * @return true if leap year
   */
  bool isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
  }
  
  /**
   * @brief Check and update night mode status
   */
  void updateNightMode() {
    bool shouldBeNightMode = (currentTime.hours >= NIGHT_MODE_START || 
                             currentTime.hours < NIGHT_MODE_END);
    
    if (shouldBeNightMode != nightModeActive) {
      nightModeActive = shouldBeNightMode;
      currentBrightness = nightModeActive ? NIGHT_BRIGHTNESS : 255;
      FastLED.setBrightness(currentBrightness);
      
      DEBUG_PRINT("Night mode ");
      DEBUG_PRINTLN(nightModeActive ? "ON" : "OFF");
    }
  }
  
  /**
   * @brief Check if displayed time has changed
   * 
   * @return true if time values have changed since last display
   */
  bool hasTimeChanged() {
    return (currentTime.hours != lastHour || 
            currentTime.minutes != lastMinute || 
            currentTime.seconds != lastSecond);
  }
  
  /**
   * @brief Update stored last displayed time
   */
  void updateLastDisplayedTime() {
    lastHour = currentTime.hours;
    lastMinute = currentTime.minutes;
    lastSecond = currentTime.seconds;
  }
  
  /**
   * @brief Update LED display based on current time
   */
  void updateLEDDisplay() {
    // Clear all LEDs
    clearAllLEDs();
    
    // Convert to 12-hour format for display
    int displayHour = currentTime.hours % 12;
    
    // Set hour LED (12 LEDs, so multiply by 5 to get position on minutes ring)
    hoursLEDs[displayHour] = CRGB(COLOR_HOURS >> 16, (COLOR_HOURS >> 8) & 0xFF, COLOR_HOURS & 0xFF);
    
    // Set minute LED
    minutesLEDs[currentTime.minutes] = CRGB(COLOR_MINUTES >> 16, (COLOR_MINUTES >> 8) & 0xFF, COLOR_MINUTES & 0xFF);
    
    // Set second LED
    minutesLEDs[currentTime.seconds] = CRGB(COLOR_SECONDS >> 16, (COLOR_SECONDS >> 8) & 0xFF, COLOR_SECONDS & 0xFF);
    
    // Handle overlap (when minute and second are the same)
    if (currentTime.minutes == currentTime.seconds) {
      minutesLEDs[currentTime.minutes] = CRGB(COLOR_OVERLAP >> 16, (COLOR_OVERLAP >> 8) & 0xFF, COLOR_OVERLAP & 0xFF);
    }
    
    // Show the updated LEDs
    FastLED.show();
  }
  
  /**
   * @brief Update hour transition animation
   */
  void updateHourAnimation() {
    unsigned long elapsed = millis() - animationStart;
    
    if (elapsed < 5000) { // 5 second animation
      // Create a spinning effect or color wave
      int pos = (elapsed / 100) % LED_RING_HOURS_COUNT;
      
      // Clear hour ring
      for (int i = 0; i < LED_RING_HOURS_COUNT; i++) {
        hoursLEDs[i] = CRGB::Black;
      }
      
      // Set animated LEDs
      for (int i = 0; i < 3; i++) {
        int ledPos = (pos + i) % LED_RING_HOURS_COUNT;
        hoursLEDs[ledPos] = CRGB::White;
      }
      
      FastLED.show();
    } else {
      // Animation finished
      inHourAnimation = false;
      DEBUG_PRINTLN("Hour animation completed");
    }
  }
  
  /**
   * @brief Clear all LED arrays
   */
  void clearAllLEDs() {
    for (int i = 0; i < LED_RING_MINUTES_COUNT; i++) {
      minutesLEDs[i] = CRGB::Black;
    }
    for (int i = 0; i < LED_RING_HOURS_COUNT; i++) {
      hoursLEDs[i] = CRGB::Black;
    }
  }
};

#endif // CLOCK_MANAGER_H