# Applikationsbeschreibung Klimaanlagen KNX Adapter

## Wichtige Hinweise

* Diese KNXprod wird nicht von der KNX Association offiziell unterstützt!
* Die Erzeugung der KNXprod geschieht auf Eure eigene Verantwortung!

## Module

Die Klimaanlagen App besteht aus folgenden Modulen:

- [Basiseinstellungen](https://github.com/OpenKNX/OGM-Common/blob/v1/doc/Applikationsbeschreibung-Common.md)
- [Konfigurationstransfer](https://github.com/OpenKNX/OFM-ConfigTransfer/blob/v1/doc/Applikationsbeschreibung-ConfigTransfer.md)
- [Netzwerk](https://github.com/OpenKNX/OFM-Network/blob/v1/doc/Applikationsbeschreibung-Netzwerk.md)
- [Logik](https://github.com/OpenKNX/OFM-LogicModule/blob/v1/doc/Applikationsbeschreibung-Logik.md)
- [Funktionsblöcke](https://github.com/OpenKNX/OFM-FunctionBlocks/blob/v1/doc/Applikationsbeschreibung-FunctionBlocks.md)

<!-- DOC -->
## Klimaanlage

Mit dieser Anwendung kann eine Klimaanlage über KNX gesteuert werden.

<!-- DOCEND -->
## Hardware

In diesem Abschnitt wird eingestellt, welche Klimaanlage gesteuert werden soll.

<!-- DOC -->
### Hersteller

Auswahl des Herstellers der Klimaanlage:

- Daikin
- Toshiba

<!-- DOCEND -->
### Mitsubishi

Folgende Einstellungen stehen nur für Mitsubishi Klimageräte zur Verfügung:

<!-- DOC -->
#### Minimale Soll-Temperatur

Hier muss die für das Klimagerät passende minimale Solltemperatur eingestellt werden.

<!-- DOC -->
## Basiseinstellungen

In diesem Abschnitt werden allgemeine Konfigurationen vorgenommen

<!-- 
### Nach Busspannungsausfall / Neustart

Hier wird konfiguriert, ob die Klimaanlage nach einem Busspannungsaufall oder Neustart geschalten werden soll.

- Keine Änderung
  Das Klimagerät erhält keine Steuerbefehle.
- Einstellungen wiederherstellen
  Die Einstellungen werden wieder hergestellt und das Klimagerät entsprechend konfiguriert.  
-->
<!-- DOC -->
### Wifi LED

Über diese Option wird das Verhalten der Wifi LED am Klimagerät vorgegeben

- WLAN Status
- Immer Aus
- Immer Ein
- Schaltbar über Gruppenobjekt


<!-- DOC -->
### Freigabe- / Sperrobjekt

Über diesen Konfigurationswert kann ein Sperrobjekt bzw. Freigabeobjekt angezeigt werden.
Über dieses Objekt kann die Bedienung gesperrt werden.

- Keines
- Freigabe
  1-Bit, Standardmäßig ist die Bedienung gesperrt.
  Benötigt "1" zur Freigabe
- Sperre
  1-Bit, Standardmäßig ist die Bedienung freigegeben.
  Benötigt "1" zur Sperre

<!-- DOC -->
#### Verhalten bei Freigabe

<!-- DOC Skip="1" -->
Diese Einstellung wird nur angezeigt, wenn ein Freigabeobjekt konfiguriert ist.

Zur Auswahl:
- Keine Änderung
- Einschalten
- Ausschalten

<!-- DOC -->
#### Verhalten bei Freigabe Ende

<!-- DOC Skip="1" -->
Diese Einstellung wird nur angezeigt, wenn ein Freigabeobjekt konfiguriert ist.

Zur Auswahl:
- Keine Änderung
- Einschalten
- Ausschalten

<!-- DOC -->
#### Verhalten bei Sperre

<!-- DOC Skip="1" -->
Diese Einstellung wird nur angezeigt, wenn ein Sperrobjekt konfiguriert ist.

Zur Auswahl:
- Keine Änderung
- Einschalten
- Ausschalten

<!-- DOC -->
#### Verhalten bei Sperre Ende

Zur Auswahl:
- Keine Änderung
- Einschalten
- Ausschalten


<!-- DOCEND -->

### Daikin

Folgende Einstellungen und Funktionen stehen für Daikin Klimageräte zur Verfügung:

Diese Implementierung bietet vollständige Unterstützung für Daikin Klimageräte über das S21-Protokoll. Die Kommunikation erfolgt direkt über die serielle Schnittstelle des Daikin-Innengeräts.

#### Unterstützte Funktionen

##### Grundfunktionen
- **Ein/Aus-Steuerung**: Vollständige Kontrolle über den Gerätestatus
- **Betriebsmodi**: Automatik, Kühlen, Heizen, Trocknen, Lüfter
- **Temperatursteuerung**: Präzise Solltemperatur-Einstellung (18-32°C, 0,5°C Schritte)
- **Lüftergeschwindigkeit**: Automatik + 5 manuelle Stufen
- **Lamellenkontrolle**: Vertikale und horizontale Schwenkbewegung

##### Erweiterte Funktionen
- **Powerful-Modus**: Erhöhte Kühl-/Heizleistung für schnelles Erreichen der Solltemperatur
- **Econo-Modus**: Energiesparender Betrieb mit reduzierter Leistung
- **Quiet-Modus**: Leiserer Betrieb des Außengeräts
- **LED-Steuerung**: Kontrolle der Wifi-LED am Innengerät
- **Streamer**: Daikin Flash Streamer Luftreinigungstechnologie
- **(Kein nativer Externer Raumtemperatur-Eingang)**: S21 bietet keine dokumentierte Möglichkeit, eine externe Raumtemperatur direkt an das Innengerät zu übergeben. Die interne Regeltemperaturquelle bleibt das Gerät.

##### Externe Raumtemperaturerfassung
Es existiert im bislang veröffentlichten und revers‑engineerten S21-Protokoll (Faikin Wiki v0–v3.40) **kein** dokumentierter Befehl, der eine alternative Raumtemperatursensorquelle (externer KNX Sensor) direkt als Regelgröße an das Daikin-Innengerät übergibt. 

Die in F6 / G6 als "Sensor" bezeichneten Bits beziehen sich auf spezielle Komfort-/Präsenz-/Intelligent‑Eye Funktionen des Geräts (inkl. LED Verhalten) – **nicht** auf einen externen Temperatursensor.

OpenKNX bietet optional eine rein logische Korrektur (Sollwertverschiebung) gegenüber der vom Gerät gemeldeten internen Raumtemperatur, um einen externen KNX-Sensor zu berücksichtigen. Dabei wird keine externe Temperatur in das Gerät injiziert, sondern nur die an das Gerät gesendete Solltemperatur rechnerisch angepasst (siehe Abschnitt "Externe Raumtemperatur" in der allgemeinen Dokumentation).

Zusammenfassung:
- Keine native S21-Funktion zur Übergabe eines externen Raumtemperaturwertes
- F6 Sensor-/LED-Bits = Komfort-/Anwesenheitsfunktion, nicht Temperaturquelle
- Externe KNX-Temperatur → nur indirekte Beeinflussung durch Sollwertkorrektur

### Daikin

Folgende Einstellungen und Funktionen stehen für Daikin Klimageräte (S21 Schnittstelle am Innengerät) zur Verfügung.

Hinweis: Das S21-Protokoll ist nicht vollständig dokumentiert und einige Funktionen sind je nach Gerätemodell / Protokoll-Version optional. Die Implementierung nutzt nur dokumentierte beziehungsweise verifizierte Teile (Stand: Faikin Wiki Protokoll v0–v3.40). Nicht unterstützte oder unsichere Bereiche wurden bewusst weggelassen.
##### Telemetrie und Überwachung
##### Grundfunktionen
- **Ein/Aus**: Steuerung über D1 (Power Bit)
- **Betriebsmodi**: Automatik, Kühlen, Heizen, Trocknen, Lüfter (gemäß F1 Rückmeldung)
- **Solltemperatur**: 18,0–30,0 °C in 0,5 °C Schritten (ASCII Kodierung @ … X). Temperaturen >30 °C werden aktuell nicht gesetzt (Spezial-/Region‑Fälle werden ignoriert)
- **Lüftergeschwindigkeit**: Automatik + 5 Stufen (Low → High). Quiet/Night wird intern als eigener Modus behandelt – Rückmeldung über RG korrekt, F1 zeigt Quiet wie Auto (Protokoll-Einschränkung)
- **Swing**: Vertikal / Horizontal / Beides über D5 sofern vom Gerät gemeldet (F2 Featurebits). Keine absolute Lamellenpositions-Steuerung.

##### Erweiterte / Optionale Funktionen
Die Verfügbarkeit wird zur Laufzeit über F2 / FK / (bei v3+) FU00 erkannt. Nicht jedes Gerät unterstützt alle Funktionen.

- **Powerful** (F6 / D6): Wenn F6 unterstützt (ab vielen v2/v3 Geräten). Frühe Geräte evtl. nur Status über F3 Bitmuster.
- **Econo** (F7 / D7): Energiesparmodus (Bit im zweiten Byte von F7)
- **Quiet** (D4 proprietär in dieser Implementierung / Status in F6 Quiet-Bit). Fan Quiet wird durch RG korrekt gelesen; F1 meldet ihn als Auto (Protokoll-Eigenheit).
- **Streamer**: Wenn in FU00 Byte 5 gesetzt und F6 Streamer-Bit reagiert.
- **LED**: Steuerung (abhängig von LED-Bit in F6 Byte 3, invertierte Logik). Fehlt bei älteren Modellen.
- **Kein externer Temperatursensor via S21**: F6 Sensor-Bit dient nicht zur Auswahl einer externen Raumtemperaturquelle; eine Übergabe externer Temperaturdaten findet nicht statt.
- **Demand / Leistungsvorgabe (F7)**: Auswertung des Rohwertes (1‑stellige Lastkennzahl), Umsetzung derzeit auf Prozent heuristisch (experimentell).

Hinweis: Eine eventuelle zukünftige Erweiterung würde deutlich gekennzeichnet und setzt belastbare Protokollnachweise voraus.
- **Robuste Kommunikation**: Retry-Mechanismen und Timeout-Behandlung
- **Innengerät Temperatur (RH)**
- **Außentemperatur (Ra)**
- **Feuchtigkeit (Re)**: Falls Gerät Sensor meldet (nicht zuverlässig bei Geräten ohne echten Sensor → oft 50%)
- **Leistungsaufnahme**: Aktuell nicht ausgewertet (FM liefert kumulierten Verbrauch Wh/10 – geplante zukünftige Auswertung)
- **Kompressorfrequenz (Rd)**
- **Fan Speed (RL / RK)**: Drehzahl / Tap (wenn verfügbar)
- **Echter Zielwert (RX)**: Interner Reglerzielwert (inkl. Powerful/Intelligent Eye Einflüsse)
- **Unit/System State (RzB2 / RzC3)**: Bitfelder für Aktiv / Defrost / Powerful etc.

**Protokollversionen**: Dynamische Erkennung v0 / v2 / v3.x (F8 + FY00 Probe). Antwort ‚v2‘ über F8 bei v3 ist Daikin-Rückwärtskompatibilität.
# Gerätemodus-Einstellungen über Gruppenobjekte
**Automatische Funktionen:**
- **Protokoll-Detection**: F8 → ggf. FY00 für v3+; Feature-Bits über F2/FK/F6/FU00
- **Polaritätshandling**: Automatische Polarity-Versuche (Inversion RX/TX Kombinationen)
- **Fallback F6→F3**: Falls F6 (Special Modes) nicht reagiert werden ggf. F3 Bits genutzt (eingeschränkt)
- **Retry / Timeout**: ACK- und Grace-Window Handling (angepasst an beobachtete Zeiten)
Der Vorgabewert wird auf den nächstliegenden fest definierten Wert gerundet.
**Getestete Geräte (Beispiele – nicht vollständig):**
- Daikin Stylish FTXA25AW 
Rückmeldungen aus Tests willkommen – unbekannte Feature-Kombinationen werden protokolliert und können zukünftige Erweiterungen steuern.

**Faikin-Bezug:**
Konzepte (Timing, Query-Reihenfolge, Interpretation von F*/R*/Rz*-Kommandos) orientieren sich an den im Faikin-Projekt dokumentierten Erkenntnissen. Nicht jede dort aufgeführte experimentelle Funktion ist hier aktiv umgesetzt.

| Szenennummer | Beschreibung         | Toshiba | Daikin |
|--------------|----------------------|---------|--------|
| 1            | Standard             | Ja      | Ja     |
| 2            | Hi-Power/Powerful    | Ja      | Ja     |
| 3            | Eco/Econo            | Ja      | Ja     |
| 4            | Leise/Quiet          | Ja      | Ja     |
| 5            | Luftreinigung        | Nein    | Ja*    |

*Für Daikin: Streamer (Flash Streamer Luftreinigungstechnologie)

Für jeden Betriebsmodus stehen auch separate Gruppenobjekte für das Schalten und den aktuellen Status zur Verfügung.

<!-- DOC -->
## Szenen

In diesem Abschnitt können Szenennummer und das zugehörige Verhalten konfiguriert werden.

<!-- DOC -->
### Szene

Gibt an, ob die Szenen Konfiguration verwendet werden soll

<!-- DOC -->
### Szenennummer

Die Szenennummer, die zum Auslösen verwendet wird.

<!-- DOC -->
### Schalten

Einstellungen, ob beim Aufruf der Szene das Gerät ein- oder ausgeschaltet werden soll.

- Keine Änderung
- Einschalten
- Ausschalten

<!-- DOC  -->
### Betriebsmodus

Ermöglicht das Aktivieren eines Betriebsmodus.

Optionen für Daikin:

- Keine Änderung
- Automatik
- Kühlen
- Heizen
- Trocknen
- Lüfter

Optionen für Toshiba:

- Keine Änderung
- Automatik
- Kühlen
- Heizen
- Trocknen
- Lüfter

<!-- DOC -->
### Solltemperatur

Vorgabe der Solltemperatur

- Keine Änderung
- Temperaturvorgabe

<!-- DOC  -->
#### Lüfter

Lüftergeschwindigkeit

Optionen für Toshiba:

- Keine Änderung
- Automatik
- Leise
- Stufe 1
- Stufe 2
- Stufe 3
- Stufe 4
- Stufe 5

<!-- DOC -->
#### Lamellenbewegung

Steuert die Bewegung der Lamellen.

Optionen für Toshiba:

- Keine Änderung
- Aus
- Vertikal
- Horizontal
- Vertikal und Horizontal

<!-- DOC -->
#### Vertikale Lamellenstellung

Diese Einstellung ist nur verfügbar, wenn bei Lamellenbewegung "Keine Änderung" oder "Aus" gewählt wurde.
Gibt die Position der Lamellen an. 

Optionen für Toshiba:

- Keine Änderung
- Ganz Oben
- Oben
- Mitte
- Unten
- Ganz Unten

Für Daikin steht diese Option nicht zur Verfügung.

<!-- DOC -->
#### Leistungsbegrenzung

Mit dieser Option, kann die maximale Leistung des Klimageräts angepasst werden.

Optionen für Toshiba:

- Keine Änderung
- 50%
- 75%
- 100%

Für Daikin steht diese Option nicht zur Verfügung.

<!-- DOC -->
#### Gerätemodus

Optionen für Daikin:

- **Standard**: Normaler Betriebsmodus
- **Hi-Power**: Entspricht Daikin "Powerful"-Modus für maximale Leistung
- **ECO**: Entspricht Daikin "Econo"-Modus für energiesparenden Betrieb
- **Leise**: Entspricht Daikin "Quiet"-Modus für reduzierten Außengeräuschpegel

Optionen für Toshiba:

- Keine Änderung
- Standard
- Hi-Power
- ECO
- Außengerät Leise 1
- Außengerät Leise 2

<!-- DOC -->
#### Luftreinigung

Verfügbare Einstellungen:

- Keine Änderung
- Aus
- Ein

Für Daikin steht diese Option nicht zur Verfügung.