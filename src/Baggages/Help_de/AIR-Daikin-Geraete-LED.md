### Geräte-/Sensor-LED separat steuern

Legt fest, ob die Daikin-Geräte-/Sensor-LED unabhängig vom WLAN-Status gesteuert wird.

Zur Auswahl:
- Nein
  Die bisherige WLAN-LED-Regelung bleibt zuständig (Standard).
- Ja
  Separate LED-Gruppenobjekte verwenden; die automatische WLAN-LED-Regelung schreibt nicht mehr in das Daikin-LED-Register.

Das LED-Objekt verwendet 0=Ein, 1=Dunkel, 2=Aus. Verfügbare Stufen und Statusgültigkeit haben eigene Rückmeldungen. Bei Intelligent-Eye-Hardware sind nur Ein/Aus verfügbar; Dunkel wird abgewiesen. Ohne diese Hardware hängt Ein/Dunkel/Aus vom Gerät ab. Hardwareerkennung und F6-Status müssen zuerst vorliegen. Das Sensorbit bezeichnet keine externe Raumtemperaturquelle.
