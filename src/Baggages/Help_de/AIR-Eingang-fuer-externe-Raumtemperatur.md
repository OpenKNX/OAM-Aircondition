### Eingang für externe Raumtemperatur

Stellt ein Gruppenobjekt für einen externen Raumtemperatursensor bereit.
Bei Klimageräten, die dies nicht unterstützen, erfolgt die Auswertung in der Steuerung durch Vorgabe einer angepassten Solltemperatur an das Klimagerät.

Wenn beispielsweise das Klimagerät eine Raumtemperatur von 22°C meldet, der externe Raumsensor jedoch 23°C, wird an das Klimagerät eine Solltemperatur übergeben, die um die Differenz (in diesem Beispiel 1°C) kleiner ist als die tatsächlich vorgegebene Solltemperatur.

Beispiel:

- Soll-Temperatur Vorgabe: 21°C
- Raum-Temperaturmessung des Klimagerätes: 21°C
- Externer Temperatursensor meldet: 22°C
- -> Daraus ergibt sich eine Differenz von 1°C zwischen interner und externer Temperatur
- -> In diesem Fall wird dem Klimagerät eine Solltemperatur um 1° weniger als die eigentliche Sollwertvorgabe von 21° übergeben, also 20°

Damit wird erreicht, dass das Klimagerät nicht aufhört zu kühlen, da die interne Vorgabe an das Gerät 20°C lautet, die Raumtemperatur des Klimagerätes aber 21°C misst.

Ein weiterer Vorteil ist eine genauere Temperaturvorgabe. Die meisten Klimageräte unterstützen nur eine Vorgabe von ganzen Grad. Die Regelung ist aber in der Lage, durch ständiges Anpassen der Solltemperatur auch Zwischenbereiche zu erreichen.

Die Anpassung der Solltemperatur geht natürlich nur innerhalb der Grenzen des Klimagerätes. D.h., wenn die errechnete Vorgabe für das Klimagerät 15°C wäre, das Klimagerät aber als kleinste Temperatur 17°C unterstützt, kann keine Anpassung erfolgen.

Wichtig: Durch die modifizierte Solltemperatur funktioniert eine eventuell am Klimagerät vorhandene Solltemperatur-Anzeige nicht richtig und kann daher nicht mehr genutzt werden.
Das Gruppenobjekt 'Solltemperatur Status' liefert aber den richtigen, für die Regelung verwendeten Temperaturwert.
Eine Änderung der Solltemperatur über die Infrarot-Fernbedienung funktioniert weiterhin, da die Steuerung den angeforderten Wert entsprechend korrigiert.

