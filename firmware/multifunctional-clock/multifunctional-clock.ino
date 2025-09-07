#include "config.h"
#include "ClockManager.h"
#include "SensorManager.h"
#include "DisplayManager.h"
#include "NetworkManager.h"
#include "UIManager.h"
#include "TimerManager.h"

// Gestionnaires principaux
TimerManager timerMgr;
ClockManager clockMgr(&timerMgr);
SensorManager sensorMgr;
DisplayManager displayMgr;
NetworkManager networkMgr;
UIManager uiMgr;

// Variables globales d'état
unsigned long lastSensorRead = 0;
unsigned long lastDisplayUpdate = 0;
unsigned long lastNetworkSync = 0;

void setup() {
  Serial.begin(115200);
  
  // Initialisation séquentielle avec gestion d'erreurs
  Serial.println("=== Horloge Multifonctions v1.0 ===");
  
  if (!timerMgr.init()) {
    Serial.println("ERREUR: Impossible d'initialiser le timer");
    while(1) delay(1000);
  }

  if (!displayMgr.init()) {
    Serial.println("ERREUR: Impossible d'initialiser l'affichage");
    while(1) delay(1000); // Arrêt critique
  }
  
  displayMgr.showBootMessage("Initialisation...");
  
  if (!sensorMgr.init()) {
    Serial.println("ATTENTION: Capteurs non disponibles");
    displayMgr.showBootMessage("Capteurs: ERREUR");
    delay(2000);
  }
  
  if (!clockMgr.init()) {
    Serial.println("ERREUR: Impossible d'initialiser l'horloge");
    displayMgr.showBootMessage("Horloge: ERREUR");
    while(1) delay(1000);
  }
  
  if (!networkMgr.init()) {
    Serial.println("ATTENTION: WiFi non disponible");
    displayMgr.showBootMessage("WiFi: ERREUR");
    delay(2000);
  } else {
    displayMgr.showBootMessage("Synchronisation...");
    clockMgr.syncWithNTP();
  }
  
  if (!uiMgr.init()) {
    Serial.println("ATTENTION: Interface utilisateur limitée");
  }
  
  displayMgr.showBootMessage("Prêt !");
  delay(1000);
  
  Serial.println("Initialisation terminée");
}

void loop() {
  unsigned long currentTime = millis();
  
  // Gestion de l'interface utilisateur (priorité haute)
  uiMgr.update();
  
  // Mise à jour de l'horloge (chaque seconde)
  if (currentTime - lastDisplayUpdate >= 1000) {
    clockMgr.update();
    lastDisplayUpdate = currentTime;
  }
  
  // Lecture des capteurs (toutes les 30 secondes)
  if (currentTime - lastSensorRead >= SENSOR_READ_INTERVAL) {
    sensorMgr.update();
    lastSensorRead = currentTime;
  }
  
  // Synchronisation réseau (une fois par jour)
  if (currentTime - lastNetworkSync >= NETWORK_SYNC_INTERVAL) {
    if (networkMgr.isConnected()) {
      clockMgr.syncWithNTP();
      networkMgr.sendSensorData(sensorMgr.getAllData());
    }
    lastNetworkSync = currentTime;
  }
  
  // Mise à jour de l'affichage selon le mode UI
  updateDisplay();
  
  // Petite pause pour éviter la saturation du processeur
//  delay(50);
}

/**
 * @brief Update display based on current UI mode
 * 
 * Routes display updates to appropriate functions based on the current
 * user interface mode. Also handles air quality LED which is always visible.
 */
void updateDisplay() {
  UIMode currentMode = uiMgr.getCurrentMode();
  
  switch (currentMode) {
    case UI_MODE_CLOCK:
      displayMgr.showClock(clockMgr.getCurrentTime());
      displayMgr.updateLEDClock(clockMgr.getCurrentTime());
      break;
      
    case UI_MODE_SENSORS:
      displayMgr.showSensorData(
        sensorMgr.getAllData(), 
        uiMgr.getSensorPage()
      );
      break;
      
    case UI_MODE_NETWORK:
      displayMgr.showNetworkInfo(networkMgr.getStatus());
      break;
      
    case UI_MODE_SETTINGS:
      displayMgr.showSettings(uiMgr.getSettingsMenu());
      break;
  }
  
  // Air quality LED update (always visible)
  displayMgr.updateAirQualityLED(sensorMgr.getAirQuality());
}