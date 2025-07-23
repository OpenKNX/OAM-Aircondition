
# OAM-Aircondition

Diese Modul erlaubt die Steuerung von Klimageräten unterschiedlicher Hersteller.

- Toshiba
  - HAORI (Getestet)
  - SHORAI EDGE (Wird bald getestet)
  - Viele andere Modelle mit WLAN Modul sollten funktionieren, bitte gerne Rückmeldung wenn ein neues Modell getestet wurde 

Künftig:
- Daikin (In Entwicklung)
- Mitsubishi (In Entwicklung)

## Features

Steuerung und Rückmeldung

- Power
- Operation Mode
- Set temperature
- Louvers
- Van speed
- WiFi LED (if available)

## Anwenderdokumentation

Die Anwenderdokumentation ist [hier](./doc/Applikationsbeschreibung.md) zu finden.

## Firmware

Eine vorkomplierte Firmware ist [hier](https://github.com/OpenKNX/OAM-Aircondition/releases) zu finden. ZIP Datei herunterladen, entpacken und der Anleitung im Readme folgen.

## Hardware

### Bauteilliste:

KNX-TP
- 2 x Adum1201
- 1 x AZDelivery ESP32 Board Dev Kit C V4
- 1 x Nano-BCU
Die Stromversorgung erfolg die Klimaanlage, der KNX Bus wird galvanisch über den Adum1201 getrennt.

KNX-IP
- 1 x Adum1201
- 1 x ESP32 Board 
- 1 x Wiederstand 330 Ω
- 1 x LED


### Klimagerät / Adum1201 (1) ###

Toshiba

Die Farben entsprechen den üblichen Farben bei den Geräten, können aber auch. abweichen. Daher bitte zuvor die ausmessen, an welche Pins GND und VCC anliegt

<p align="center">

| Pin      | Farbe | Klimagerät      | Adum1201 (1)|
|----------|-------|-----------------|-------------|
|    1     | blue  | RX              | VOB         |
|    2     | pink  | GND             | GND2        |
|    3     | black | 5V              | VDD2        |
|    4     | white | TX              | VOA         |
|    5     | pink  | nicht verwendet |             |

</p>

### Adum1201 (1) / ESP32 Board

<p align="center">

| Adum1201 (1)| ESP32 Board    |
|-------------|----------------|
| VDD2        | 5V (Eingang)   |
| GND2        | GND            |
| VDD1        | 3,3V (Ausgang) |
| GND1        | GND            |
| VIB         | 26 (RX)        |
| VOA         | 27 (TX)        |

Für RX und TX können über die Defines OPENKNX_AIR_CONDITION_SERIAL_RX bzw. OPENKNX_AIR_CONDITION_SERIAL_TX in der [platformio.custom.ini](platformio.custom.ini) auch andere Pins verwendet werden.

</p>

### Verkabelung BCU (Nur bei KNX-TP)

<p align="center">

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

<p align="center">

Für RX und TX können über die Defines KNX_UART_RX_PIN bzw. KNX_UART_TX_PIN in der [platformio.custom.ini](platformio.custom.ini) auch andere Pins verwendet werden.

</p>

| ESP32        |                        | 
|--------------|------------------------|
| 25           | Wiederstand 1. Seite   |

Über das Define PROG_LED_PIN in der [platformio.custom.ini](platformio.custom.ini) kann auch ein anderer Pin vergeben werden.

| LED                    |                        | 
|------------------------|------------------------|
| Anode (langer Draht)   | Wiederstand 2. Seite   |
| Kathode (kurzer Draht) | GND ESP                |

Der Prog Taster ist der Boot Taster am ESP32 Board

## Danke

Diese Projekt konnte nur durch die Vorarbeit von vielen Personen und Projekten realisiert werden.

Besonderen Dank an folgende Projekte:

- [pedobry/esphome_toshiba_suzumi](https://github.com/pedobry/esphome_toshiba_suzumi) das als Vorlage für die Toshiba Anbindung gedient hat
- [toremick/shorai-esp32](https://github.com/toremick/shorai-esp32) für die Infos zur HW Anbindung für Toshiba

## Lizenz

Diese Software steht unter der [GNU GPL v3](LICENSE).