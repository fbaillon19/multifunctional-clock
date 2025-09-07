/**
 * @file TimerManager.h
 * @brief Hardware timer management using RTC for Multifunctional Clock
 * @author Your Name
 * @version 1.0
 * @date 2025
 * 
 * This class handles precise timing using the Arduino UNO R4 WiFi's built-in RTC.
 * It provides hardware-based interrupts for accurate time keeping independent of
 * main loop processing delays.
 */

#ifndef TIMER_MANAGER_H
#define TIMER_MANAGER_H

#include "config.h"
#include "RTC.h"

/**
 * @class TimerManager
 * @brief Manages hardware-based timing using RTC interrupts
 * 
 * The TimerManager provides:
 * - Hardware RTC-based 1-second interrupts
 * - Time synchronization and management
 * - Animation and display state management
 * - Precise timing independent of main loop delays
 */
class TimerManager {
private:
  // Interrupt flags
  volatile bool animationActive;   ///< Whether hour animation is playing
  volatile int animationTimer;     ///< Animation duration counter
  volatile int displayTimer;       ///< Temporary display timer
  
  // Current time (managed by RTC)
  RTCTime currentTime;            ///< Current time from RTC
  bool timeInitialized;           ///< Whether RTC time has been set

public:
  volatile bool secondTickFlag;    ///< Set by RTC interrupt every second
  /**
   * @brief Constructor
   */
  TimerManager() : 
    secondTickFlag(false),
    animationActive(false),
    animationTimer(0),
    displayTimer(0),
    timeInitialized(false) {}
  
  /**
   * @brief Initialize the timer manager
   * 
   * Sets up RTC and configures 1-second periodic interrupt.
   * 
   * @return true if initialization successful, false otherwise
   */
  bool init() {
    DEBUG_PRINTLN("Initialisation de TimerManager...");
    
    // Initialize the RTC
    if (!RTC.begin()) {
      DEBUG_PRINTLN("ERREUR: Impossible d'initialiser le RTC");
      return false;
    }
    
    // Set up 1-second periodic callback
    if (!RTC.setPeriodicCallback(rtcCallback, Period::ONCE_EVERY_1_SEC)) {
      DEBUG_PRINTLN("ERREUR: Impossible de configurer le callback RTC");
      return false;
    }
    
    // Set initial time (will be updated by NTP later)
    // Set initial time using millis() to approximate current time
    unsigned long bootTime = millis() / 1000; // Seconds since boot
    // Assume system started around current time (approximation for testing)
    unsigned long estimatedEpoch = 1736000000 + bootTime; // Approximation janvier 2025
    RTCTime initialTime(estimatedEpoch);
    RTC.setTime(initialTime);
    
    DEBUG_PRINTLN("TimerManager initialisé avec succès");
    return true;
  }
  
  /**
   * @brief Update timer state
   * 
   * Should be called in main loop to process timer events.
   */
  void update() {
    // Process second tick if flag is set
    if (secondTickFlag) {
      // Get current time from RTC
      RTC.getTime(currentTime);
      
      // Reset the flag
      secondTickFlag = false;

      DEBUG_PRINT("RTC Tick: ");
      DEBUG_PRINT(currentTime.getHour());
      DEBUG_PRINT(":");
      DEBUG_PRINT(currentTime.getMinutes());
      DEBUG_PRINT(":");
      DEBUG_PRINTLN(currentTime.getSeconds());
    } else {
      // Debug: pourquoi pas de tick ?
      static unsigned long lastDebug = 0;
      if (millis() - lastDebug > 5000) {
        DEBUG_PRINTLN("Pas de tick RTC reçu");
        lastDebug = millis();
      }
    }
          
    // Handle animation timing
    if (animationActive && animationTimer > 0) {
      animationTimer--;
      if (animationTimer <= 0) {
        animationActive = false;
        DEBUG_PRINTLN("Animation completed");
      }
    }
    
    // Handle temporary display timing
    if (displayTimer > 0) {
      displayTimer--;
    }
  }
  
  /**
   * @brief Check if a second tick occurred
   * 
   * @return true if RTC generated a new second tick
   */
  bool hasSecondTick() {
    return secondTickFlag;
  }
  
  /**
   * @brief Get current time from RTC
   * 
   * @return RTCTime object with current time
   */
  RTCTime getCurrentTime() {
    RTC.getTime(currentTime);
    return currentTime;
  }
  
  /**
   * @brief Set time from NTP epoch timestamp
   * 
   * @param epochTime Unix timestamp from NTP
   * @param timezoneOffset Timezone offset in hours
   */
  void setTimeFromEpoch(unsigned long epochTime, int timezoneOffset = 1) {
    // Apply timezone offset
    epochTime += timezoneOffset * 3600;
    
    // Create RTCTime from epoch
    RTCTime newTime(epochTime);
    RTC.setTime(newTime);
    
    timeInitialized = true;
    
    DEBUG_PRINT("Time set from NTP: ");
    DEBUG_PRINT(newTime.getHour());
    DEBUG_PRINT(":");
    DEBUG_PRINT(newTime.getMinutes());
    DEBUG_PRINT(":");
    DEBUG_PRINTLN(newTime.getSeconds());
  }
  
  /**
   * @brief Check if time has been initialized
   * 
   * @return true if time has been set via NTP or manually
   */
  bool isTimeValid() const {
    return timeInitialized;
  }
  
  /**
   * @brief Start hour animation
   * 
   * @param durationMs Animation duration in milliseconds
   */
  void startAnimation(int durationMs = 5000) {
    animationActive = true;
    animationTimer = durationMs / 100; // Convert to 100ms ticks
    DEBUG_PRINTLN("Starting hour animation");
  }
  
  /**
   * @brief Check if animation is currently active
   * 
   * @return true if animation is playing
   */
  bool isAnimationActive() const {
    return animationActive;
  }
  
  /**
   * @brief Get animation progress
   * 
   * @return Animation timer value (decreases to 0)
   */
  int getAnimationTimer() const {
    return animationTimer;
  }
  
  /**
   * @brief Start temporary display timer
   * 
   * @param durationMs Display duration in milliseconds
   */
  void startDisplayTimer(int durationMs = 4000) {
    displayTimer = durationMs / 100; // Convert to 100ms ticks
  }
  
  /**
   * @brief Check if display timer is active
   * 
   * @return true if temporary display is active
   */
  bool isDisplayTimerActive() const {
    return displayTimer > 0;
  }
  
  /**
   * @brief Get display timer value
   * 
   * @return Display timer value (decreases to 0)
   */
  int getDisplayTimer() const {
    return displayTimer;
  }
  
  /**
   * @brief Check if it's time for sensor readings
   * 
   * Sensors should be read every 2 seconds to avoid overload.
   * 
   * @return true if sensors should be read now
   */
  bool shouldReadSensors() {
    RTC.getTime(currentTime);
    return (currentTime.getSeconds() % 2) == 0;
  }
  
  /**
   * @brief Check if it's time for NTP sync
   * 
   * NTP sync should happen daily at midnight.
   * 
   * @return true if NTP sync should be performed
   */
  bool shouldSyncNTP() {
    RTC.getTime(currentTime);
    return (currentTime.getHour() == 0 && 
            currentTime.getMinutes() == 0 && 
            currentTime.getSeconds() == 0);
  }
  
  /**
   * @brief Check if hour animation should start
   * 
   * Animation triggers at the top of every hour.
   * 
   * @return true if hour animation should start
   */
  bool shouldStartHourAnimation() {
    RTC.getTime(currentTime);
    return (currentTime.getMinutes() == 0 && 
            currentTime.getSeconds() == 0 && 
            !animationActive);
  }

private:
  /**
   * @brief RTC interrupt callback function
   * 
   * This function is called by the RTC hardware every second.
   * It sets flags that are processed in the main loop.
   */
  static void rtcCallback() {
    // Solution : utiliser une variable statique globale
    static volatile bool* flagPtr = nullptr;
    
    if (flagPtr == nullptr) {
      // Première utilisation, chercher l'instance
      extern TimerManager timerMgr;
      flagPtr = &timerMgr.secondTickFlag;
    }
    
    // Simplement activer le flag
    *flagPtr = true;
  }
};

#endif // TIMER_MANAGER_H