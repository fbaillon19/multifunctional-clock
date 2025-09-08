/**
 * @file ds3231_basic_test.ino
 * @brief Basic validation test for DS3231 RTC module
 * @author Frédéric BAILLON
 * @date 2025
 * 
 * This test validates:
 * - I2C communication with DS3231
 * - Time reading functionality
 * - Timing precision vs internal RTC
 */

#include <Wire.h>
#include <RTClib.h>

RTC_DS3231 rtc;
unsigned long lastPrint = 0;
int secondCount = 0;

void setup() {
  Serial.begin(115200);
  while (!Serial && millis() < 3000);
  
  Serial.println("=== Test DS3231 RTC Module ===");
  
  if (!rtc.begin()) {
    Serial.println("ERREUR: DS3231 non détecté sur I2C");
    Serial.println("Vérifiez:");
    Serial.println("- Câblage SDA/SCL");
    Serial.println("- Alimentation 3.3V");
    while (1) delay(1000);
  }
  
  Serial.println("DS3231 détecté avec succès");
  
  // Régler l'heure de compilation si nécessaire
  if (rtc.lostPower()) {
    Serial.println("DS3231 sans alimentation, réglage heure de compilation...");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  // Afficher la température du DS3231 (fonctionnalité bonus)
  Serial.print("Température DS3231: ");
  Serial.print(rtc.getTemperature());
  Serial.println("°C");
  
  Serial.println("Début du test de précision (60 secondes):");
  lastPrint = millis();
}

void loop() {
  unsigned long currentTime = millis();
  
  // Afficher l'heure chaque seconde
  if (currentTime - lastPrint >= 1000) {
    DateTime now = rtc.now();
    
    Serial.print("DS3231: ");
    if (now.hour() < 10) Serial.print("0");
    Serial.print(now.hour());
    Serial.print(":");
    if (now.minute() < 10) Serial.print("0");
    Serial.print(now.minute());
    Serial.print(":");
    if (now.second() < 10) Serial.print("0");
    Serial.print(now.second());
    
    // Afficher aussi le timestamp millis pour comparaison
    Serial.print(" (millis: ");
    Serial.print(currentTime);
    Serial.println(")");
    
    lastPrint = currentTime;
    secondCount++;
    
    // Test de précision après 60 secondes
    if (secondCount == 60) {
      Serial.println("=== Test de 60 secondes terminé ===");
      Serial.println("Vérifiez que 60 secondes DS3231 = 60 secondes réelles");
      secondCount = 0; // Recommencer
    }
  }
}