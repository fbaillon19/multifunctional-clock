#ifndef CONFIG_H
#define CONFIG_H

// ===========================================
// CONFIGURATION HARDWARE
// ===========================================

// Pins LED
#define LED_RING_MINUTES_PIN    6   // 60 LEDs pour minutes/secondes
#define LED_RING_HOURS_PIN      7   // 12 LEDs pour heures  
#define LED_STRIP_AIR_PIN       8   // 10 LEDs qualité air

// Nombre de LEDs
#define LED_RING_MINUTES_COUNT  60
#define LED_RING_HOURS_COUNT    12
#define LED_STRIP_AIR_COUNT     10

// Pins capteurs
#define DHT22_INDOOR_PIN        2
#define DHT22_OUTDOOR_PIN       3
#define BMP180_SDA_PIN         A4
#define BMP180_SCL_PIN         A5
#define MQ135_PIN              A0

// Pins interface utilisateur
#define BUTTON_MODE_PIN         4
#define BUTTON_SELECT_PIN       5

// LCD I2C
#define LCD_I2C_ADDRESS      0x27
#define LCD_COLS                20
#define LCD_ROWS                 4

// ===========================================
// CONFIGURATION TEMPORELLE
// ===========================================

#define SENSOR_READ_INTERVAL    30000UL  // 30 secondes
#define NETWORK_SYNC_INTERVAL   86400000UL // 24 heures
#define BUTTON_DEBOUNCE_DELAY   50       // 50ms
#define ANIMATION_SPEED         100      // ms entre frames

// Synchronisation NTP
#define NTP_SERVER           "pool.ntp.org"
#define TIMEZONE_OFFSET      1           // UTC+1 (France)
#define DST_OFFSET           1           // Heure d'été

// ===========================================
// CONFIGURATION WIFI
// ===========================================

#define WIFI_SSID            "VotreSSID"
#define WIFI_PASSWORD        "VotreMotDePasse"
#define WIFI_TIMEOUT         10000       // 10 secondes

// API Web
#define WEB_SERVER_PORT      80
#define API_ENDPOINT         "/api/data"
#define WEB_UPDATE_INTERVAL  300000UL    // 5 minutes

// ===========================================
// CONFIGURATION COULEURS LED
// ===========================================

// Couleurs horloge (format RGB)
#define COLOR_SECONDS        0xFF0000    // Rouge
#define COLOR_MINUTES        0x00FF00    // Vert  
#define COLOR_HOURS          0x0000FF    // Bleu
#define COLOR_OVERLAP        0xFFFF00    // Jaune (superposition)

// Couleurs qualité air
#define COLOR_AIR_EXCELLENT  0x00FF00    // Vert
#define COLOR_AIR_GOOD       0x80FF00    // Vert-jaune
#define COLOR_AIR_MODERATE   0xFFFF00    // Jaune
#define COLOR_AIR_POOR       0xFF8000    // Orange
#define COLOR_AIR_UNHEALTHY  0xFF0000    // Rouge
#define COLOR_AIR_DANGEROUS  0x800080    // Violet

// Mode nuit
#define NIGHT_MODE_START     22          // 22h00
#define NIGHT_MODE_END       7           // 07h00
#define NIGHT_BRIGHTNESS     50          // 50/255

// ===========================================
// CONFIGURATION CAPTEURS
// ===========================================

// Seuils qualité air (PPM)
#define AIR_EXCELLENT_MAX    50
#define AIR_GOOD_MAX         100
#define AIR_MODERATE_MAX     200
#define AIR_POOR_MAX         300
#define AIR_UNHEALTHY_MAX    500

// Calibration MQ135
#define MQ135_RZERO          76.63       // À calibrer
#define MQ135_PARA           116.6020682
#define MQ135_PARB           2.769034857

// Limites capteurs
#define TEMP_MIN             -40.0
#define TEMP_MAX             80.0
#define HUMIDITY_MIN         0.0
#define HUMIDITY_MAX         100.0

// ===========================================
// CONFIGURATION INTERFACE
// ===========================================

// Modes d'affichage
enum UIMode {
  UI_MODE_CLOCK = 0,
  UI_MODE_SENSORS,
  UI_MODE_NETWORK, 
  UI_MODE_SETTINGS,
  UI_MODE_COUNT
};

// Pages capteurs
enum SensorPage {
  SENSOR_PAGE_TEMP_IN = 0,
  SENSOR_PAGE_TEMP_OUT,
  SENSOR_PAGE_PRESSURE,
  SENSOR_PAGE_AIR_QUALITY,
  SENSOR_PAGE_COUNT
};

// Timeouts interface
#define UI_TIMEOUT           30000       // Retour auto au mode horloge
#define MENU_BLINK_INTERVAL  500         // Clignotement sélection

// ===========================================
// DEBUG CONFIGURATION
// ===========================================

#define DEBUG_MODE           true        ///< Enable/disable debug output
#define DEBUG_SENSORS        true        ///< Enable sensor debug messages
#define DEBUG_NETWORK        true        ///< Enable network debug messages
#define DEBUG_LEDS           false       ///< Enable LED debug messages

/**
 * @brief Debug print macros
 * 
 * These macros can be enabled/disabled at compile time to control
 * debug output without affecting performance in production.
 */
#if DEBUG_MODE
  #define DEBUG_PRINT(x)     Serial.print(x)     ///< Debug print macro
  #define DEBUG_PRINTLN(x)   Serial.println(x)   ///< Debug println macro
#else
  #define DEBUG_PRINT(x)      ///< Disabled debug print
  #define DEBUG_PRINTLN(x)    ///< Disabled debug println
#endif

#endif // CONFIG_H