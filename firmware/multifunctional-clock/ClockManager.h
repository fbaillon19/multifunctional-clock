/**
 * @file ClockManager.h
 * @brief Clock and LED display management for Multifunctional Clock
 * @author Your Name
 * @version 2.0
 * @date 2025
 * 
 * This class handles LED clock display and NTP synchronization.
 * Timing is now managed by TimerManager using hardware RTC interrupts.
 */

#ifndef CLOCK_MANAGER_H
#define CLOCK_MANAGER_H

#include "config.h"
#include "TimerManager.h"
#include <WiFiS3.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <FastLED.h>

class TestManager;

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
 * @brief Manages LED clock display and NTP synchronization
 * 
 * The ClockManager handles:
 * - LED ring management for visual clock display
 * - Hour transition animations
 * - Night mode brightness adjustment
 * - NTP synchronization (delegating timing to TimerManager)
 */
class ClockManager {
private:
  // Reference to timer manager
  TimerManager* timerMgr;
  bool shouldTriggerAnimation(int minutes, int seconds);
  bool animationActive;
  unsigned long animationStart;
  int animationStep;
  
  // NTP client for time synchronization
  WiFiUDP ntpUDP;
  NTPClient* timeClient;
  
  // LED arrays for clock display
  CRGB minutesLEDs[LED_RING_MINUTES_COUNT];  ///< 60 LEDs for minutes/seconds
  CRGB hoursLEDs[LED_RING_HOURS_COUNT];      ///< 12 LEDs for hours
  
  // Display state
  bool timeValidated;
  bool nightModeActive;
  uint8_t currentBrightness;
  
  // Last displayed values (to detect changes)
  int lastHour;
  int lastMinute;
  int lastSecond;
  
  // Animation state
//  int animationStep;
  int firstPixelHue;

public:
  /**
   * @brief Constructor
   * 
   * @param timer Pointer to TimerManager instance
   */
  ClockManager(TimerManager* timer) : 
    timerMgr(timer),
    timeClient(nullptr),
    timeValidated(false),
    nightModeActive(false),
    currentBrightness(255),
    lastHour(-1),
    lastMinute(-1),
    lastSecond(-1),
    firstPixelHue(0),
    animationActive(false),
    animationStart(0),
    animationStep(0) {}
  
  /**
   * @brief Initialize the clock manager
   * 
   * Sets up NTP client and LED strips.
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
    
    DEBUG_PRINTLN("ClockManager initialized successfully");
    return true;
  }
  
  /**
   * @brief Update clock state
   * 
   * Called from main loop to handle display updates and animations.
   */
  void update() {
    RTCTime currentTime = timerMgr->getCurrentTime();
  
    // Déclencher animation
    if (shouldTriggerAnimation(currentTime.getMinutes(), currentTime.getSeconds()) && !animationActive) {
      animationActive = true;
      animationStart = millis();
      animationStep = 0;
      DEBUG_PRINTLN("Animation démarrée");
    }
    
    // Gérer l'animation
    if (animationActive) {
      unsigned long elapsed = millis() - animationStart;
      if (elapsed > 5000) { // 5 secondes
        animationActive = false;
        DEBUG_PRINTLN("Animation terminée");
        updateLEDDisplay(); // Revenir à l'affichage normal
      } else {
        updateHourAnimation();
      }
    } else {
      // Affichage normal de l'horloge
      updateLEDDisplay();
    }
    
    updateNightMode();
  }
  
  /**
   * @brief Synchronize time with NTP server
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
      // Get epoch time and set in TimerManager
      unsigned long epochTime = timeClient->getEpochTime();
      timerMgr->setTimeFromEpoch(epochTime, TIMEZONE_OFFSET);
      
      timeValidated = true;
      
      RTCTime currentTime = timerMgr->getCurrentTime();
      DEBUG_PRINT("NTP sync successful. Time: ");
      DEBUG_PRINT(currentTime.getHour());
      DEBUG_PRINT(":");
      DEBUG_PRINT(currentTime.getMinutes());
      DEBUG_PRINT(":");
      DEBUG_PRINTLN(currentTime.getSeconds());
      
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
    RTCTime rtcTime = timerMgr->getCurrentTime();
    
    TimeInfo timeInfo;
    timeInfo.hours = rtcTime.getHour();
    timeInfo.minutes = rtcTime.getMinutes();
    timeInfo.seconds = rtcTime.getSeconds();
    timeInfo.day = rtcTime.getDayOfMonth();
    timeInfo.month = (int)rtcTime.getMonth();
    timeInfo.year = rtcTime.getYear();
    timeInfo.weekday = (int)rtcTime.getDayOfWeek();
    timeInfo.isValid = timerMgr->isTimeValid();
    
    return timeInfo;
  }
  
  /**
   * @brief Check if time is valid/synchronized
   * 
   * @return true if time has been synchronized with NTP
   */
  bool isTimeValid() const {
    return timeValidated && timerMgr->isTimeValid();
  }
  
  /**
   * @brief Force LED display update
   */
  void forceDisplayUpdate() {
    updateLEDDisplay();
  }

private:
  /**
   * @brief Update LED display based on current time
   */
  void updateLEDDisplay() {
    RTCTime currentTime = timerMgr->getCurrentTime();
    
    // Check if time actually changed
    if (currentTime.getHour() == lastHour && 
        currentTime.getMinutes() == lastMinute && 
        currentTime.getSeconds() == lastSecond) {
      return; // No change, skip update
    }
    
    // Clear all LEDs
    clearAllLEDs();
    
    // Convert to 12-hour format for display
    int displayHour = currentTime.getHour() % 12;
    
    // Set hour LED
    hoursLEDs[displayHour] = CRGB(COLOR_HOURS >> 16, (COLOR_HOURS >> 8) & 0xFF, COLOR_HOURS & 0xFF);
    
    // Set minute LED
    minutesLEDs[currentTime.getMinutes()] = CRGB(COLOR_MINUTES >> 16, (COLOR_MINUTES >> 8) & 0xFF, COLOR_MINUTES & 0xFF);
    
    // Set second LED
    minutesLEDs[currentTime.getSeconds()] = CRGB(COLOR_SECONDS >> 16, (COLOR_SECONDS >> 8) & 0xFF, COLOR_SECONDS & 0xFF);
    
    // Handle overlap (when minute and second are the same)
    if (currentTime.getMinutes() == currentTime.getSeconds()) {
      minutesLEDs[currentTime.getMinutes()] = CRGB(COLOR_OVERLAP >> 16, (COLOR_OVERLAP >> 8) & 0xFF, COLOR_OVERLAP & 0xFF);
    }
    
    // Show the updated LEDs
    FastLED.show();
    
    // Update last displayed values
    lastHour = currentTime.getHour();
    lastMinute = currentTime.getMinutes();
    lastSecond = currentTime.getSeconds();
  }
  
  /**
   * @brief Update hour transition animation
   */
  void updateHourAnimation() {
    unsigned long elapsed = millis() - animationStart;
    
    // Clear all LEDs for animation
    for (int i = 0; i < LED_RING_MINUTES_COUNT; i++) {
      minutesLEDs[i] = CRGB::Black;
    }
    
    // Simple rainbow chase animation
    int numActive = 10; // Nombre de LEDs allumées
    int position = (elapsed / 100) % LED_RING_MINUTES_COUNT; // Position qui avance
    
    for (int i = 0; i < numActive; i++) {
      int ledIndex = (position + i) % LED_RING_MINUTES_COUNT;
      
      // Couleurs simples qui tournent
      if (i < 3) {
        minutesLEDs[ledIndex] = CRGB::Red;
      } else if (i < 6) {
        minutesLEDs[ledIndex] = CRGB::Green;
      } else {
        minutesLEDs[ledIndex] = CRGB::Blue;
      }
    }
    
    FastLED.show();
  }
  
  /**
   * @brief Check and update night mode status
   */
  void updateNightMode() {
    RTCTime currentTime = timerMgr->getCurrentTime();
    bool shouldBeNightMode = (currentTime.getHour() >= NIGHT_MODE_START || 
                             currentTime.getHour() < NIGHT_MODE_END);
    
    if (shouldBeNightMode != nightModeActive) {
      nightModeActive = shouldBeNightMode;
      currentBrightness = nightModeActive ? NIGHT_BRIGHTNESS : 255;
      FastLED.setBrightness(currentBrightness);
      
      DEBUG_PRINT("Night mode ");
      DEBUG_PRINTLN(nightModeActive ? "ON" : "OFF");
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