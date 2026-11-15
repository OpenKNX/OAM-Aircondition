# Applikationsbeschreibung Klimaanlagen KNX Adapter

## Wichtige Hinweise

* Diese KNXprod wird nicht von der KNX Association offiziell unterstützt!
* Die Erzeugung der KNXprod geschieht auf Eure eigene Verantwortung!

## Module

Die Klimaanlagen App besteht aus folgenden Modulen:

- [Basiseinstellungen](https://github.com/OpenKNX/OGM-Common/blob/v1dev/doc/Applikationsbeschreibung-Common.md)
- [Konfigurationstransfer](https://github.com/OpenKNX/OFM-ConfigTransfer/blob/v1/doc/Applikationsbeschreibung-ConfigTransfer.md)
- [Netzwerk](https://github.com/OpenKNX/OFM-Network/blob/v1dev/doc/Applikationsbeschreibung-Netzwerk.md)
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
- Mitsubishi

<!-- DOCEND -->
### Mitsubishi

Folgende Einstellungen stehen nur für Mitsubishi Klimageräte zur Verfügung:

<!-- DOC -->
#### Minimale Soll-Temperatur

Hier muss die für das Klimagerät passende minimale Solltemperatur eingestellt werden (16°C oder 19°C).

<!-- DOC -->
## Basiseinstellungen

In diesem Abschnitt werden allgemeine Konfigurationen vorgenommen

<!-- DOC -->
### Wifi LED

Über diese Option wird das Verhalten der Wifi LED am Klimagerät vorgegeben

- WLAN Status
- Immer Aus
- Immer Ein
- Schaltbar über Gruppenobjekt

Daikin: Ohne separate Geräte-/Sensor-LED-Freigabe bleibt diese WLAN-LED-Regelung aktiv. Wird die separate LED-Steuerung aktiviert, schreibt die automatische WLAN-LED-Regelung nicht mehr in das Daikin-LED-Register.


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

Der Treiber steuert Innengeräte über S21. Der Funktionsumfang hängt vom Modell, dessen Firmware und dem Betriebsmodus ab; eine erkannte Protokollversion ist keine vollständige Funktionsfreigabe.

- Grundsteuerung: Ein/Aus, Auto/Kühlen/Heizen/Trocknen/Lüfter, Auto/Geräuscharm/Stufe 1–5 und getrennte Swing-Achsen.
- Solltemperaturen werden auf 0,5 °C gerundet. Ohne verfeinerte Modellgrenzen gilt als Rückfallbereich 18–32 °C; FU02 und lokale Modellprofile können die Grenzen ändern. Szenen bieten 17–30 °C einschließlich der halben Zwischenwerte an; der Treiber lehnt Werte außerhalb seiner aktiven Grenzen ab. Die Temperatur-Auf/Ab-Objekte verwenden weiterhin ganze Gradschritte.
- Powerful, Econo, Außengerät Quiet, Comfort, Intelligent Eye und Streamer besitzen unabhängige Steuer-/Statusobjekte. Econo und Quiet können gemeinsam angefordert werden; Powerful ist dazu exklusiv. Der einzelne Gerätemodus-/Szenen-Preset ersetzt die anderen Power-Presets.
- Vertikale Positionscodes und modellabhängig freigeschaltete horizontale Positionscodes sind auch in Szenen verfügbar. Code 0 hält die aktuelle Position; Codes sind keine Gradangaben.
- Demand benötigt ein gespeichertes, ausdrücklich aktiviertes Modellprofil für Minimum und Schrittweite. Timer benötigen eine eigene Freigabe mit Schrittweite und Obergrenze.
- Relative Ein-/Ausschalttimer melden konfigurierte Verzögerungen und Aktiv-Bits, keinen Countdown. Die separate Timerleuchte wird über DB geschrieben und über FB gelesen.
- Leistungsaufnahme, 64-Bit-Energiezähler, Innen-/Außentemperatur, Raumfeuchte, Innengerät-Lüfterdrehzahl, rohe Lamellenwinkel und Kompressordaten stehen je nach Antwortumfang mit Gültigkeitskennzeichen bereit. Rohwinkel werden nur mit konfigurierter Modellskalierung in Grad umgerechnet.
- Qualitative Feuchtestufen (Aus/Niedrig/Standard/Hoch/Kontinuierlich) sind von gemessenen RH-Prozentwerten getrennt und werden gegen Gerätefähigkeiten und Betriebsmodus geprüft. Erweiterte Klima-Varianten sind derzeit lokale Diagnosekommandos, keine zusätzlichen Standard-KNX-HVAC-Modi.

Ein S21-ACK bestätigt nur die Annahme auf Transportebene. Der Treiber wartet auf passende Rückmeldung und erhält dabei andere Felder desselben Registers. Es gibt keinen F6→F3-Fallback für Spezialmodi; F3 enthält Timerdaten. Econo wird aus F7 gelesen, ein negatives FU00-Flag unterdrückt diesen Status nicht. Rz-basierte Betriebs-/Abtauinterpretation erfordert eine ausdrückliche lokale Vertrauensfreigabe.

Für Daikin ist weder eine native Übergabe externer Raumtemperaturen noch eine automatische Sollwertkorrektur aus einem KNX-Raumsensor implementiert. Das F6-Sensorbit betrifft Intelligent Eye, keine alternative Temperaturquelle. Die allgemeine Beschreibung der externen Regelung beschreibt einen künftigen Ansatz.

Objektnummern, Datentypen und Statusmasken stehen in der [Daikin-KNX-Referenz](Daikin-KNX-controls.md). Lokale `s21 profiles`, `s21 writes` und `s21 errors` zeigen Konfiguration, Schreibzustände und begrenzte Rohdaten abgewiesener Antworten. Abfragbarkeit bestätigt weder Schreibunterstützung noch physische Wirkung.

### Mitsubishi

Folgende Sonder-Einstellungen und Funktionen stehen für Mitsubishi Klimageräte zur Verfügung.

- TODO: Externe Ziel-Temperatur
- TODO: Horizontale Lamellen-Position

<!-- DOC -->
## Szenen

Zehn unabhängige Szenendefinitionen A–J können aktiviert und einer Szenennummer 1–64 zugeordnet werden. Die Nummer löst die jeweilige Konfiguration aus; sie ist kein fest vorgegebener Gerätemodus. Mehrere aktive Definitionen mit gleicher Nummer werden nacheinander ausgeführt.

„Keine Änderung“ lässt das jeweilige Feld unverändert. Bei Daikin können Modus, Temperatur, Lüfter, Swing, vertikale/horizontale Position, Power-Preset und Streamer konfiguriert werden. Der Treiber prüft Werte gegen seine aktiven Grenzen und Fähigkeiten. Der Power-Auftrag wird nach den anderen Szenenaufträgen gesetzt; die Gerätebestätigung kann zeitversetzt eintreffen.

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

Optionen für Mitsubishi:

- Keine Änderung
- Automatik
- Kühlen
- Heizen
- Trocknen
- Lüfter

<!-- DOC -->
### Solltemperatur

Vorgabe der Solltemperatur

Daikin-Szenen bieten 17–30 °C einschließlich halber Zwischenwerte. Es gelten zusätzlich die aktiven Treiber-/Modellgrenzen; außerhalb liegende Vorgaben werden abgewiesen. Absolute Gruppenobjekte erlauben 0,5 °C, die Auf/Ab-Objekte ändern um 1 °C.

- Keine Änderung
- Temperaturvorgabe

<!-- DOC  -->
#### Lüfter

Lüftergeschwindigkeit

Daikin: „Keine Änderung“, Automatik, Geräuscharm und Stufe 1–5. Geräuscharm am Innengerät-Lüfter und Außengerät Quiet sind unterschiedliche Funktionen. Prozent-Gruppenobjekte werden auf die numerischen Lüfterstufen gerundet; Prozentwerte sind keine Drehzahlvorgabe.

Optionen für Toshiba:

- Keine Änderung
- Automatik
- Leise
- Stufe 1
- Stufe 2
- Stufe 3
- Stufe 4
- Stufe 5

Optionen für Mitsubishi:

- Keine Änderung
- Automatik
- Stufe 1 / Leise
- Stufe 2
- Stufe 3
- Stufe 4
- Stufe 5

<!-- DOC -->
#### Lamellenbewegung

Steuert die Bewegung der Lamellen.

Daikin unterstützt getrennte vertikale und horizontale Swing-Achsen. „Aus“ beendet Swing auf der jeweiligen Achse. Feste Positionscodes sind separate Einstellungen; horizontale Codes benötigen das konfigurierte Modellprofil. Für reproduzierbare Positionen Swing auf der betreffenden Achse vorher ausschalten.

Optionen für Toshiba:

- Keine Änderung
- Aus
- Vertikal
- Horizontal
- Vertikal und Horizontal

<!-- DOC -->
#### Vertikale Lamellenstellung

Gibt die vertikale Position der Lamellen an. Bei Toshiba/Mitsubishi wird diese Szenenoption nur bei Lamellenbewegung „Keine Änderung“ oder „Aus“ angeboten.

Optionen für Toshiba:

- Keine Änderung
- Ganz Oben
- Oben
- Mitte
- Unten
- Ganz Unten

Optionen für Mitsubishi:

- Keine Änderung
- Ganz Oben
- Oben
- Mitte
- Unten
- Ganz Unten

Daikin-Szenen bieten:

- Keine Änderung
- Halten (Code 0)
- Position 1–5

Code 0 hält die aktuelle Stellung. Nach Erkennung des FR-Status sind die Indizes adressierbar; daraus folgt keine Anzahl verschiedener physischer Winkel. Über das separate Code-Gruppenobjekt ist zusätzlich Code 6 für den S21-Automatikmarker `?` verfügbar, nicht über die Szene. Änderungen erhalten die horizontale Position und die übrigen Registerbytes. Für feste Positionen Swing auf dieser Achse vorher ausschalten.

<!-- DOC HelpContext="Horizontale-Lamellenstellung" -->
#### Horizontale Lamellenstellung

Daikin-Szenen bieten:

- Keine Änderung
- Halten (Code 0)
- Position 1–8

Voraussetzung sind ein lesbarer FR-Status und ein ausdrücklich konfiguriertes horizontales Modellmaximum. Werte oberhalb dieses Maximums werden abgewiesen. Für feste Stellungen Swing auf der horizontalen Achse vorher ausschalten.

Der Code 0 hält die aktuelle Position und bezeichnet weder Mitte noch einen Winkel. Die Indizes sind modellabhängig; die Anzahl unterschiedlicher Winkel muss am Gerät bestimmt werden. Eine Änderung erhält den vertikalen Code und die übrigen Registerbytes.

Das separate Code-Objekt hat Status, Gültigkeit, konfiguriertes Maximum und eine Bestätigungsmaske. Eine passende Rückmeldung bestätigt den geschriebenen Code, nicht die physische Stellung.


<!-- DOC -->
#### Leistungsbegrenzung

Mit dieser Option, kann die maximale Leistung des Klimageräts angepasst werden.

Optionen für Toshiba:

- Keine Änderung
- 50%
- 75%
- 100%

Bei Daikin ist eine Reduzierung nur mit einem ausdrücklich konfigurierten,
modellgeprüften Demand-Profil möglich. Minimum und Schrittweite können im
Daikin-Bereich gespeichert werden. Ohne Profil bleibt die Untergrenze 100 %.
Die Demand-Anforderung ist keine lineare Begrenzung der gemessenen Wattzahl. In Daikin-Szenen gibt es dafür kein eigenes Feld; die Steuerung erfolgt über das separate Leistungsbegrenzungsobjekt mit dem konfigurierten Profil.

Für Mitsubishi steht diese Option nicht zur Verfügung.

<!-- DOC -->
#### Gerätemodus

Optionen für Daikin:

- **Standard**: Normaler Betriebsmodus
- **Hi-Power**: Entspricht Daikin "Powerful"-Modus für maximale Leistung
- **ECO**: Econo über D7/F7. Der Gerätemodus-/Szenen-Preset schaltet Powerful und Quiet ab und wartet auf die passende Rückmeldung, bevor Econo gesetzt wird. Ein negatives FU00-Flag allein sperrt Econo nicht.
- **Leise 1**: Daikin "Quiet". S21 bietet dafür genau einen Ein/Aus-Zustand.
- **Leise 2 (Legacy-Gruppenobjekte)**: Kompatibilitätsalias für Leise 1, keine zweite Geräuschstufe. Die Rückmeldung erfolgt als Leise 1; Leise-2-Status bleibt 0. Daikin-Szenen bieten diese Alias-Auswahl nicht an.

In Szenen steht zusätzlich „Keine Änderung“ zur Verfügung.

Über die unabhängigen Objekte können Econo und Außengerät Quiet gemeinsam angefordert werden. Powerful schließt beide aus; der einzelne Gerätemodus-/Szenen-Preset wählt dagegen jeweils einen Power-Preset.

Ein noch nicht schreibbestätigter unabhängiger Econo-Ein-Auftrag wird bei aktivem Powerful zurückgewiesen. Für diesen ersten Einzelversuch Powerful vorher ausschalten; die ausdrückliche Preset-Wahl ECO führt den Wechsel selbst aus. Die tatsächliche Ausführung bleibt modell- und betriebsmodusabhängig.

Die unabhängigen Optionen melden ihre Lesbarkeit/Verfügbarkeit im Bitmaskenobjekt „Optionen abfragbar“ (Econo: Bit 1). Lesbarkeit und Verfügbarkeit bestätigen noch keinen erfolgreichen Schreibzugriff.

Optionen für Toshiba:

- Keine Änderung
- Standard
- Hi-Power
- ECO
- Außengerät Leise 1
- Außengerät Leise 2

Für Mitsubishi steht diese Option nicht zur Verfügung.

<!-- DOC -->
#### Luftreinigung

Verfügbare Einstellungen:

- Keine Änderung
- Aus
- Ein

Bei Daikin steuert diese Option Streamer, sofern der Status lesbar und die Funktion verfügbar ist. Für Mitsubishi ist hier keine Luftreinigungssteuerung implementiert.

<!-- DOCEND -->
## Daikin-Modellprofile

<!-- DOC HelpContext="Daikin-Demandprofil" -->
#### Modellgeprüftes Demand-Profil

Gibt die reduzierte Leistungsbegrenzung für das Daikin-Gerät frei. Minimum und Schrittweite müssen für das konkrete Gerät geeignet sein.

Zur Auswahl:
- Nein
  Reduzierte Demand-Werte sind gesperrt.
- Ja
  Die Vorgaben „Geprüftes Demand-Minimum“ und „Geprüfte Demand-Schrittweite“ werden verwendet.

Standard: Nein. Das Profil wird gespeichert und beim Start geladen. Das Ändern der Freigabe allein sendet keinen Leistungsauftrag und setzt keine bereits am Gerät eingestellte Begrenzung zurück. Lesbarer Status oder ein ACK bestätigen noch keine erfolgreiche Leistungsbegrenzung.

Demand begrenzt die angeforderte Konditionierungsleistung, nicht linear die Leistungsaufnahme in Watt. Die Ausführung ist modell- und betriebsmodusabhängig.

<!-- DOC HelpContext="Daikin-Demand-Minimum" -->
#### Geprüftes Demand-Minimum

Kleinster Prozentwert, der bei aktiviertem Demand-Profil angefordert werden darf.

- Wertebereich: 0–100 %
- Standard: 100 % (keine Leistungsreduzierung)

Nur einen für das Gerät geeigneten Mindestwert eintragen. Die zugelassenen Werte werden von 100 % abwärts in der konfigurierten Schrittweite gebildet. Die Differenz zwischen 100 % und dem Minimum muss durch die Schrittweite teilbar sein. 99 % ist kein geeigneter Profilwert, da der mehrdeutige Rückgabewert 99 % nicht als gültige Demand-Rückmeldung bestätigt wird.

<!-- DOC HelpContext="Daikin-Demand-Schrittweite" -->
#### Geprüfte Demand-Schrittweite

Abstand der zugelassenen Prozentwerte von 100 % abwärts bis zum konfigurierten Minimum.

- Wertebereich: 1–100 Prozentpunkte
- Standard: 1 Prozentpunkt

Die Differenz zwischen 100 % und dem Minimum muss durch die Schrittweite teilbar sein. Die Einstellung gibt keine ungeprüften Gerätestufen frei. Der mehrdeutige Wert 99 % wird nicht als gültige Demand-Rückmeldung verwendet und ist deshalb kein geeigneter Profilwert.

<!-- DOC HelpContext="Daikin-Horizontalprofil" -->
#### Modellgeprüfte horizontale Positionen

Legt das höchste freigegebene horizontale Positionskennzeichen des Daikin-Geräts fest.

Zur Auswahl:
- 0
  Horizontale Positionsaufträge deaktiviert (Standard).
- 1–8
  Codes 0 bis zum eingestellten Maximum freigeben. Innerhalb eines aktivierten Profils bedeutet Positionscode 0 „Halten“.

Das Maximum muss für das konkrete Gerät bestimmt werden. Ein lesbarer FR-Status oder ACK bestätigt noch keine physisch nutzbare Lamellenposition. Positionscodes sind keine Grad- oder Prozentwinkel. Das gespeicherte Profil wird beim Start geladen; dabei wird kein Positionsauftrag gesendet.

<!-- DOC HelpContext="Daikin-Timer" -->
#### Timer-Steuerung aktivieren

Gibt die relative Ein-/Ausschalttimer-Steuerung und die separate Timerleuchte für Daikin frei.

Zur Auswahl:
- Nein
  Timer-Gruppenobjekte ausblenden (Standard). Bereits im Gerät aktive Timer werden dadurch nicht ausgeschaltet.
- Ja
  Timer-Gruppenobjekte mit der eingestellten Schrittweite und Obergrenze verwenden.

Nur aktivieren, wenn die Timerwerte zum Gerät passen. Die Freigabe allein schaltet keinen Timer ein. Ein positiver Minutenauftrag aktiviert den jeweiligen Timer, 0 deaktiviert ihn. Ein Aktiv-Auftrag benötigt eine zuletzt bekannte positive Verzögerung; diese muss nach Neustart oder Kommunikationsverlust erneut bekannt sein.

Die Rückmeldung zeigt die konfigurierte Verzögerung und zählt nicht herunter. Die Aktiv-Bits entscheiden, ob ein Timer läuft. Änderungen an einem Timer können den Zeitbezug des anderen Timers neu starten; es ist kein Uhrzeit- oder Wochenprogramm.

Timeraufträge benötigen einen lesbaren F3-Status. Zum Deaktivieren muss die Aus-Kodierung der jeweiligen Achse bekannt sein. Die orange Timerleuchte wird getrennt von Geräte-/Sensor-LEDs gesteuert (DB schreiben, FB lesen). Ein ACK allein bestätigt weder das Setzen eines Timers noch dessen Auslösung.

<!-- DOC HelpContext="Daikin-Timer-Schrittweite" -->
#### Timer-Schrittweite

Zeitabstand der erlaubten Timerverzögerungen in Minuten.

- Wertebereich: 10–1440 Minuten, positive Vielfache von 10
- Standard: 60 Minuten

Das S21-Protokoll verwendet Zehn-Minuten-Einheiten. Das konkrete Gerät kann gröbere Schritte verlangen. Die Schrittweite muss zur Timer-Obergrenze passen; die Obergrenze muss ein Vielfaches der Schrittweite sein. Eine kleinere Schrittweite verkürzt nicht automatisch einen bereits programmierten Timer.

<!-- DOC HelpContext="Daikin-Timer-Obergrenze" -->
#### Timer-Obergrenze

Größte erlaubte Verzögerung für Ein- und Ausschalttimer in Minuten.

- Wertebereich: 10–1440 Minuten
- Standard: 720 Minuten

Der Wert muss mindestens der Schrittweite entsprechen und durch sie teilbar sein. Nur eine zum Gerät passende Obergrenze eintragen. Der Protokollbereich bis 1440 Minuten bestätigt nicht, dass jedes Gerät diese Zeitspanne unterstützt. Die Rückmeldung ist eine konfigurierte Verzögerung, kein Restzeitwert.

<!-- DOC HelpContext="Daikin-Geraete-LED" -->
#### Geräte-/Sensor-LED separat steuern

Legt fest, ob die Daikin-Geräte-/Sensor-LED unabhängig vom WLAN-Status gesteuert wird.

Zur Auswahl:
- Nein
  Die bisherige WLAN-LED-Regelung bleibt zuständig (Standard).
- Ja
  Separate LED-Gruppenobjekte verwenden; die automatische WLAN-LED-Regelung schreibt nicht mehr in das Daikin-LED-Register.

Das LED-Objekt verwendet 0=Ein, 1=Dunkel, 2=Aus. Verfügbare Stufen und Statusgültigkeit haben eigene Rückmeldungen. Bei Intelligent-Eye-Hardware sind nur Ein/Aus verfügbar; Dunkel wird abgewiesen. Ohne diese Hardware hängt Ein/Dunkel/Aus vom Gerät ab. Hardwareerkennung und F6-Status müssen zuerst vorliegen. Das Sensorbit bezeichnet keine externe Raumtemperaturquelle.

<!-- DOC HelpContext="LuftreinigungStreamer" -->
#### Luftreinigung / Streamer

Schaltet die Streamer-Funktion ein oder aus, sofern das Daikin-Gerät sie bereitstellt. Die Szene bietet „Keine Änderung“, „Aus“ oder „Ein“. Die bestätigte Rückmeldung beschreibt die Geräteeinstellung, nicht eine gemessene Luftreinigungswirkung.
