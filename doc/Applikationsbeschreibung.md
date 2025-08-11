# Applikationsbeschreibung Klimanlagen KNX Adapter

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
## Klimanalage

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
### Eingang für externe Raumtemperatur

Stellt ein Gruppenobjekt für einen externen Raumtemperatursensor bereit.
Bei Klimageräten die dies nicht unterstüzten, erfolgt die Auswertung in der Steuerung durch Vorgabe einer angepassten Solltemperatur an das Klimagerät.

Wenn beispielsweise das Klimageräte eine Raumtemperatur von 22°C meldet, der externe Raumsensor jedoch 23°C, wird an das Klimagerät eine Solltemperatur übergeben die um die Differenz (in diesem Beispiel 1°C) kleiner ist, als die tatsächlich vorgegeben Solltemperatur.

Beispiel:

- Soll-Temperatur Vorgabe: 21°C
- Raum-Temperaturmessung des Klimagerätes 21°C
- Exterer Temperatursensor meldet 22°C
- -> Daraus ergibt sich eine Differenz von 1°C zwischen interne und externer Temperatur
- -> In diesem Fall wird dem Klimageräte eine Solltemperatur um 1° weniger als die eigentliche Sollwertvorgabe von 21° übergeben, also 20°

Damit wird erreicht, das Klimageräte nicht aufhöhrt zu kühlen, da die interne Vorgabe an das Gerät 20°C lautet, die Raumtemperatur des Klimagerätes aber 21°C misst.

Ein weiterer Vorteil ist eine genauere Temperaturvorgabe. Die meisten Klimageräte unterstützen nur eine Vorgabe von ganzen Grad. Die Regelung ist aber in der Lage durch ständiges Anpassen der Solltemperatur auch zwischenbereiche zu erreichen.

Die Anpassung der Solltemperatur geht natürlich nur innerhalb der Grenzen des Klimagerätes. D.H. wenn die errechnet Vorgabe für das Klimageräte 15°C wäre, dass Klimageräte aber als kleinste Temperatur 17°C unterstützt, kann keine Anpassung erfolgen.

Wichtig: Durch die modifizierte Solltemperatur funktioniert eine eventuell am Klimagerät vorhanden Solltemperatur-Anzeige nicht richtig und kann daher nicht mehr genutzt werden kann.
Das Gruppenobjekt 'Solltemperatur Status' liefert aber den für richtigen, für die Regelung verwendete Temperatur.
Eine Änderung der Solltemperatur über die Infrarot Fernbedienung funktioniert weiterhin, da die Steuerung den angeforderten Werte entsprechend korrigiert.

<!-- DOC -->
### Gruppenobjekt 'Klimagerät Solltemperatur Ausgang' (für Diagnose)

Wird die Regelung für die externe Raumtemperatur verwendet, kann über ein Gruppenobjekt die Stelltemperatur (Solltemperatur) die dem Klimagerät aktuell übergeben ist, ausgelesen werden.

<!-- DOC -->
### Externe Raumtemperatur überwachen

Wird die Regelung für die externe Raumtemperatur verwendet, kann der externe Raumtemperatureingang überwacht werden.

- Nein
- Ja, nach Zeitablauf Lesen, dann ignorieren
  Bleibt das Telegramm für die externe Raumtemperatur länger als die eingestellte Zeit aus, wird zuerst eine Lesesignal geschickt, danach wird auf den Raumtemperatursensor des Klimagerätes umgeschalten
- Ja, nach Zeitablauf ignorieren
  Bleibt das Telegramm für die externe Raumtemperatur länger als die eingestellte Zeit aus, wird auf den Raumtemperatursensor des Klimagerätes umgeschalten

<!-- DOC  -->
### Überwachungszeit

Überwachungszeit, nach der beim Ausbleiben von Telegrammen die eingestellte Aktion ausgelöst wird.

<!-- DOC  -->
#### Überwachung Zeitbasis

Die Einheit für die Überwachungszeit

<!-- DOC  -->
#### Überwachung Zeit

Die Überwachungszeit

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
