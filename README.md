
# OAM-Aircondition

Dieses Modul erlaubt die Steuerung von Klimageräten unterschiedlicher Hersteller.

- Toshiba
  - HAORI (Getestet)
  - SHORAI EDGE (Getestet)
  - Viele andere Modelle mit WLAN-Modul sollten funktionieren. Bitte gerne Rückmeldung geben, wenn ein neues Modell getestet wurde.

- Daikin
  - S21 Protocol v3.30 (Vollständig implementiert)
  - Alle Modelle mit S21-Schnittstelle (HA-Anschluss) unterstützt
  - Features: Grundsteuerung, Spezialmodi (Powerful, Quiet, Comfort, Streamer), Intelligent Eye Sensor, Humidity Control, Swing-Modi
  - Getestet mit verschiedenen FTXM/FTXF Serien

Geplant:
- Mitsubishi (In Entwicklung)

## Features

### Allgemeine Steuerung und Rückmeldung
- Power
- Operation Mode
- Set temperature
- Louvers
- Fan speed
- WiFi LED (if available)

### Herstellerspezifische Features

#### Toshiba
- WLAN-LED Kontrolle
- Erweiterte Lüftersteuerung

#### Daikin (S21 Protocol)
- **Spezialmodi**: Powerful, Quiet, Comfort, Streamer
- **Intelligent Eye Sensor**: Automatische Temperaturanpassung basierend auf Anwesenheitserkennung
- **Humidity Control**: 5 Stufen (Off, Low, Standard, High, Continuous, Moisturizing)
- **Swing Modi**: Vertikal, Horizontal, Both
- **Umfassende Sensorik**: Innen-/Außentemperatur, Luftfeuchtigkeit, Kompressorfrequenz
- **Energy Monitoring**: Stromverbrauch-Tracking
- **Erweiterte Diagnostik**: Vollständige S21 v3.30 Protokoll-Unterstützung

## Anwenderdokumentation

Die Anwenderdokumentation ist [hier](./doc/Applikationsbeschreibung.md) zu finden.

## Firmware

Eine vorkompilierte Firmware ist [hier](https://github.com/OpenKNX/OAM-Aircondition/releases) zu finden. ZIP-Datei herunterladen, entpacken und der Anleitung im Readme folgen.

## Hardware

### Bauteilliste:

KNX-TP
- 2 x Adum1201
- 1 x AZDelivery ESP32 Board Dev Kit C V4
- 1 x Nano-BCU
Die Stromversorgung erfolgt über die Klimaanlage, der KNX-Bus wird galvanisch über den Adum1201 getrennt.

KNX-IP
- 1 x Adum1201
- 1 x ESP32 Board 
- 1 x Widerstand 330 Ω
- 1 x LED


### Klimagerät / Adum1201 (1) ###

#### Toshiba

Die Farben entsprechen den üblichen Farben bei den Geräten, können aber auch abweichen. Daher bitte zuvor ausmessen, an welchen Pins GND und VCC anliegt.

| Pin      | Farbe | Klimagerät      | Adum1201 (1)|
|----------|-------|-----------------|-------------|
|    1     | blue  | RX              | VOB         |
|    2     | pink  | GND             | GND2        |
|    3     | black | 5V              | VDD2        |
|    4     | white | TX              | VIA         |
|    5     | pink  | nicht verwendet |             |

#### Daikin (S21 Protocol)

Verbindung über den S21/HA-Anschluss des Innengeräts. Meistens ein 5-poliger Stecker.

| Pin      | Signal          | Klimagerät      | 
|----------|-----------------|-----------------|
|    1     | +12V/+5V        | VCC             | 
|    2     | TX (vom Gerät)  | TX              | 
|    3     | RX (zum Gerät)  | RX              | 
|    4     | GND             | GND             | 
|    5     | NC/Shield       | nicht verwendet |

**Wichtig**: Vor Anschluss Spannungslevel prüfen! Manche Geräte nutzen 12V, andere 5V.
Die S21-Schnittstelle arbeitet mit 2400 baud, even parity, 2 stop bits.

### Adum1201 (1) / ESP32 Board

| Adum1201 (1)| ESP32 Board    |
|-------------|----------------|
| VDD2        | 5V (Eingang)   |
| GND2        | GND            |
| VDD1        | 3,3V (Ausgang) |
| GND1        | GND            |
| VOA         | 26 (RX)        |
| VIB         | 27 (TX)        |

Für RX und TX können über die Defines OPENKNX_AIR_CONDITION_SERIAL_RX bzw. OPENKNX_AIR_CONDITION_SERIAL_TX in der [platformio.custom.ini](platformio.custom.ini) auch andere Pins verwendet werden.

### Verkabelung BCU (Nur bei KNX-TP)

| BCU         | Adum1201 (2) | KNX         |
|-------------|--------------|-------------|
| RX          | VOB          |             |
| TX          | VIA          |             |
| 3,3V        | VDD2         |             |
| GND         | GND2         | Schwarz     |
| KNX         |              | Rot         |

ACHTUNG: GND und 3,3 V dürfen nicht mit dem ESP verbunden werden!

| Adum1201 (2) | ESP32           |
|--------------|-----------------|
| VDD1         | 3,3 V (Ausgang) |
| GND1         | GND             |
| VOA          | 16 RX           |
| VIB          | 17 TX.          |

Für RX und TX können über die Defines KNX_UART_RX_PIN bzw. KNX_UART_TX_PIN in der [platformio.custom.ini](platformio.custom.ini) auch andere Pins verwendet werden.

### Verkabelung Programmier-LED

Neuere Boards haben keine OnBoard-LED. 
Benötigt man eine Programmier-LED, muss diese auf den Pin 2 verbunden werden.
Ältere DevKit V1 Boards haben bereits eine LED auf diesen PIN angeschlossen.
Da die Programmier-LED aber nur einmal zum Programmieren der KNX-Adresse leuchtet, kann man auch darauf verzichten.

| ESP32        |                        | 
|--------------|------------------------|
| 2            | Widerstand 1. Seite    |

Über das Define PROG_LED_PIN in der [platformio.custom.ini](platformio.custom.ini) kann auch ein anderer Pin vergeben werden.

| LED                    |                        | 
|------------------------|------------------------|
| Anode (langer Draht)   | Widerstand 2. Seite    |
| Kathode (kurzer Draht) | GND ESP                |


### Programmier-Taster

Der Programmier-Taster ist der Boot Taster am ESP32 Board.

### Haftungsausschluss

Die Software wird von Privatpersonen als Open-Source zur Verfügung gestellt. Die Nutzung erfolgt auf eigenem Risiko. Die Entwickler übernehmen keine Haftung oder Gewährleistung! Insbesondere können wir nicht für Schäden die am Klimageräte oder durch herab gesetzte Herstellergarantie entstehen, die Haftung übernehmen.

## Danke

Dieses Projekt konnte nur durch die Vorarbeit von vielen Personen und Projekten realisiert werden.

Besonderen Dank an folgende Projekte:

### Toshiba
- [pedobry/esphome_toshiba_suzumi](https://github.com/pedobry/esphome_toshiba_suzumi), das als Vorlage für die Toshiba-Anbindung gedient hat
- [toremick/shorai-esp32](https://github.com/toremick/shorai-esp32) für die Infos zur HW-Anbindung für Toshiba

### Daikin
- [revk/ESP32-Faikin](https://github.com/revk/ESP32-Faikin) für die umfassende S21 Protokoll-Dokumentation und Reverse Engineering
- [ESP32-Faikin S21 Protocol Wiki](https://github.com/revk/ESP32-Faikin/wiki/S21-Protocol) für die detaillierte S21 v3.30 Spezifikation
- Die Daikin S21 Community für Feldtests und Protokoll-Verifikation

## Lizenz

Diese Software steht unter der [GNU GPL v3](LICENSE).
