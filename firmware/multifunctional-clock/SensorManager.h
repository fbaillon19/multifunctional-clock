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

class TestManager; // Déclaration forward

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
  void applyTestValues();

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
      // Valeurs de base
      currentData.tempIndoor += (random(-10, 11) / 10.0);
      currentData.tempOutdoor += (random(-10, 11) / 10.0);
      currentData.airQuality += random(-5, 6);
      
      // Appliquer les valeurs de test via une fonction externe
      applyTestValues();
      
      // Contraintes normales
      currentData.tempIndoor = constrain(currentData.tempIndoor, -20.0, 50.0);
      currentData.tempOutdoor = constrain(currentData.tempOutdoor, -20.0, 50.0);
      currentData.airQuality = constrain(currentData.airQuality, 10, 500);
      
      lastReading = currentTime;
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