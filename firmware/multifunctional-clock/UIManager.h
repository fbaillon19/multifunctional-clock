/**
 * @file UIManager.h
 * @brief User Interface Manager for Multifunctional Clock
 * @author Frédéric BAILLON
 * @version 1.0
 * @date 2025
 * 
 * This class handles all user interface interactions including button processing,
 * mode switching, menu navigation, and timeout management. It provides a clean
 * state machine for UI modes and sensor page navigation.
 */

#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "config.h"

/**
 * @class UIManager
 * @brief Manages user interface states and button interactions
 * 
 * The UIManager handles:
 * - Button debouncing and state management
 * - UI mode transitions (Clock, Sensors, Network, Settings)
 * - Sensor page navigation within sensor mode
 * - Automatic timeout return to clock mode
 * - Settings menu navigation
 */
class UIManager {
private:
  // Interface state
  UIMode currentMode;           ///< Current UI display mode
  SensorPage currentSensorPage; ///< Current sensor data page
  
  // Button state management
  bool lastModeButtonState;     ///< Previous state of mode button
  bool lastSelectButtonState;   ///< Previous state of select button
  unsigned long lastModePress;  ///< Timestamp of last mode button press
  unsigned long lastSelectPress; ///< Timestamp of last select button press
  unsigned long lastActivity;   ///< Timestamp of last user activity
  
  // Settings menu state
  int settingsMenuItem;         ///< Current settings menu item
  bool inSettingsEdit;          ///< Whether user is editing a setting

public:
  /**
   * @brief Constructor
   * 
   * Initializes all state variables to default values.
   */
  UIManager() : 
    currentMode(UI_MODE_CLOCK),
    currentSensorPage(SENSOR_PAGE_TEMP_IN),
    lastModeButtonState(HIGH),
    lastSelectButtonState(HIGH),
    lastModePress(0),
    lastSelectPress(0),
    lastActivity(0),
    settingsMenuItem(0),
    inSettingsEdit(false) {}
  
  /**
   * @brief Initialize the UI manager
   * 
   * Sets up button pins and initializes timing variables.
   * 
   * @return true if initialization successful, false otherwise
   */
  bool init() {
    pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
    lastActivity = millis();
    return true;
  }
  
  /**
   * @brief Update UI manager state
   * 
   * Should be called every loop iteration to handle button presses
   * and timeout management.
   */
  void update() {
    handleButtons();
    checkTimeout();
  }
  
  /**
   * @brief Get current UI mode
   * 
   * @return Current UIMode (Clock, Sensors, Network, or Settings)
   */
  UIMode getCurrentMode() const {
    return currentMode;
  }
  
  /**
   * @brief Get current sensor page
   * 
   * @return Current SensorPage being displayed
   */
  SensorPage getSensorPage() const {
    return currentSensorPage;
  }
  
  /**
   * @brief Get current settings menu item
   * 
   * @return Index of current settings menu item
   */
  int getSettingsMenu() const {
    return settingsMenuItem;
  }
  
  /**
   * @brief Check if user is currently editing settings
   * 
   * @return true if in settings edit mode, false otherwise
   */
  bool isInSettingsEdit() const {
    return inSettingsEdit;
  }

private:
  /**
   * @brief Handle button press detection with debouncing
   * 
   * Reads both buttons, applies debouncing, and calls appropriate
   * handlers when valid presses are detected.
   */
  void handleButtons() {
    unsigned long currentTime = millis();
    
    // Mode button handling with debouncing
    bool modePressed = digitalRead(BUTTON_MODE_PIN) == LOW;
    if (modePressed != lastModeButtonState) {
      if (currentTime - lastModePress > BUTTON_DEBOUNCE_DELAY) {
        if (modePressed) {
          onModeButtonPressed();
          lastActivity = currentTime;
        }
        lastModePress = currentTime;
      }
      lastModeButtonState = modePressed;
    }
    
    // Select button handling with debouncing
    bool selectPressed = digitalRead(BUTTON_SELECT_PIN) == LOW;
    if (selectPressed != lastSelectButtonState) {
      if (currentTime - lastSelectPress > BUTTON_DEBOUNCE_DELAY) {
        if (selectPressed) {
          onSelectButtonPressed();
          lastActivity = currentTime;
        }
        lastSelectPress = currentTime;
      }
      lastSelectButtonState = selectPressed;
    }
  }
  
  /**
   * @brief Handle mode button press events
   * 
   * Cycles through UI modes or exits settings edit mode.
   */
  void onModeButtonPressed() {
    DEBUG_PRINTLN("Mode button pressed");
    
    if (inSettingsEdit) {
      // Exit settings edit mode
      inSettingsEdit = false;
      return;
    }
    
    // Cycle to next UI mode
    currentMode = (UIMode)((currentMode + 1) % UI_MODE_COUNT);
    
    // Reset sub-menus when entering new modes
    if (currentMode == UI_MODE_SENSORS) {
      currentSensorPage = SENSOR_PAGE_TEMP_IN;
    } else if (currentMode == UI_MODE_SETTINGS) {
      settingsMenuItem = 0;
    }
    
    DEBUG_PRINT("New mode: ");
    DEBUG_PRINTLN(getModeString(currentMode));
  }
  
  /**
   * @brief Handle select button press events
   * 
   * Behavior depends on current UI mode:
   * - Clock: No action
   * - Sensors: Navigate between sensor pages
   * - Network: Trigger network actions
   * - Settings: Navigate menu or edit values
   */
  void onSelectButtonPressed() {
    DEBUG_PRINTLN("Select button pressed");
    
    switch (currentMode) {
      case UI_MODE_CLOCK:
        // No specific action in clock mode
        break;
        
      case UI_MODE_SENSORS:
        // Cycle through sensor pages
        currentSensorPage = (SensorPage)((currentSensorPage + 1) % SENSOR_PAGE_COUNT);
        DEBUG_PRINT("Sensor page: ");
        DEBUG_PRINTLN(currentSensorPage);
        break;
        
      case UI_MODE_NETWORK:
        // Force WiFi reconnection or data send
        // This action will be handled by NetworkManager
        break;
        
      case UI_MODE_SETTINGS:
        if (inSettingsEdit) {
          // Modify current setting value
          modifyCurrentSetting();
        } else {
          // Navigate to next settings item or enter edit mode
          if (settingsMenuItem < getSettingsCount() - 1) {
            settingsMenuItem++;
          } else {
            settingsMenuItem = 0;
          }
        }
        break;
    }
  }
  
  /**
   * @brief Check for UI timeout and return to clock mode
   * 
   * Automatically returns to clock mode after UI_TIMEOUT period
   * of inactivity to prevent staying in menus indefinitely.
   */
  void checkTimeout() {
    // Auto return to clock mode after inactivity
    if (currentMode != UI_MODE_CLOCK && 
        millis() - lastActivity > UI_TIMEOUT) {
      currentMode = UI_MODE_CLOCK;
      inSettingsEdit = false;
      DEBUG_PRINTLN("Timeout - returning to clock mode");
    }
  }
  
  /**
   * @brief Modify the currently selected setting
   * 
   * Placeholder for settings modification logic.
   * Implementation depends on available settings.
   */
  void modifyCurrentSetting() {
    // TODO: Implement setting modification based on settingsMenuItem
    DEBUG_PRINT("Modifying setting: ");
    DEBUG_PRINTLN(settingsMenuItem);
  }
  
  /**
   * @brief Get total number of configurable settings
   * 
   * @return Number of settings menu items
   */
  int getSettingsCount() const {
    return 5; // Number of configurable parameters
  }
  
  /**
   * @brief Convert UI mode to readable string
   * 
   * @param mode UIMode to convert
   * @return Human-readable string representation of the mode
   */
  const char* getModeString(UIMode mode) const {
    switch (mode) {
      case UI_MODE_CLOCK: return "Clock";
      case UI_MODE_SENSORS: return "Sensors"; 
      case UI_MODE_NETWORK: return "Network";
      case UI_MODE_SETTINGS: return "Settings";
      default: return "Unknown";
    }
  }
};

#endif // UI_MANAGER_H#ifndef UI_MANAGER_H
#define UI_MANAGER_H

#include "config.h"

class UIManager {
private:
  // État de l'interface
  UIMode currentMode;
  SensorPage currentSensorPage;
  
  // Gestion boutons
  bool lastModeButtonState;
  bool lastSelectButtonState;
  unsigned long lastModePress;
  unsigned long lastSelectPress;
  unsigned long lastActivity;
  
  // État menu paramètres
  int settingsMenuItem;
  bool inSettingsEdit;
  
public:
  UIManager() : 
    currentMode(UI_MODE_CLOCK),
    currentSensorPage(SENSOR_PAGE_TEMP_IN),
    lastModeButtonState(HIGH),
    lastSelectButtonState(HIGH),
    lastModePress(0),
    lastSelectPress(0),
    lastActivity(0),
    settingsMenuItem(0),
    inSettingsEdit(false) {}
  
  bool init() {
    pinMode(BUTTON_MODE_PIN, INPUT_PULLUP);
    pinMode(BUTTON_SELECT_PIN, INPUT_PULLUP);
    lastActivity = millis();
    return true;
  }
  
  void update() {
    handleButtons();
    checkTimeout();
  }
  
  UIMode getCurrentMode() const {
    return currentMode;
  }
  
  SensorPage getSensorPage() const {
    return currentSensorPage;
  }
  
  int getSettingsMenu() const {
    return settingsMenuItem;
  }
  
  bool isInSettingsEdit() const {
    return inSettingsEdit;
  }
  
private:
  void handleButtons() {
    unsigned long currentTime = millis();
    
    // Lecture bouton Mode avec anti-rebond
    bool modePressed = digitalRead(BUTTON_MODE_PIN) == LOW;
    if (modePressed != lastModeButtonState) {
      if (currentTime - lastModePress > BUTTON_DEBOUNCE_DELAY) {
        if (modePressed) {
          onModeButtonPressed();
          lastActivity = currentTime;
        }
        lastModePress = currentTime;
      }
      lastModeButtonState = modePressed;
    }
    
    // Lecture bouton Select avec anti-rebond
    bool selectPressed = digitalRead(BUTTON_SELECT_PIN) == LOW;
    if (selectPressed != lastSelectButtonState) {
      if (currentTime - lastSelectPress > BUTTON_DEBOUNCE_DELAY) {
        if (selectPressed) {
          onSelectButtonPressed();
          lastActivity = currentTime;
        }
        lastSelectPress = currentTime;
      }
      lastSelectButtonState = selectPressed;
    }
  }
  
  void onModeButtonPressed() {
    DEBUG_PRINTLN("Bouton Mode pressé");
    
    if (inSettingsEdit) {
      // Sortir du mode édition
      inSettingsEdit = false;
      return;
    }
    
    // Passer au mode suivant
    currentMode = (UIMode)((currentMode + 1) % UI_MODE_COUNT);
    
    // Réinitialiser les sous-menus
    if (currentMode == UI_MODE_SENSORS) {
      currentSensorPage = SENSOR_PAGE_TEMP_IN;
    } else if (currentMode == UI_MODE_SETTINGS) {
      settingsMenuItem = 0;
    }
    
    DEBUG_PRINT("Nouveau mode: ");
    DEBUG_PRINTLN(getModeString(currentMode));
  }
  
  void onSelectButtonPressed() {
    DEBUG_PRINTLN("Bouton Select pressé");
    
    switch (currentMode) {
      case UI_MODE_CLOCK:
        // Pas d'action particulière en mode horloge
        break;
        
      case UI_MODE_SENSORS:
        // Parcourir les pages de capteurs
        currentSensorPage = (SensorPage)((currentSensorPage + 1) % SENSOR_PAGE_COUNT);
        DEBUG_PRINT("Page capteur: ");
        DEBUG_PRINTLN(currentSensorPage);
        break;
        
      case UI_MODE_NETWORK:
        // Forcer une reconnexion WiFi ou un envoi de données
        // Cette action sera gérée par le NetworkManager
        break;
        
      case UI_MODE_SETTINGS:
        if (inSettingsEdit) {
          // Modifier la valeur du paramètre
          modifyCurrentSetting();
        } else {
          // Entrer en mode édition ou passer au paramètre suivant
          if (settingsMenuItem < getSettingsCount() - 1) {
            settingsMenuItem++;
          } else {
            settingsMenuItem = 0;
          }
        }
        break;
    }
  }
  
  void checkTimeout() {
    // Retour automatique au mode horloge après inactivité
    if (currentMode != UI_MODE_CLOCK && 
        millis() - lastActivity > UI_TIMEOUT) {
      currentMode = UI_MODE_CLOCK;
      inSettingsEdit = false;
      DEBUG_PRINTLN("Timeout - retour mode horloge");
    }
  }
  
  void modifyCurrentSetting() {
    // À implémenter selon les paramètres disponibles
    DEBUG_PRINT("Modification paramètre: ");
    DEBUG_PRINTLN(settingsMenuItem);
  }
  
  int getSettingsCount() const {
    return 5; // Nombre de paramètres configurables
  }
  
  const char* getModeString(UIMode mode) const {
    switch (mode) {
      case UI_MODE_CLOCK: return "Horloge";
      case UI_MODE_SENSORS: return "Capteurs"; 
      case UI_MODE_NETWORK: return "Réseau";
      case UI_MODE_SETTINGS: return "Paramètres";
      default: return "Inconnu";
    }
  }
};

#endif // UI_MANAGER_H