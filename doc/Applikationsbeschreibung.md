# Applikationsbeschreibung Klimaanlagen KNX Adapter

## Wichtige Hinweise

* Diese KNXprod wird nicht von der KNX Association offiziell unterstützt!
* Die Erzeugung der KNXprod geschieht auf Eure eigene Verantwortung!

## Module

Die Klimaanlagen App besteht aus folgenden Modulen:

- [Basiseinstellungen](https://github.com/OpenKNX/OGM-Common/blob/v1/doc/Applikationsbeschreibung-Common.md)
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
- Midea
- Mitsubishi
- Toshiba

<!-- DOCEND -->
### Mitsubishi

Folgende Einstellungen stehen nur für Mitsubishi Klimageräte zur Verfügung:

<!-- DOC -->
#### Minimale Soll-Temperatur

Hier muss die für das Klimagerät passende minimale Solltemperatur eingestellt werden.

<!-- DOC -->
## Basiseinstellungen

In diesem Abschnitt werden allgemeine Konfiguration vorgenommen

<!-- 
### Nach Busspannungsausfall / Neustart

Hier wird konfiguriert, ob die Klimaanlage nach einem Busspannungsaufall oder Neustart geschalten werden soll.

- Kein Änderung
  Das Klimagerät erhält keine Steuerbefehle.
- Einstellungen wiederherstellen
  Die Einstellung werden wieder hergestellt und das Klimagerät entsprechend konfiguriert.  
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
Über diese Objekt kann die Bedienung gesperrt werden.

- Keines
- Freigabe
  1-Bit, Standardmäßig ist die Bedienung gesperrt.
  Benötigt "1" zur Freigabe
- Sperre
  1-Bit, Standardmäßig ist die Bedienung freigeben.
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
#### Verhalten-bei-Sperre

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

# Gerätemodus Einstellungen über Gruppenobjekte

## Leistungsbeschränkung

Die Leistungsbeschränkung wird in Prozent vorgegeben.
Je nach Klimagerätetype werden aber nur fix definierte Werte unterstützt.
Der Vorgabewert wird auf den nächstliegenden fix definierten Wert gerundet.
Die Statusrückmeldung liefert den aktuellen auf den zulässig gerundenden Wert, des Gerätes.

## Gerätesmodus

Über den Eingang 'Gerätemodus' kann über eine Szenennummer das Klimagerät in einem speziellen Modus geschalten werden.
Abhängig vom Klimagerät können folgende Modus verwendet werden:

| Szenennummer | Beschreibung         | Toshiba |
|--------------|----------------------|---------|
| 1            | Standard             | Ja      |
| 2            | Hi-Power             | Ja      |
| 3            | Eco                  | Ja      |
| 4            | Leise 1 (Außengerät) | Ja      |
| 5            | Leise 2 (Außengerät) | Ja      |

Für jeden Betriebsmodus stehen auch getrennt Gruppenobjekte für Schalten und den aktuellen Status zur Verfügung.

<!-- DOC -->
## Szenen

In diesem Abschnitt können Szenenummer und das zugehörige Verhalten konfiguriert werden.

<!-- DOC -->
### Szene

Gibt an, ob die Szenen Konfiguration verwendet werden soll

<!-- DOC -->
### Szenennummer

Die Szenennummer die zum Auslösen verwendet wird.

<!-- DOC -->
### Schalten

Einstellungen ob beim Aufruf der Szene das Geräte Ein- oder Ausgeschalten werden soll.

- Keine Änderung
- Einschalten
- Ausschalten

<!-- DOC  -->
### Betriebsmodus

Ermöglich das aktivieren einen Betriebsmodus.

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

- Keine Änderung
- Automatik
- Leise
- Stufe 1
- Stufe 2
- Stufe 3
- Stufe 4
- Stufe 5

<!-- DOC -->
#### Lamellen Bewegung

Steuert die Bewegung der Lamellen.

- Keine Änderung
- Aus
- Vertikal
- Horizontal
- Vertikal und Horizontal

<!-- DOC -->
#### Vertikale Lamellenstellung

Diese Einstellung ist nur verfügbar, wenn bei Lamellen Bewegung "Keine Änderung" oder "Aus" gewählt wurde.
Gibt die Position der Lamellen an. 

- Keine Änderung
- Ganz Oben
- Oben
- Mitte
- Unten
- Ganz Unten

<!-- DOC -->
#### Leistungsbegrenzung

Verfügbare Einstellungen:

- Keine Änderung
- 50%
- 75%
- 100%

<!-- DOC -->
#### Gerätemodus

Verfügbare Einstellungen:

- Keine Änderung
- Standard
- Hi-Power>
- ECO
- Leise 1 (Außengerät)
- Leise 2 (Außengerät)

<!-- DOC -->
#### Luftreinigung

Verfügbare Einstellungen:

- Keine Änderung
- Aus
- Ein