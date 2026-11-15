### Timer-Steuerung aktivieren

Gibt die relative Ein-/Ausschalttimer-Steuerung und die separate Timerleuchte für Daikin frei.

Zur Auswahl:
- Nein
  Timer-Gruppenobjekte ausblenden (Standard). Bereits im Gerät aktive Timer werden dadurch nicht ausgeschaltet.
- Ja
  Timer-Gruppenobjekte mit der eingestellten Schrittweite und Obergrenze verwenden.

Nur aktivieren, wenn die Timerwerte zum Gerät passen. Die Freigabe allein schaltet keinen Timer ein. Ein positiver Minutenauftrag aktiviert den jeweiligen Timer, 0 deaktiviert ihn. Ein Aktiv-Auftrag benötigt eine zuletzt bekannte positive Verzögerung; diese muss nach Neustart oder Kommunikationsverlust erneut bekannt sein.

Die Rückmeldung zeigt die konfigurierte Verzögerung und zählt nicht herunter. Die Aktiv-Bits entscheiden, ob ein Timer läuft. Änderungen an einem Timer können den Zeitbezug des anderen Timers neu starten; es ist kein Uhrzeit- oder Wochenprogramm.

Timeraufträge benötigen einen lesbaren F3-Status. Zum Deaktivieren muss die Aus-Kodierung der jeweiligen Achse bekannt sein. Die orange Timerleuchte wird getrennt von Geräte-/Sensor-LEDs gesteuert (DB schreiben, FB lesen). Ein ACK allein bestätigt weder das Setzen eines Timers noch dessen Auslösung.
