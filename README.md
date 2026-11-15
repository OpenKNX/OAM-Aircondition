# OAM-Aircondition

Dieses Modul erlaubt die Steuerung von Klimageräten unterschiedlicher Hersteller.

Feedback zu weiteren funktionierenden Geräten folgender Hersteller gerne [KNX-User-Forum](https://knx-user-forum.de/forum/projektforen/openknx/2051393-openknx-klimaanlagen-knx-adapter-toshiba-daikin-mitsubishi-dich-kühlen-soll) hinterlassen, dann ergänzen wir die jeweilige List

## Toshiba
  - HAORI (Getestet)
  - SHORAI EDGE (Getestet)
  - Viele andere Modelle mit WLAN-Modul sollten funktionieren. Bitte gerne Rückmeldung geben, wenn ein neues Modell getestet wurde.
  - [Anschluss an UP1-GW-UART](doc/toshiba.md)

## Daikin

- Anbindung über die S21-Schnittstelle des Innengeräts; Funktionen sind modell- und betriebsmodusabhängig.
- Ein/Aus, Auto/Kühlen/Heizen/Trocknen/Lüfter, halbe Solltemperaturgrade, Lüfter Auto/Geräuscharm/Stufe 1–5 und getrennte Swing-Achsen.
- Unabhängige KNX-Steuerung für Powerful, Econo, Außengerät Quiet, Comfort, Intelligent Eye, Streamer und Geräte-/Sensor-LED, soweit verfügbar.
- Vertikale Positionscodes sowie horizontale Positionscodes mit ausdrücklich konfiguriertem Modellprofil; beide Achsen auch in Szenen.
- Modellprofile für Demand-Leistungsbegrenzung und relative Ein-/Ausschalttimer, separate Timerleuchte sowie Status-/Schreibbestätigungen.
- Zusätzliche Temperatur-, Lüfter-, Leistungs-/Energie- und Diagnosedaten mit Gültigkeitskennzeichen.
- Bedienung und Gruppenobjekte: [Daikin-KNX-Referenz](doc/Daikin-KNX-controls.md).

## Mitsubishi (Mitsubishi Electric)
  - Geräte: Es sollten alle Geräte unterstützt werden, die den CN105 Connector haben
    - `MSZ-APxxx` (mit dieser Serie wurde der Treiber entwickelt und getestet)
    - `MSZ-EFxxx` Siehe [KNX-User-Forum](https://knx-user-forum.de/forum/projektforen/openknx/2051393-openknx-klimaanlagen-knx-adapter-toshiba-daikin-mitsubishi-dich-kühlen-soll?p=2110824#post2110824)
    - `MSZ-*` sollten mit sehr hoher Wahrscheinlichkeit funktionieren 
  - [Anschluss an UP1-GW-UART](doc/mitsubishi.md)
  - Features:
    - Ein/Aus-Steuerung
    - Betriebsmodi: Automatik, Kühlen, Heizen, Trocknen, Lüfter
    - Temperatursteuerung: Solltemperatur-Einstellung (16/19-30°C, 1°C Schritte)
    - Lüftergeschwindigkeit: Automatik + 5 manuelle Stufen
    - Lamellenkontrolle: Vertikale und horizontale Schwenkbewegung
    - Lamellenposition: Vertikale Position. TODO: horizontale Position
    - TODO: Externe Ist-Temperatur​


# Release notes

- 0.15 Aktualisierte Abhängigkeiten, LogicModule 4.4 und angepasste Applikationsmetadaten.
- 0.14 Modellprofil für horizontale Daikin-Positionen, zusätzliche KNX-Objekte und horizontale Szenenpositionen.

- 0.10 Fix issue for missing scene after upgrade from version 0.8
- 0.9 Fix Szenen setzen bei Toshiba
- 0.9 Daikin Support
- 0.9 Mitsubishi Support
- 0.8 Spannungsüberwachung des Klimagerätes
- 0.8 Umbennung der Gruppenobjekte auf OpenKNX Standard
- 0.8 Daikin Support

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

#### Daikin (S21)

Lesbarer Status, Geräte-Fähigkeiten und bestätigte Schreibzugriffe werden getrennt behandelt. Ein ACK allein bestätigt keine angewendete Einstellung; Rückmeldungen können verzögert eintreffen. Unbekannte oder veraltete Messwerte müssen zusammen mit ihrem Gültigkeitskennzeichen und dem Online-Status ausgewertet werden.

Econo und Außengerät Quiet können über ihre unabhängigen Objekte gemeinsam angefordert werden; Powerful schließt beide aus. „Leise 2“ ist auf S21 ein Alias für „Leise 1“. Der einzelne Gerätemodus-/Szenen-Preset wählt weiterhin genau einen dieser Presets. Die Ausführung bleibt vom Gerät und Betriebsmodus abhängig.

Vertikal stehen nach FR-Erkennung Codes 0–5 und über das Code-Objekt der Automatikmarker 6 zur Verfügung. Szenen verwenden 0–5. Horizontal ist die Steuerung standardmäßig deaktiviert; erst das modellgeprüfte Maximum (bis 8) gibt Codes 0–Maximum frei. Code 0 hält die aktuelle Stellung. Positionscodes und Prozentwerte sind keine kalibrierten Winkel.

Demand und Timer sind ebenfalls opt-in: Minimum/Schrittweite beziehungsweise Timer-Schrittweite/Obergrenze müssen zum konkreten Gerät passen. Die S21-Timerkodierung hat Zehn-Minuten-Schritte; ein Gerät kann gröbere Schritte verlangen. Timerstatuswerte sind konfigurierte Verzögerungen, kein Countdown. Die Timerleuchte wird über DB geschrieben und über FB zurückgelesen.

Luftfeuchtemessung und qualitative Feuchteregelung sind verschiedene Funktionen. Feuchtestufen, LED-Stufen und zusätzliche Sensorwerte werden nur entsprechend der erkannten Möglichkeiten bereitgestellt. Ein externer KNX-Raumtemperaturwert wird vom Daikin-Treiber nicht als Regelgröße an das Innengerät übertragen; eine automatische Sollwertkorrektur ist ebenfalls nicht implementiert.

## Anwenderdokumentation

Die Anwenderdokumentation ist [hier](./doc/Applikationsbeschreibung.md) zu finden.

## Firmware

Eine vorkompilierte Firmware ist [hier](https://github.com/OpenKNX/OAM-Aircondition/releases) zu finden. ZIP-Datei herunterladen, entpacken und der Anleitung im Readme folgen.

Die vorkompilierte Firmware unterstützt:

TP ohne WLAN:
- [OpenKNX+UP1+UART+Gateway](https://openknx.atlassian.net/wiki/spaces/OpenKNX/pages/484802573/OpenKNX+UP1+UART+Gateway)  
*Hinweis: diese Board unterstüzt kein WLAN/LAN, damit sind die Einstellungen in der ETS für Wifi und Webserver wirkungslos*

WLAN:
- AZDelivery ESP32 Board Dev Kit C V4  
  *Webserver und Web-Console sind für die ESP32-Netzwerk-Builds aktiviert. Der Webserver wird über die ETS-Einstellung freigegeben.*

- Adafruit_Feather_ESP32_V2

## Hardware

Wer kein OpenKNX Gerät verwenden will, kann sich eine HW selber zusammenlöten. 

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

### Klimagerät / Adum1201 (1)

#### Toshiba

Die Farben entsprechen den üblichen Farben bei den Geräten, können aber auch abweichen. Daher bitte zuvor ausmessen, an welchen Pins GND und VCC anliegt.

| Pin | Farbe | Klimagerät      | Adum1201 (1) |
| --- | ----- | --------------- | ------------ |
| 1   | blue  | RX              | VOB          |
| 2   | pink  | GND             | GND2         |
| 3   | black | 5V              | VDD2         |
| 4   | white | TX              | VIA          |
| 5   | pink  | nicht verwendet |              |

#### Daikin (S21 Protocol)

Verbindung über den S21-Anschluss des Innengeräts. Pinbelegung, Versorgung und Pegelanpassung müssen aus der Dokumentation des konkreten Geräts und Adapters übernommen werden; hier wird keine universelle Steckerbelegung vorgegeben.

Die serielle Kommunikation verwendet 2400 Baud, 8 Datenbits, gerade Parität und 2 Stoppbits (8E2). RX/TX-Pins und die verwendete serielle Schnittstelle sind in der jeweiligen Build-Konfiguration festgelegt.

#### Mitsubishi

Analog zu "[Anschluss an UP1-GW-UART](doc/mitsubishi.md)"


### Adum1201 (1) / ESP32 Board

| Adum1201 (1) | ESP32 Board    |
| ------------ | -------------- |
| VDD2         | 5V (Eingang)   |
| GND2         | GND            |
| VDD1         | 3,3V (Ausgang) |
| GND1         | GND            |
| VOA          | 26 (RX)        |
| VIB          | 27 (TX)        |

Für RX und TX können über die Defines OPENKNX_AIR_CONDITION_SERIAL_RX bzw. OPENKNX_AIR_CONDITION_SERIAL_TX in der [platformio.custom.ini](platformio.custom.ini) auch andere Pins verwendet werden.

### Verkabelung BCU (Nur bei KNX-TP)

| BCU  | Adum1201 (2) | KNX     |
| ---- | ------------ | ------- |
| RX   | VOB          |         |
| TX   | VIA          |         |
| 3,3V | VDD2         |         |
| GND  | GND2         | Schwarz |
| KNX  |              | Rot     |

ACHTUNG: GND und 3,3 V dürfen nicht mit dem ESP verbunden werden!

| Adum1201 (2) | ESP32           |
| ------------ | --------------- |
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

| ESP32 |                     |
| ----- | ------------------- |
| 2     | Widerstand 1. Seite |

Über das Define PROG_LED_PIN in der [platformio.custom.ini](platformio.custom.ini) kann auch ein anderer Pin vergeben werden.

| LED                    |                     |
| ---------------------- | ------------------- |
| Anode (langer Draht)   | Widerstand 2. Seite |
| Kathode (kurzer Draht) | GND ESP             |

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

- [revk/ESP32-Faikout](https://codeberg.org/RevK/ESP32-Faikout) für die umfassende S21 Protokoll-Dokumentation und Reverse Engineering
- [ESP32-Faikout S21 Protocol Wiki](https://codeberg.org/RevK/ESP32-Faikout/wiki/S21-Protocol) für die Dokumentation der S21-Befehle
- Die Daikin S21 Community für Feldtests und Protokoll-Verifikation

## Lizenz

Diese Software steht unter der [GNU GPL v3](LICENSE).
