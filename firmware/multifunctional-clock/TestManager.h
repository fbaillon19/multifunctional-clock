// Nouveau fichier : TestManager.h
/**
 * @file TestManager.h
 * @brief Test framework for accelerated development and debugging
 */

#ifndef TEST_MANAGER_H
#define TEST_MANAGER_H

#include "config.h"

class TestManager {
private:
  bool testModeActive;
  int timeAccelerationFactor;
  unsigned long testStartTime;
  int testSequenceStep;

public:
  TestManager() : 
    testModeActive(false), 
    timeAccelerationFactor(1),
    testStartTime(0),
    testSequenceStep(0) {}

  bool init() {
    DEBUG_PRINTLN("TestManager initialized");
    return true;
  }

  void enableTestMode(bool enable = true) {
    testModeActive = enable;
    if (enable) {
      DEBUG_PRINTLN("=== MODE TEST ACTIVÉ ===");
    } else {
      DEBUG_PRINTLN("Mode test désactivé");
    }
  }

  bool isTestMode() const { return testModeActive; }

  // Test rapide animation (chaque minute au lieu d'heure)
  bool shouldTriggerHourAnimation(int minutes, int seconds) {
    if (testModeActive) {
      return (seconds == 0); // Chaque minute en mode test
    } else {
      return (minutes == 0 && seconds == 0); // Chaque heure en mode normal
    }
  }

  // Simulation de valeurs capteurs extrêmes
  float getTestTemperature(float normalValue) {
    if (!testModeActive) return normalValue;
    
    // Cycle entre différentes valeurs de test
    unsigned long cycle = (millis() / 10000) % 4; // Change toutes les 10s
    switch(cycle) {
      case 0: return -10.0; // Très froid
      case 1: return 45.0;  // Très chaud
      case 2: return 0.0;   // Zéro
      case 3: return normalValue; // Normal
    }
    return normalValue;
  }

  int getTestAirQuality(int normalValue) {
    if (!testModeActive) return normalValue;
    
    unsigned long cycle = (millis() / 8000) % 5;
    switch(cycle) {
      case 0: return 25;   // Excellent
      case 1: return 150;  // Modéré
      case 2: return 250;  // Mauvais
      case 3: return 450;  // Très mauvais
      case 4: return normalValue; // Normal
    }
    return normalValue;
  }

  void printTestStatus() {
    if (testModeActive) {
      DEBUG_PRINT("MODE TEST - Cycle: ");
      DEBUG_PRINTLN((millis() / 5000) % 10);
    }
  }
};

#endif