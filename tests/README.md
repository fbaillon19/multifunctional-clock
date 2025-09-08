# Tests de validation hardware

Ce dossier contient les tests de validation pour chaque composant du projet.

## DS3231 RTC Module

**Fichier:** `ds3231_basic_test/`
**Objectif:** Valider communication I2C et précision du timing
**Prérequis:** Librairie RTClib installée

## DHT22 Sensors

**Fichier:** `dht22_test/`
**Objectif:** Valider lecture température/humidité

## LED Strips

**Fichier:** `led_strips_test/`
**Objectif:** Valider contrôle des rubans WS2812B

## Instructions

1. Ouvrir le test souhaité dans Arduino IDE
2. Installer les librairies requises
3. Connecter le hardware selon le schéma
4. Upload et vérifier les résultats dans Serial Monitor