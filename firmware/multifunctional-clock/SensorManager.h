/**
 * @file SensorManager.h
 * @brief Simplified Sensor Manager for initial testing
 * @author Your Name
 * @version 1.0
 * @date 2025
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include "config.h"

/**
 * @struct SensorData
 * @brief Structure to hold all sensor readings
 */
struct SensorData {
  float tempIndoor;      ///< Indoor temperature (°C)
  float tempOutdoor;     ///< Outdoor temperature (°C)
  float humidityIndoor;  ///< Indoor humidity (%)
  float humidityOutdoor; ///< Outdoor humidity (%)
  float pressure;        ///< Atmospheric pressure (hPa)
  int airQuality;        ///< Air quality (PPM)
  bool isValid;          ///< Whether readings are valid
};

/**
 * @class SensorManager
 * @brief Simplified sensor manager for testing
 */
class SensorManager {
private:
  SensorData currentData;
  unsigned long lastReading;

public:
  /**
   * @brief Constructor
   */
  SensorManager() : lastReading(0) {
    // Initialize with test data
    currentData.tempIndoor = 22.5;
    currentData.tempOutdoor = 15.3;
    currentData.humidityIndoor = 45.0;
    currentData.humidityOutdoor = 65.0;
    currentData.pressure = 1013.25;
    currentData.airQuality = 75;
    currentData.isValid = true;
  }
  
  /**
   * @brief Initialize sensor manager
   */
  bool init() {
    DEBUG_PRINTLN("Initializing SensorManager...");
    lastReading = millis();
    DEBUG_PRINTLN("SensorManager initialized (test mode)");
    return true;
  }
  
  /**
   * @brief Update sensor readings
   */
  void update() {
    unsigned long currentTime = millis();
    
    if (currentTime - lastReading >= SENSOR_READ_INTERVAL) {
      // Simulate sensor readings with small variations
      currentData.tempIndoor += (random(-10, 11) / 10.0);
      currentData.tempOutdoor += (random(-10, 11) / 10.0);
      currentData.humidityIndoor += (random(-5, 6) / 10.0);
      currentData.humidityOutdoor += (random(-5, 6) / 10.0);
      currentData.pressure += (random(-10, 11) / 10.0);
      currentData.airQuality += random(-5, 6);
      
      // Keep values in reasonable ranges
      currentData.tempIndoor = constrain(currentData.tempIndoor, 18.0, 28.0);
      currentData.tempOutdoor = constrain(currentData.tempOutdoor, 10.0, 25.0);
      currentData.humidityIndoor = constrain(currentData.humidityIndoor, 30.0, 70.0);
      currentData.humidityOutdoor = constrain(currentData.humidityOutdoor, 40.0, 90.0);
      currentData.pressure = constrain(currentData.pressure, 980.0, 1040.0);
      currentData.airQuality = constrain(currentData.airQuality, 30, 150);
      
      lastReading = currentTime;
      
      DEBUG_PRINT("Sensors updated - Temp: ");
      DEBUG_PRINT(currentData.tempIndoor);
      DEBUG_PRINT("°C, Air Quality: ");
      DEBUG_PRINTLN(currentData.airQuality);
    }
  }
  
  /**
   * @brief Get all sensor data
   */
  SensorData getAllData() const {
    return currentData;
  }
  
  /**
   * @brief Get air quality reading
   */
  int getAirQuality() const {
    return currentData.airQuality;
  }
};

#endif // SENSOR_MANAGER_H