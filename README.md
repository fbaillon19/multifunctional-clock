# Multifunctional Clock 🕐

A WiFi-enabled multifunctional clock displaying time with LED rings and environmental data on LCD screen.

![Clock in action](docs/images/clock_demo.gif)

## ✨ Features

- **Visual Time Display**: 60-LED ring for minutes/seconds, 12-LED ring for hours
- **Environmental Monitoring**: Indoor/outdoor temperature, humidity, air pressure, air quality
- **WiFi Connectivity**: NTP time synchronization and data logging
- **Multiple Display Modes**: Clock, sensors, network status, settings
- **User Interface**: Two-button navigation with LCD menu system
- **Night Mode**: Automatic brightness adjustment
- **Air Quality Indicator**: Real-time color-coded LED strip

## 🛠️ Hardware Requirements

### Main Components
- Arduino UNO R4 WiFi
- LCD 20x4 with I2C backpack
- 3x WS2812B LED strips (60, 12, and 10 LEDs)
- 5V 5A power supply

### Sensors
- 2x DHT22 (indoor/outdoor temperature & humidity)
- BMP180 (atmospheric pressure)
- MQ135 (air quality)

### Interface
- 2x Push buttons
- Breadboard and jumper wires

See [Hardware Guide](docs/HARDWARE.md) for detailed wiring diagrams.

## 📋 Software Requirements

- Arduino IDE 2.0+ or PlatformIO
- Required libraries (see [Installation Guide](docs/INSTALLATION.md))

## 🚀 Quick Start

### 1. Hardware Setup
1. Follow the [wiring diagram](hardware/wiring_diagram.png)
2. Connect all components according to pin definitions in `config.h`
3. Ensure 5V power supply can handle LED current draw

### 2. Software Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/multifunctional-clock.git
   cd multifunctional-clock
   ```

2. Copy configuration template:
   ```bash
   cp firmware/secrets.h.template firmware/secrets.h
   ```

3. Edit `secrets.h` with your WiFi credentials:
   ```cpp
   #define WIFI_SSID "Your_WiFi_Name"
   #define WIFI_PASSWORD "Your_WiFi_Password"
   ```

4. Install required libraries (see [Installation Guide](docs/INSTALLATION.md))

5. Upload firmware to Arduino UNO R4 WiFi

### 3. First Boot
1. Power on the device
2. Watch initialization sequence on LCD
3. Clock will sync time via WiFi
4. Use buttons to navigate between modes

## 🎮 Usage

### Button Controls
- **Mode Button**: Cycle through display modes (Clock → Sensors → Network → Settings)
- **Select Button**: Navigate within current mode or modify settings

### Display Modes
- **Clock Mode**: Shows time on LED rings with basic info on LCD
- **Sensor Mode**: Detailed environmental data (temperature, humidity, pressure, air quality)
- **Network Mode**: WiFi status, IP address, connection quality
- **Settings Mode**: Configuration menu for various parameters

### LED Indicators
- **Minutes Ring (60 LEDs)**: Green for minutes, red for seconds, yellow when overlapping
- **Hours Ring (12 LEDs)**: Blue for current hour
- **Air Quality Strip (10 LEDs)**: Color-coded air quality from green (excellent) to purple (dangerous)

## 🔧 Configuration

Key settings in `config.h`:
```cpp
// LED pins and counts
#define LED_RING_MINUTES_PIN 6
#define LED_RING_HOURS_PIN 7
#define LED_STRIP_AIR_PIN 8

// Sensor pins
#define DHT22_INDOOR_PIN 2
#define DHT22_OUTDOOR_PIN 3
#define MQ135_PIN A0

// Timing intervals
#define SENSOR_READ_INTERVAL 30000UL    // 30 seconds
#define NETWORK_SYNC_INTERVAL 86400000UL // 24 hours
```

## 🌐 Web Interface (Optional)

The clock can serve a web interface for remote monitoring:
- Real-time sensor data
- Historical graphs
- Configuration options
- Data export capabilities

Access at `http://[device-ip]/` when connected to WiFi.

## 📊 Data Logging

Environmental data can be automatically sent to:
- Local web server
- Remote API endpoint
- CSV file export via web interface

Configure endpoints in `secrets.h`.

## 🔍 Troubleshooting

### Common Issues

**Clock not displaying time**
- Check NTP server connectivity
- Verify timezone settings in `config.h`

**Sensors showing invalid readings**
- Check sensor wiring
- Verify sensor-specific calibration values
- Enable debug mode to see raw sensor data

**LEDs not working**
- Verify power supply capacity (5V 5A recommended)
- Check LED strip wiring and pin definitions
- Test individual LED strips with simple sketch

**WiFi connection fails**
- Verify credentials in `secrets.h`
- Check signal strength
- Ensure 2.4GHz network (5GHz not supported)

See [Troubleshooting Guide](docs/TROUBLESHOOTING.md) for detailed solutions.

## 🛡️ Calibration

### Air Quality Sensor (MQ135)
1. Power on in clean air for 24 hours
2. Note average reading
3. Update `MQ135_RZERO` in `config.h`

### Temperature Sensors
Compare readings with reference thermometer and adjust offsets if needed.

See [Calibration Guide](docs/CALIBRATION.md) for detailed procedures.

## 🤝 Contributing

Contributions are welcome! Please read [CONTRIBUTING.md](CONTRIBUTING.md) for guidelines.

### Development Setup
1. Fork the repository
2. Create feature branch: `git checkout -b feature/amazing-feature`
3. Follow coding standards in [STYLE.md](docs/STYLE.md)
4. Test thoroughly
5. Create pull request

## 📜 License

This project is licensed under the MIT License - see [LICENSE](LICENSE) file for details.

## 🙏 Acknowledgments

- Arduino community for excellent libraries
- Weather station projects for inspiration
- Contributors and testers

## 📞 Support

- 🐛 **Bug Reports**: [Open an issue](https://github.com/yourusername/multifunctional-clock/issues)
- 💡 **Feature Requests**: [Discussion board](https://github.com/yourusername/multifunctional-clock/discussions)
- 📖 **Documentation**: [Wiki](https://github.com/yourusername/multifunctional-clock/wiki)

## 📸 Gallery

| ![Clock View](docs/images/clock_front.jpg) | ![Sensor Display](docs/images/sensor_mode.jpg) |
|:---:|:---:|
| Clock Mode | Sensor Mode |

| ![Night Mode](docs/images/night_mode.jpg) | ![Web Interface](docs/images/web_interface.png) |
|:---:|:---:|
| Night Mode | Web Interface |

---

**⭐ Star this repository if you found it useful!**