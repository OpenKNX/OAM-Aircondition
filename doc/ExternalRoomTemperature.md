#  Temperaturreglung über OpenKNX

Die Temperaturreglung steht derzeit nicht noch nicht zur Verfügung.
Diese Abschnitt dient rein für die Dokumentation einer künftigen Entwicklung.

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