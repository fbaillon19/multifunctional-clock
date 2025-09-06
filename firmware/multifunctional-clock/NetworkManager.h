/**
 * @file NetworkManager.h
 * @brief Simplified Network Manager for initial testing
 * @author Your Name
 * @version 1.0
 * @date 2025
 */

#ifndef NETWORK_MANAGER_H
#define NETWORK_MANAGER_H

#include "config.h"
#include "SensorManager.h"

/**
 * @class NetworkManager
 * @brief Simplified network manager for testing
 */
class NetworkManager {
private:
  bool connected;
  unsigned long lastConnectionCheck;

public:
  /**
   * @brief Constructor
   */
  NetworkManager() : connected(false), lastConnectionCheck(0) {}
  
  /**
   * @brief Initialize network manager
   */
  bool init() {
    DEBUG_PRINTLN("Initializing NetworkManager...");
    
    // Simulate network connection attempt
    connected = true; // For testing, assume we're connected
    lastConnectionCheck = millis();
    
    DEBUG_PRINTLN("NetworkManager initialized (test mode - simulated connection)");
    return true;
  }
  
  /**
   * @brief Check if connected to network
   */
  bool isConnected() const {
    return connected;
  }
  
  /**
   * @brief Get network status
   */
  int getStatus() const {
    return connected ? 1 : 0;
  }
  
  /**
   * @brief Send sensor data to remote server
   */
  bool sendSensorData(SensorData data) {
    if (!connected) {
      DEBUG_PRINTLN("Cannot send data - not connected");
      return false;
    }
    
    // Simulate data sending
    static unsigned long lastDataSend = 0;
    if (millis() - lastDataSend > 30000) { // Every 30 seconds
      DEBUG_PRINTLN("Sending sensor data to server (simulated)");
      DEBUG_PRINT("  Temperature: ");
      DEBUG_PRINT(data.tempIndoor);
      DEBUG_PRINT("°C, Air Quality: ");
      DEBUG_PRINT(data.airQuality);
      DEBUG_PRINTLN(" PPM");
      lastDataSend = millis();
    }
    
    return true;
  }
  
  /**
   * @brief Update network status
   */
  void update() {
    unsigned long currentTime = millis();
    
    // Check connection status periodically
    if (currentTime - lastConnectionCheck > 60000) { // Every minute
      // In real implementation, check actual WiFi status
      DEBUG_PRINTLN("Network status check (simulated - OK)");
      lastConnectionCheck = currentTime;
    }
  }
};

#endif // NETWORK_MANAGER_H